#include "RuleEngine.hpp"
#include "../clients/Metadata.hpp"
#include "../database/Database.hpp"
#include "../common/utils.hpp"
#include "../clients/Command.hpp"
#include "../clients/Rule.hpp"

#include <stdlib.h>
#include <ctime>
#include <thread>
#include <unistd.h>
#include <math.h>

namespace coreservices
{
    bool ActionTimestampList::compareTime(ActionTimestamp action1, ActionTimestamp action2)
    {
        return (action1.timestamp > action2.timestamp);
    }

    void ActionTimestampList::push_back(ActionTimestamp action)
    {
        const std::lock_guard<std::mutex> lock(this->ActionTimestampList::mutex_);
        this->actions.push_back(action);

        // sort the timestamp in ascending order
        sort(this->actions.begin(), this->actions.end(), compareTime);
    }

    bool ActionTimestampList::back(ActionTimestamp &actionTimestamp)
    {
        const std::lock_guard<std::mutex> lock(this->ActionTimestampList::mutex_);
        if (this->actions.empty())
        {
            return false;
        }

        actionTimestamp = this->actions.back();
        return true;
    }

    void ActionTimestampList::pop_back()
    {
        const std::lock_guard<std::mutex> lock(this->ActionTimestampList::mutex_);
        if (this->actions.empty())
        {
            return;
        }

        return this->actions.pop_back();
    }

    void ActionTimestampList::removeByRuleId(string id)
    {
        const std::lock_guard<std::mutex> lock(this->ActionTimestampList::mutex_);
        if (this->actions.empty())
        {
            return;
        }

        for (auto iter = this->actions.begin(); iter != this->actions.end();)
        {
            if (iter->action.RuleId == id)
            {
                iter = this->actions.erase(iter);
            }
            else
            {
                ++iter;
            }
        }
    }

    void ActionQueue::push(Action action)
    {
        const std::lock_guard<std::mutex> lock(this->ActionQueue::mutex_);
        this->queue_.push(action);
    }

    bool ActionQueue::pop(Action &action)
    {
        const std::lock_guard<std::mutex> lock(this->ActionQueue::mutex_);
        if (!this->queue_.empty())
        {
            action = this->queue_.front();
            this->queue_.pop();
            return true;
        }
        return false;
    }

    uint64_t calTimestamp(bool checkCurrentTime, uint64_t currentTime, uint64_t startTime, uint64_t endTime, uint32_t startInDayTime, uint32_t endInDayTime, vector<string> repeatDays, uint64_t timeInDayCondition, uint32_t delayTime)
    {
        string dayInWeek = getDayInWeek();
        uint64_t startDayTimestamp = (currentTime / 86400) * 86400;

        if (count(repeatDays.begin(), repeatDays.end(), dayInWeek))
        {
            uint64_t triggerTimeInDay = timeInDayCondition;
            uint64_t triggerTime = startDayTimestamp + triggerTimeInDay;

            if ((!checkCurrentTime || (triggerTime >= currentTime)) &&
                ((triggerTime >= startTime) && (triggerTime <= endTime)) &&
                ((triggerTimeInDay >= startInDayTime) && (triggerTimeInDay <= endInDayTime)))
            {
                return triggerTime + delayTime;
            }
        }
        return 0;
    }

    void RuleEngine::init(Command commandClient)
    {
        this->commandClient = commandClient;

        function<void()> scheduler = bind(&RuleEngine::scheduleProcess, this);
        thread thr1(scheduler);
        thr1.detach();

        function<void()> actioner = bind(&RuleEngine::executeActionProcess, this);
        thread thr2(actioner);
        thr2.detach();

        this->refreshActionTimestampList(true);
    }

    void RuleEngine::refreshActionTimestampList(bool checkCurrentTime)
    {
        string err;
        auto listRule = Db::getDb()->listRule("", err);
        if (err != "")
        {
            cout << "Handle Rule with database error: " << err << endl;
            return;
        }

        for (auto rule : listRule)
        {
            if (rule.type != 0)
            {
                addActionTimestampListFromRule(rule, checkCurrentTime);
            }
        }
    }

    void RuleEngine::addActionTimestampListFromRule(model::Rule rule, bool checkCurrentTime)
    {
        // Check mode
        auto resources = Db::getDb()->listReading("Gateway");
        if (resources[0].value != "1")
        {
            cout << "Select Schedule mode" << endl;
            return;
        }
        ActionTimestamp actionTimestamp;
        Action action;

        action.RuleId = rule.id;
        uint64_t currentTime = localTimestamp();

        if (rule.activeState == ActiveState::DISABLED)
        {
            return;
        }
        int noOfTimestamps = 0;
        for (auto time : rule.timeInDayConditions)
        {
            cout << time << endl;
            noOfTimestamps++;
        }

        for (int i = 0; i < noOfTimestamps; i += 2)
        {
            cout << "Rule - Start time \t\t " << rule.timeInDayConditions[i] << endl;
            for (auto devAction : rule.deviceActions)
            {
                uint64_t timestamp = calTimestamp(
                    checkCurrentTime,
                    currentTime,
                    rule.startTime,
                    rule.endTime,
                    rule.startInDayTime,
                    rule.endInDayTime,
                    rule.repeatDays,
                    rule.timeInDayConditions[i],
                    // time,
                    devAction.delayTime * 60);
                if (timestamp == 0)
                {
                    continue;
                }

                action.DeviceId = devAction.deviceId;
                action.ResourceName = devAction.resourceName;
                action.Value = devAction.value;
                cout << "ResourceName :: " << action.ResourceName << "  ---  Value :: " << action.Value << endl;
                action.ActionRuleId = "";

                actionTimestamp.action = action;
                actionTimestamp.timestamp = timestamp + 2;
                cout << "\033[0;38mRule :: " << action.RuleId << " \t--- :: --- Time (Unix) :: " << actionTimestamp.timestamp << "  --- Device Action :: " << action.DeviceId << endl;
                this->ActionTimestampList::push_back(actionTimestamp);
            }
            for (auto ruleAction : rule.ruleActions)
            {
                uint64_t timestamp = calTimestamp(
                    checkCurrentTime,
                    currentTime,
                    rule.startTime,
                    rule.endTime,
                    rule.startInDayTime,
                    rule.endInDayTime,
                    rule.repeatDays,
                    //                    time,
                    rule.timeInDayConditions[i],
                    ruleAction.delayTime * 60);
                if (timestamp == 0)
                {
                    continue;
                }

                action.DeviceId = "";
                action.ResourceName = "";
                action.Value = "";
                action.ActionRuleId = ruleAction.actionRuleId;

                actionTimestamp.action = action;
                actionTimestamp.timestamp = timestamp + 2;
                this->ActionTimestampList::push_back(actionTimestamp);
            }
        }

        for (int i = 1; i < noOfTimestamps; i += 2)
        {
            cout << "Rule - Stop time \t\t " << rule.timeInDayConditions[i] << endl;
            for (auto devAction : rule.deviceActions)
            {
                uint64_t timestamp = calTimestamp(
                    checkCurrentTime,
                    currentTime,
                    rule.startTime,
                    rule.endTime,
                    rule.startInDayTime,
                    rule.endInDayTime,
                    rule.repeatDays,
                    rule.timeInDayConditions[i],
                    // time,
                    devAction.delayTime * 60);
                if (timestamp == 0)
                {
                    continue;
                }

                action.DeviceId = devAction.deviceId;
                action.ResourceName = "OnOff";
                action.Value = "false";
                cout << "ResourceName :: " << action.ResourceName << "  \t---  Value :: " << action.Value << endl;
                action.ActionRuleId = "";

                actionTimestamp.action = action;
                actionTimestamp.timestamp = timestamp + 2;
                cout << "\033[0;38mTrigger rule :: " << action.RuleId << " \t--- :: --- Time (Unix) :: " << actionTimestamp.timestamp << "  --- Device Action :: " << action.DeviceId << endl;
                this->ActionTimestampList::push_back(actionTimestamp);
            }
            for (auto ruleAction : rule.ruleActions)
            {
                uint64_t timestamp = calTimestamp(
                    checkCurrentTime,
                    currentTime,
                    rule.startTime,
                    rule.endTime,
                    rule.startInDayTime,
                    rule.endInDayTime,
                    rule.repeatDays,
                    //                    time,
                    rule.timeInDayConditions[i],
                    ruleAction.delayTime * 60);
                if (timestamp == 0)
                {
                    continue;
                }

                action.DeviceId = "";
                action.ResourceName = "";
                action.Value = "";
                action.ActionRuleId = ruleAction.actionRuleId;

                actionTimestamp.action = action;
                actionTimestamp.timestamp = timestamp + 2;
                this->ActionTimestampList::push_back(actionTimestamp);
            }
        }
    }

    void RuleEngine::addOrRemoveActionTimestampListFromTriggerRule(string id, TriggerState state, string &err)
    {
        auto rule = Db::getDb()->readRule(id, err);
        if (err != "")
        {
            return;
        }
        cout << "rule  ::  " << rule.id << endl;
        cout << "rule name  ::  " << rule.name << endl;
        ActionTimestamp actionTimestamp;
        Action action;

        action.RuleId = rule.id;

        for (auto time : rule.timeInDayConditions)
        {
            cout << time << endl;
            for (auto devAction : rule.deviceActions)
            {
                action.DeviceId = devAction.deviceId;
                action.ResourceName = devAction.resourceName;
                action.Value = devAction.value;
                action.ActionRuleId = "";

                actionTimestamp.action = action;
                actionTimestamp.timestamp = localTimestamp() + devAction.delayTime;
                this->ActionTimestampList::push_back(actionTimestamp);
            }
            for (auto ruleAction : rule.ruleActions)
            {
                action.DeviceId = "";
                action.ResourceName = "";
                action.Value = "";
                action.ActionRuleId = ruleAction.actionRuleId;

                actionTimestamp.action = action;
                actionTimestamp.timestamp = localTimestamp() + ruleAction.delayTime;
                this->ActionTimestampList::push_back(actionTimestamp);
            }
        }
    }

    void RuleEngine::addOrRemoveActionTimestampListFromActiveRule(string id, ActiveState state, string &err)
    {
        if (state == ActiveState::DISABLED)
        {
            this->ActionTimestampList::removeByRuleId(id);
            return;
        }

        auto rule = Db::getDb()->readRule(id, err);
        if (err != "")
        {
            return;
        }

        if (rule.type == RuleType::RULE)
        {
            return;
        }

        ActionTimestamp actionTimestamp;
        Action action;

        action.RuleId = rule.id;

        uint64_t currentTime = localTimestamp();
        uint64_t startDayTimestamp = (currentTime / 86400) * 86400;
        if (rule.type == RuleType::SCHEDULE)
        {
            if (currentTime > startDayTimestamp + rule.startInDayTime + 60)
            {
                return;
            }
            for (auto time : rule.timeInDayConditions)
            {
                cout << time << endl;
                for (auto devAction : rule.deviceActions)
                {
                    action.DeviceId = devAction.deviceId;
                    action.ResourceName = devAction.resourceName;
                    action.Value = devAction.value;
                    action.ActionRuleId = "";

                    actionTimestamp.action = action;
                    actionTimestamp.timestamp = startDayTimestamp + time + devAction.delayTime * 60;
                    cout << "currentTime + devAction.delayTime * 60     " << currentTime + devAction.delayTime * 60;
                    this->ActionTimestampList::push_back(actionTimestamp);
                }
                for (auto ruleAction : rule.ruleActions)
                {
                    action.DeviceId = "";
                    action.ResourceName = "";
                    action.Value = "";
                    action.ActionRuleId = ruleAction.actionRuleId;

                    actionTimestamp.action = action;
                    actionTimestamp.timestamp = startDayTimestamp + time + ruleAction.delayTime * 60;
                    this->ActionTimestampList::push_back(actionTimestamp);
                }
            }
        }
        else
        {
            cout << "Rule.type ....."
                 << "SCENE" << endl;
            for (auto devAction : rule.deviceActions)
            {
                action.DeviceId = devAction.deviceId;
                action.ResourceName = devAction.resourceName;
                action.Value = devAction.value;
                action.ActionRuleId = "";

                actionTimestamp.action = action;
                actionTimestamp.timestamp = currentTime + devAction.delayTime * 60;
                this->ActionTimestampList::push_back(actionTimestamp);
            }
        }
    }

    void RuleEngine::executeAction(Action action)
    {
        string err;

        if (action.DeviceId != "")
        {
            string rqi = "";
            map<string, string> resources;
            map<string, string> options;

            resources.insert({action.ResourceName, action.Value});
            options.insert({"response", "false"});

            err = this->commandClient.issueSetCommand(rqi, action.DeviceId, resources, options);
        }
        else if (action.ActionRuleId != "")
        {
            this->setTriggerCallback(action.ActionRuleId, TriggerState::ON, err);
        }
    }

    void RuleEngine::addCallback(model::Rule rule, string &err)
    {
        this->ActionTimestampList::removeByRuleId(rule.id);
        this->addActionTimestampListFromRule(rule, true);
    }

    void RuleEngine::updateCallback(model::Rule rule, string &err)
    {
        this->ActionTimestampList::removeByRuleId(rule.id);
        this->addActionTimestampListFromRule(rule, true);
    }

    void RuleEngine::deleteCallback(string id, string &err)
    {
        this->ActionTimestampList::removeByRuleId(id);
    }

    void RuleEngine::setStateCallback(string id, ActiveState state, string &err)
    {
        cout << "setStateCallback :: " << id << "  state: " << state << endl;
        this->addOrRemoveActionTimestampListFromActiveRule(id, state, err);
    }

    void RuleEngine::setTriggerCallback(string id, TriggerState state, string &err)
    {
        cout << "setTriggerCallback :: " << id << " state: " << state << endl;
        this->addOrRemoveActionTimestampListFromTriggerRule(id, state, err);
    }

    void RuleEngine::readingCallback(vector<model::Reading> readings, string &err)
    {
        // Check mode
        auto resources = Db::getDb()->listReading("Gateway");
        if (resources[0].value != "2")
        {
            cout << "Select Rule mode" << endl;
            return;
        }
        vector<string> ruleOperationIds;
        for (auto &reading : readings)
        {
            if (reading.resourceName == "OnOff" || reading.resourceName == "Battery")
            {
                continue;
            }
            string err;
            auto listRule = Db::getDb()->listRule("", err);
            if (err != "")
            {
                cout << "Handle Rule with database error: " << err << endl;
                return;
            }
            for (auto rule : listRule)
            {
                if (rule.type == 0 && rule.activeState == ActiveState::ENABLED)
                {
                    uint64_t currentTime = localTimestamp();
                    uint64_t startDayTimestamp = (currentTime / 86400) * 86400;
                    if ((currentTime >= startDayTimestamp + rule.startInDayTime) && (currentTime <= startDayTimestamp + rule.endInDayTime) && (rule.activeState == ActiveState ::ENABLED))
                    {
                        auto deviceConditions = rule.deviceConditions;
                        for (auto deviceCondition : deviceConditions)
                        {
                            if (reading.deviceId == deviceCondition.deviceId)
                            {
                                ruleOperationIds.push_back(rule.id);
                            }
                        }
                    }
                }
            }
        }
        ruleOperationIds.erase(unique(ruleOperationIds.begin(), ruleOperationIds.end()), ruleOperationIds.end());
        uint8_t sizeOfRuleList = ruleOperationIds.size();
        for (int i = 0; i < sizeOfRuleList; ++i)
        {
            cout << ruleOperationIds[i] << endl;
            this->checkRule(ruleOperationIds[i]);
        }
    }

    void RuleEngine::checkRule(string ruleId)
    {
        string err;
        auto rule = Db::getDb()->readRule(ruleId, err);
        bool value_valid_min = false, value_valid_max = false, runRule = false;
        if (rule.conditionLogic == RuleLogic::AND)
        {
            for (auto deviceCondition : rule.deviceConditions)
            {
                auto resources = Db::getDb()->listReading(deviceCondition.deviceId);
                for (auto resource : resources)
                {
                    if (resource.resourceName == deviceCondition.resourceName)
                    {
                        if (deviceCondition.compareOperator == CompareOperator ::GREATER)
                        {
                            cout << "resource.value rule AND " << resource.value << endl;
                            if (stoi(resource.value) >= stoi(deviceCondition.value))
                            {
                                runRule = true;
                            }
                            else
                            {
                                runRule = false;
                                return;
                            }
                        }
                        else if (deviceCondition.compareOperator == CompareOperator ::LESS)
                        {
                            if (stoi(resource.value) <= stoi(deviceCondition.value))
                            {
                                runRule = true;
                            }
                            else
                            {
                                runRule = false;
                                return;
                            }
                        }
                    }
                }
            }
        }
        // Rule OR
        else
        {
            for (auto deviceCondition : rule.deviceConditions)
            {
                cout << deviceCondition.resourceName << " : " << deviceCondition.value << endl;
                auto resources = Db::getDb()->listReading(deviceCondition.deviceId);
                for (auto resource : resources)
                {
                    if (resource.resourceName == deviceCondition.resourceName)
                    {
                        if (deviceCondition.compareOperator == CompareOperator::GREATER)
                        {
                            if (stoi(resource.value) >= stoi(deviceCondition.value))
                            {
                                value_valid_min = true;
                            }
                            else
                            {
                                value_valid_min = false;
                            }
                        }
                        else if (deviceCondition.compareOperator == CompareOperator::LESS)
                        {
                            if (stoi(resource.value) <= stoi(deviceCondition.value))
                            {
                                value_valid_max = true;
                                runRule = true;
                            }
                            else
                            {
                                value_valid_max = false;
                            }
                        }
                    }
                }
                if (value_valid_min && value_valid_max)
                {
                    runRule = true;
                    break;
                }
                else
                {
                    runRule = false;
                }
            }
        }
        if (runRule)
        {
            ActionTimestamp actionTimestamp;
            Action action;

            action.RuleId = rule.id;
            uint64_t currentTime = localTimestamp();

            for (auto time : rule.timeInDayConditions)
            {
                time++;
                for (auto devAction : rule.deviceActions)
                {
                    action.DeviceId = devAction.deviceId;
                    action.ResourceName = devAction.resourceName;
                    action.Value = devAction.value;
                    action.ActionRuleId = "";

                    actionTimestamp.action = action;
                    actionTimestamp.timestamp = currentTime + devAction.delayTime * 60 + 2;
                    this->ActionTimestampList::push_back(actionTimestamp);
                }
                for (auto ruleAction : rule.ruleActions)
                {
                    action.DeviceId = "";
                    action.ResourceName = "";
                    action.Value = "";
                    action.ActionRuleId = ruleAction.actionRuleId;
                    string err = "";
                    auto ruleCheck = Db::getDb()->readRule(ruleAction.actionRuleId, err);
                    if (err != "")
                    {
                        return;
                    }

                    if (ruleCheck.type == RuleType::SCENE)
                    {
                        Rule client;
                        string rqi = genUuid();
                        ActiveState activeState = ActiveState::ENABLED;
                        string err;
                        this->addOrRemoveActionTimestampListFromActiveRule(ruleAction.actionRuleId, activeState, err);
                    }
                }
            }
        }
    }

    void RuleEngine::scheduleProcess()
    {
        uint64_t currentTime;
        uint64_t lastDayTimestamp, currentDayTimestamp;
        ActionTimestamp actionTimestamp;

        currentTime = localTimestamp();
        lastDayTimestamp = (currentTime / 86400) * 86400;

        while (true)
        {
            currentTime = localTimestamp();
            currentDayTimestamp = (currentTime / 86400) * 86400;
            if (currentDayTimestamp != lastDayTimestamp)
            {
                this->refreshActionTimestampList(false);
                lastDayTimestamp = currentDayTimestamp;

                continue;
            }

            while (true)
            {
                if (!this->ActionTimestampList::back(actionTimestamp))
                {
                    break;
                }
                cout << "\033[0;32mNext event after: \t\t " << actionTimestamp.timestamp - currentTime - 1 << "s" << endl;
                if (actionTimestamp.timestamp <= currentTime)
                {
                    this->ActionTimestampList::pop_back();
                    this->ActionQueue::push(actionTimestamp.action);
                    cout << "\033[0;36mExecute time \t\t " << actionTimestamp.timestamp << endl;
                }
                else
                {
                    break;
                }

                usleep(5000);
            }
            sleep(1);
        }
    }

    void RuleEngine::executeActionProcess()
    {
        Action action;
        while (true)
        {
            if (this->ActionQueue::pop(action))
            {
                cout << "\033[0;38mExecute rule :: " << action.RuleId << endl;
                this->executeAction(action);
            }
            sleep(1);
        }
    }
}

#include "RuleEngine.hpp"
#include "clients/Metadata.hpp"
#include "database/Database.hpp"
#include "common/utils.hpp"
#include "clients/Command.hpp"
#include "clients/Rule.hpp"

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
        // uint64_t timestamp;
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
        ActionTimestamp actionTimestamp;
        Action action;

        action.RuleId = rule.id;
        uint64_t currentTime = localTimestamp();

        if (rule.activeState == ActiveState::DISABLED)
        {
            return;
        }

        for (auto time : rule.timeInDayConditions)
        {
            cout << time << endl;
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
                    time,
                    devAction.delayTime * 60);
                if (timestamp == 0)
                {
                    continue;
                }

                action.DeviceId = devAction.deviceId;
                action.ResourceName = devAction.resourceName;
                action.Value = devAction.value;
                action.ActionRuleId = "";

                actionTimestamp.action = action;
                actionTimestamp.timestamp = timestamp + 2;
                cout << "\033[0;38mLIST RULE :: " << action.RuleId << " ------ :: ------ START TIME (UNIX) -> :: :: " << actionTimestamp.timestamp << endl;
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
                    time,
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
        // if (TriggerState::OFF)
        // {
        //     this->ActionTimestampList::removeByRuleId(id);
        //     return;
        // }
        cout << "---------------> Rule >> " << id << endl;
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
        // cout << "RuleEngine::addOrRemoveActionTimestampListFromActiveRule :: " << id << "  state -> " << state << endl;
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

        if (rule.type == RuleType::SCHEDULE)
        {
            uint64_t currentTime = localTimestamp();
            uint64_t startDayTimestamp = (currentTime / 86400) * 86400;
            cout << "currentTime  " << currentTime << " startDayTimestamp " << startDayTimestamp << endl;
            cout << rule.startInDayTime << endl;
            cout << startDayTimestamp + rule.startInDayTime << endl;
            cout << currentTime << "   " << startDayTimestamp + rule.startInDayTime + 60 << endl;
            if (currentTime < startDayTimestamp + rule.startInDayTime || currentTime > startDayTimestamp + rule.startInDayTime + 60)
            {
                return;
            }
        }

        ActionTimestamp actionTimestamp;
        Action action;

        action.RuleId = rule.id;
        uint64_t currentTime = localTimestamp();

        for (auto time : rule.timeInDayConditions)
        {
            cout << time << endl;
            for (auto devAction : rule.deviceActions)
            {
                // uint64_t timestamp = calTimestamp(
                //     true,
                //     currentTime,
                //     rule.startTime,
                //     rule.endTime,
                //     rule.startInDayTime,
                //     rule.endInDayTime,
                //     rule.repeatDays,
                //     time,
                //     devAction.delayTime);

                // cout << "timestamp :: " << timestamp << endl;

                // if (timestamp == 0)
                // {
                //     continue;
                // }

                // action.DeviceId = devAction.deviceId;
                // action.ResourceName = devAction.resourceName;
                // action.Value = devAction.value;
                // action.ActionRuleId = "";

                // actionTimestamp.action = action;
                // actionTimestamp.timestamp = timestamp;
                // this->ActionTimestampList::push_back(actionTimestamp);
                action.DeviceId = devAction.deviceId;
                action.ResourceName = devAction.resourceName;
                action.Value = devAction.value;
                action.ActionRuleId = "";

                actionTimestamp.action = action;
                actionTimestamp.timestamp = currentTime + devAction.delayTime * 60;
                this->ActionTimestampList::push_back(actionTimestamp);
                // usleep(50000);
            }
            for (auto ruleAction : rule.ruleActions)
            {
                // uint64_t timestamp = calTimestamp(
                //     true,
                //     currentTime,
                //     rule.startTime,
                //     rule.endTime,
                //     rule.startInDayTime,
                //     rule.endInDayTime,
                //     rule.repeatDays,
                //     time,
                //     ruleAction.delayTime);
                // if (timestamp == 0)
                // {
                //     continue;
                // }

                // action.DeviceId = "";
                // action.ResourceName = "";
                // action.Value = "";
                // action.ActionRuleId = ruleAction.actionRuleId;

                // actionTimestamp.action = action;
                // actionTimestamp.timestamp = timestamp;
                // this->ActionTimestampList::push_back(actionTimestamp);
                action.DeviceId = "";
                action.ResourceName = "";
                action.Value = "";
                action.ActionRuleId = ruleAction.actionRuleId;

                actionTimestamp.action = action;
                actionTimestamp.timestamp = currentTime + ruleAction.delayTime * 60;
                this->ActionTimestampList::push_back(actionTimestamp);
                // usleep(50000);
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

            // TODO: handle error
            err = this->commandClient.issueSetCommand(rqi, action.DeviceId, resources, options);
            // usleep(100000);
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
        // cout << "setStateCallback :: " << id << "  state: " << state << endl;
        this->addOrRemoveActionTimestampListFromActiveRule(id, state, err);
    }

    void RuleEngine::setTriggerCallback(string id, TriggerState state, string &err)
    {
        cout << "setTriggerCallback :: " << id << " state: " << state << endl;
        this->addOrRemoveActionTimestampListFromTriggerRule(id, state, err);
    }

    void RuleEngine::readingCallback(vector<model::Reading> readings, string &err)
    {
        // cout << "readings.size() :: " << readings.size() << endl;
        cout << "Reading Callback" << endl;
        for (auto &reading : readings)
        {
            // cout << "\033[0;32mReading:: \t\t\t \033[0;35mName: \t\t\t" << reading.resourceName << "\t\t\t Value : " << reading.value << "\033[0;36m" << endl;
            if (reading.resourceName == "OnOff" || reading.resourceName == "Battery")
            {
                return;
            }
            bool value_valid_min = false;
            bool value_valid_max = false;
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
                                // cout << "deviceCondition.compareOperator :: " << deviceCondition.compareOperator << endl;
                                if (deviceCondition.compareOperator == CompareOperator::GREATER)
                                {
                                    if (stoi(reading.value) >= stoi(deviceCondition.value))
                                    {
                                        value_valid_min = true;
                                    }
                                }
                                else if (deviceCondition.compareOperator == CompareOperator::LESS)
                                {
                                    if (stoi(reading.value) <= stoi(deviceCondition.value))
                                    {
                                        value_valid_max = true;
                                    }
                                }
                            }
                        }
                        if (value_valid_min && value_valid_max)
                        {
                            if (rule.conditionLogic == RuleLogic::AND)
                            {
                                bool value_valid_min_logic = true;
                                bool value_valid_max_logic = true;
                                for (auto deviceCondition : deviceConditions)
                                {
                                    auto resources = Db::getDb()->listReading(deviceCondition.deviceId);
                                    for (auto resource : resources)
                                    {
                                        if (resource.deviceId != deviceCondition.deviceId || (reading.deviceId == deviceCondition.deviceId && reading.resourceName == resource.resourceName) || resource.resourceName == "Battery")
                                        {
                                            continue;
                                        }
                                        // cout << "--------------------------------------------------------------------------------------------------------------------------------" << endl;
                                        // cout << "resource.deviceId :: " << resource.deviceId << " :: deviceCondition.deviceId :: " << deviceCondition.deviceId << endl;
                                        // cout << "deviceCondition.resourceName :: " << deviceCondition.resourceName << " :: deviceCondition.value :: " << deviceCondition.value << endl;
                                        // cout << "resource.resourceName :: " << resource.resourceName << " :: resource.value :: " << resource.value << endl;
                                        // cout << "deviceCondition.compareOperator :: " << deviceCondition.compareOperator << endl;
                                        if (deviceCondition.compareOperator == CompareOperator ::GREATER)
                                        {
                                            // cout << "stoi(resource.value)" << stoi(resource.value) << endl;
                                            // cout << "stoi(deviceCondition.value)" << stoi(deviceCondition.value) << endl;
                                            if (stoi(resource.value) < stoi(deviceCondition.value))
                                            {
                                                value_valid_min_logic = false;
                                            }
                                        }
                                        else if (deviceCondition.compareOperator == CompareOperator ::LESS)
                                        {
                                            if (stoi(resource.value) > stoi(deviceCondition.value))
                                            {
                                                value_valid_max_logic = false;
                                            }
                                        }
                                    }
                                }
                                // cout << endl;
                                // cout << "value_valid_min_logic && value_valid_max_logic   " << value_valid_min_logic << "  &&  " << value_valid_max_logic << endl;
                                // cout << endl;
                                if (value_valid_min_logic && value_valid_max_logic)
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

                                            actionTimestamp.action = action;
                                            actionTimestamp.timestamp = currentTime + ruleAction.delayTime * 60 + 2;
                                            this->ActionTimestampList::push_back(actionTimestamp);
                                        }
                                    }
                                }
                            }
                            else
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

                                        actionTimestamp.action = action;
                                        actionTimestamp.timestamp = currentTime + ruleAction.delayTime * 60 + 2;
                                        this->ActionTimestampList::push_back(actionTimestamp);
                                    }
                                }
                            }
                        }
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
                cout << "\033[0;32m actionTimestamp.timestamp :: " << actionTimestamp.timestamp << endl;
                // TODO : check
                if (actionTimestamp.timestamp <= currentTime)
                {
                    this->ActionTimestampList::pop_back();
                    this->ActionQueue::push(actionTimestamp.action);
                    cout << endl
                         << "\033[1;34m GET FROM QUEUE :: " << actionTimestamp.timestamp << endl;
                }
                else
                {
                    break;
                }

                usleep(5000);
            }
            // cout << "                                                      "
            //      << "\033[0;38m Time:: " << currentTime << endl;
            sleep(1);
            // usleep(500000);
        }
    }

    void RuleEngine::executeActionProcess()
    {
        Action action;
        while (true)
        {
            if (this->ActionQueue::pop(action))
            {
                cout << endl
                     << "\033[0;37m EXECUTE ACTION :: " << action.RuleId << endl;
                this->executeAction(action);
            }

            // usleep(500000);
            sleep(1);
        }
    }
}

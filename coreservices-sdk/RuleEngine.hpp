#pragma once

#include "../clients/Command.hpp"

#include <algorithm>
#include <queue>
#include <mutex>

namespace coreservices
{
    struct Action
    {
        string RuleId;
        string DeviceId;
        string ActionRuleId;
        string ResourceName;
        string Value;
    };

    struct Reading
    {
        string deviceId;
        string resourceName;
        string value;
        uint64_t timestamp;
        string valueType;
        string unit;
    };

    struct ActionTimestamp
    {
        Action action;
        uint64_t timestamp;
    };

    class ActionTimestampList
    {
    private:
        mutex mutex_;
        vector<ActionTimestamp> actions;

        static bool compareTime(ActionTimestamp action1, ActionTimestamp action2);

    public:
        ActionTimestampList() {}
        void push_back(ActionTimestamp action);
        bool back(ActionTimestamp &actionTimestamp);
        void pop_back();
        void removeByRuleId(string id);
    };

    class ActionQueue
    {
    private:
        mutex mutex_;
        queue<Action> queue_;

    public:
        ActionQueue() {}
        void push(Action action);
        bool pop(Action &action);
    };

    class RuleEngine : public ActionTimestampList, public ActionQueue
    {
    private:
        Command commandClient;

        void refreshActionTimestampList(bool checkCurrentTime);
        void addActionTimestampListFromRule(model::Rule rule, bool checkCurrentTime);
        void addOrRemoveActionTimestampListFromTriggerRule(string id, TriggerState state, string &err);
        void addOrRemoveActionTimestampListFromActiveRule(string id, ActiveState state, string &err);

        void executeAction(Action action);

        void scheduleProcess();
        void executeActionProcess();

    public:
        RuleEngine(){};
        void init(Command commandClient);
        void addCallback(model::Rule rule, string &err);
        void updateCallback(model::Rule rule, string &err);
        void deleteCallback(string id, string &err);
        void setStateCallback(string id, ActiveState state, string &err);
        void setTriggerCallback(string id, TriggerState state, string &err);
        void readingCallback(vector<model::Reading> readings, string &err);
        void checkRule(string ruleId);
    };
}

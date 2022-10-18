#pragma once

#include <iostream>
#include <vector>
#include <map>
#include <stdint.h>

#include "../../models/models.hpp"

using namespace std;

namespace entity
{
    struct DeviceResource
    {
        string name;
        int valueType;
        int readWrite;
        string unit;
        string minimum;
        string maximum;
    };

    struct ResourceInProfile
    {
        string profileName;
        string resourceName;
        string attributes;
    };

    struct DeviceProfile
    {
        string name;
        string model;
    };

    struct DeviceService
    {
        string name;
        string address;
    };

    struct DeviceInGroup
    {
        string groupId;
        string deviceId;
        string state;
    };

    struct Device
    {
        string id;
        string name;
        int type;
        uint64_t created;
        uint64_t modified;
        bool adminState;
        bool operatingState;
        uint64_t lastConnected;
        string profileName;
        string serviceName;
        string protocols;
    };

    struct ResourceValue
    {
        string deviceId;
        string resourceName;
        string value;
        uint64_t timestamp;
    };

    struct DeviceCondition
    {
        string id;
        string ruleId;
        string deviceId;
        string resourceName;
        int compareOperator;
        string value;
    };

    struct RuleCondition
    {
        string ruleId;
        string conditionRuleId;
        int compareOperator;
    };

    struct DeviceAction
    {
        string ruleId;
        string deviceId;
        string resourceName;
        uint64_t delayTime;
        string value;
    };

    struct RuleAction
    {
        string ruleId;
        string actionRuleId;
        uint64_t delayTime;
    };

    struct Rule
    {
        string id;
        string name;
        int type;
        uint64_t priority;
        bool activeState;
        bool triggerState;
        uint64_t startTime;
        uint64_t endTime;
        uint32_t startInDayTime;
        uint32_t endInDayTime;
        string repeatDays;
        int conditionLogic;

        string timeInDayConditions;
        bool enableNotify;
        int notifyLevel;
        string notifyContent;
    };

    struct Notification
    {
        string id;
        uint64_t timestamp;
        int level;
        string content;
    };

    string mapToString(map<string, string> mapData);
    map<string, string> mapFromString(string str);
    string arrayToString(vector<string> arrData);
    vector<string> arrayFromString(string str);

    DeviceResource fromModel(model::DeviceResource md);
    model::DeviceResource toModel(DeviceResource et);
    ResourceInProfile fromModel(string profileName, model::ResourceInProfile md);
    model::ResourceInProfile toModel(ResourceInProfile et);
    DeviceProfile fromModel(model::DeviceProfile md);
    model::DeviceProfile toModel(DeviceProfile et);
    DeviceService fromModel(model::DeviceService md);
    model::DeviceService toModel(DeviceService et);
    DeviceInGroup fromModel(string groupId, model::DeviceInGroup md);
    model::DeviceInGroup toModel(DeviceInGroup et);
    Device fromModel(model::Device md);
    model::Device toModel(Device et);
    ResourceValue fromModel(model::Reading md);
    model::Reading toModel(ResourceValue et);
    DeviceCondition fromModel(string id, string ruleId, model::DeviceCondition md);
    model::DeviceCondition toModel(DeviceCondition et);
    RuleCondition fromModel(string ruleId, model::RuleCondition md);
    model::RuleCondition toModel(RuleCondition et);
    DeviceAction fromModel(string ruleId, model::DeviceAction md);
    model::DeviceAction toModel(DeviceAction et);
    RuleAction fromModel(string ruleId, model::RuleAction md);
    model::RuleAction toModel(RuleAction et);
    Rule fromModel(model::Rule md);
    model::Rule toModel(Rule et);
    Notification fromModel(string id, model::Notification md);
    model::Notification toModel(Notification et);
}
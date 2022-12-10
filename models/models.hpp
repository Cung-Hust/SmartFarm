#pragma once

#include <string>
#include <stdint.h>
#include <vector>
#include <map>

#include "constants.hpp"

using namespace std;

namespace model
{
    struct Reading
    {
        string deviceId;
        string resourceName;
        string value;
        uint64_t timestamp;
        ValueType valueType;
        string unit;
    };

    struct DeviceResource
    {
        string name;
        ValueType valueType;
        ReadWrite readWrite;
        string unit;
        string minimum;
        string maximum;
    };

    struct ResourceInProfile
    {
        string resourceName;
        map<string, string> attributes;
    };

    struct DeviceProfile
    {
        string name;
        string model;
        vector<ResourceInProfile> resources;
    };

    struct DeviceService
    {
        string name;
        string address;
    };

    struct DeviceInGroup
    {
        string deviceId;
        string state;
    };

    struct Device
    {
        string id;
        string name;
        DeviceType type;
        uint64_t created;
        uint64_t modified;
        AdminState adminState;
        OperatingState operatingState;
        uint64_t lastConnected;
        string profileName;
        string serviceName;
        vector<DeviceInGroup> deviceList;
        map<string, string> protocols;
    };

    struct DeviceCondition
    {
        string deviceId;
        string resourceName;
        CompareOperator compareOperator;
        string value;
    };

    struct RuleCondition
    {
        string conditionRuleId;
        CompareOperator compareOperator;
    };

    struct DeviceAction
    {
        string deviceId;
        uint64_t delayTime;
        string resourceName;
        string value;
    };

    struct RuleAction
    {
        string actionRuleId;
        uint64_t delayTime;
    };

    struct Rule
    {
        string id;
        string name;
        RuleType type;
        uint64_t priority;
        ActiveState activeState;
        TriggerState triggerState;
        uint64_t startTime;
        uint64_t endTime;
        uint32_t startInDayTime;
        uint32_t endInDayTime;
        vector<string> repeatDays;
        RuleLogic conditionLogic;

        vector<uint32_t> timeInDayConditions;
        vector<DeviceCondition> deviceConditions;
        vector<RuleCondition> ruleConditions;

        vector<DeviceAction> deviceActions;
        vector<RuleAction> ruleActions;
        bool enableNotify;
        NotifyLevel notifyLevel;
        string notifyContent;
    };

    struct Notification
    {
        uint64_t timestamp;
        NotifyLevel level;
        string content;
    };

    struct Gateway
    {
        string id;
        string macAddress;
        uint8_t mode;
    };
}
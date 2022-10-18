#include "entity.hpp"

#include "../../json/json.hpp"

using namespace std;
using json = nlohmann::json;

namespace entity
{
    string mapToString(map<string, string> mapData)
    {
        json j_map(mapData);
        return j_map.dump();
    }
    map<string, string> mapFromString(string str)
    {
        json js = json::parse(str);
        auto result = js.get<map<string, string>>();

        return result;
    }

    string arrayToString(vector<string> arrData)
    {
        json j_arr(arrData);
        return j_arr.dump();
    }

    vector<string> arrayFromString(string str)
    {
        json js = json::parse(str);
        auto result = js.get<vector<string>>();

        return result;
    }

    string arrayIntToString(vector<uint32_t> arrData)
    {
        json j_arr(arrData);
        return j_arr.dump();
    }

    vector<uint32_t> arrayIntFromString(string str)
    {
        json js = json::parse(str);
        auto result = js.get<vector<uint32_t>>();

        return result;
    }

    DeviceResource fromModel(model::DeviceResource md)
    {
        DeviceResource et;
        et.name = md.name;
        et.readWrite = (int)md.readWrite;
        et.valueType = (int)md.valueType;
        et.unit = md.unit;
        et.minimum = md.minimum;
        et.maximum = md.maximum;

        return et;
    };

    model::DeviceResource toModel(DeviceResource et)
    {
        model::DeviceResource md;
        md.name = et.name;
        md.readWrite = (ReadWrite)et.readWrite;
        md.valueType = (ValueType)et.valueType;
        md.unit = et.unit;
        md.minimum = et.minimum;
        md.maximum = et.maximum;

        return md;
    };

    ResourceInProfile fromModel(string profileName, model::ResourceInProfile md)
    {
        ResourceInProfile et;
        et.profileName = profileName;
        et.resourceName = md.resourceName;
        et.attributes = mapToString(md.attributes);

        return et;
    };

    model::ResourceInProfile toModel(ResourceInProfile et)
    {
        model::ResourceInProfile md;
        md.resourceName = et.resourceName;
        md.attributes = mapFromString(et.attributes);

        return md;
    };

    DeviceProfile fromModel(model::DeviceProfile md)
    {
        DeviceProfile et;
        et.name = md.name;
        et.model = md.model;

        return et;
    }

    model::DeviceProfile toModel(DeviceProfile et)
    {
        model::DeviceProfile md;
        md.name = et.name;
        md.model = et.model;

        return md;
    }

    DeviceService fromModel(model::DeviceService md)
    {
        DeviceService et;
        et.name = md.name;
        et.address = md.address;

        return et;
    };

    model::DeviceService toModel(DeviceService et)
    {
        model::DeviceService md;
        md.name = et.name;
        md.address = md.address;

        return md;
    };

    DeviceInGroup fromModel(string groupId, model::DeviceInGroup md)
    {
        DeviceInGroup et;
        et.groupId = groupId;
        et.deviceId = md.deviceId;
        et.state = md.state;

        return et;
    };

    model::DeviceInGroup toModel(DeviceInGroup et)
    {
        model::DeviceInGroup md;
        md.deviceId = et.deviceId;
        md.state = et.state;

        return md;
    };

    Device fromModel(model::Device md)
    {
        Device et;
        et.id = md.id;
        et.name = md.name;
        et.type = (int)md.type;
        et.created = md.created;
        et.modified = md.modified;
        et.adminState = (int)md.adminState;
        et.operatingState = (int)md.operatingState;
        et.lastConnected = md.lastConnected;
        et.protocols = mapToString(md.protocols);
        et.serviceName = md.serviceName;
        et.profileName = md.profileName;

        return et;
    };

    model::Device toModel(Device et)
    {
        model::Device md;
        md.id = et.id;
        md.name = et.name;
        md.type = (DeviceType)et.type;
        md.created = et.created;
        md.modified = et.modified;
        md.adminState = (AdminState)et.adminState;
        md.operatingState = (OperatingState)et.operatingState;
        md.lastConnected = et.lastConnected;
        md.protocols = mapFromString(et.protocols);
        md.serviceName = et.serviceName;
        md.profileName = et.profileName;

        return md;
    };

    ResourceValue fromModel(model::Reading md)
    {
        ResourceValue et;
        et.deviceId = md.deviceId;
        et.resourceName = md.resourceName;
        et.value = md.value;
        et.timestamp = md.timestamp;

        return et;
    };

    model::Reading toModel(ResourceValue et)
    {
        model::Reading md;
        md.deviceId = et.deviceId;
        md.resourceName = et.resourceName;
        md.value = et.value;
        md.timestamp = et.timestamp;

        return md;
    };

    DeviceCondition fromModel(string id, string ruleId, model::DeviceCondition md)
    {
        DeviceCondition et;
        et.id = id;
        et.ruleId = ruleId;
        et.deviceId = md.deviceId;
        et.resourceName = md.resourceName;
        et.compareOperator = md.compareOperator;
        et.value = md.value;

        return et;
    };

    model::DeviceCondition toModel(DeviceCondition et)
    {
        model::DeviceCondition md;
        md.deviceId = et.deviceId;
        md.resourceName = et.resourceName;
        md.compareOperator = (CompareOperator)et.compareOperator;
        md.value = et.value;

        return md;
    };

    RuleCondition fromModel(string ruleId, model::RuleCondition md)
    {
        RuleCondition et;
        et.ruleId = ruleId;
        et.conditionRuleId = md.conditionRuleId;
        et.compareOperator = (int)md.compareOperator;

        return et;
    };

    model::RuleCondition toModel(RuleCondition et)
    {
        model::RuleCondition md;

        md.conditionRuleId = et.conditionRuleId;
        md.compareOperator = (CompareOperator)et.compareOperator;

        return md;
    };

    DeviceAction fromModel(string ruleId, model::DeviceAction md)
    {
        DeviceAction et;
        et.ruleId = ruleId;
        et.deviceId = md.deviceId;
        et.resourceName = md.resourceName;
        et.value = md.value;
        et.delayTime = md.delayTime;

        return et;
    };

    model::DeviceAction toModel(DeviceAction et)
    {
        model::DeviceAction md;
        md.deviceId = et.deviceId;
        md.resourceName = et.resourceName;
        md.value = et.value;
        md.delayTime = et.delayTime;

        return md;
    };

    RuleAction fromModel(string ruleId, model::RuleAction md)
    {
        RuleAction et;
        et.ruleId = ruleId;
        et.actionRuleId = md.actionRuleId;
        et.delayTime = md.delayTime;

        return et;
    };

    model::RuleAction toModel(RuleAction et)
    {
        model::RuleAction md;
        md.actionRuleId = et.actionRuleId;
        md.delayTime = et.delayTime;

        return md;
    };

    Rule fromModel(model::Rule md)
    {
        Rule et;
        et.id = md.id;
        et.name = md.name;
        et.type = (int)md.type;
        et.activeState = (int)md.activeState;
        et.triggerState = (int)md.triggerState;
        et.priority = md.priority;
        et.startTime = md.startTime;
        et.endTime = md.endTime;
        et.startInDayTime = md.startInDayTime;
        et.endInDayTime = md.endInDayTime;
        et.repeatDays = arrayToString(md.repeatDays);
        et.conditionLogic = (int)md.conditionLogic;
        et.timeInDayConditions = arrayIntToString(md.timeInDayConditions);
        et.enableNotify = md.enableNotify;
        et.notifyLevel = (int)md.notifyLevel;
        et.notifyContent = md.notifyContent;

        return et;
    };

    model::Rule toModel(Rule et)
    {
        model::Rule md;
        md.id = et.id;
        md.name = et.name;
        md.type = (RuleType)et.type;
        md.activeState = (ActiveState)et.activeState;
        md.triggerState = (TriggerState)et.triggerState;
        md.priority = et.priority;
        md.startTime = et.startTime;
        md.endTime = et.endTime;
        md.startInDayTime = et.startInDayTime;
        md.endInDayTime = et.endInDayTime;
        md.repeatDays = arrayFromString(et.repeatDays);
        md.conditionLogic = (RuleLogic)et.conditionLogic;
        md.timeInDayConditions = arrayIntFromString(et.timeInDayConditions);
        md.enableNotify = et.enableNotify;
        md.notifyLevel = (NotifyLevel)et.notifyLevel;
        md.notifyContent = et.notifyContent;

        return md;
    };

    Notification fromModel(string id, model::Notification md)
    {
        Notification et;
        et.id = id;
        et.level = (int)md.level;
        et.content = md.content;
        et.timestamp = md.timestamp;

        return et;
    };

    model::Notification toModel(Notification et)
    {
        model::Notification md;
        md.level = (NotifyLevel)et.level;
        md.content = et.content;
        md.timestamp = et.timestamp;

        return md;
    };
}
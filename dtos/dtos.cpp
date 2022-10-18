#include "base.hpp"
#include "requestes.hpp"
#include "responses.hpp"

namespace dto
{
    /*
        Define Serialize/Deserialize JSON functions for Request
    */
    void to_json(nlohmann::json &json_j, const HeaderRequest &json_t)
    {
        json_j["client"] = json_t.client;
        json_j["rqi"] = json_t.rqi;
        json_j["op"] = json_t.op;
    }

    void from_json(const nlohmann::json &json_j, HeaderRequest &json_t)
    {
        json_t.client = json_j.value("client", "");
        json_t.rqi = json_j.value("rqi", "");
        json_t.op = json_j.value("op", "");
    }

    void to_json(nlohmann::json &json_j, const HeaderResponse &json_t)
    {
        json_j["client"] = json_t.client;
        json_j["rqi"] = json_t.rqi;
        json_j["success"] = json_t.success;
        json_j["err"] = json_t.error;
    }

    void from_json(const nlohmann::json &json_j, HeaderResponse &json_t)
    {
        json_t.client = json_j.value("client", "");
        json_t.rqi = json_j.value("rqi", "");
        json_t.success = json_j.value("success", false);
        json_t.error = json_j.value("error", "");
    }

    void to_json(nlohmann::json &json_j, const Reading &json_t)
    {
        json_j["deviceid"] = json_t.deviceId;
        json_j["resourcename"] = json_t.resourceName;
        json_j["value"] = json_t.value;
        json_j["timestamp"] = json_t.timestamp;
        json_j["valuetype"] = json_t.valueType;
        json_j["unit"] = json_t.unit;
    }

    void from_json(const nlohmann::json &json_j, Reading &json_t)
    {
        json_t.deviceId = json_j.value("deviceid", "");
        json_t.resourceName = json_j.value("resourcename", "");
        json_t.value = json_j.value("value", "");
        json_t.timestamp = json_j.value("timestamp", 0);
        json_t.valueType = json_j.value("valuetype", "");
        json_t.unit = json_j.value("unit", "");
    }

    void to_json(nlohmann::json &json_j, const DeviceResource &json_t)
    {
        json_j["name"] = json_t.name;
        json_j["valuetype"] = json_t.valueType;
        json_j["readwrite"] = json_t.readWrite;
        json_j["unit"] = json_t.unit;
        json_j["minimum"] = json_t.minimum;
        json_j["maximum"] = json_t.maximum;
    }

    void from_json(const nlohmann::json &json_j, DeviceResource &json_t)
    {
        json_t.name = json_j.value("name", "");
        json_t.valueType = json_j.value("valuetype", "");
        json_t.readWrite = json_j.value("readwrite", "");
        json_t.unit = json_j.value("unit", "");
        json_t.minimum = json_j.value("minimum", "");
        json_t.maximum = json_j.value("maximum", "");
    }

    void to_json(nlohmann::json &json_j, const ResourceInProfile &json_t)
    {
        json_j["resourcename"] = json_t.resourceName;
        json_j["attributes"] = json_t.attributes;
    }

    void from_json(const nlohmann::json &json_j, ResourceInProfile &json_t)
    {
        json_t.resourceName = json_j.value("resourcename", "");
        json_t.attributes = json_j.value("attributes", std::map<std::string, std::string>());
    }

    void to_json(nlohmann::json &json_j, const DeviceProfile &json_t)
    {
        json_j["name"] = json_t.name;
        json_j["model"] = json_t.model;
        json_j["resources"] = json_t.resources;
    }

    void from_json(const nlohmann::json &json_j, DeviceProfile &json_t)
    {
        json_t.name = json_j.value("name", "");
        json_t.model = json_j.value("model", "");
        json_t.resources = json_j.value("resources", std::vector<ResourceInProfile>());
    }

    void to_json(nlohmann::json &json_j, const DeviceInGroup &json_t)
    {
        json_j["device"] = json_t.deviceId;
        json_j["state"] = json_t.state;
    }

    void to_json(nlohmann::json &json_j, const DeviceService &json_t)
    {
        json_j["name"] = json_t.name;
        json_j["address"] = json_t.address;
    }

    void from_json(const nlohmann::json &json_j, DeviceService &json_t)
    {
        json_t.name = json_j.value("name", "");
        json_t.address = json_j.value("address", "");
    }

    void from_json(const nlohmann::json &json_j, DeviceInGroup &json_t)
    {
        json_t.deviceId = json_j.value("device", "");
        json_t.state = json_j.value("state", "");
    }

    void to_json(nlohmann::json &json_j, const Device &json_t)
    {
        json_j["id"] = json_t.id;
        json_j["name"] = json_t.name;
        json_j["type"] = json_t.type;
        json_j["created"] = json_t.created;
        json_j["modified"] = json_t.modified;
        json_j["adminstate"] = json_t.adminState;
        json_j["operatingstate"] = json_t.operatingState;
        json_j["lastconnected"] = json_t.lastConnected;
        json_j["profilename"] = json_t.profileName;
        json_j["servicename"] = json_t.serviceName;
        json_j["devicelist"] = json_t.deviceList;
        json_j["protocols"] = json_t.protocols;
    }

    void from_json(const nlohmann::json &json_j, Device &json_t)
    {
        json_t.id = json_j.value("id", "");
        json_t.name = json_j.value("name", "");
        json_t.type = json_j.value("type", "");
        json_t.created = json_j.value("created", 0);
        json_t.modified = json_j.value("modified", 0);
        json_t.adminState = json_j.value("adminstate", "");
        json_t.operatingState = json_j.value("operatingstate", "");
        json_t.lastConnected = json_j.value("lastconnected", 0);
        json_t.profileName = json_j.value("profilename", "");
        json_t.serviceName = json_j.value("servicename", "");
        json_t.deviceList = json_j.value("devicelist", std::vector<DeviceInGroup>());
        json_t.protocols = json_j.value("protocols", std::map<std::string, std::string>());
    }

    void to_json(nlohmann::json &json_j, const UpdateDevice &json_t)
    {
        json_j["id"] = json_t.id;
        if (json_t.name != NULL)
        {
            json_j["name"] = *json_t.name;
        }
        if (json_t.type != NULL)
        {
            json_j["type"] = *json_t.type;
        }
        if (json_t.created != NULL)
        {
            json_j["created"] = *json_t.created;
        }
        if (json_t.modified != NULL)
        {
            json_j["modified"] = *json_t.modified;
        }
        if (json_t.adminState != NULL)
        {
            json_j["adminstate"] = *json_t.adminState;
        }
        if (json_t.operatingState != NULL)
        {
            json_j["operatingstate"] = *json_t.operatingState;
        }
        if (json_t.lastConnected != NULL)
        {
            json_j["lastconnected"] = *json_t.lastConnected;
        }
        if (json_t.profileName != NULL)
        {
            json_j["profilename"] = *json_t.profileName;
        }
        if (json_t.serviceName != NULL)
        {
            json_j["servicename"] = *json_t.serviceName;
        }
        if (json_t.deviceList != NULL)
        {
            json_j["devicelist"] = *json_t.deviceList;
        }
        if (json_t.protocols != NULL)
        {
            json_j["protocols"] = *json_t.protocols;
        }
    }

    void from_json(const nlohmann::json &json_j, UpdateDevice &json_t)
    {
        json_t.id = json_j.value("id", "");
        if (json_j.contains("name"))
        {
            std::string value = json_j.at("name");
            json_t.name = new std::string(value);
        }
        if (json_j.contains("type"))
        {
            std::string value = json_j.at("type");
            json_t.type = new std::string(value);
        }
        if (json_j.contains("created"))
        {
            uint64_t value = json_j.at("created");
            json_t.created = new uint64_t(value);
        }
        if (json_j.contains("modified"))
        {
            uint64_t value = json_j.at("modified");
            json_t.created = new uint64_t(value);
        }
        if (json_j.contains("adminstate"))
        {
            std::string value = json_j.at("adminstate");
            json_t.adminState = new std::string(value);
        }
        if (json_j.contains("operatingstate"))
        {
            std::string value = json_j.at("operatingstate");
            json_t.operatingState = new std::string(value);
        }
        if (json_j.contains("lastconnected"))
        {
            uint64_t value = json_j.at("lastconnected");
            json_t.lastConnected = new uint64_t(value);
        }
        if (json_j.contains("profilename"))
        {
            std::string value = json_j.at("profilename");
            json_t.profileName = new std::string(value);
        }
        if (json_j.contains("servicename"))
        {
            std::string value = json_j.at("servicename");
            json_t.serviceName = new std::string(value);
        }
        if (json_j.contains("devicelist"))
        {
            std::vector<DeviceInGroup> value = json_j.at("devicelist");
            json_t.deviceList = new std::vector<DeviceInGroup>(value);
        }
        if (json_j.contains("protocols"))
        {
            std::map<std::string, std::string> value = json_j.at("protocols");
            json_t.protocols = new std::map<std::string, std::string>(value);
        }
    }

    void to_json(nlohmann::json &json_j, const DeviceCondition &json_t)
    {
        json_j["deviceid"] = json_t.deviceId;
        json_j["resourcename"] = json_t.resourceName;
        json_j["compareoperator"] = json_t.compareOperator;
        json_j["value"] = json_t.value;
    }

    void from_json(const nlohmann::json &json_j, DeviceCondition &json_t)
    {
        json_t.deviceId = json_j.value("deviceid", "");
        json_t.resourceName = json_j.value("resourcename", "");
        json_t.compareOperator = json_j.value("compareoperator", "");
        json_t.value = json_j.value("value", "");
    }

    void to_json(nlohmann::json &json_j, const RuleCondition &json_t)
    {
        json_j["conditionruleid"] = json_t.conditionRuleId;
        json_j["compareoperator"] = json_t.compareOperator;
    }

    void from_json(const nlohmann::json &json_j, RuleCondition &json_t)
    {
        json_t.conditionRuleId = json_j.value("conditionruleid", "");
        json_t.compareOperator = json_j.value("compareoperator", "");
    }

    void to_json(nlohmann::json &json_j, const DeviceAction &json_t)
    {
        json_j["deviceid"] = json_t.deviceId;
        json_j["delaytime"] = json_t.delayTime;
        json_j["resourcename"] = json_t.resourceName;
        json_j["value"] = json_t.value;
    }

    void from_json(const nlohmann::json &json_j, DeviceAction &json_t)
    {
        json_t.deviceId = json_j.value("deviceid", "");
        json_t.delayTime = json_j.value("delaytime", 0);
        json_t.resourceName = json_j.value("resourcename", "");
        json_t.value = json_j.value("value", "");
    }

    void to_json(nlohmann::json &json_j, const RuleAction &json_t)
    {
        json_j["actionruleid"] = json_t.actionRuleId;
        json_j["delaytime"] = json_t.delayTime;
    }

    void from_json(const nlohmann::json &json_j, RuleAction &json_t)
    {
        json_t.actionRuleId = json_j.value("actionruleid", "");
        json_t.delayTime = json_j.value("delaytime", 0);
    }

    void to_json(nlohmann::json &json_j, const Rule &json_t)
    {
        json_j["id"] = json_t.id;
        json_j["name"] = json_t.name;
        json_j["type"] = json_t.type;
        json_j["priority"] = json_t.priority;
        json_j["activestate"] = json_t.activeState;
        json_j["triggerstate"] = json_t.triggerState;
        json_j["starttime"] = json_t.startTime;
        json_j["endtime"] = json_t.endTime;
        json_j["startindaytime"] = json_t.startInDayTime;
        json_j["endindaytime"] = json_t.endInDayTime;
        json_j["repeatdays"] = json_t.repeatDays;
        json_j["conditionlogic"] = json_t.conditionLogic;
        json_j["timeindayconditions"] = json_t.timeInDayConditions;
        json_j["deviceconditions"] = json_t.deviceConditions;
        json_j["ruleconditions"] = json_t.ruleConditions;
        json_j["deviceactions"] = json_t.deviceActions;
        json_j["ruleactions"] = json_t.ruleActions;
        json_j["enablenotify"] = json_t.enableNotify;
        json_j["notifylevel"] = json_t.notifyLevel;
        json_j["notifycontent"] = json_t.notifyContent;
    }

    void from_json(const nlohmann::json &json_j, Rule &json_t)
    {
        json_t.id = json_j.value("id", "");
        json_t.name = json_j.value("name", "");
        json_t.type = json_j.value("type", "");
        json_t.priority = json_j.value("priority", 0);
        json_t.activeState = json_j.value("activestate", "");
        json_t.triggerState = json_j.value("triggerstate", "");
        json_t.startTime = json_j.value("starttime", 0);
        json_t.endTime = json_j.value("endtime", 0);
        json_t.startInDayTime = json_j.value("startindaytime", 0);
        json_t.endInDayTime = json_j.value("endindaytime", 0);
        json_t.repeatDays = json_j.value("repeatdays", std::vector<std::string>());
        json_t.conditionLogic = json_j.value("conditionlogic", "");
        json_t.timeInDayConditions = json_j.value("timeindayconditions", std::vector<uint32_t>());
        json_t.deviceConditions = json_j.value("deviceconditions", std::vector<DeviceCondition>());
        json_t.ruleConditions = json_j.value("ruleconditions", std::vector<RuleCondition>());
        json_t.deviceActions = json_j.value("deviceactions", std::vector<DeviceAction>());
        json_t.ruleActions = json_j.value("ruleactions", std::vector<RuleAction>());
        json_t.enableNotify = json_j.value("enablenotify", false);
        json_t.notifyLevel = json_j.value("notifylevel", "");
        json_t.notifyContent = json_j.value("notifycontent", "");
    }

    void to_json(nlohmann::json &json_j, const UpdateRule &json_t)
    {
        json_j["id"] = json_t.id;
        if (json_t.name != NULL)
        {
            json_j["name"] = *json_t.name;
        }
        if (json_t.type != NULL)
        {
            json_j["type"] = *json_t.type;
        }
        if (json_t.priority != NULL)
        {
            json_j["priority"] = *json_t.priority;
        }
        if (json_t.activeState != NULL)
        {
            json_j["activestate"] = *json_t.activeState;
        }
        if (json_t.triggerState != NULL)
        {
            json_j["triggerstate"] = *json_t.triggerState;
        }
        if (json_t.startTime != NULL)
        {
            json_j["starttime"] = *json_t.startTime;
        }
        if (json_t.endTime != NULL)
        {
            json_j["endtime"] = *json_t.endTime;
        }
        if (json_t.startInDayTime != NULL)
        {
            json_j["startindaytime"] = *json_t.startInDayTime;
        }
        if (json_t.endInDayTime != NULL)
        {
            json_j["endindaytime"] = *json_t.endInDayTime;
        }
        if (json_t.repeatDays != NULL)
        {
            json_j["repeatdays"] = *json_t.repeatDays;
        }
        if (json_t.conditionLogic != NULL)
        {
            json_j["conditionlogic"] = *json_t.conditionLogic;
        }
        if (json_t.timeInDayConditions != NULL)
        {
            json_j["timeindayconditions"] = *json_t.timeInDayConditions;
        }
        if (json_t.deviceConditions != NULL)
        {
            json_j["deviceconditions"] = *json_t.deviceConditions;
        }
        if (json_t.ruleConditions != NULL)
        {
            json_j["ruleconditions"] = *json_t.ruleConditions;
        }
        if (json_t.deviceActions != NULL)
        {
            json_j["deviceactions"] = *json_t.deviceActions;
        }
        if (json_t.ruleActions != NULL)
        {
            json_j["ruleactions"] = *json_t.ruleActions;
        }
        if (json_t.enableNotify != NULL)
        {
            json_j["enablenotify"] = *json_t.enableNotify;
        }
        if (json_t.notifyLevel != NULL)
        {
            json_j["notifylevel"] = *json_t.notifyLevel;
        }
        if (json_t.notifyContent != NULL)
        {
            json_j["notifycontent"] = *json_t.notifyContent;
        }
    }

    void from_json(const nlohmann::json &json_j, UpdateRule &json_t)
    {
        json_t.id = json_j.value("id", "");
        if (json_j.contains("name"))
        {
            std::string value = json_j.at("name");
            json_t.name = new std::string(value);
        }
        if (json_j.contains("type"))
        {
            std::string value = json_j.at("type");
            json_t.type = new std::string(value);
        }
        if (json_j.contains("priority"))
        {
            uint64_t value = json_j.at("priority");
            json_t.priority = new uint64_t(value);
        }
        if (json_j.contains("activestate"))
        {
            std::string value = json_j.at("activestate");
            json_t.activeState = new std::string(value);
        }
        if (json_j.contains("triggerstate"))
        {
            std::string value = json_j.at("triggerstate");
            json_t.triggerState = new std::string(value);
        }
        if (json_j.contains("starttime"))
        {
            uint64_t value = json_j.at("starttime");
            json_t.startTime = new uint64_t(value);
        }
        if (json_j.contains("endtime"))
        {
            uint64_t value = json_j.at("endtime");
            json_t.endTime = new uint64_t(value);
        }
        if (json_j.contains("startindaytime"))
        {
            uint32_t value = json_j.at("startindaytime");
            json_t.startInDayTime = new uint32_t(value);
        }
        if (json_j.contains("endindaytime"))
        {
            uint32_t value = json_j.at("endindaytime");
            json_t.endInDayTime = new uint32_t(value);
        }
        if (json_j.contains("repeatdays"))
        {
            std::vector<std::string> value = json_j.at("repeatdays");
            json_t.repeatDays = new std::vector<std::string>(value);
        }
        if (json_j.contains("conditionlogic"))
        {
            std::string value = json_j.at("conditionlogic");
            json_t.conditionLogic = new std::string(value);
        }
        if (json_j.contains("timeindayconditions"))
        {
            std::vector<uint32_t> value = json_j.at("timeindayconditions");
            json_t.timeInDayConditions = new std::vector<uint32_t>(value);
        }
        if (json_j.contains("deviceconditions"))
        {
            std::vector<DeviceCondition> value = json_j.at("deviceconditions");
            json_t.deviceConditions = new std::vector<DeviceCondition>(value);
        }
        if (json_j.contains("ruleconditions"))
        {
            std::vector<RuleCondition> value = json_j.at("ruleconditions");
            json_t.ruleConditions = new std::vector<RuleCondition>(value);
        }
        if (json_j.contains("deviceactions"))
        {
            std::vector<DeviceAction> value = json_j.at("deviceactions");
            json_t.deviceActions = new std::vector<DeviceAction>(value);
        }
        if (json_j.contains("ruleactions"))
        {
            std::vector<RuleAction> value = json_j.at("ruleactions");
            json_t.ruleActions = new std::vector<RuleAction>(value);
        }
        if (json_j.contains("enablenotify"))
        {
            bool value = json_j.at("enablenotify");
            json_t.enableNotify = new bool(value);
        }
        if (json_j.contains("notifylevel"))
        {
            std::string value = json_j.at("notifylevel");
            json_t.notifyLevel = new std::string(value);
        }
        if (json_j.contains("notifycontent"))
        {
            std::string value = json_j.at("notifycontent");
            json_t.notifyContent = new std::string(value);
        }
    }

    void to_json(nlohmann::json &json_j, const Notification &json_t)
    {
        json_j["timestamp"] = json_t.timestamp;
        json_j["level"] = json_t.level;
        json_j["content"] = json_t.content;
    }

    void from_json(const nlohmann::json &json_j, Notification &json_t)
    {
        json_t.timestamp = json_j.value("timestamp", 0);
        json_t.level = json_j.value("level", 0);
        json_t.content = json_j.value("content", "");
    }

    void to_json(nlohmann::json &json_j, const TestRequest &json_t)
    {
        json_j["req"] = json_t.req;
        json_j["body"] = json_t.body;
    }

    void from_json(const nlohmann::json &json_j, TestRequest &json_t)
    {
        json_t.req = json_j.value("req", std::vector<HeaderRequest>());
        json_t.body = json_j.value("body", "");
    }

    void to_json(nlohmann::json &json_j, const SimpleRequest &json_t)
    {
        json_j["header"] = json_t.header;
    }

    void from_json(const nlohmann::json &json_j, SimpleRequest &json_t)
    {
        json_t.header = json_j.value("header", HeaderRequest{});
    }

    void to_json(nlohmann::json &json_j, const RequestWithId &json_t)
    {
        json_j["header"] = json_t.header;
        json_j["id"] = json_t.id;
    }

    void from_json(const nlohmann::json &json_j, RequestWithId &json_t)
    {
        json_t.header = json_j.value("header", HeaderRequest{});
        json_t.id = json_j.value("id", "");
    }

    void to_json(nlohmann::json &json_j, const RequestWithName &json_t)
    {
        json_j["header"] = json_t.header;
        json_j["name"] = json_t.name;
    }

    void from_json(const nlohmann::json &json_j, RequestWithName &json_t)
    {
        json_t.header = json_j.value("header", HeaderRequest{});
        json_t.name = json_j.value("name", "");
    }

    void to_json(nlohmann::json &json_j, const RequestWithOptions &json_t)
    {
        json_j["header"] = json_t.header;
        json_j["options"] = json_t.options;
    }

    void from_json(const nlohmann::json &json_j, RequestWithOptions &json_t)
    {
        json_t.header = json_j.value("header", HeaderRequest{});
        json_t.options = json_j.value("options", std::map<std::string, std::string>());
    }

    void to_json(nlohmann::json &json_j, const RequestWithOperations &json_t)
    {
        json_j["header"] = json_t.header;
        json_j["operations"] = json_t.operations;
    }

    void from_json(const nlohmann::json &json_j, RequestWithOperations &json_t)
    {
        json_t.header = json_j.value("header", HeaderRequest{});
        json_t.operations = json_j.value("operations", std::map<std::string, std::string>());
    }

    void to_json(nlohmann::json &json_j, const AddReadingsRequest &json_t)
    {
        json_j["header"] = json_t.header;
        json_j["readings"] = json_t.readings;
    }

    void from_json(const nlohmann::json &json_j, AddReadingsRequest &json_t)
    {
        json_t.header = json_j.value("header", HeaderRequest{});
        json_t.readings = json_j.value("readings", std::vector<Reading>());
    }

    void to_json(nlohmann::json &json_j, const AddResourceRequest &json_t)
    {
        json_j["header"] = json_t.header;
        json_j["resources"] = json_t.resources;
    }

    void from_json(const nlohmann::json &json_j, AddResourceRequest &json_t)
    {
        json_t.header = json_j.value("header", HeaderRequest{});
        json_t.resources = json_j.value("resources", std::vector<DeviceResource>());
    }

    void to_json(nlohmann::json &json_j, const AddDeviceRequest &json_t)
    {
        json_j["header"] = json_t.header;
        json_j["device"] = json_t.device;
        json_j["notify"] = json_t.notify;
    }

    void from_json(const nlohmann::json &json_j, AddDeviceRequest &json_t)
    {
        json_t.header = json_j.value("header", HeaderRequest{});
        json_t.device = json_j.value("device", Device{});
        json_t.notify = json_j.value("notify", false);
    }

    void to_json(nlohmann::json &json_j, const UpdateDeviceRequest &json_t)
    {
        json_j["header"] = json_t.header;
        json_j["device"] = json_t.device;
        json_j["notify"] = json_t.notify;
    }

    void from_json(const nlohmann::json &json_j, UpdateDeviceRequest &json_t)
    {
        json_t.header = json_j.value("header", HeaderRequest{});
        json_t.device = json_j.value("device", UpdateDevice{});
        json_t.notify = json_j.value("notify", false);
    }

    void to_json(nlohmann::json &json_j, const DeleteDeviceRequest &json_t)
    {
        json_j["header"] = json_t.header;
        json_j["id"] = json_t.id;
        json_j["notify"] = json_t.notify;
    }

    void from_json(const nlohmann::json &json_j, DeleteDeviceRequest &json_t)
    {
        json_t.header = json_j.value("header", HeaderRequest{});
        json_t.id = json_j.value("id", "");
        json_t.notify = json_j.value("notify", false);
    }

    void to_json(nlohmann::json &json_j, const SetAdminStateDeviceRequest &json_t)
    {
        json_j["header"] = json_t.header;
        json_j["id"] = json_t.id;
        json_j["adminstate"] = enumToString(json_t.adminState);
    }

    void from_json(const nlohmann::json &json_j, SetAdminStateDeviceRequest &json_t)
    {
        json_t.header = json_j.value("header", HeaderRequest{});
        json_t.id = json_j.value("id", "");
        std::string str = json_j.value("adminstate", "");
        enumFormString(str, json_t.adminState);
    }

    void to_json(nlohmann::json &json_j, const AddProfileRequest &json_t)
    {
        json_j["header"] = json_t.header;
        json_j["profile"] = json_t.profile;
    }

    void from_json(const nlohmann::json &json_j, AddProfileRequest &json_t)
    {
        json_t.header = json_j.value("header", HeaderRequest{});
        json_t.profile = json_j.value("profile", DeviceProfile{});
    }

    void to_json(nlohmann::json &json_j, const AddServiceRequest &json_t)
    {
        json_j["header"] = json_t.header;
        json_j["service"] = json_t.service;
    }

    void from_json(const nlohmann::json &json_j, AddServiceRequest &json_t)
    {
        json_t.header = json_j.value("header", HeaderRequest{});
        json_t.service = json_j.value("service", DeviceService{});
    }

    void to_json(nlohmann::json &json_j, const CallbackDeviceRequest &json_t)
    {
        json_j["header"] = json_t.header;
        json_j["device"] = json_t.device;
    }

    void from_json(const nlohmann::json &json_j, CallbackDeviceRequest &json_t)
    {
        json_t.header = json_j.value("header", HeaderRequest{});
        json_t.device = json_j.value("device", Device{});
    }

    void to_json(nlohmann::json &json_j, const AddRuleRequest &json_t)
    {
        json_j["header"] = json_t.header;
        json_j["rule"] = json_t.rule;
    }

    void from_json(const nlohmann::json &json_j, AddRuleRequest &json_t)
    {
        json_t.header = json_j.value("header", HeaderRequest{});
        json_t.rule = json_j.value("rule", Rule{});
    }

    void to_json(nlohmann::json &json_j, const UpdateRuleRequest &json_t)
    {
        json_j["header"] = json_t.header;
        json_j["rule"] = json_t.rule;
    }

    void from_json(const nlohmann::json &json_j, UpdateRuleRequest &json_t)
    {
        json_t.header = json_j.value("header", HeaderRequest{});
        json_t.rule = json_j.value("rule", UpdateRule{});
    }

    void to_json(nlohmann::json &json_j, const SetStateRuleRequest &json_t)
    {
        json_j["header"] = json_t.header;
        json_j["id"] = json_t.id;
        json_j["activestate"] = enumToString(json_t.activestate);
    }

    void from_json(const nlohmann::json &json_j, SetStateRuleRequest &json_t)
    {
        json_t.header = json_j.value("header", HeaderRequest{});
        json_t.id = json_j.value("id", "");
        std::string str = json_j.value("activestate", "");
        enumFormString(str, json_t.activestate);
    }

    void to_json(nlohmann::json &json_j, const IssueGetCommandRequest &json_t)
    {
        json_j["header"] = json_t.header;
        json_j["id"] = json_t.id;
        json_j["resources"] = json_t.resources;
        json_j["options"] = json_t.options;
    }

    void to_json(nlohmann::json &json_j, const TriggerRuleRequest &json_t)
    {
        json_j["header"] = json_t.header;
        json_j["id"] = json_t.id;
        json_j["triggerstate"] = enumToString(json_t.triggerstate);
    }

    void from_json(const nlohmann::json &json_j, TriggerRuleRequest &json_t)
    {
        json_t.header = json_j.value("header", HeaderRequest{});
        json_t.id = json_j.value("id", "");
        std::string str = json_j.value("triggerstate", "");
        enumFormString(str, json_t.triggerstate);
    }

    void from_json(const nlohmann::json &json_j, IssueGetCommandRequest &json_t)
    {
        json_t.header = json_j.value("header", HeaderRequest{});
        json_t.id = json_j.value("id", "");
        json_t.resources = json_j.value("resources", std::vector<std::string>());
        json_t.options = json_j.value("options", std::map<std::string, std::string>());
    }

    void to_json(nlohmann::json &json_j, const IssueSetCommandRequest &json_t)
    {
        json_j["header"] = json_t.header;
        json_j["id"] = json_t.id;
        json_j["resources"] = json_t.resources;
        json_j["options"] = json_t.options;
    }

    void from_json(const nlohmann::json &json_j, IssueSetCommandRequest &json_t)
    {
        json_t.header = json_j.value("header", HeaderRequest{});
        json_t.id = json_j.value("id", "");
        json_t.resources = json_j.value("resources", std::map<std::string, std::string>());
        json_t.options = json_j.value("options", std::map<std::string, std::string>());
    }

    void to_json(nlohmann::json &json_j, const IssueDiscoveryCommandRequest &json_t)
    {
        json_j["header"] = json_t.header;
        json_j["device"] = json_t.device;
        json_j["options"] = json_t.options;
    }

    void from_json(const nlohmann::json &json_j, IssueDiscoveryCommandRequest &json_t)
    {
        json_t.header = json_j.value("header", HeaderRequest{});
        json_t.device = json_j.value("device", Device{});
        json_t.options = json_j.value("options", std::map<std::string, std::string>());
    }

    void to_json(nlohmann::json &json_j, const AddNotificationRequest &json_t)
    {
        json_j["header"] = json_t.header;
        json_j["notify"] = json_t.notify;
    }

    void from_json(const nlohmann::json &json_j, AddNotificationRequest &json_t)
    {
        json_t.header = json_j.value("header", HeaderRequest{});
        json_t.notify = json_j.value("notify", Notification{});
    }

    void to_json(nlohmann::json &json_j, const SetOperationRequest &json_t)
    {
        json_j["header"] = json_t.header;
        json_j["operations"] = json_t.operations;
    }

    void from_json(const nlohmann::json &json_j, SetOperationRequest &json_t)
    {
        json_t.header = json_j.value("header", HeaderRequest{});
        json_t.operations = json_j.value("operations", map<string, string>());
    }

    /*
        Define Serialize/Deserialize JSON functions for Response
    */
    void to_json(nlohmann::json &json_j, const SimpleResponse &json_t)
    {
        json_j["header"] = json_t.header;
    }

    void from_json(const nlohmann::json &json_j, SimpleResponse &json_t)
    {
        json_t.header = json_j.value("header", HeaderResponse{});
    }

    void to_json(nlohmann::json &json_j, const ResponseWithId &json_t)
    {
        json_j["header"] = json_t.header;
        json_j["id"] = json_t.id;
    }

    void from_json(const nlohmann::json &json_j, ResponseWithId &json_t)
    {
        json_t.header = json_j.value("header", HeaderResponse{});
        json_t.id = json_j.value("id", "");
    }

    void to_json(nlohmann::json &json_j, const CountResponse &json_t)
    {
        json_j["header"] = json_t.header;
        json_j["count"] = json_t.count;
    }

    void from_json(const nlohmann::json &json_j, CountResponse &json_t)
    {
        json_t.header = json_j.value("header", HeaderResponse{});
        json_t.count = json_j.value("count", 0);
    }

    void to_json(nlohmann::json &json_j, const MetricResponse &json_t)
    {
        json_j["header"] = json_t.header;
        json_j["metric"] = json_t.metric;
    }

    void from_json(const nlohmann::json &json_j, MetricResponse &json_t)
    {
        json_t.header = json_j.value("header", HeaderResponse{});
        json_t.metric = json_j.value("metric", map<string, string>());
    }

    void to_json(nlohmann::json &json_j, const DeviceResponse &json_t)
    {
        json_j["header"] = json_t.header;
        json_j["device"] = json_t.device;
    }

    void from_json(const nlohmann::json &json_j, DeviceResponse &json_t)
    {
        json_t.header = json_j.value("header", HeaderResponse{});
        json_t.device = json_j.value("device", Device{});
    }

    void to_json(nlohmann::json &json_j, const MultiDeviceResponse &json_t)
    {
        json_j["header"] = json_t.header;
        json_j["devices"] = json_t.devices;
    }

    void from_json(const nlohmann::json &json_j, MultiDeviceResponse &json_t)
    {
        json_t.header = json_j.value("header", HeaderResponse{});
        json_t.devices = json_j.value("devices", vector<Device>());
    }

    void to_json(nlohmann::json &json_j, const MultiProfileResponse &json_t)
    {
        json_j["header"] = json_t.header;
        json_j["profiles"] = json_t.profiles;
    }

    void from_json(const nlohmann::json &json_j, MultiProfileResponse &json_t)
    {
        json_t.header = json_j.value("header", HeaderResponse{});
        json_t.profiles = json_j.value("profiles", vector<DeviceProfile>());
    }

    void to_json(nlohmann::json &json_j, const MultiServiceResponse &json_t)
    {
        json_j["header"] = json_t.header;
        json_j["services"] = json_t.services;
    }

    void from_json(const nlohmann::json &json_j, MultiServiceResponse &json_t)
    {
        json_t.header = json_j.value("header", HeaderResponse{});
        json_t.services = json_j.value("sevices", vector<DeviceService>());
    }

    void to_json(nlohmann::json &json_j, const MultiReadingResponse &json_t)
    {
        json_j["header"] = json_t.header;
        json_j["readings"] = json_t.readings;
    }

    void from_json(const nlohmann::json &json_j, MultiReadingResponse &json_t)
    {
        json_t.header = json_j.value("header", HeaderResponse{});
        json_t.readings = json_j.value("readings", vector<Reading>());
    }

    void to_json(nlohmann::json &json_j, const MultiResourceResponse &json_t)
    {
        json_j["header"] = json_t.header;
        json_j["resources"] = json_t.resources;
    }

    void from_json(const nlohmann::json &json_j, MultiResourceResponse &json_t)
    {
        json_t.header = json_j.value("header", HeaderResponse{});
        json_t.resources = json_j.value("resources", vector<DeviceResource>());
    }

    void to_json(nlohmann::json &json_j, const RuleResponse &json_t)
    {
        json_j["header"] = json_t.header;
        json_j["rule"] = json_t.rule;
    }

    void from_json(const nlohmann::json &json_j, RuleResponse &json_t)
    {
        json_t.header = json_j.value("header", HeaderResponse{});
        json_t.rule = json_j.value("rule", Rule{});
    }

    void to_json(nlohmann::json &json_j, const MultiRuleResponse &json_t)
    {
        json_j["header"] = json_t.header;
        json_j["rules"] = json_t.rules;
    }

    void from_json(const nlohmann::json &json_j, MultiRuleResponse &json_t)
    {
        json_t.header = json_j.value("header", HeaderResponse{});
        json_t.rules = json_j.value("rules", vector<Rule>());
    }

    /*
        Define Convert DTO to/from Model functions
    */

    std::vector<model::Reading> toModel(const std::vector<Reading> &dtos)
    {
        std::vector<model::Reading> mds;
        for (auto &dto : dtos)
        {
            model::Reading md;
            md.deviceId = dto.deviceId;
            md.value = dto.value;
            md.resourceName = dto.resourceName;
            enumFromString(dto.valueType, md.valueType);
            md.timestamp = dto.timestamp;
            md.unit = dto.unit;
            mds.push_back(md);
        }

        return mds;
    }

    std::vector<Reading> fromModel(const std::vector<model::Reading> &mds)
    {
        std::vector<Reading> dtos;
        for (auto &md : mds)
        {
            Reading dto;
            dto.deviceId = md.deviceId;
            dto.resourceName = md.resourceName;
            dto.value = md.value;
            dto.valueType = enumToString(md.valueType);
            dto.timestamp = md.timestamp;
            dto.unit = md.unit;
            dtos.push_back(dto);
        }

        return dtos;
    }

    std::vector<model::DeviceResource> toModel(const std::vector<DeviceResource> &dtos)
    {
        std::vector<model::DeviceResource> mds;
        for (auto &dto : dtos)
        {
            model::DeviceResource md;
            md.name = dto.name;
            enumFromString(dto.valueType, md.valueType);
            enumFromString(dto.readWrite, md.readWrite);
            md.unit = dto.unit;
            md.minimum = dto.minimum;
            md.maximum = dto.maximum;
            mds.push_back(md);
        }

        return mds;
    }

    std::vector<DeviceResource> fromModel(const std::vector<model::DeviceResource> &mds)
    {
        std::vector<DeviceResource> dtos;
        for (auto &md : mds)
        {
            DeviceResource dto;
            dto.name = md.name;
            dto.valueType = enumToString(md.valueType);
            dto.readWrite = enumToString(md.readWrite);
            dto.unit = md.unit;
            dto.minimum = md.minimum;
            dto.maximum = md.maximum;
            dtos.push_back(dto);
        }

        return dtos;
    }

    std::vector<model::ResourceInProfile> toModel(const std::vector<ResourceInProfile> &dtos)
    {
        std::vector<model::ResourceInProfile> mds;
        for (auto &dto : dtos)
        {
            model::ResourceInProfile md;
            md.resourceName = dto.resourceName;
            md.attributes = dto.attributes;
            mds.push_back(md);
        }

        return mds;
    }

    std::vector<ResourceInProfile> fromModel(const std::vector<model::ResourceInProfile> &mds)
    {
        std::vector<ResourceInProfile> dtos;
        for (auto &md : mds)
        {
            ResourceInProfile dto;
            dto.resourceName = md.resourceName;
            dto.attributes = md.attributes;
            dtos.push_back(dto);
        }

        return dtos;
    }

    model::DeviceProfile toModel(const DeviceProfile &dto)
    {
        model::DeviceProfile md;
        md.name = dto.name;
        md.model = dto.model;
        md.resources = toModel(dto.resources);

        return md;
    }

    DeviceProfile fromModel(const model::DeviceProfile &md)
    {
        DeviceProfile dto;
        dto.name = md.name;
        dto.model = md.model;
        dto.resources = fromModel(md.resources);

        return dto;
    }

    model::DeviceService toModel(const DeviceService &dto)
    {
        model::DeviceService md;
        md.name = dto.name;
        md.address = dto.address;

        return md;
    }

    DeviceService fromModel(const model::DeviceService &md)
    {
        DeviceService dto;
        dto.name = md.name;
        dto.address = md.address;

        return dto;
    }

    std::vector<model::DeviceInGroup> toModel(const std::vector<DeviceInGroup> &dtos)
    {
        std::vector<model::DeviceInGroup> mds;
        for (auto &dto : dtos)
        {
            model::DeviceInGroup md;
            md.deviceId = dto.deviceId;
            md.state = dto.state;
            mds.push_back(md);
        }

        return mds;
    }

    std::vector<DeviceInGroup> fromModel(const std::vector<model::DeviceInGroup> &mds)
    {
        std::vector<DeviceInGroup> dtos;
        for (auto &md : mds)
        {
            DeviceInGroup dto;
            dto.deviceId = md.deviceId;
            dto.state = md.state;
            dtos.push_back(dto);
        }
        return dtos;
    }

    model::Device toModel(const Device &dto)
    {
        model::Device md;
        md.id = dto.id;
        md.name = dto.name;
        enumFromString(dto.type, md.type);
        md.created = dto.created;
        md.modified = dto.modified;
        enumFromString(dto.adminState, md.adminState);
        enumFromString(dto.operatingState, md.operatingState);
        md.lastConnected = dto.lastConnected;
        md.profileName = dto.profileName;
        md.serviceName = dto.serviceName;
        md.deviceList = toModel(dto.deviceList);
        md.protocols = dto.protocols;

        return md;
    }

    Device fromModel(const model::Device &md)
    {
        Device dto;
        dto.id = md.id;
        dto.name = md.name;
        dto.type = enumToString(md.type);
        dto.created = md.created;
        dto.modified = md.modified;
        dto.adminState = enumToString(md.adminState);
        dto.operatingState = enumToString(md.operatingState);
        dto.lastConnected = md.lastConnected;
        dto.profileName = md.profileName;
        dto.serviceName = md.serviceName;
        dto.deviceList = fromModel(md.deviceList);
        dto.protocols = md.protocols;

        return dto;
    }

    std::vector<model::DeviceCondition> toModel(const std::vector<DeviceCondition> &dtos)
    {
        std::vector<model::DeviceCondition> mds;
        for (auto &dto : dtos)
        {
            model::DeviceCondition md;
            md.deviceId = dto.deviceId;
            md.resourceName = dto.resourceName;
            enumFormString(dto.compareOperator, md.compareOperator);
            md.value = dto.value;
            mds.push_back(md);
        }

        return mds;
    }

    std::vector<DeviceCondition> fromModel(const std::vector<model::DeviceCondition> &mds)
    {
        std::vector<DeviceCondition> dtos;
        for (auto &md : mds)
        {
            DeviceCondition dto;
            dto.deviceId = md.deviceId;
            dto.resourceName = md.resourceName;
            dto.compareOperator = enumToString(md.compareOperator);
            dto.value = md.value;
            dtos.push_back(dto);
        }

        return dtos;
    }

    std::vector<model::RuleCondition> toModel(const std::vector<RuleCondition> &dtos)
    {
        std::vector<model::RuleCondition> mds;
        for (auto &dto : dtos)
        {
            model::RuleCondition md;
            md.conditionRuleId = dto.conditionRuleId;
            enumFormString(dto.compareOperator, md.compareOperator);
            mds.push_back(md);
        }

        return mds;
    }

    std::vector<RuleCondition> fromModel(const std::vector<model::RuleCondition> &mds)
    {
        std::vector<RuleCondition> dtos;
        for (auto &md : mds)
        {
            RuleCondition dto;
            dto.conditionRuleId = md.conditionRuleId;
            dto.compareOperator = enumToString(md.compareOperator);
            dtos.push_back(dto);
        }

        return dtos;
    }

    std::vector<model::DeviceAction> toModel(const std::vector<DeviceAction> &dtos)
    {
        std::vector<model::DeviceAction> mds;
        for (auto &dto : dtos)
        {
            model::DeviceAction md;
            md.deviceId = dto.deviceId;
            md.delayTime = dto.delayTime;
            md.resourceName = dto.resourceName;
            md.value = dto.value;
            mds.push_back(md);
        }

        return mds;
    }

    std::vector<DeviceAction> fromModel(const std::vector<model::DeviceAction> &mds)
    {
        std::vector<DeviceAction> dtos;
        for (auto &md : mds)
        {
            DeviceAction dto;
            dto.deviceId = md.deviceId;
            dto.delayTime = md.delayTime;
            dto.resourceName = md.resourceName;
            dto.value = md.value;
            dtos.push_back(dto);
        }

        return dtos;
    }

    std::vector<model::RuleAction> toModel(const std::vector<RuleAction> &dtos)
    {
        std::vector<model::RuleAction> mds;
        for (auto &dto : dtos)
        {
            model::RuleAction md;
            md.actionRuleId = dto.actionRuleId;
            md.delayTime = dto.delayTime;
            mds.push_back(md);
        }

        return mds;
    }

    std::vector<RuleAction> fromModel(const std::vector<model::RuleAction> &mds)
    {
        std::vector<RuleAction> dtos;
        for (auto &md : mds)
        {
            RuleAction dto;
            dto.actionRuleId = md.actionRuleId;
            dto.delayTime = md.delayTime;
            dtos.push_back(dto);
        }

        return dtos;
    }

    model::Rule toModel(const Rule &dto)
    {
        model::Rule md;
        md.id = dto.id;
        md.name = dto.name;
        enumFromString(dto.type, md.type);
        md.priority = dto.priority;
        enumFromString(dto.activeState, md.activeState);
        enumFromString(dto.triggerState, md.triggerState);
        md.startTime = dto.startTime;
        md.endTime = dto.endTime;
        md.startInDayTime = dto.startInDayTime;
        md.endInDayTime = dto.endInDayTime;
        md.repeatDays = dto.repeatDays;
        enumFormString(dto.conditionLogic, md.conditionLogic);
        md.timeInDayConditions = dto.timeInDayConditions;
        md.deviceConditions = toModel(dto.deviceConditions);
        md.ruleConditions = toModel(dto.ruleConditions);
        md.deviceActions = toModel(dto.deviceActions);
        md.ruleActions = toModel(dto.ruleActions);
        md.enableNotify = dto.enableNotify;
        enumFromString(dto.notifyLevel, md.notifyLevel);
        md.notifyContent = dto.notifyContent;

        return md;
    }

    Rule fromModel(const model::Rule &md)
    {
        Rule dto;
        dto.id = md.id;
        dto.name = md.name;
        dto.type = enumToString(md.type);
        dto.priority = md.priority;
        dto.activeState = enumToString(md.activeState);
        dto.triggerState = enumToString(md.triggerState);
        dto.startTime = md.startTime;
        dto.endTime = md.endTime;
        dto.startInDayTime = md.startInDayTime;
        dto.endInDayTime = md.endInDayTime;
        dto.repeatDays = md.repeatDays;
        dto.conditionLogic = enumToString(md.conditionLogic);

        dto.timeInDayConditions = md.timeInDayConditions;
        dto.deviceConditions = fromModel(md.deviceConditions);
        dto.ruleConditions = fromModel(md.ruleConditions);
        dto.deviceActions = fromModel(md.deviceActions);
        dto.ruleActions = fromModel(md.ruleActions);
        dto.enableNotify = md.enableNotify;
        dto.notifyLevel = enumToString(md.notifyLevel);
        dto.notifyContent = md.notifyContent;

        return dto;
    }

    /*
        Other functions
    */
    void replaceDeviceModelFieldsWithDto(model::Device *md, const UpdateDevice &dto)
    {
        if (dto.name != NULL)
        {
            md->name = *dto.name;
        }
        if (dto.type != NULL)
        {
            enumFromString(*dto.type, md->type);
        }
        if (dto.created != NULL)
        {
            md->created = *dto.created;
        }
        if (dto.modified != NULL)
        {
            md->modified = *dto.modified;
        }
        if (dto.adminState != NULL)
        {
            enumFromString(*dto.adminState, md->adminState);
        }
        if (dto.operatingState != NULL)
        {
            enumFromString(*dto.operatingState, md->operatingState);
        }
        if (dto.lastConnected != NULL)
        {
            md->lastConnected = *dto.lastConnected;
        }
        if (dto.profileName != NULL)
        {
            md->profileName = *dto.profileName;
        }
        if (dto.serviceName != NULL)
        {
            md->serviceName = *dto.serviceName;
        }
        if (dto.deviceList != NULL)
        {
            md->deviceList = toModel(*dto.deviceList);
        }
        if (dto.protocols != NULL)
        {
            md->protocols = *dto.protocols;
        }
    }

    UpdateDevice fromDeviceModel(const model::Device &md)
    {
        Device d = fromModel(md);
        UpdateDevice dto;
        dto.id = d.id;
        dto.name = new string(d.name);
        dto.type = new string(d.type);
        dto.created = new uint64_t(d.created);
        dto.modified = new uint64_t(d.modified);
        dto.adminState = new string(d.adminState);
        dto.operatingState = new string(d.operatingState);
        dto.lastConnected = new uint64_t(d.lastConnected);
        dto.profileName = new string(d.profileName);
        dto.serviceName = new string(d.serviceName);
        dto.deviceList = new std::vector<DeviceInGroup>(d.deviceList);
        dto.protocols = new std::map<std::string, std::string>(d.protocols);

        return dto;
    }

    void deleteUpdateDevice(UpdateDevice obj)
    {
        if (!obj.name)
        {
            delete obj.name;
        }

        if (!obj.type)
        {
            delete obj.type;
        }

        if (!obj.created)
        {
            delete obj.created;
        }

        if (!obj.modified)
        {
            delete obj.modified;
        }

        if (!obj.adminState)
        {
            delete obj.adminState;
        }

        if (!obj.operatingState)
        {
            delete obj.operatingState;
        }

        if (!obj.lastConnected)
        {
            delete obj.lastConnected;
        }

        if (!obj.profileName)
        {
            delete obj.profileName;
        }

        if (!obj.serviceName)
        {
            delete obj.serviceName;
        }

        if (!obj.deviceList)
        {
            delete obj.deviceList;
        }

        if (!obj.protocols)
        {
            delete obj.protocols;
        }
    }

    void ReplaceRuleModelFieldsWithDto(model::Rule *md, const UpdateRule &dto)
    {
        if (dto.name != NULL)
        {
            md->name = *dto.name;
        }
        if (dto.type != NULL)
        {
            enumFromString(*dto.type, md->type);
        }
        if (dto.priority != NULL)
        {
            md->priority = *dto.priority;
        }
        if (dto.activeState != NULL)
        {
            enumFromString(*dto.activeState, md->activeState);
        }
        if (dto.triggerState != NULL)
        {
            enumFromString(*dto.triggerState, md->triggerState);
        }
        if (dto.startTime != NULL)
        {
            md->startTime = *dto.startTime;
        }
        if (dto.endTime != NULL)
        {
            md->endTime = *dto.endTime;
        }
        if (dto.startInDayTime != NULL)
        {
            md->startInDayTime = *dto.startInDayTime;
        }
        if (dto.endInDayTime != NULL)
        {
            md->endInDayTime = *dto.endInDayTime;
        }
        if (dto.repeatDays != NULL)
        {
            md->repeatDays = *dto.repeatDays;
        }
        if (dto.conditionLogic != NULL)
        {
            enumFromString(*dto.conditionLogic, md->conditionLogic);
        }
        if (dto.timeInDayConditions != NULL)
        {
            md->timeInDayConditions = *dto.timeInDayConditions;
        }
        if (dto.deviceConditions != NULL)
        {
            md->deviceConditions = toModel(*dto.deviceConditions);
        }
        if (dto.ruleConditions != NULL)
        {
            md->ruleConditions = toModel(*dto.ruleConditions);
        }
        if (dto.deviceActions != NULL)
        {
            md->deviceActions = toModel(*dto.deviceActions);
        }
        if (dto.ruleActions != NULL)
        {
            md->ruleActions = toModel(*dto.ruleActions);
        }
        if (dto.enableNotify != NULL)
        {
            md->enableNotify = *dto.enableNotify;
        }
        if (dto.notifyLevel != NULL)
        {
            enumFromString(*dto.notifyLevel, md->notifyLevel);
        }
        if (dto.notifyContent != NULL)
        {
            md->notifyContent = *dto.notifyContent;
        }
    }

    UpdateRule fromRuleModel(const model::Rule &md)
    {
        Rule d = fromModel(md);
        UpdateRule dto;
        dto.id = d.id;
        dto.name = new std::string(d.name);
        dto.type = new std::string(d.type);
        dto.priority = new uint64_t(d.priority);
        dto.activeState = new std::string(d.activeState);
        dto.triggerState = new std::string(d.triggerState);
        dto.startTime = new uint64_t(d.startTime);
        dto.endTime = new uint64_t(d.endTime);
        dto.startInDayTime = new uint32_t(d.startInDayTime);
        dto.endInDayTime = new uint32_t(d.endInDayTime);
        dto.repeatDays = new std::vector<std::string>(d.repeatDays);
        dto.conditionLogic = new std::string(d.conditionLogic);

        dto.timeInDayConditions = new std::vector<uint32_t>(d.timeInDayConditions);
        dto.deviceConditions = new std::vector<DeviceCondition>(d.deviceConditions);
        dto.ruleConditions = new std::vector<RuleCondition>(d.ruleConditions);
        dto.deviceActions = new std::vector<DeviceAction>(d.deviceActions);
        dto.ruleActions = new std::vector<RuleAction>(d.ruleActions);
        dto.enableNotify = new bool(d.enableNotify);
        dto.notifyLevel = new std::string(d.notifyLevel);
        dto.notifyContent = new std::string(d.notifyContent);

        return dto;
    }

    void deleteUpdateRule(UpdateRule obj)
    {
        if (!obj.name)
        {
            delete obj.name;
        }

        if (!obj.type)
        {
            delete obj.type;
        }

        if (!obj.priority)
        {
            delete obj.priority;
        }

        if (!obj.activeState)
        {
            delete obj.activeState;
        }

        if (!obj.triggerState)
        {
            delete obj.triggerState;
        }

        if (!obj.startTime)
        {
            delete obj.startTime;
        }

        if (!obj.endTime)
        {
            delete obj.endTime;
        }

        if (!obj.startInDayTime)
        {
            delete obj.startInDayTime;
        }

        if (!obj.endInDayTime)
        {
            delete obj.endInDayTime;
        }

        if (!obj.repeatDays)
        {
            delete obj.repeatDays;
        }

        if (!obj.conditionLogic)
        {
            delete obj.conditionLogic;
        }

        if (!obj.timeInDayConditions)
        {
            delete obj.timeInDayConditions;
        }

        if (!obj.deviceConditions)
        {
            delete obj.deviceConditions;
        }

        if (!obj.ruleConditions)
        {
            delete obj.ruleConditions;
        }

        if (!obj.deviceActions)
        {
            delete obj.deviceActions;
        }

        if (!obj.ruleActions)
        {
            delete obj.ruleActions;
        }

        if (!obj.enableNotify)
        {
            delete obj.enableNotify;
        }

        if (!obj.notifyLevel)
        {
            delete obj.notifyLevel;
        }

        if (!obj.notifyContent)
        {
            delete obj.notifyContent;
        }
    }

    model::Notification toModel(const Notification &dto)
    {
        model::Notification md;
        enumFromString(dto.level, md.level);
        md.content = dto.content;
        md.timestamp = dto.timestamp;

        return md;
    }

    Notification fromModel(const model::Notification &md)
    {
        Notification dto;
        dto.content = md.content;
        dto.timestamp = md.timestamp;
        dto.level = enumToString(md.level);

        return dto;
    }

    /*
        Define other functions
    */
    HeaderRequest createHeaderRequest(string client, string op, string rqi)
    {
        return HeaderRequest{client, rqi, op};
    }

    HeaderResponse createHeaderResponse(string client, string rqi, string error)
    {
        bool success = false;
        if (error == "")
        {
            success = true;
        }

        return HeaderResponse{client, rqi, success, error};
    }
}
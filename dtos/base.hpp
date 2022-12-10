#pragma once

#include <iostream>
#include <vector>
#include <map>
#include <stdint.h>

#include "../json/json.hpp"
#include "../models/models.hpp"

template <class T>
static const std::string serialize(T &obj)
{
    nlohmann::json j = obj;
    return j.dump();
}

template <class T>
static void deserialize(std::string str, T &obj)
{
    const auto j = nlohmann::json::parse(str);
    obj = j.get<T>();
}

namespace dto
{

    struct HeaderRequest
    {
        std::string client;
        std::string rqi;
        std::string op;
    };

    struct HeaderResponse
    {
        std::string client;
        std::string rqi;
        bool success;
        std::string error;
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

    struct DeviceResource
    {
        std::string name;
        std::string valueType; // Loại giá trị của Resource: "int8", "uint8"...
        std::string readWrite; // = {"R", "W", "RW"}: tài nguyên này chỉ đọc, chỉ ghi hay cả đọc và ghi
        std::string unit;      // Đơn vị của Resource
        std::string minimum;   // Giá trị tối thiểu của Resource
        std::string maximum;   // Giá trị tối đa của Resource
    };

    struct ResourceInProfile
    {
        std::string resourceName;
        std::map<std::string, std::string> attributes; // biểu diễn các thông tin khác cho Resource, thường là thông tin theo giao thức mạng.
        // Ví dụ Resource OnOff trong BLE có ModelID = 0x1000 => Attribute = {"Model": "0x1000"}
    };

    struct DeviceProfile
    {
        std::string name;
        std::string model; // Model của Profile, ví dụ: RD-01
        std::vector<ResourceInProfile> resources;
    };

    struct DeviceService
    {
        std::string name;
        std::string address;
    };

    struct DeviceInGroup
    {
        std::string deviceId;
        std::string state;
    };

    struct Device
    {
        std::string id;
        std::string name;
        std::string type;
        uint64_t created;
        uint64_t modified;
        std::string adminState;
        std::string operatingState;
        uint64_t lastConnected;
        std::string profileName;
        std::string serviceName;
        std::vector<DeviceInGroup> deviceList;
        std::map<std::string, std::string> protocols;
    };

    struct UpdateDevice
    {
        std::string id;
        std::string *name;
        std::string *type;
        uint64_t *created;
        uint64_t *modified;
        std::string *adminState;
        std::string *operatingState;
        uint64_t *lastConnected;
        std::string *profileName;
        std::string *serviceName;
        std::vector<DeviceInGroup> *deviceList;
        std::map<std::string, std::string> *protocols;
    };

    struct DeviceCondition
    {
        std::string deviceId;
        std::string ruleId;
        std::string resourceName;
        std::string compareOperator;
        std::string value;
    };

    struct RuleCondition
    {
        std::string conditionRuleId;
        std::string compareOperator;
    };

    struct DeviceAction
    {
        std::string deviceId;
        uint64_t delayTime;
        std::string resourceName;
        std::string value;
    };

    struct RuleAction
    {
        std::string actionRuleId;
        uint64_t delayTime;
    };

    struct Rule
    {
        std::string id;
        std::string name;
        std::string type;
        uint64_t priority;
        std::string activeState;
        std::string triggerState;
        uint64_t startTime;
        uint64_t endTime;
        uint32_t startInDayTime;
        uint32_t endInDayTime;
        std::vector<std::string> repeatDays;
        std::string conditionLogic;

        std::vector<uint32_t> timeInDayConditions;
        std::vector<DeviceCondition> deviceConditions;
        std::vector<RuleCondition> ruleConditions;

        std::vector<DeviceAction> deviceActions;
        std::vector<RuleAction> ruleActions;
        bool enableNotify;
        std::string notifyLevel;
        std::string notifyContent;
    };

    struct UpdateRule
    {
        std::string id;
        std::string *name;
        std::string *type;
        uint64_t *priority;
        std::string *activeState;
        std::string *triggerState;
        uint64_t *startTime;
        uint64_t *endTime;
        uint32_t *startInDayTime;
        uint32_t *endInDayTime;
        std::vector<std::string> *repeatDays;
        std::string *conditionLogic;

        std::vector<uint32_t> *timeInDayConditions;
        std::vector<DeviceCondition> *deviceConditions;
        std::vector<RuleCondition> *ruleConditions;

        std::vector<DeviceAction> *deviceActions;
        std::vector<RuleAction> *ruleActions;
        bool *enableNotify;
        std::string *notifyLevel;
        std::string *notifyContent;
    };

    struct Notification
    {
        uint64_t timestamp;
        std::string level;
        std::string content;
    };

    /*
        Declare Serialize/Deserialize JSON functions
    */
    void to_json(nlohmann::json &json_j, const HeaderRequest &json_t);

    void from_json(const nlohmann::json &json_j, HeaderRequest &json_t);

    void to_json(nlohmann::json &json_j, const HeaderResponse &json_t);

    void from_json(const nlohmann::json &json_j, HeaderResponse &json_t);

    void to_json(nlohmann::json &json_j, const Reading &json_t);

    void from_json(const nlohmann::json &json_j, Reading &json_t);

    void to_json(nlohmann::json &json_j, const DeviceResource &json_t);

    void from_json(const nlohmann::json &json_j, DeviceResource &json_t);

    void to_json(nlohmann::json &json_j, const ResourceInProfile &json_t);

    void from_json(const nlohmann::json &json_j, ResourceInProfile &json_t);

    void to_json(nlohmann::json &json_j, const DeviceProfile &json_t);

    void from_json(const nlohmann::json &json_j, DeviceProfile &json_t);

    void to_json(nlohmann::json &json_j, const DeviceService &json_t);

    void from_json(const nlohmann::json &json_j, DeviceService &json_t);

    void to_json(nlohmann::json &json_j, const DeviceInGroup &json_t);

    void from_json(const nlohmann::json &json_j, DeviceInGroup &json_t);

    void to_json(nlohmann::json &json_j, const Device &json_t);

    void from_json(const nlohmann::json &json_j, Device &json_t);

    void to_json(nlohmann::json &json_j, const UpdateDevice &json_t);

    void from_json(const nlohmann::json &json_j, UpdateDevice &json_t);

    void to_json(nlohmann::json &json_j, const DeviceCondition &json_t);

    void from_json(const nlohmann::json &json_j, DeviceCondition &json_t);

    void to_json(nlohmann::json &json_j, const RuleCondition &json_t);

    void from_json(const nlohmann::json &json_j, RuleCondition &json_t);

    void to_json(nlohmann::json &json_j, const DeviceAction &json_t);

    void from_json(const nlohmann::json &json_j, DeviceAction &json_t);

    void to_json(nlohmann::json &json_j, const RuleAction &json_t);

    void from_json(const nlohmann::json &json_j, RuleAction &json_t);

    void to_json(nlohmann::json &json_j, const Rule &json_t);

    void from_json(const nlohmann::json &json_j, Rule &json_t);

    void to_json(nlohmann::json &json_j, const UpdateRule &json_t);

    void from_json(const nlohmann::json &json_j, UpdateRule &json_t);

    void to_json(nlohmann::json &json_j, const Notification &json_t);

    void from_json(const nlohmann::json &json_j, Notification &json_t);

    /*
        Declare Convert DTO to/from Model functions
    */
    std::vector<model::Reading> toModel(const std::vector<Reading> &dtos);

    std::vector<Reading> fromModel(const std::vector<model::Reading> &mds);

    std::vector<model::DeviceResource> toModel(const std::vector<DeviceResource> &dtos);

    std::vector<DeviceResource> fromModel(const std::vector<model::DeviceResource> &mds);

    std::vector<model::ResourceInProfile> toModel(const std::vector<ResourceInProfile> &dtos);

    std::vector<ResourceInProfile> fromModel(const std::vector<model::ResourceInProfile> &mds);

    model::DeviceProfile toModel(const DeviceProfile &dto);

    DeviceProfile fromModel(const model::DeviceProfile &md);

    model::DeviceService toModel(const DeviceService &dto);

    DeviceService fromModel(const model::DeviceService &md);

    std::vector<model::DeviceInGroup> toModel(const std::vector<DeviceInGroup> &dtos);

    std::vector<DeviceInGroup> fromModel(const std::vector<model::DeviceInGroup> &mds);

    model::Device toModel(const Device &dto);

    Device fromModel(const model::Device &md);

    std::vector<model::DeviceCondition> toModel(const std::vector<DeviceCondition> &dtos);

    std::vector<DeviceCondition> fromModel(const std::vector<model::DeviceCondition> &mds);

    std::vector<model::RuleCondition> toModel(const std::vector<RuleCondition> &dtos);

    std::vector<RuleCondition> fromModel(const std::vector<model::RuleCondition> &mds);

    std::vector<model::DeviceAction> toModel(const std::vector<DeviceAction> &dtos);

    std::vector<DeviceAction> fromModel(const std::vector<model::DeviceAction> &mds);

    std::vector<model::RuleAction> toModel(const std::vector<RuleAction> &dtos);

    std::vector<RuleAction> fromModel(const std::vector<model::RuleAction> &mds);

    model::Rule toModel(const Rule &dto);

    Rule fromModel(const model::Rule &md);

    model::Notification toModel(const Notification &dto);

    Notification fromModel(const model::Notification &md);

    /*
        Other functions
    */
    void deleteUpdateDevice(UpdateDevice obj);

    void replaceDeviceModelFieldsWithDto(model::Device *md, const UpdateDevice &dto);

    UpdateDevice fromDeviceModel(const model::Device &md);

    void deleteUpdateRule(UpdateRule obj);

    void ReplaceRuleModelFieldsWithDto(model::Rule *md, const UpdateRule &dto);

    UpdateRule fromRuleModel(const model::Rule &md);
}
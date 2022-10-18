#pragma once

#include <iostream>
#include <vector>
#include <map>
#include <stdint.h>

#include "../json/json.hpp"
#include "base.hpp"

namespace dto
{
    struct TestRequest
    {
        std::vector<HeaderRequest> req;
        std::string body;
    };

    struct SimpleRequest
    {
        HeaderRequest header;
    };

    struct RequestWithId
    {
        HeaderRequest header;
        std::string id;
    };

    struct RequestWithName
    {
        HeaderRequest header;
        std::string name;
    };

    struct RequestWithOptions
    {
        HeaderRequest header;
        std::map<std::string, std::string> options;
    };

    struct RequestWithOperations
    {
        HeaderRequest header;
        std::map<std::string, std::string> operations;
    };

    struct AddReadingsRequest
    {
        HeaderRequest header;
        std::vector<Reading> readings;
    };

    struct AddResourceRequest
    {
        HeaderRequest header;
        std::vector<DeviceResource> resources;
    };

    struct AddDeviceRequest
    {
        HeaderRequest header;
        Device device;
        bool notify;
    };

    struct UpdateDeviceRequest
    {
        HeaderRequest header;
        UpdateDevice device;
        bool notify;
    };

    struct SetAdminStateDeviceRequest
    {
        HeaderRequest header;
        std::string id;
        AdminState adminState;
    };

    struct DeleteDeviceRequest
    {
        HeaderRequest header;
        std::string id;
        bool notify;
    };

    struct CallbackDeviceRequest
    {
        HeaderRequest header;
        Device device;
    };

    struct AddProfileRequest
    {
        HeaderRequest header;
        DeviceProfile profile;
    };

    struct AddServiceRequest
    {
        HeaderRequest header;
        DeviceService service;
    };

    struct AddRuleRequest
    {
        HeaderRequest header;
        Rule rule;
    };

    struct UpdateRuleRequest
    {
        HeaderRequest header;
        UpdateRule rule;
    };

    struct SetStateRuleRequest
    {
        HeaderRequest header;
        std::string id;
        ActiveState activestate;
    };

    struct TriggerRuleRequest
    {
        HeaderRequest header;
        std::string id;
        TriggerState triggerstate;
    };

    struct IssueGetCommandRequest
    {
        HeaderRequest header;
        std::string id;
        std::vector<std::string> resources;
        std::map<std::string, std::string> options;
    };

    struct IssueSetCommandRequest
    {
        HeaderRequest header;
        std::string id;
        std::map<std::string, std::string> resources;
        std::map<std::string, std::string> options;
    };

    struct IssueDiscoveryCommandRequest
    {
        HeaderRequest header;
        Device device;
        std::map<std::string, std::string> options;
    };

    struct AddNotificationRequest
    {
        HeaderRequest header;
        Notification notify;
    };

    struct SetOperationRequest
    {
        HeaderRequest header;
        std::map<std::string, std::string> operations;
    };

    /*
        Declare Serialize/Deserialize JSON functions
    */
    void to_json(nlohmann::json &json_j, const TestRequest &json_t);

    void from_json(const nlohmann::json &json_j, TestRequest &json_t);

    void to_json(nlohmann::json &json_j, const SimpleRequest &json_t);

    void from_json(const nlohmann::json &json_j, SimpleRequest &json_t);

    void to_json(nlohmann::json &json_j, const RequestWithId &json_t);

    void from_json(const nlohmann::json &json_j, RequestWithId &json_t);

    void to_json(nlohmann::json &json_j, const RequestWithName &json_t);

    void from_json(const nlohmann::json &json_j, RequestWithName &json_t);

    void to_json(nlohmann::json &json_j, const RequestWithOptions &json_t);

    void from_json(const nlohmann::json &json_j, RequestWithOptions &json_t);

    void to_json(nlohmann::json &json_j, const RequestWithOperations &json_t);

    void from_json(const nlohmann::json &json_j, RequestWithOperations &json_t);

    void to_json(nlohmann::json &json_j, const AddReadingsRequest &json_t);

    void from_json(const nlohmann::json &json_j, AddReadingsRequest &json_t);

    void to_json(nlohmann::json &json_j, const AddResourceRequest &json_t);

    void from_json(const nlohmann::json &json_j, AddResourceRequest &json_t);

    void to_json(nlohmann::json &json_j, const AddDeviceRequest &json_t);

    void from_json(const nlohmann::json &json_j, AddDeviceRequest &json_t);

    void to_json(nlohmann::json &json_j, const UpdateDeviceRequest &json_t);

    void from_json(const nlohmann::json &json_j, UpdateDeviceRequest &json_t);

    void to_json(nlohmann::json &json_j, const DeleteDeviceRequest &json_t);

    void from_json(const nlohmann::json &json_j, DeleteDeviceRequest &json_t);

    void to_json(nlohmann::json &json_j, const SetAdminStateDeviceRequest &json_t);

    void from_json(const nlohmann::json &json_j, SetAdminStateDeviceRequest &json_t);

    void to_json(nlohmann::json &json_j, const AddProfileRequest &json_t);

    void from_json(const nlohmann::json &json_j, AddProfileRequest &json_t);

    void to_json(nlohmann::json &json_j, const AddServiceRequest &json_t);

    void from_json(const nlohmann::json &json_j, AddServiceRequest &json_t);

    void to_json(nlohmann::json &json_j, const CallbackDeviceRequest &json_t);

    void from_json(const nlohmann::json &json_j, CallbackDeviceRequest &json_t);

    void to_json(nlohmann::json &json_j, const AddRuleRequest &json_t);

    void from_json(const nlohmann::json &json_j, AddRuleRequest &json_t);

    void to_json(nlohmann::json &json_j, const UpdateRuleRequest &json_t);

    void from_json(const nlohmann::json &json_j, UpdateRuleRequest &json_t);

    void to_json(nlohmann::json &json_j, const SetStateRuleRequest &json_t);

    void from_json(const nlohmann::json &json_j, SetStateRuleRequest &json_t);

    void to_json(nlohmann::json &json_j, const TriggerRuleRequest &json_t);

    void from_json(const nlohmann::json &json_j, TriggerRuleRequest &json_t);

    void to_json(nlohmann::json &json_j, const IssueGetCommandRequest &json_t);

    void from_json(const nlohmann::json &json_j, IssueGetCommandRequest &json_t);

    void to_json(nlohmann::json &json_j, const IssueSetCommandRequest &json_t);

    void from_json(const nlohmann::json &json_j, IssueSetCommandRequest &json_t);

    void to_json(nlohmann::json &json_j, const IssueDiscoveryCommandRequest &json_t);

    void from_json(const nlohmann::json &json_j, IssueDiscoveryCommandRequest &json_t);

    void to_json(nlohmann::json &json_j, const AddNotificationRequest &json_t);

    void from_json(const nlohmann::json &json_j, AddNotificationRequest &json_t);

    void to_json(nlohmann::json &json_j, const SetOperationRequest &json_t);

    void from_json(const nlohmann::json &json_j, SetOperationRequest &json_t);

    /*
        Declare Convert DTO to/from Model functions
    */

    /*
         Declare other functions
    */
    HeaderRequest createHeaderRequest(string client, string op, string rqi);

}
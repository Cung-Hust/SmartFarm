#pragma once

#include <iostream>
#include <vector>
#include <map>
#include <stdint.h>

#include "../json/json.hpp"
#include "base.hpp"

namespace dto
{
    struct SimpleResponse
    {
        HeaderResponse header;
    };

    struct ResponseWithId
    {
        HeaderResponse header;
        string id;
    };

    struct CountResponse
    {
        HeaderResponse header;
        uint64_t count;
    };

    struct MetricResponse
    {
        HeaderResponse header;
        std::map<std::string, std::string> metric;
    };

    struct DeviceResponse
    {
        HeaderResponse header;
        Device device;
    };

    struct MultiDeviceResponse
    {
        HeaderResponse header;
        vector<Device> devices;
    };

    struct MultiProfileResponse
    {
        HeaderResponse header;
        vector<DeviceProfile> profiles;
    };

    struct MultiServiceResponse
    {
        HeaderResponse header;
        vector<DeviceService> services;
    };

    struct MultiReadingResponse
    {
        HeaderResponse header;
        vector<Reading> readings;
    };

    struct MultiResourceResponse
    {
        HeaderResponse header;
        vector<DeviceResource> resources;
    };

    struct RuleResponse
    {
        HeaderResponse header;
        Rule rule;
    };

    struct MultiRuleResponse
    {
        HeaderResponse header;
        vector<Rule> rules;
    };

    /*
        Declare Serialize/Deserialize JSON functions
    */
    void to_json(nlohmann::json &json_j, const SimpleResponse &json_t);

    void from_json(const nlohmann::json &json_j, SimpleResponse &json_t);

    void to_json(nlohmann::json &json_j, const ResponseWithId &json_t);

    void from_json(const nlohmann::json &json_j, ResponseWithId &json_t);

    void to_json(nlohmann::json &json_j, const CountResponse &json_t);

    void from_json(const nlohmann::json &json_j, CountResponse &json_t);

    void to_json(nlohmann::json &json_j, const MetricResponse &json_t);

    void from_json(const nlohmann::json &json_j, MetricResponse &json_t);

    void to_json(nlohmann::json &json_j, const DeviceResponse &json_t);

    void from_json(const nlohmann::json &json_j, DeviceResponse &json_t);

    void to_json(nlohmann::json &json_j, const MultiDeviceResponse &json_t);

    void from_json(const nlohmann::json &json_j, MultiDeviceResponse &json_t);

    void to_json(nlohmann::json &json_j, const MultiProfileResponse &json_t);

    void from_json(const nlohmann::json &json_j, MultiProfileResponse &json_t);

    void to_json(nlohmann::json &json_j, const MultiServiceResponse &json_t);

    void from_json(const nlohmann::json &json_j, MultiServiceResponse &json_t);

    void to_json(nlohmann::json &json_j, const MultiReadingResponse &json_t);

    void from_json(const nlohmann::json &json_j, MultiReadingResponse &json_t);

    void to_json(nlohmann::json &json_j, const MultiResourceResponse &json_t);

    void from_json(const nlohmann::json &json_j, MultiResourceResponse &json_t);

    void to_json(nlohmann::json &json_j, const RuleResponse &json_t);

    void from_json(const nlohmann::json &json_j, RuleResponse &json_t);

    void to_json(nlohmann::json &json_j, const MultiRuleResponse &json_t);

    void from_json(const nlohmann::json &json_j, MultiRuleResponse &json_t);
    /*
        Declare other functions
    */
    HeaderResponse createHeaderResponse(string client, string rqi, string error);
}
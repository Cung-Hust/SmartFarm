#pragma once

#include <string>
#include <map>
#include "../json/json.hpp"
#include "../models/models.hpp"
#include "ReadingQueue.hpp"

namespace deviceservice
{
    using namespace std;
    using json = nlohmann::json;

    class IDriver
    {
    public:
        virtual string init(json config, ReadingQueue *readingQueue) = 0;
        virtual vector<model::Reading> handleGetCommand(std::string id, std::vector<std::string> resources, std::map<std::string, std::string> options, string &err) = 0;
        virtual void handleSetCommand(std::string id, std::map<std::string, std::string> resources, std::map<std::string, std::string> options, string &err) = 0;
        virtual model::Device handleDiscoveryCommand(model::Device device, std::map<std::string, std::string> options, string &err) = 0;
        virtual void addDeviceCallback(model::Device device, string &err) = 0;
        virtual void updateDeviceCallback(model::Device device, string &err) = 0;
        virtual void deleteDeviceCallback(model::Device device, string &err) = 0;
    };
}
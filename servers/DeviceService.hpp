#pragma once

#include "../models/models.hpp"
#include "../dtos/requestes.hpp"
#include "../transports/IPublisher.hpp"

namespace server
{
    class DeviceService
    {
    private:
        string serverName;
        IPublisher *transport;
        string commandResponseTopic;
        string coredataRequestTopic;

    public:
        DeviceService(){};
        DeviceService(string serverName, string commandResponseTopic, string coredataRequestTopic);

        string responseGetCommand(string rqi, string error, vector<model::Reading> readings);
        string responseSetCommand(string rqi, string error);
        string responseTestCommand(string message);
        string responseDiscoveryCommand(string rqi, string error, model::Device device);
        string publishMultiReadings(string rqi, vector<model::Reading> readings);
    };
}

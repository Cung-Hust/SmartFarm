#pragma once

#include "../models/models.hpp"
#include "../transports/IPublisher.hpp"

namespace server
{

    class Metadata
    {
    private:
        string serverName;
        IPublisher *transport;

    public:
        Metadata(){};
        Metadata(string serverName);

        string response(string toTopic, string rqi, string error);
        string responseId(string toTopic, string rqi, string error, string id);
        string responseCount(string toTopic, string rqi, string error, uint64_t count);
        string responseDevice(string toTopic, string rqi, string error, model::Device device);
        string responseMultiDevice(string toTopic, string rqi, string error, vector<model::Device> devices);
        string responseMultiResource(string toTopic, string rqi, string error, vector<model::DeviceResource> resources);
        string responseMultiProfile(string toTopic, string rqi, string error, vector<model::DeviceProfile> profiles);
        string responseMultiService(string toTopic, string rqi, string error, vector<model::DeviceService> services);
        string publishAddDeviceCallback(string toTopic, string rqi, model::Device device);
        string publishUpdateDeviceCallback(string toTopic, string rqi, model::Device device);
        string publishDeleteDeviceCallback(string toTopic, string rqi, model::Device device);
    };
}
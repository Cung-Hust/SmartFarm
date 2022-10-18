#pragma once

#include "../models/models.hpp"
#include "../transports/IPublisher.hpp"

class Metadata
{
private:
    IPublisher *transport;
    string clientName;
    string resquestTopic;

public:
    Metadata(){};
    Metadata(string clientName, string resquestTopic);

    string addDevice(string &rqi, model::Device device, bool isNotify);
    string updateDevice(string &rqi, model::Device device, bool isNotify);
    string countDevice(string &rqi, std::map<std::string, std::string> options);
    string getAllDevice(string &rqi, std::map<std::string, std::string> options);
    string getDevice(string &rqi, string id);
    string deleteDevice(string &rqi, string id, bool isNotify);
    string updateAdminStateDevice(string &rqi, string id, AdminState state);
    string addResource(string &rqi, std::vector<model::DeviceResource> resources);
    string getAllResource(string &rqi);
    string deleteResource(string &rqi, string name);
    string addProfile(string &rqi, model::DeviceProfile profile);
    string getAllProfile(string &rqi);
    string deleteProfile(string &rqi, string name);
    string addService(string &rqi, model::DeviceService service);
    string getAllService(string &rqi);
    string deleteService(string &rqi, string name);
};
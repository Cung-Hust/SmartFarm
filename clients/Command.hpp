#pragma once

#include "../models/models.hpp"
#include "../transports/IPublisher.hpp"

class Command
{
private:
    IPublisher *transport;
    string clientName;
    string resquestTopic;

public:
    Command(){};
    Command(string clientName, string resquestTopic);

    string issueGetCommand(string &rqi, string devId, vector<string> resources, map<string, string> options);
    string issueSetCommand(string &rqi, string devId, map<string, string> resources, map<string, string> options);
    string issueDiscoveryCommand(string &rqi, model::Device device, map<string, string> options);
};
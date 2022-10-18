#pragma once

#include "../models/models.hpp"
#include "../dtos/requestes.hpp"
#include "../transports/IPublisher.hpp"

namespace server
{
    class Command
    {
    private:
        string serverName;
        IPublisher *transport;

    public:
        Command(){};
        Command(string serverName);

        string forwardMessage(string toTopic, string rqi, string message);
        string response(string toTopic, string rqi, string error);
        string responseMultiReadings(string toTopic, string rqi, string error, vector<model::Reading> readings);
        string issueGetCommandToDeviceService(dto::IssueGetCommandRequest rootRequest, string toTopic);
        string issueSetCommandToDeviceService(dto::IssueSetCommandRequest rootRequest, string toTopic);
        string issueDiscoveryCommandToDeviceService(dto::IssueDiscoveryCommandRequest rootRequest, string toTopic);
    };
}

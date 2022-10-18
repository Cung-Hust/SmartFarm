#pragma once

#include "../models/models.hpp"
#include "../transports/IPublisher.hpp"

namespace server
{
    class Management
    {
    private:
        string serverName;
        string publishTopic;
        IPublisher *transport;

    public:
        Management(){};
        Management(string serverName);

        string response(string toTopic, string rqi, string error);
        string responseMetric(string toTopic, string rqi, string error, std::map<std::string, std::string> metric);
    };
}

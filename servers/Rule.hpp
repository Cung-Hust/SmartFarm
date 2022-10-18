#pragma once

#include "../models/models.hpp"
#include "../transports/IPublisher.hpp"

namespace server
{

    class Rule
    {
    private:
        string serverName;
        IPublisher *transport;

    public:
        Rule(){};
        Rule(string serverName);

        string response(string toTopic, string rqi, string error);
        string responseId(string toTopic, string rqi, string error, string id);
        string responseCount(string toTopic, string rqi, string error, uint64_t count);
        string responseRule(string toTopic, string rqi, string error, model::Rule rule);
        string responseMultiRule(string toTopic, string rqi, string error, vector<model::Rule> rules);
    };
}
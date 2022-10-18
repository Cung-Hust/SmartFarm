#pragma once

#include "../models/models.hpp"
#include "../transports/IPublisher.hpp"

namespace server
{
    class Coredata
    {
    private:
        string serverName;
        string publishTopic;
        IPublisher *transport;

    public:
        Coredata(){};
        Coredata(string serverName, string publishTopic);

        string response(string toTopic, string rqi, string error);
        string responseMultiReadings(string toTopic, string rqi, string error, vector<model::Reading> readings);
        string publishMultiReadings(string rqi, vector<model::Reading> readings);
    };
}

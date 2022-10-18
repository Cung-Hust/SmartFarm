#pragma once

#include "../models/models.hpp"
#include "../transports/IPublisher.hpp"

class Coredata
{
private:
    IPublisher *transport;
    string clientName;
    string resquestTopic;

public:
    Coredata(){};
    Coredata(string clientName, string resquestTopic);

    string addReadings(string &rqi, vector<model::Reading> readings);
    string getReadingOfDevice(string &rqi, string devId);
    string getAllReading(string &rqi, map<string, string> options);
};
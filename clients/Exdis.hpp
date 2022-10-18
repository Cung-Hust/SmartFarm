#pragma once

#include "../models/models.hpp"
#include "../transports/IPublisher.hpp"

class Exdis
{
private:
    IPublisher *transport;
    string clientName;
    string resquestTopic;

public:
    Exdis(){};
    Exdis(string clientName, string resquestTopic);

    string addNotify(string &rqi, model::Notification notify);
};
#pragma once

#include "../models/models.hpp"
#include "../dtos/requestes.hpp"
#include "../transports/IPublisher.hpp"

namespace server
{
    class ExDis
    {
    private:
        string serverName;
        IPublisher *transport;

    public:
        ExDis(){};
        ExDis(string serverName);

        string forwardMessage(string toTopic, string message);
    };
}
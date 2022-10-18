#include "ExDis.hpp"
#include "../dtos/requestes.hpp"
#include "../dtos/responses.hpp"
#include "../transports/Transports.hpp"
#include "../common/constants.hpp"
#include "../common/utils.hpp"

namespace server
{
    ExDis::ExDis(string serverName)
    {
        this->serverName = serverName;
        this->transport = ServerTransports::getPublisher();
    };

    string ExDis::forwardMessage(string toTopic, string message)
    {
        if (this->transport->publish(toTopic, message))
        {
            return "";
        };

        return "request to Topic: {" + toTopic + "} failed";
    }

}
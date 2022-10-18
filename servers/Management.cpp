#include "Management.hpp"
#include "../dtos/requestes.hpp"
#include "../dtos/responses.hpp"
#include "../transports/Transports.hpp"
#include "../common/constants.hpp"
#include "../common/utils.hpp"

namespace server
{
    Management::Management(string serverName)
    {
        this->serverName = serverName;
        this->transport = Transports::getPublisher();
    };

    string Management::response(string toTopic, string rqi, string error)
    {
        dto::HeaderResponse header = dto::createHeaderResponse(this->serverName, rqi, error);

        dto::SimpleResponse response;
        response.header = header;

        string responseStr = serialize<dto::SimpleResponse>(response);
        if (this->transport->publish(toTopic, responseStr))
        {
            return "";
        };

        return "request {" + rqi + "} failed";
    };

    string Management::responseMetric(string toTopic, string rqi, string error, std::map<std::string, std::string> metric)
    {
        dto::HeaderResponse header = dto::createHeaderResponse(this->serverName, rqi, error);

        dto::MetricResponse response;
        response.header = header;
        response.metric = metric;

        string responseStr = serialize<dto::MetricResponse>(response);
        if (this->transport->publish(toTopic, responseStr))
        {
            return "";
        };

        return "request {" + rqi + "} failed";
    };
}

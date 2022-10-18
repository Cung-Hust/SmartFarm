#include "Coredata.hpp"
#include "../dtos/requestes.hpp"
#include "../dtos/responses.hpp"
#include "../transports/Transports.hpp"
#include "../common/constants.hpp"
#include "../common/utils.hpp"
#include <unistd.h>

namespace server
{
    Coredata::Coredata(string serverName, string publishTopic)
    {
        this->serverName = serverName;
        this->publishTopic = publishTopic;
        this->transport = Transports::getPublisher();
    };

    string Coredata::response(string toTopic, string rqi, string error)
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

    string Coredata::responseMultiReadings(string toTopic, string rqi, string error, vector<model::Reading> readings)
    {
        dto::HeaderResponse header = dto::createHeaderResponse(this->serverName, rqi, error);
        vector<dto::Reading> dtoReadings = dto::fromModel(readings);

        dto::MultiReadingResponse response;
        response.header = header;
        response.readings = dtoReadings;

        string responseStr = serialize<dto::MultiReadingResponse>(response);
        if (this->transport->publish(toTopic, responseStr))
        {
            return "";
        };

        return "request {" + rqi + "} failed";
    };

    string Coredata::publishMultiReadings(string rqi, vector<model::Reading> readings)
    {
        dto::HeaderRequest header = dto::createHeaderRequest(this->serverName, OP_READINGS_ADD, rqi);
        vector<dto::Reading> dtoReadings = dto::fromModel(readings);

        dto::AddReadingsRequest request;
        request.header = header;
        request.readings = dtoReadings;

        string requestStr = serialize<dto::AddReadingsRequest>(request);
        cout << "<--------------------------------- SENT --------------------------------->" << endl;
        if (this->transport->publish(this->publishTopic, requestStr))
        {
            return "";
        };

        return "request {" + rqi + "} failed";
    };
}

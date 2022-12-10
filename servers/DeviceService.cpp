#include "DeviceService.hpp"
#include "../dtos/requestes.hpp"
#include "../dtos/responses.hpp"
#include "../transports/Transports.hpp"
#include "../common/constants.hpp"
#include "../common/utils.hpp"

namespace server
{
    DeviceService::DeviceService(string serverName, string commandResponseTopic, string coredataRequestTopic)
    {
        this->serverName = serverName;
        this->transport = Transports::getPublisher();
        this->commandResponseTopic = commandResponseTopic;
        this->coredataRequestTopic = coredataRequestTopic;
    };

    string DeviceService::responseSetCommand(string rqi, string error)
    {
        dto::HeaderResponse header = dto::createHeaderResponse(this->serverName, rqi, error);

        dto::SimpleResponse response;
        response.header = header;

        string responseStr = serialize<dto::SimpleResponse>(response);
        if (this->transport->publish(this->commandResponseTopic, responseStr))
        {
            return "";
        };

        return "request {" + rqi + "} failed";
    };

    string DeviceService::responseGetCommand(string rqi, string error, vector<model::Reading> readings)
    {
        dto::HeaderResponse header = dto::createHeaderResponse(this->serverName, rqi, error);
        vector<dto::Reading> dtoReadings = dto::fromModel(readings);

        dto::MultiReadingResponse response;
        response.header = header;
        response.readings = dtoReadings;

        string responseStr = serialize<dto::MultiReadingResponse>(response);
        if (this->transport->publish(this->commandResponseTopic, responseStr))
        {
            return "";
        };

        return "request {" + rqi + "} failed";
    };

    string DeviceService::responseTestCommand(string message)
    {
        if (this->transport->publish("RD_RESPONSE", message))
        {
            return "";
        };
    };

    string DeviceService::responseDiscoveryCommand(string rqi, string error, model::Device device)
    {
        dto::HeaderResponse header = dto::createHeaderResponse(this->serverName, rqi, error);
        dto::Device dtoDevice = dto::fromModel(device);

        dto::DeviceResponse response;
        response.header = header;
        response.device = dtoDevice;

        string responseStr = serialize<dto::DeviceResponse>(response);
        if (this->transport->publish(this->commandResponseTopic, responseStr))
        {
            return "";
        };

        return "request {" + rqi + "} failed";
    }

    string DeviceService::publishMultiReadings(string rqi, vector<model::Reading> readings)
    {
        dto::HeaderRequest header = dto::createHeaderRequest(this->serverName, OP_READINGS_ADD, rqi);
        vector<dto::Reading> dtoReadings = dto::fromModel(readings);

        dto::AddReadingsRequest request;
        request.header = header;
        request.readings = dtoReadings;

        string requestStr = serialize<dto::AddReadingsRequest>(request);
        if (this->transport->publish(this->coredataRequestTopic, requestStr))
        {
            return "";
        };

        return "request {" + rqi + "} failed";
    };
}

#include "Command.hpp"
#include "../dtos/requestes.hpp"
#include "../dtos/responses.hpp"
#include "../transports/Transports.hpp"
#include "../common/constants.hpp"
#include "../common/utils.hpp"

namespace server
{
    Command::Command(string serverName)
    {
        this->serverName = serverName;
        this->transport = Transports::getPublisher();
    };

    string Command::forwardMessage(string toTopic, string rqi, string message)
    {
        if (this->transport->publish(toTopic, message))
        {
            return "";
        };

        return "request {" + rqi + "} failed";
    }

    string Command::response(string toTopic, string rqi, string error)
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

    string Command::responseMultiReadings(string toTopic, string rqi, string error, vector<model::Reading> readings)
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

    string Command::issueGetCommandToDeviceService(dto::IssueGetCommandRequest rootRequest, string toTopic)
    {
        rootRequest.header.client = this->serverName;
        rootRequest.header.op = OP_DEVICE_GET;

        string requestStr = serialize<dto::IssueGetCommandRequest>(rootRequest);
        if (this->transport->publish(toTopic, requestStr))
        {
            return "";
        };

        return "request {" + rootRequest.header.rqi + "} failed";
    }

    string Command::issueSetCommandToDeviceService(dto::IssueSetCommandRequest rootRequest, string toTopic)
    {
        rootRequest.header.client = this->serverName;
        rootRequest.header.op = OP_DEVICE_SET;

        string requestStr = serialize<dto::IssueSetCommandRequest>(rootRequest);
        if (this->transport->publish(toTopic, requestStr))
        {
            return "";
        };

        return "request {" + rootRequest.header.rqi + "} failed";
    }

    string Command::issueDiscoveryCommandToDeviceService(dto::IssueDiscoveryCommandRequest rootRequest, string toTopic)
    {
        rootRequest.header.client = this->serverName;
        rootRequest.header.op = OP_DEVICE_DISCOVERY;

        string requestStr = serialize<dto::IssueDiscoveryCommandRequest>(rootRequest);
        if (this->transport->publish(toTopic, requestStr))
        {
            return "";
        };

        return "request {" + rootRequest.header.rqi + "} failed";
    }
}

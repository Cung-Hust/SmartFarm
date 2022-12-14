#include "Command.hpp"
#include "../dtos/requestes.hpp"
#include "../transports/Transports.hpp"
#include "../common/constants.hpp"
#include "../common/utils.hpp"

Command::Command(string clientName, string resquestTopic)
{
    this->transport = Transports::getPublisher();
    this->clientName = clientName;
    this->resquestTopic = resquestTopic;
};

string Command::issueGetCommand(string &rqi, string devId, vector<string> resources, map<string, string> options)
{
    if (rqi == "")
    {
        rqi = genUuid();
    }

    dto::HeaderRequest header = dto::createHeaderRequest(this->clientName, OP_COMMAND_GET, rqi);

    dto::IssueGetCommandRequest request;
    request.header = header;
    request.id = devId;
    request.resources = resources;
    request.options = options;

    string requestStr = serialize<dto::IssueGetCommandRequest>(request);
    if (this->transport->publish(this->resquestTopic, requestStr))
    {
        return "";
    };

    return "request {" + rqi + "} failed";
}

string Command::issueSetCommand(string &rqi, string devId, map<string, string> resources, map<string, string> options)
{
    if (rqi == "")
    {
        rqi = genUuid();
    }

    dto::HeaderRequest header = dto::createHeaderRequest(this->clientName, OP_COMMAND_SET, rqi);

    dto::IssueSetCommandRequest request;
    request.header = header;
    request.id = devId;
    request.resources = resources;
    request.options = options;

    string requestStr = serialize<dto::IssueSetCommandRequest>(request);
    if (this->transport->publish(this->resquestTopic, requestStr))
    {
        return "";
    };

    return "request {" + rqi + "} failed";
}

string Command::issueDiscoveryCommand(string &rqi, model::Device device, map<string, string> options)
{
        if (rqi == "")
    {
        rqi = genUuid();
    }

    dto::HeaderRequest header = dto::createHeaderRequest(this->clientName, OP_COMMAND_DISCOVERY, rqi);
    dto::Device dtoDev = dto::fromModel(device);

    dto::IssueDiscoveryCommandRequest request;
    request.header = header;
    request.device = dtoDev;
    request.options = options;

    string requestStr = serialize<dto::IssueDiscoveryCommandRequest>(request);
    if (this->transport->publish(this->resquestTopic, requestStr))
    {
        return "";
    };

    return "request {" + rqi + "} failed";
}
#include "Coredata.hpp"
#include "../dtos/requestes.hpp"
#include "../transports/Transports.hpp"
#include "../common/constants.hpp"
#include "../common/utils.hpp"

Coredata::Coredata(string clientName, string resquestTopic)
{
    this->transport = Transports::getPublisher();
    this->clientName = clientName;
    this->resquestTopic = resquestTopic;
};

string Coredata::addReadings(string &rqi, vector<model::Reading> readings)
{
    if (rqi == "")
    {
        rqi = genUuid();
    }

    dto::HeaderRequest header = dto::createHeaderRequest(this->clientName, OP_READINGS_ADD, rqi);
    vector<dto::Reading> dtoReadings = dto::fromModel(readings);

    dto::AddReadingsRequest request;
    request.header = header;
    request.readings = dtoReadings;

    string requestStr = serialize<dto::AddReadingsRequest>(request);
    if (this->transport->publish(this->resquestTopic, requestStr))
    {
        return "";
    };

    return "request {" + rqi + "} failed";
}

string Coredata::getReadingOfDevice(string &rqi, string devId)
{
    if (rqi == "")
    {
        rqi = genUuid();
    }

    dto::HeaderRequest header = dto::createHeaderRequest(this->clientName, OP_READINGS_LIST, rqi);
    map<string, string> options;
    options["id"] = devId;

    dto::RequestWithOptions request;
    request.header = header;
    request.options = options;

    string requestStr = serialize<dto::RequestWithOptions>(request);
    if (this->transport->publish(this->resquestTopic, requestStr))
    {
        return "";
    };

    return "request {" + rqi + "} failed";
}

string Coredata::getAllReading(string &rqi, map<string, string> options)
{
    if (rqi == "")
    {
        rqi = genUuid();
    }

    dto::HeaderRequest header = dto::createHeaderRequest(this->clientName, OP_READINGS_LIST, rqi);

    dto::RequestWithOptions request;
    request.header = header;
    request.options = options;

    string requestStr = serialize<dto::RequestWithOptions>(request);
    if (this->transport->publish(this->resquestTopic, requestStr))
    {
        return "";
    };

    return "request {" + rqi + "} failed";
}
#include "Exdis.hpp"
#include "../dtos/requestes.hpp"
#include "../transports/Transports.hpp"
#include "../common/constants.hpp"
#include "../common/utils.hpp"

Exdis::Exdis(string clientName, string resquestTopic)
{
    this->transport = Transports::getPublisher();
    this->clientName = clientName;
    this->resquestTopic = resquestTopic;
};

string Exdis::addNotify(string &rqi, model::Notification notify)
{
    if (rqi == "")
    {
        rqi = genUuid();
    }

    dto::HeaderRequest header = dto::createHeaderRequest(this->clientName, OP_NOTIFY_ADD, rqi);
    dto::Notification dtoNotify = dto::fromModel(notify);

    dto::AddNotificationRequest request;
    request.header = header;
    request.notify = dtoNotify;

    string requestStr = serialize<dto::AddNotificationRequest>(request);
    if (this->transport->publish(this->resquestTopic, requestStr))
    {
        return "";
    };

    return "request {" + rqi + "} failed";
}
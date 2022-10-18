#include "Rule.hpp"
#include "../dtos/requestes.hpp"
#include "../transports/Transports.hpp"
#include "../common/constants.hpp"
#include "../common/utils.hpp"

Rule::Rule(string clientName, string resquestTopic)
{
    this->transport = Transports::getPublisher();
    this->clientName = clientName;
    this->resquestTopic = resquestTopic;
};

string Rule::addRule(string &rqi, model::Rule rule)
{
    if (rqi == "")
    {
        rqi = genUuid();
    }

    dto::HeaderRequest header = dto::createHeaderRequest(this->clientName, OP_RULE_ADD, rqi);
    dto::Rule dtoRule = dto::fromModel(rule);

    dto::AddRuleRequest request;
    request.header = header;
    request.rule = dtoRule;

    string requestStr = serialize<dto::AddRuleRequest>(request);
    if (this->transport->publish(this->resquestTopic, requestStr))
    {
        return "";
    };

    return "request {" + rqi + "} failed";
};

string Rule::updateRule(string &rqi, model::Rule rule)
{
    if (rqi == "")
    {
        rqi = genUuid();
    }

    dto::HeaderRequest header = dto::createHeaderRequest(this->clientName, OP_RULE_UPDATE, rqi);
    dto::UpdateRule dtoRule = dto::fromRuleModel(rule);

    dto::UpdateRuleRequest request;
    request.header = header;
    request.rule = dtoRule;

    string requestStr = serialize<dto::UpdateRuleRequest>(request);
    deleteUpdateRule(dtoRule);
    if (this->transport->publish(this->resquestTopic, requestStr))
    {
        return "";
    };

    return "request {" + rqi + "} failed";
};

string Rule::getAllRule(string &rqi, map<string, string> options)
{
    if (rqi == "")
    {
        rqi = genUuid();
    }

    dto::HeaderRequest header = dto::createHeaderRequest(this->clientName, OP_RULE_LIST, rqi);

    dto::RequestWithOptions request;
    request.header = header;
    request.options = options;

    string requestStr = serialize<dto::RequestWithOptions>(request);
    if (this->transport->publish(this->resquestTopic, requestStr))
    {
        return "";
    };

    return "request {" + rqi + "} failed";
};

string Rule::getRule(string &rqi, string id)
{
    if (rqi == "")
    {
        rqi = genUuid();
    }

    dto::HeaderRequest header = dto::createHeaderRequest(this->clientName, OP_RULE_READ, rqi);

    dto::RequestWithId request;
    request.header = header;
    request.id = id;

    string requestStr = serialize<dto::RequestWithId>(request);
    if (this->transport->publish(this->resquestTopic, requestStr))
    {
        return "";
    };

    return "request {" + rqi + "} failed";
};

string Rule::deleteRule(string &rqi, string id)
{
    if (rqi == "")
    {
        rqi = genUuid();
    }

    dto::HeaderRequest header = dto::createHeaderRequest(this->clientName, OP_RULE_DELETE, rqi);

    dto::RequestWithId request;
    request.header = header;
    request.id = id;

    string requestStr = serialize<dto::RequestWithId>(request);
    if (this->transport->publish(this->resquestTopic, requestStr))
    {
        return "";
    };

    return "request {" + rqi + "} failed";
};

string Rule::updateStateRule(string &rqi, string id, ActiveState state)
{
    cout << "updateStateRule :: " << state << endl;
    if (rqi == "")
    {
        rqi = genUuid();
    }

    dto::HeaderRequest header = dto::createHeaderRequest(this->clientName, OP_RULE_STATE, rqi);

    dto::SetStateRuleRequest request;
    request.header = header;
    request.id = id;
    request.activestate = state;

    string requestStr = serialize<dto::SetStateRuleRequest>(request);
    if (this->transport->publish(this->resquestTopic, requestStr))
    {
        return "";
    };

    return "request {" + rqi + "} failed";
};

string Rule::triggerRule(string &rqi, string id)
{
    if (rqi == "")
    {
        rqi = genUuid();
    }

    dto::HeaderRequest header = dto::createHeaderRequest(this->clientName, OP_RULE_TRIGGER, rqi);

    dto::TriggerRuleRequest request;
    request.header = header;
    request.id = id;
    request.triggerstate = TriggerState::ON;

    string requestStr = serialize<dto::TriggerRuleRequest>(request);
    if (this->transport->publish(this->resquestTopic, requestStr))
    {
        return "";
    };

    return "request {" + rqi + "} failed";
};

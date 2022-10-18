#include "Rule.hpp"
#include "../dtos/requestes.hpp"
#include "../dtos/responses.hpp"
#include "../transports/Transports.hpp"
#include "../common/constants.hpp"
#include "../common/utils.hpp"

namespace server
{
    Rule::Rule(string serverName)
    {
        this->serverName = serverName;
        this->transport = Transports::getPublisher();
    };

    string Rule::response(string toTopic, string rqi, string error)
    {
        // cout << "start response function" << endl;
        dto::HeaderResponse header = dto::createHeaderResponse(this->serverName, rqi, error);

        dto::SimpleResponse response;
        response.header = header;

        string responseStr = serialize<dto::SimpleResponse>(response);
        // cout << "before sending message" << endl;
        if (this->transport->publish(toTopic, responseStr))
        {
            return "";
        };

        return "request {" + rqi + "} failed";
    };

    string Rule::responseId(string toTopic, string rqi, string error, string id)
    {
        dto::HeaderResponse header = dto::createHeaderResponse(this->serverName, rqi, error);

        dto::ResponseWithId response;
        response.header = header;
        response.id = id;

        string responseStr = serialize<dto::ResponseWithId>(response);
        if (this->transport->publish(toTopic, responseStr))
        {
            return "";
        };

        return "request {" + rqi + "} failed";
    };

    string Rule::responseCount(string toTopic, string rqi, string error, uint64_t count)
    {
        dto::HeaderResponse header = dto::createHeaderResponse(this->serverName, rqi, error);

        dto::CountResponse response;
        response.header = header;
        response.count = count;

        string responseStr = serialize<dto::CountResponse>(response);
        if (this->transport->publish(toTopic, responseStr))
        {
            return "";
        };

        return "request {" + rqi + "} failed";
    };

    string Rule::responseRule(string toTopic, string rqi, string error, model::Rule rule)
    {
        dto::HeaderResponse header = dto::createHeaderResponse(this->serverName, rqi, error);
        dto::Rule dtoRule = dto::fromModel(rule);

        dto::RuleResponse response;
        response.header = header;
        response.rule = dtoRule;

        string responseStr = serialize<dto::RuleResponse>(response);
        if (this->transport->publish(toTopic, responseStr))
        {
            return "";
        };

        return "request {" + rqi + "} failed";
    };

    string Rule::responseMultiRule(string toTopic, string rqi, string error, vector<model::Rule> rules)
    {
        dto::HeaderResponse header = dto::createHeaderResponse(this->serverName, rqi, error);
        vector<dto::Rule> dtoRules;

        for (auto &rule : rules)
        {
            dto::Rule dtoRule = dto::fromModel(rule);
            dtoRules.push_back(dtoRule);
        }

        dto::MultiRuleResponse response;
        response.header = header;
        response.rules = dtoRules;

        string responseStr = serialize<dto::MultiRuleResponse>(response);
        if (this->transport->publish(toTopic, responseStr))
        {
            return "";
        };

        return "request {" + rqi + "} failed";
    };
}
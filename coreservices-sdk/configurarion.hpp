/*
 * configurarion.hpp
 *
 *  Created on: Aug 2, 2022
 *      Author: phanhai
 */

#pragma once

#include <iostream>
#include <string>
#include <map>

#include <fstream>
#include "../json/json.hpp"

#define METATDATA_SERVICE_JSON_KEY "metadata"
#define COMMAND_SERVICE_JSON_KEY "command"
#define COREDATA_SERVICE_JSON_KEY "coredata"
#define MANAGEMENT_SERVICE_JSON_KEY "management"
#define RULE_SERVICE_JSON_KEY "rule"

#define DEFAULT_CLIENT "exdis"

namespace coreservices
{
    using namespace std;
    using json = nlohmann::json;

    struct MetadataServiceConfiguration
    {
        string name;
        string requestServiceTopic;
        string responseServiceTopic;
        string profileFile;
        string resourceFile;
        map<string, string> requestTopicMap;
        map<string, string> responseTopicMap;

        void loadConfig(string filename)
        {
            std::ifstream f(filename);
            json data = json::parse(f);

            this->requestTopicMap = data.value("client-request-topics", map<string, string>{});
            this->responseTopicMap = data.value("client-response-topics", map<string, string>{});

            if (data.contains(METATDATA_SERVICE_JSON_KEY))
            {
                auto transport = data[METATDATA_SERVICE_JSON_KEY].get<json>();
                this->name = transport.value("name", METATDATA_SERVICE_JSON_KEY);
                this->profileFile = transport.value("profilefile", "");
                this->resourceFile = transport.value("resourcefile", "");
                this->requestServiceTopic = transport.value("requesttopic", "");
                this->responseServiceTopic = transport.value("responsetopic", "");
            }
        }

        string getRequestTopic(string clientName)
        {
            if (clientName == "")
            {
                clientName = DEFAULT_CLIENT;
            }
            return this->requestTopicMap[clientName];
        }

        string getReponseTopic(string clientName)
        {
            if (clientName == "")
            {
                clientName = DEFAULT_CLIENT;
            }
            return this->responseTopicMap[clientName];
        }
    };

    struct CommandServiceConfiguration
    {
        string name;
        string requestServiceTopic;
        string responseServiceTopic;
        int timeoutSeconds;
        map<string, string> requestTopicMap;
        map<string, string> responseTopicMap;

        void loadConfig(string filename)
        {
            std::ifstream f(filename);
            json data = json::parse(f);

            this->requestTopicMap = data.value("client-request-topics", map<string, string>{});
            this->responseTopicMap = data.value("client-response-topics", map<string, string>{});

            if (data.contains(COMMAND_SERVICE_JSON_KEY))
            {
                auto transport = data[COMMAND_SERVICE_JSON_KEY].get<json>();
                this->name = transport.value("name", COMMAND_SERVICE_JSON_KEY);
                this->requestServiceTopic = transport.value("requesttopic", "");
                this->responseServiceTopic = transport.value("responsetopic", "");
                this->timeoutSeconds = transport.value("timeout", 10);
            }
        }

        string getRequestTopic(string clientName)
        {
            if (clientName == "")
            {
                clientName = DEFAULT_CLIENT;
            }
            return this->requestTopicMap[clientName];
        }

        string getReponseTopic(string clientName)
        {
            if (clientName == "")
            {
                clientName = DEFAULT_CLIENT;
            }
            return this->responseTopicMap[clientName];
        }
    };

    struct CoredataServiceConfiguration
    {
        string name;
        string requestServiceTopic;
        string responseServiceTopic;
        string publishServiceTopic;
        map<string, string> requestTopicMap;
        map<string, string> responseTopicMap;

        void loadConfig(string filename)
        {
            std::ifstream f(filename);
            json data = json::parse(f);

            this->requestTopicMap = data.value("client-request-topics", map<string, string>{});
            this->responseTopicMap = data.value("client-response-topics", map<string, string>{});

            if (data.contains(COREDATA_SERVICE_JSON_KEY))
            {
                auto transport = data[COREDATA_SERVICE_JSON_KEY].get<json>();
                this->name = transport.value("name", COREDATA_SERVICE_JSON_KEY);
                this->requestServiceTopic = transport.value("requesttopic", "");
                this->responseServiceTopic = transport.value("responsetopic", "");
                this->publishServiceTopic = transport.value("publishtopic", "");
            }
        }

        string getRequestTopic(string clientName)
        {
            if (clientName == "")
            {
                clientName = DEFAULT_CLIENT;
            }
            return this->requestTopicMap[clientName];
        }

        string getReponseTopic(string clientName)
        {
            if (clientName == "")
            {
                clientName = DEFAULT_CLIENT;
            }
            return this->responseTopicMap[clientName];
        }
    };

    struct ManagementServiceConfiguration
    {
        string name;
        string requestServiceTopic;
        string responseServiceTopic;
        map<string, string> requestTopicMap;
        map<string, string> responseTopicMap;
        map<string, string> servicePathMap;

        void loadConfig(string filename)
        {
            std::ifstream f(filename);
            json data = json::parse(f);

            this->requestTopicMap = data.value("client-request-topics", map<string, string>{});
            this->responseTopicMap = data.value("client-response-topics", map<string, string>{});
            this->servicePathMap = data.value("service-paths", map<string, string>{});

            if (data.contains(MANAGEMENT_SERVICE_JSON_KEY))
            {
                auto transport = data[MANAGEMENT_SERVICE_JSON_KEY].get<json>();
                this->name = transport.value("name", MANAGEMENT_SERVICE_JSON_KEY);
                this->requestServiceTopic = transport.value("requesttopic", "");
                this->responseServiceTopic = transport.value("responsetopic", "");
            }
        }

        string getRequestTopic(string clientName)
        {
            if (clientName == "")
            {
                clientName = DEFAULT_CLIENT;
            }
            return this->requestTopicMap[clientName];
        }

        string getReponseTopic(string clientName)
        {
            if (clientName == "")
            {
                clientName = DEFAULT_CLIENT;
            }
            return this->responseTopicMap[clientName];
        }

        string getServicePath(string service)
        {
            if (service == "")
            {
                service = DEFAULT_CLIENT;
            }
            return this->servicePathMap[service];
        }
    };

    struct RuleServiceConfiguration
    {
        string name;
        string requestServiceTopic;
        string responseServiceTopic;
        string readingServiceTopic;
        map<string, string> requestTopicMap;
        map<string, string> responseTopicMap;

        void loadConfig(string filename)
        {
            std::ifstream f(filename);
            json data = json::parse(f);

            this->requestTopicMap = data.value("client-request-topics", map<string, string>{});
            this->responseTopicMap = data.value("client-response-topics", map<string, string>{});

            if (data.contains(RULE_SERVICE_JSON_KEY))
            {
                auto transport = data[RULE_SERVICE_JSON_KEY].get<json>();
                this->name = transport.value("name", RULE_SERVICE_JSON_KEY);
                this->requestServiceTopic = transport.value("requesttopic", "");
                this->responseServiceTopic = transport.value("responsetopic", "");
                this->readingServiceTopic = transport.value("readingtopic", "");
            }
        }

        string getRequestTopic(string clientName)
        {
            if (clientName == "")
            {
                clientName = DEFAULT_CLIENT;
            }
            return this->requestTopicMap[clientName];
        }

        string getReponseTopic(string clientName)
        {
            if (clientName == "")
            {
                clientName = DEFAULT_CLIENT;
            }
            return this->responseTopicMap[clientName];
        }
    };
}
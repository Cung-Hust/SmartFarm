/*
 * configurarion.hpp
 *
 *  Created on: Aug 2, 2022
 *      Author: phanhai
 */
#pragma once

#include <string>
#include <map>

#include <fstream>
#include "json/json.hpp"

#define CONFIG_JSON_KEY "service"
#define DEFAULT_CLIENT ""

namespace deviceservice
{
    using namespace std;
    using json = nlohmann::json;

    struct DeviceServiceConfiguration
    {
        string name;
        string requestServiceTopic;
        string responseServiceTopic;
        uint64_t queueSize;
        map<string, string> requestTopicMap;
        map<string, string> responseTopicMap;
        json userConfig;

        void loadConfig(string filename)
        {
            std::ifstream f(filename);
            json data = json::parse(f);

            this->requestTopicMap = data.value("client-request-topics", map<string, string>{});
            this->responseTopicMap = data.value("client-response-topics", map<string, string>{});
            this->userConfig = data.value("user-config", json());

            if (data.contains(CONFIG_JSON_KEY))
            {
                auto serviceCfg = data[CONFIG_JSON_KEY].get<json>();
                this->name = serviceCfg.value("name", CONFIG_JSON_KEY);
                this->queueSize = serviceCfg.value("maxqueue", 0);
                this->requestServiceTopic = serviceCfg.value("requesttopic", "");
                this->responseServiceTopic = serviceCfg.value("responsetopic", "");
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

        map<string, string> getMyConfig()
        {
            return this->userConfig;
        }
    };
}
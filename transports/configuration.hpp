#pragma once

#include <string>
#include <map>

#include <fstream>
#include "../json/json.hpp"

using namespace std;
using json = nlohmann::json;

#define TRANSPORT_CONFIG_JSON_KEY "transport"
#define SERVER_TRANSPORT_CONFIG_JSON_KEY "server-transport"

struct TransportConfiguration
{
    string type;
    map<string, string> params;

    void loadConfig(string filename)
    {
        std::ifstream f(filename);
        json data = json::parse(f);

        if (data.contains(TRANSPORT_CONFIG_JSON_KEY))
        {
            auto transport = data[TRANSPORT_CONFIG_JSON_KEY].get<json>();
            this->type = transport.value("type", "");
            this->params = transport.value("params", map<string, string>{});
        }
    }

    void loadServerConfig(string filename)
    {
        std::ifstream f(filename);
        json data = json::parse(f);

        if (data.contains(SERVER_TRANSPORT_CONFIG_JSON_KEY))
        {
            auto transport = data[SERVER_TRANSPORT_CONFIG_JSON_KEY].get<json>();
            this->type = transport.value("type", "");
            this->params = transport.value("params", map<string, string>{});
        }
    }
};
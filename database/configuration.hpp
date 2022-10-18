#pragma once

#include <string>
#include <map>

#include <fstream>
#include "../json/json.hpp"

using namespace std;
using json = nlohmann::json;

#define DATABASE_CONFIG_JSON_KEY "database"

struct DatabaseConfiguration
{
    string type;
    map<string, string> params;

    void loadConfig(string filename)
    {
        std::ifstream f(filename);
        json data = json::parse(f);

        if (data.contains(DATABASE_CONFIG_JSON_KEY))
        {
            auto transport = data[DATABASE_CONFIG_JSON_KEY].get<json>();
            this->type = transport.value("type", "");
            this->params = transport.value("params", map<string, string>{});
        }
    }
};
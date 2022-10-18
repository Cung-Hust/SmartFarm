/*
 * handlerCmd.hpp
 *
 *  Created on: Jul 27, 2022
 *      Author: rd
 */

#pragma once

#include <iostream>
#include "json/json.hpp"
#include "clients/Rule.hpp"

using namespace std;
using nlohmann::json;

class GetInfoHandler
{
private:
    string cmd;

public:
    GetInfoHandler();
    ~GetInfoHandler();
    // void reporter(bool isDetach, string message);
    void reporter(string message);
    void run(string message);
    void handler(string message);
};

/*
 * handlerCmd.cpp
 *
 *  Created on: Jul 27, 2022
 *      Author: rd
 */

#include "../servers/ExDis.hpp"
#include "../common/utils.hpp"
#include "GetInfoHandler.hpp"
#include "../database/Database.hpp"
#include "../transports/Transports.hpp"

#include <stdlib.h>
#include <ctime>
#include <unistd.h>
#include <stdint.h>
#include <thread>
#include <pthread.h>

using namespace std;

GetInfoHandler::GetInfoHandler(){};

GetInfoHandler::~GetInfoHandler(){};

void GetInfoHandler::reporter(string message)
{
    this->handler(message);
}

void GetInfoHandler::run(string responseServiceTopic)
{
    string data;
    while (true)
    {
        if (Transports::getMessager()->getMessage(responseServiceTopic, data))
        {
            this->handler(data);
        }
        usleep(50000);
    }
}
void GetInfoHandler::handler(string data)
{
    this->cmd = data;
    // cout << "GetInfoHandler :: run" << data << endl;
    server::ExDis server("ExDis");

    dto::AddReadingsRequest req;
    try
    {
        deserialize<dto::AddReadingsRequest>(this->cmd, req);
    }
    catch (...)
    {
        std::cout << "Parse message error" << std::endl;
        return;
    }

    if (req.readings.size() <= 0)
    {
        std::cout << "Reading array is null" << std::endl;
        return;
    }

    string rqi = req.header.rqi;
    auto readings = req.readings;
    string deviceId = readings[0].deviceId;

    json sensors = json::array();
    json msgToCloud_j;

    string err;
    auto device = Db::getDb()->readDevice(deviceId, err);
    if (err != "")
    {
        cout << "err..." << err << std::endl;
        return;
    }

    string profileName = device.profileName;
    cout << "profileName: "
         << "--> " << profileName << " <--" << endl;
    if (profileName == "Relay")
    {
        int relayState;
        json relays;

        for (auto &reading : readings)
        {
            if (reading.resourceName == "OnOff")
            {
                relayState = reading.value == "true" ? 1 : 0;
                json relay = {
                    {"ID", reading.deviceId},
                    {"Type", 1},
                    {"Pull", relayState},
                };
                relays.push_back(relay);
            }
        }
        msgToCloud_j = {
            {"Head",
             {{"IDMessage", rqi},
              {"TypeMessage", 0},
              {"FormData", 1},
              {"IDGateway", getMacGW()},
              {"DateTime", time(0)}}},
            {"Control", relays}};
    }
    else if (profileName == "Lora")
    {
        int type = 0;
        auto resources = Db::getDb()->listReading(deviceId);
        uint8_t battery = 100;
        uint8_t reportSensorTime = 0;
        bool isConfig = true;
        float value = 0;

        for (auto &reading : readings)
        {
            string resourceName = reading.resourceName;
            string valueStr = reading.value;
            cout << "resourceName:  " << resourceName << endl;

            if (resourceName == "Battery")
            {
                battery = stoi(valueStr);
                break;
            }
            if (resourceName == "ReportSensorTime")
            {
                reportSensorTime = stoi(valueStr);
                return;
            }
        }
        isConfig = reportSensorTime == 1 ? false : true;
        for (auto &reading : readings)
        {
            string resourceName = reading.resourceName;
            string valueStr = reading.value;
            cout << "resourceName:  " << resourceName << endl;

            if (resourceName == "Battery")
            {
                continue;
            }
            if (resourceName == "Temperature")
            {
                type = 0;
                value = roundf(stoi(valueStr) * 100) / 100;
            }
            else if (resourceName == "Humidity")
            {
                type = 1;
                value = roundf(stoi(valueStr) * 100) / 100;
            }
            else if (resourceName == "SoilMoisture")
            {
                type = 2;
                value = roundf(stoi(valueStr) * 100) / 100;
            }
            else if (resourceName == "Lux")
            {
                type = 3;
                value = roundf(stoi(valueStr) * 100) / 100;
            }
            else if (resourceName == "CO2")
            {
                type = 4;
                value = roundf(stoi(valueStr) * 100) / 100;
            }
            else if (resourceName == "EC")
            {
                type = 5;
                value = roundf(stoi(valueStr) * 100) / 100;
            }
            else if (resourceName == "PH")
            {
                type = 6;
                value = roundf(stoi(valueStr) * 100) / 100;
            }
            json sensor = {
                {"ID", deviceId},
                {"TypeSensor", type},
                {"Value", value},
                {"Battery", battery},
                {"IsConfig", isConfig},
            };
            sensors.push_back(sensor);
        }
        msgToCloud_j = {
            {"Head",
             {{"IDMessage", rqi},
              {"TypeMessage", 0},
              {"FormData", 0},
              {"IDGateway", getMacGW()},
              {"DateTime", time(0)}}},
            {"Sensor", sensors}};
    }
    else if (profileName == "Gateway")
    {
        int gatewayMode;
        json gateways;

        for (auto &reading : readings)
        {
            if (reading.resourceName == "Mode")
            {
                gatewayMode = stoi(reading.value);
                json gateway = {
                    {"Mode", to_string(gatewayMode)},
                };
                gateways.push_back(gateway);
            }
        }
        msgToCloud_j = {
            {"Head",
             {{"IDMessage", rqi},
              {"TypeMessage", 0},
              {"FormData", 8},
              {"IDGateway", getMacGW()},
              {"DateTime", time(0)}}},
            {"Gateway", gateways}};
    }
    cout << "Msg send to cloud ... " << msgToCloud_j << endl;
    json devList;
    std::string msgToCloud_str = msgToCloud_j.dump();
    string rspTopic = getMacGW() + "gateway";
    server.forwardMessage(rspTopic, msgToCloud_str);
}

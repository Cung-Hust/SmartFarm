/*
 * handlerCmd.cpp
 *
 *  Created on: Jul 27, 2022
 *      Author: rd
 */

#include "../servers/ExDis.hpp"
#include "../common/utils.hpp"
#include "../common/constants.hpp"
#include "GetResponseHandler.hpp"
#include "../database/Database.hpp"
#include "../transports/Transports.hpp"

#include <stdlib.h>
#include <ctime>
#include <unistd.h>
#include <stdint.h>
#include <thread>
#include <pthread.h>

using namespace std;

GetResponseHandler::GetResponseHandler(){};

GetResponseHandler::~GetResponseHandler(){};

void GetResponseHandler::responder(string message)
{
    this->handler(message);
}

void GetResponseHandler::run(string readingServiceTopic)
{
    string data;
    while (true)
    {
        if (ServerTransports::getMessager()->getMessage(readingServiceTopic, data))
        {
            this->handler(data);
        }
        usleep(5000);
    }
}

void GetResponseHandler::handler(string data)
{
    this->cmd = data;
    server::ExDis server("ExDis");

    dto::AddDeviceRequest req;
    try
    {
        deserialize<dto::AddDeviceRequest>(this->cmd, req);
    }
    catch (...)
    {
        std::cout << "Parse message error" << std::endl;
        return;
    }

    if (req.header.op == OP_DEVICE_UPDATE)
    {
        return;
    }
    string rqi = req.header.rqi;
    auto device = req.device;
    string deviceId = device.id;
    if (deviceId == "")
    {
        cout << "Message error !" << endl;
        return;
    }

    json sensors = json::array();
    json msgToCloud_j;
    string typeSensor = device.protocols["Type"];
    int cnv_typeSensor = 0;
    int min_value = 0;
    int max_value = 0;
    string devName;

    if (typeSensor == TEMP_SENSOR_NAME)
    {
        cnv_typeSensor = TEMP_SENSOR_TYPE;
        min_value = TEMP_SENSOR_MIN_VALUE;
        max_value = TEMP_SENSOR_MAX_VALUE;
        devName = "CB Nhiệt độ " + device.protocols["MAC"];
    }
    else if (typeSensor == SOIL_SENSOR_NAME)
    {
        cnv_typeSensor = SOIL_SENSOR_TYPE;
        min_value = SOIL_SENSOR_MIN_VALUE;
        max_value = SOIL_SENSOR_MAX_VALUE;
        devName = "CB Độ ẩm đất " + device.protocols["MAC"];
    }
    else if (typeSensor == LIGHT_SENSOR_NAME)
    {
        cnv_typeSensor = LIGHT_SENSOR_TYPE;
        min_value = LIGHT_SENSOR_MIN_VALUE;
        max_value = LIGHT_SENSOR_MAX_VALUE;
        devName = "CB Ánh sáng " + device.protocols["MAC"];
    }
    else if (typeSensor == CO2_SENSOR_NAME)
    {
        cnv_typeSensor = CO2_SENSOR_TYPE;
        min_value = CO2_SENSOR_MIN_VALUE;
        max_value = CO2_SENSOR_MAX_VALUE;
        devName = "CB CO2 " + device.protocols["MAC"];
    }
    else if (typeSensor == EC_SENSOR_NAME)
    {
        cnv_typeSensor = EC_SENSOR_TYPE;
        min_value = EC_SENSOR_MIN_VALUE;
        max_value = EC_SENSOR_MAX_VALUE;
        devName = "CB EC " + device.protocols["MAC"];
    }
    else if (typeSensor == PH_SENSOR_NAME)
    {
        cnv_typeSensor = PH_SENSOR_TYPE;
        min_value = PH_SENSOR_MIN_VALUE;
        max_value = PH_SENSOR_MAX_VALUE;
        devName = "CB PH " + device.protocols["MAC"];
    }
    else
    {
        return;
    }

    if (cnv_typeSensor == 0)
    {
        devName = "CB Nhiệt độ - Độ ẩm " + device.protocols["MAC"];
        json j_device_ = {{
            {"ID", deviceId},
            {"Type", 0},
            {"Action", 1},
            {"Name", devName},
            {"TypeSensor", 1},
            {"Min", 0},
            {"Max", 100},
            {"Param_a", 0},
            {"Param_b", 0},
            {"Param_c", 0},
            {"Value", 0},
            {"Battery", 100},
            {"Delay", 10},
            {"isConfig", true},
        }};
        json msgToCloud_j_ = {
            {"Head",
             {{"IDMessage", rqi},
              {"TypeMessage", 0},
              {"FormData", 2},
              {"IDGateway", getMacGW()},
              {"DateTime", time(0)}}},
            {"Device", j_device_}};
        std::string msgToCloud_str_ = msgToCloud_j_.dump();
        string rspTopic = getMacGW() + "gateway";
        server.forwardMessage(rspTopic, msgToCloud_str_);
    }

    json j_device = {{
        {"ID", deviceId},
        {"Type", 0},
        {"Action", 1},
        {"Name", devName},
        {"TypeSensor", cnv_typeSensor},
        {"Min", min_value},
        {"Max", max_value},
        {"Param_a", 0},
        {"Param_b", 0},
        {"Param_c", 0},
        {"Value", 0},
        {"Battery", 100},
        {"Delay", 10},
        {"isConfig", true},
    }};
    msgToCloud_j = {
        {"Head",
         {{"IDMessage", rqi},
          {"TypeMessage", 0},
          {"FormData", 2},
          {"IDGateway", getMacGW()},
          {"DateTime", time(0)}}},
        {"Device", j_device}};

    std::string msgToCloud_str = msgToCloud_j.dump();
    string rspTopic = getMacGW() + "gateway";
    server.forwardMessage(rspTopic, msgToCloud_str);
}

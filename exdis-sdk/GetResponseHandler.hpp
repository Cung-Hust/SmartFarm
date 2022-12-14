/*
 * handlerCmd.hpp
 *
 *  Created on: Jul 27, 2022
 *      Author: rd
 */

#pragma once

#include <iostream>
#include "../json/json.hpp"
#include "../clients/Rule.hpp"

using namespace std;
using nlohmann::json;

#define TEMP_SENSOR_NAME "AirHumiTempSensor"
#define TEMP_SENSOR_TYPE 0
#define TEMP_SENSOR_MIN_VALUE 0
#define TEMP_SENSOR_MAX_VALUE 55

#define HUMI_SENSOR_NAME "AirHumiTempSensor"
#define HUMI_SENSOR_TYPE 1
#define HUMI_SENSOR_MIN_VALUE 0
#define HUMI_SENSOR_MAX_VALUE 100

#define SOIL_SENSOR_NAME "SoilMoistureSensor"
#define SOIL_SENSOR_TYPE 2
#define SOIL_SENSOR_MIN_VALUE 0
#define SOIL_SENSOR_MAX_VALUE 100

#define LIGHT_SENSOR_NAME "LightSensor"
#define LIGHT_SENSOR_TYPE 3
#define LIGHT_SENSOR_MIN_VALUE 0
#define LIGHT_SENSOR_MAX_VALUE 100000

#define CO2_SENSOR_NAME "CO2Sensor"
#define CO2_SENSOR_TYPE 4
#define CO2_SENSOR_MIN_VALUE 0
#define CO2_SENSOR_MAX_VALUE 5000

#define EC_SENSOR_NAME "ECSensor"
#define EC_SENSOR_TYPE 5
#define EC_SENSOR_MIN_VALUE 0
#define EC_SENSOR_MAX_VALUE 20000

#define PH_SENSOR_NAME "PHSensor"
#define PH_SENSOR_TYPE 6
#define PH_SENSOR_MIN_VALUE 3
#define PH_SENSOR_MAX_VALUE 9

class GetResponseHandler
{
private:
    string cmd;

public:
    GetResponseHandler();
    ~GetResponseHandler();
    void responder(string message);
    void run(string readingServiceTopic);
    void handler(string data);
};

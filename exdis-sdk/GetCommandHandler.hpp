/*
 * handlerCmd.hpp
 *
 *  Created on: Jul 27, 2022
 *      Author: rd
 */

#pragma once

#include <iostream>
#include "json/json.hpp"
#include "clients/Command.hpp"
#include "clients/Rule.hpp"

using namespace std;
using nlohmann::json;

#define FORM_DATA_SENSOR 0
#define FORM_DATA_CONTROL 1
#define FORM_DATA_DEVICE 2
#define FORM_DATA_RULE 3
#define FORM_DATA_SYNC 4
#define FORM_DATA_RESPONSE 5

#define TYPE_ACTION_UPDATE 0
#define TYPE_ACTION_CREATE 1
#define TYPE_ACTION_DELETE 2
#define TYPE_ACTION_SCAN 3

#define TYPE_DEVICE_SENSOR 0
#define TYPE_DEVICE_RELAY 1
#define TYPE_DEVICE_LED 2
#define TYPE_DEVICE_SCRIPT 3

#define TYPE_RULE_SCHEDULE 0
#define TYPE_RULE_NATIVE 1
#define TYPE_RULE_SCRIPT 2

#define TYPE_SENSOR_TEMP 0
#define TYPE_SENSOR_HUM 1
#define TYPE_SENSOR_SOIL 2
#define TYPE_SENSOR_LIGHT 3
#define TYPE_SENSOR_CO2 4

#define TYPE_MESSAGE_REP 0
#define TYPE_MESSAGE_NOT_REP 1

#define STATUS_SUCCESS 0
#define STATUS_FAIL 1

#define END_TIME 4102419600 // time to 2100

class GetCommandHandler
{
private:
    string cmd;

public:
    GetCommandHandler();
    ~GetCommandHandler();
    void commander(bool isDetach, string requestTopic);
    void run(string requestTopic);
    void handler(string data);
    void handler_device(int formData);
    void handler_rule();
    void handler_device_scan();
    void handler_device_create();
    void handler_device_control();
    void handler_device_update();
    void handler_device_delete();
    void handler_rule_create();
    void handler_rule_update();
    void handler_sync();
    void handler_response();
    void handler_error();
    void handler_rule_schedule();
    void handler_rule_scene();
    void handler_rule_native();
};

void handler_device_relay_control(Command client, json command);
void handler_device_led_control(Command client, json command);
void handler_rule_schedule_update(Rule client, json command);
void handler_rule_schedule_create(Rule client, json command);
void handler_rule_scene_update(Rule client, json command);
void handler_rule_scene_create(Rule client, json command);
void handler_rule_native_update(Rule client, json command);
void handler_rule_native_create(Rule client, json command);
void handler_rule_delete(Rule client, json command);

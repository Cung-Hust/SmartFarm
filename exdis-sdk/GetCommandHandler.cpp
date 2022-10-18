/*
 * handlerCmd.cpp
 *
 *  Created on: Jul 27, 2022
 *      Author: rd
 */

#include <stdlib.h>
#include <ctime>
#include <thread>
#include <unistd.h>
#include <pthread.h>

#include "clients/Metadata.hpp"
#include "servers/ExDis.hpp"
#include "common/utils.hpp"
#include "database/Database.hpp"
#include "GetCommandHandler.hpp"
#include "transports/Transports.hpp"

GetCommandHandler::GetCommandHandler(){};

GetCommandHandler::~GetCommandHandler(){};

void GetCommandHandler::commander(bool isDetach, string requestTopic)
{
    if (isDetach)
    {
        function<void()> server = bind(&GetCommandHandler::run, this, requestTopic);
        thread thr(server);
        thr.detach();
    }
    else
    {
        this->run(requestTopic);
    }
}

void GetCommandHandler::run(string requestTopic)
{
    string data;
    while (true)
    {
        if (ServerTransports::getMessager()->getMessage(requestTopic, data))
        {
            this->handler(data);
        }
        usleep(5000);
    }
}

void GetCommandHandler::handler(string data)
{
    this->cmd = data;
    cout << "Running command " << data << endl;
    server::ExDis server("ExDis");
    json command = json::parse(data);
    cout << "\033[0;36m\t Command:  " << command << endl;
    int formData = command["Head"]["FormData"];
    string rspTopic = getMacGW() + "gateway";
    if (formData == 1000)
    {
        json msgToCloud_j = {
            {"Head",
             {{"IDMessage", command["Head"]["IDMessage"]},
              {"TypeMessage", 1},
              {"FormData", 1000},
              {"IDGateway", getMacGW()},
              {"DateTime", time(0)}}}};
        std::string msgToCloud_str = msgToCloud_j.dump();
        server.forwardMessage(rspTopic, msgToCloud_str);
        return;
    }
    json msgToCloud_j = {
        {"Head",
         {{"IDMessage", command["Head"]["IDMessage"]},
          {"TypeMessage", 1},
          {"FormData", 5},
          {"IDGateway", getMacGW()},
          {"DateTime", time(0)}}}};
    json devList;
    std::string msgToCloud_str = msgToCloud_j.dump();
    server.forwardMessage(rspTopic, msgToCloud_str);
    switch (formData)
    {
    case FORM_DATA_SENSOR:
    case FORM_DATA_CONTROL:
    case FORM_DATA_DEVICE:
        this->handler_device(formData);
        break;
    case FORM_DATA_RULE:
        this->handler_rule();
        break;
    case FORM_DATA_SYNC:
        this->handler_sync();
        break;
    case FORM_DATA_RESPONSE:
        this->handler_response();
        break;
    default:
        this->handler_error();
        break;
    }
}

void GetCommandHandler::handler_device(int formData)
{
    json command = json::parse(this->cmd);
    switch (formData)
    {
    case FORM_DATA_CONTROL:
        handler_device_control();
        break;
    case FORM_DATA_DEVICE:
        int action = command["Device"][0]["Action"];
        switch (action)
        {
        case TYPE_ACTION_UPDATE:
            handler_device_update();
            break;
        case TYPE_ACTION_CREATE:
            handler_device_create();
            break;
        case TYPE_ACTION_DELETE:
            handler_device_delete();
            break;
        case TYPE_ACTION_SCAN:
            handler_device_scan();
            break;
        }
        break;
    }
}

void GetCommandHandler::handler_rule()
{
    json command = json::parse(this->cmd);
    int typeValue = command["Rule"][0]["Type"];
    switch (typeValue)
    {
    case TYPE_RULE_SCHEDULE:
        this->handler_rule_schedule();
        break;
    case TYPE_RULE_NATIVE:
        this->handler_rule_native();
        break;
    case TYPE_RULE_SCRIPT:
        this->handler_rule_scene();
        break;
    default:
        break;
    }
}

void GetCommandHandler::handler_device_control()
{
    json command = json::parse(this->cmd);
    Command client("exdis", "rd/command/request");
    int typeDevice = command["Control"][0]["Type"];
    switch (typeDevice)
    {
    case TYPE_DEVICE_RELAY:
        handler_device_relay_control(client, command);
        break;
    case TYPE_DEVICE_LED:
        handler_device_led_control(client, command);
        break;
    }
}

void handler_device_relay_control(Command client, json command)
{
    string uuid = genUuid();
    map<string, string> resources;
    string devId = command["Control"][0]["ID"];
    string req_state = "false";
    if (command["Control"][0]["Pull"] == 1)
    {
        req_state = "true";
    }
    resources["OnOff"] = req_state;
    map<string, string> options;
    options["response"] = "true";
    cout << "Device ID:  " << devId << endl;
    client.issueSetCommand(uuid, devId, resources, options);
}

void handler_device_led_control(Command client, json command)
{
    string uuid = genUuid();
    map<string, string> resources;
    string devId = command["Control"][0]["ID"];
    if (command["Control"][0].contains("Dim"))
    {
        resources["Dim"] = to_string(command["Control"][0]["Dim"]);
    }
    else if (command["Control"][0].contains("CCT"))
    {
        resources["CCT"] = to_string(command["Control"][0]["CCT"]);
    }
    else
    {
        resources["OnOff"] = command["Control"][0]["Pull"] == 0 ? "false" : "true";
    }
    map<string, string> options;
    options["response"] = "true";
    client.issueSetCommand(uuid, devId, resources, options);
}

void GetCommandHandler::handler_device_scan()
{
    json command = json::parse(this->cmd);
    string rqi = genUuid();

    Command client("exdis", "rd/command/request");
    model::Device dev;
    dev.type = DeviceType::SENSOR;
    map<string, string> options;
    dev.profileName = "Lora";
    dev.serviceName = "local";
    client.issueDiscoveryCommand(rqi, dev, options);
}

void GetCommandHandler::handler_device_create()
{
    json command = json::parse(this->cmd);
    string uuid = genUuid();
    Metadata client("exdis", "rd/metadata/request");
    model::Device dev;

    string idMessage;
    idMessage = command["Head"]["IDMessage"];
    dev.id = command["Device"][0]["ID"];
    dev.name = command["Device"][0]["Name"];
    dev.type = command["Device"][0]["Type"] == 1 ? DeviceType::ACTUATOR : DeviceType::SENSOR;
    dev.created = time(0);
    dev.modified = time(0);
    dev.adminState = AdminState::UNLOCKED;
    dev.operatingState = OperatingState::ONLINE;
    dev.lastConnected = time(0);
    dev.profileName = dev.type == DeviceType::ACTUATOR ? "Relay" : "Ble";
    dev.serviceName = "local";
    map<string, string> mapTest;
    mapTest["PIN"] = to_string(command["Device"][0]["Pin"]);
    mapTest["ID"] = command["Device"][0]["ID"];
    // ToDo: Fixed, change by server
    mapTest["Address"] = mapTest["PIN"];
    // end
    dev.protocols = mapTest;
    client.addDevice(uuid, dev, true);
}

void GetCommandHandler::handler_device_update()
{
    json command = json::parse(this->cmd);
    uint8_t typeDev = command["Device"][0]["Type"];
    string uuid = genUuid();
    Metadata client("exdis", "rd/metadata/request");
    model::Device dev;

    dev.id = command["Device"][0]["ID"];
    dev.name = command["Device"][0]["Name"];
    dev.modified = localTimestamp();
    string err;
    auto device = Db::getDb()->readDevice(dev.id, err);
    cout << device.id << endl;
    cout << device.created << endl;
    if (err != "")
    {
        cout << "err..." << err << std::endl;
        return;
    }
    dev.protocols = device.protocols;
    if (typeDev == 0)
    {
        dev.protocols["MaxValue"] = to_string(command["Device"][0]["Max"]);
        dev.protocols["MinValue"] = to_string(command["Device"][0]["Min"]);
        dev.protocols["Calib_1"] = to_string(command["Device"][0]["Param_a"]);
        dev.protocols["Calib_2"] = to_string(command["Device"][0]["Param_b"]);
        dev.protocols["Calib_3"] = to_string(command["Device"][0]["Param_c"]);
        dev.protocols["ReportSensorTime"] = to_string(command["Device"][0]["Delay"]);
        dev.profileName = "Lora";
    }
    else
    {
        dev.profileName = "Relay";
    }
    dev.serviceName = "local";
    dev.lastConnected = localTimestamp();
    client.updateDevice(uuid, dev, true);
}

void GetCommandHandler::handler_device_delete()
{
    json command = json::parse(this->cmd);
    string uuid = genUuid();
    Metadata client("exdis", "rd/metadata/request");
    model::Device dev;

    string id = command["Device"][0]["ID"];
    client.deleteDevice(uuid, id, true);
}

void GetCommandHandler::handler_sync()
{
}

void GetCommandHandler::handler_response()
{
}

void GetCommandHandler::handler_error()
{
}

void GetCommandHandler::handler_rule_schedule()
{
    json command = json::parse(this->cmd);
    Rule client("exdis", "rd/rule/request");
    uint8_t action = command["Rule"][0]["Action"];
    switch (action)
    {
    case TYPE_ACTION_UPDATE:
        handler_rule_schedule_update(client, command);
        break;
    case TYPE_ACTION_CREATE:
        handler_rule_schedule_create(client, command);
        break;
    case TYPE_ACTION_DELETE:
        handler_rule_delete(client, command);
        break;
    }
}

void GetCommandHandler::handler_rule_scene()
{
    json command = json::parse(this->cmd);
    Rule client("exdis", "rd/rule/request");
    uint8_t action = command["Rule"][0]["Action"];
    switch (action)
    {
    case TYPE_ACTION_UPDATE:
        handler_rule_scene_update(client, command);
        break;
    case TYPE_ACTION_CREATE:
        handler_rule_scene_create(client, command);
        break;
    case TYPE_ACTION_DELETE:
        handler_rule_delete(client, command);
        break;
    }
}

void GetCommandHandler::handler_rule_native()
{
    json command = json::parse(this->cmd);
    Rule client("exdis", "rd/rule/request");
    uint8_t action = command["Rule"][0]["Action"];
    switch (action)
    {
    case TYPE_ACTION_UPDATE:
        handler_rule_native_update(client, command);
        break;
    case TYPE_ACTION_CREATE:
        handler_rule_native_create(client, command);
        break;
    case TYPE_ACTION_DELETE:
        handler_rule_delete(client, command);
        break;
    }
}

void handler_rule_schedule_create(Rule client, json command)
{
    string rqi = genUuid();
    model::Rule rule;

    string idMessage = command["Head"]["IDMessage"];
    rule.id = command["Rule"][0]["ID"];
    rule.name = command["Rule"][0]["Name"];
    rule.type = RuleType::SCHEDULE;
    rule.priority = 100;
    int activeValue = command["Rule"][0]["Active"];
    rule.activeState = activeValue == 0 ? ActiveState::DISABLED : ActiveState::ENABLED;
    rule.triggerState = TriggerState::OFF;
    rule.startTime = 0;
    rule.endTime = END_TIME;
    uint32_t startInDayTime = command["Rule"][0]["Input"]["Schedule"]["TimeStart"];
    rule.startInDayTime = startInDayTime;
    cout << endl
         << "rule.startInDayTime :: :: :: " << rule.startInDayTime << endl;
    rule.endInDayTime = 86400;
    vector<string> repeatDays;
    if (command["Rule"][0]["Input"]["Loop"]["Monday"] == true)
    {
        repeatDays.push_back("Mon");
    }
    if (command["Rule"][0]["Input"]["Loop"]["Tuesday"] == true)
    {
        repeatDays.push_back("Tue");
    }
    if (command["Rule"][0]["Input"]["Loop"]["Wednesday"] == true)
    {
        repeatDays.push_back("Wed");
    }
    if (command["Rule"][0]["Input"]["Loop"]["Thursday"] == true)
    {
        repeatDays.push_back("Thu");
    }
    if (command["Rule"][0]["Input"]["Loop"]["Friday"] == true)
    {
        repeatDays.push_back("Fri");
    }
    if (command["Rule"][0]["Input"]["Loop"]["Saturday"] == true)
    {
        repeatDays.push_back("Sat");
    }
    if (command["Rule"][0]["Input"]["Loop"]["Sunday"] == true)
    {
        repeatDays.push_back("Sun");
    }
    if (repeatDays.size() == 0)
    {
        string dayInWeek = getDayInWeek();
        repeatDays.push_back(dayInWeek);
    }
    rule.repeatDays = repeatDays;
    vector<uint32_t> timeInDayConditions;
    timeInDayConditions.push_back(startInDayTime);
    rule.timeInDayConditions = timeInDayConditions;
    rule.deviceConditions = vector<model::DeviceCondition>();
    rule.ruleConditions = vector<model::RuleCondition>();
    vector<model::DeviceAction> deviceActions;
    json outputDevs = command["Rule"][0]["Execute"];
    model::DeviceAction devAction;
    for (auto &elem : outputDevs)
    {
        devAction.deviceId = elem["ID"];
        devAction.resourceName = "OnOff";
        devAction.value = elem["Pull"] == 0 ? "false" : "true";
        devAction.delayTime = 0;
        deviceActions.push_back(devAction);
    }
    rule.deviceActions = deviceActions;
    rule.ruleActions = vector<model::RuleAction>();
    rule.enableNotify = true;
    rule.notifyLevel = NORMAL;
    rule.notifyContent = "RULE SCHEDULE CREATED";
    string resp = client.addRule(rqi, rule);
}

void handler_rule_schedule_update(Rule client, json command)
{
    string rqi = genUuid();
    model::Rule rule;

    if (command["Rule"][0].contains("Input") && command["Rule"][0].contains("Execute"))
    {
        string idMessage = command["Head"]["IDMessage"];
        rule.id = command["Rule"][0]["ID"];
        rule.name = command["Rule"][0]["Name"];
        rule.type = RuleType::SCHEDULE;
        rule.priority = 100;
        int activeValue = command["Rule"][0]["Active"];
        rule.activeState = activeValue == 0 ? ActiveState::DISABLED : ActiveState::ENABLED;
        rule.triggerState = TriggerState::OFF;
        rule.startTime = 0;
        rule.endTime = END_TIME;
        uint32_t startInDayTime = command["Rule"][0]["Input"]["Schedule"]["TimeStart"];
        rule.startInDayTime = startInDayTime;
        rule.endInDayTime = 86400;
        rule.repeatDays = vector<string>();
        rule.conditionLogic = AND;
        vector<string> repeatDays;
        if (command["Rule"][0]["Input"]["Loop"]["Monday"] == true)
        {
            repeatDays.push_back("Mon");
        }
        if (command["Rule"][0]["Input"]["Loop"]["Tuesday"] == true)
        {
            repeatDays.push_back("Tue");
        }
        if (command["Rule"][0]["Input"]["Loop"]["Wednesday"] == true)
        {
            repeatDays.push_back("Wed");
        }
        if (command["Rule"][0]["Input"]["Loop"]["Thursday"] == true)
        {
            repeatDays.push_back("Thu");
        }
        if (command["Rule"][0]["Input"]["Loop"]["Friday"] == true)
        {
            repeatDays.push_back("Fri");
        }
        if (command["Rule"][0]["Input"]["Loop"]["Saturday"] == true)
        {
            repeatDays.push_back("Sat");
        }
        if (command["Rule"][0]["Input"]["Loop"]["Sunday"] == true)
        {
            repeatDays.push_back("Sun");
        }
        if (repeatDays.size() == 0)
        {
            string dayInWeek = getDayInWeek();
            repeatDays.push_back(dayInWeek);
        }
        rule.repeatDays = repeatDays;
        vector<uint32_t> timeInDayConditions;
        timeInDayConditions.push_back(rule.startInDayTime);
        rule.timeInDayConditions = timeInDayConditions;
        rule.deviceConditions = vector<model::DeviceCondition>();
        rule.ruleConditions = vector<model::RuleCondition>();

        // OUTPUT
        vector<model::DeviceAction> deviceActions;
        json outputDevs = command["Rule"][0]["Execute"];
        model::DeviceAction devAction;
        for (auto &elem : outputDevs)
        {
            devAction.deviceId = elem["ID"];
            devAction.resourceName = "OnOff";
            devAction.value = elem["Pull"] == 0 ? "false" : "true";
            devAction.delayTime = 0;
            deviceActions.push_back(devAction);
        }
        rule.deviceActions = deviceActions;

        rule.ruleActions = vector<model::RuleAction>();
        rule.enableNotify = true;
        rule.notifyLevel = NORMAL;
        rule.notifyContent = "";
        client.updateRule(rqi, rule);
    }
    else
    {
        string ruleId = command["Rule"][0]["ID"];
        int activeValue = command["Rule"][0]["Active"];
        ActiveState activeState = activeValue == 0 ? ActiveState::DISABLED : ActiveState::ENABLED;
        client.updateStateRule(rqi, ruleId, activeState);
    }
}

void handler_rule_scene_create(Rule client, json command)
{
    string rqi = genUuid();
    model::Rule rule;

    string idMessage = command["Head"]["IDMessage"];
    rule.id = command["Rule"][0]["ID"];
    rule.name = command["Rule"][0]["Name"];
    rule.type = RuleType::SCENE;
    rule.priority = 100;
    int activeValue = command["Rule"][0]["Active"];
    rule.activeState = activeValue == 0 ? ActiveState::DISABLED : ActiveState::ENABLED;
    rule.triggerState = TriggerState::OFF;
    rule.startTime = 0;
    rule.endTime = END_TIME;
    rule.startInDayTime = localTimestamp() - (localTimestamp() / 86400) * 86400 + 2;
    rule.endInDayTime = 86400;
    vector<string> repeatDays;
    string dayInWeek = getDayInWeek();
    repeatDays.push_back(dayInWeek);
    rule.repeatDays = repeatDays;
    rule.conditionLogic = AND;
    vector<uint32_t> timeInDayConditions;
    timeInDayConditions.push_back(rule.startInDayTime);
    rule.timeInDayConditions = timeInDayConditions;
    rule.deviceConditions = vector<model::DeviceCondition>();
    rule.ruleConditions = vector<model::RuleCondition>();
    vector<model::DeviceAction> deviceActions;
    json outputDevs = command["Rule"][0]["Execute"];
    model::DeviceAction devAction;
    for (auto &elem : outputDevs)
    {
        devAction.deviceId = elem["ID"];
        devAction.delayTime = elem["Delay"];
        devAction.resourceName = "OnDuring";
        devAction.value = to_string(elem["During"]);
        deviceActions.push_back(devAction);
    }
    rule.deviceActions = deviceActions;
    rule.ruleActions = vector<model::RuleAction>();
    rule.enableNotify = true;
    rule.notifyLevel = NORMAL;
    rule.notifyContent = "";
    client.addRule(rqi, rule);
}

void handler_rule_scene_update(Rule client, json command)
{
    string rqi = genUuid();
    model::Rule rule;

    if (command["Rule"][0].contains("Execute"))
    {

        string idMessage = command["Head"]["IDMessage"];
        rule.id = command["Rule"][0]["ID"];
        rule.name = command["Rule"][0]["Name"];
        rule.type = RuleType::SCENE;
        rule.priority = 100;
        int activeValue = command["Rule"][0]["Active"];
        rule.activeState = activeValue == 0 ? ActiveState::DISABLED : ActiveState::ENABLED;
        rule.triggerState = TriggerState::OFF;
        rule.startTime = 0;
        rule.endTime = END_TIME;
        rule.startInDayTime = localTimestamp() - (localTimestamp() / 86400) * 86400 + 2;
        rule.endInDayTime = 86400;
        vector<string> repeatDays;
        string dayInWeek = getDayInWeek();
        repeatDays.push_back(dayInWeek);
        rule.repeatDays = repeatDays;
        rule.conditionLogic = AND;
        vector<uint32_t> timeInDayConditions;
        timeInDayConditions.push_back(rule.startInDayTime);
        rule.timeInDayConditions = timeInDayConditions;
        rule.deviceConditions = vector<model::DeviceCondition>();
        rule.ruleConditions = vector<model::RuleCondition>();
        {
            vector<model::DeviceAction> deviceActions;
            json outputDevs = command["Rule"][0]["Execute"];
            model::DeviceAction devAction;
            for (auto &elem : outputDevs)
            {
                devAction.deviceId = elem["ID"];
                devAction.delayTime = elem["Delay"];
                devAction.resourceName = "OnDuring";
                devAction.value = to_string(elem["During"]);
                deviceActions.push_back(devAction);
            }
            rule.deviceActions = deviceActions;
        }

        rule.ruleActions = vector<model::RuleAction>();
        rule.enableNotify = true;
        rule.notifyLevel = NORMAL;
        rule.notifyContent = "";
        client.updateRule(rqi, rule);
    }
    else
    {
        string ruleId = command["Rule"][0]["ID"];
        int activeValue = command["Rule"][0]["Active"];
        cout << "activeValue :: " << activeValue << endl;
        ActiveState activeState = activeValue == 0 ? ActiveState::DISABLED : ActiveState::ENABLED;
        cout << "ActiveState activeState :: " << activeState << endl;
        client.updateStateRule(rqi, ruleId, activeState);
    }
}

void handler_rule_native_create(Rule client, json command)
{
    string rqi = genUuid();
    model::Rule rule;

    string uuid = genUuid();
    string idMessage = command["Head"]["IDMessage"];
    rule.id = command["Rule"][0]["ID"];
    rule.name = command["Rule"][0]["Name"];
    rule.type = RuleType::RULE;
    rule.priority = 100;
    int activeValue = command["Rule"][0]["Active"];
    rule.activeState = ActiveState::ENABLED;
    if (activeValue == 0)
    {
        rule.activeState = ActiveState::DISABLED;
    }
    rule.triggerState = TriggerState::OFF;
    rule.startTime = 0;
    rule.endTime = END_TIME;
    if (command["Rule"][0]["Input"]["Schedule"].contains("TimeStart"))
    {
        rule.startInDayTime = command["Rule"][0]["Input"]["Schedule"]["TimeStart"];
    }
    else
    {
        rule.startInDayTime = time(0);
    }
    if (command["Rule"][0]["Input"]["Schedule"].contains("TimeStop"))
    {
        rule.endInDayTime = command["Rule"][0]["Input"]["Schedule"]["TimeStop"];
    }
    else
    {
        rule.endInDayTime = 86400;
    }
    vector<string> repeatDays;
    if (command["Rule"][0]["Input"]["Loop"]["Monday"] == true)
    {
        repeatDays.push_back("Mon");
    }
    if (command["Rule"][0]["Input"]["Loop"]["Tuesday"] == true)
    {
        repeatDays.push_back("Tue");
    }
    if (command["Rule"][0]["Input"]["Loop"]["Wednesday"] == true)
    {
        repeatDays.push_back("Wed");
    }
    if (command["Rule"][0]["Input"]["Loop"]["Thursday"] == true)
    {
        repeatDays.push_back("Thu");
    }
    if (command["Rule"][0]["Input"]["Loop"]["Friday"] == true)
    {
        repeatDays.push_back("Fri");
    }
    if (command["Rule"][0]["Input"]["Loop"]["Saturday"] == true)
    {
        repeatDays.push_back("Sat");
    }
    if (command["Rule"][0]["Input"]["Loop"]["Sunday"] == true)
    {
        repeatDays.push_back("Sun");
    }
    rule.repeatDays = repeatDays;
    vector<uint32_t> timeInDayConditions;
    timeInDayConditions.push_back(rule.startInDayTime);
    rule.timeInDayConditions = timeInDayConditions;
    rule.conditionLogic = (command["Rule"][0]["Input"]["Condition"]["Logic"] == 0) ? RuleLogic::AND : RuleLogic::OR;
    vector<model::DeviceCondition> deviceConditions;
    json inputDevs = command["Rule"][0]["Input"]["Condition"]["Compare"];
    model::DeviceCondition deviceCondition;
    string maxValue = "";
    for (auto &elem : inputDevs)
    {
        deviceCondition.deviceId = elem["ID"];
        uint8_t typeSensor = elem["TypeSensor"];
        cout << "typeSensor" << typeSensor << endl;
        switch (typeSensor)
        {
        case 0:
            deviceCondition.resourceName = "Temperature";
            maxValue = "100";
            break;
        case 1:
            deviceCondition.resourceName = "Humidity";
            maxValue = "100";
            break;
        case 2:
            deviceCondition.resourceName = "SoilMoisture";
            maxValue = "100";
            break;
        case 3:
            deviceCondition.resourceName = "Lux";
            maxValue = "100000";
            break;
        case 4:
            deviceCondition.resourceName = "CO2";
            maxValue = "1000";
            break;
        case 5:
            deviceCondition.resourceName = "EC";
            maxValue = "1000";
            break;
        case 6:
            deviceCondition.resourceName = "PH";
            maxValue = "1000";
            break;
        }
        deviceCondition.compareOperator = CompareOperator::GREATER;
        deviceCondition.value = to_string(elem["minValue"]);
        deviceConditions.push_back(deviceCondition);
        deviceCondition.compareOperator = CompareOperator::LESS;
        deviceCondition.value = to_string(elem["maxValue"]);
        deviceConditions.push_back(deviceCondition);
    }
    rule.deviceConditions = deviceConditions;
    rule.ruleConditions = vector<model::RuleCondition>();
    vector<model::DeviceAction> deviceActions;
    vector<model::RuleAction> ruleActions;
    json outputDevs = command["Rule"][0]["Execute"];
    model::DeviceAction devAction;
    model::RuleAction ruleAction;
    for (auto &elem : outputDevs)
    {
        if (elem["Type"] == 1)
        {
            devAction.deviceId = elem["ID"];
            devAction.delayTime = elem["Delay"];
            devAction.resourceName = "OnDuring";
            devAction.value = to_string(elem["During"]);
            deviceActions.push_back(devAction);
        }
        else
        {
            rule.deviceActions = vector<model::DeviceAction>();
            ruleAction.actionRuleId = elem["ID"];
            ruleAction.delayTime = 0;
            ruleActions.push_back(ruleAction);
        }
    }
    rule.deviceActions = deviceActions;
    rule.ruleActions = ruleActions;
    rule.enableNotify = true;
    rule.notifyLevel = NORMAL;
    rule.notifyContent = "";
    client.addRule(rqi, rule);
}

void handler_rule_native_update(Rule client, json command)
{
    string rqi = genUuid();
    model::Rule rule;

    if (command["Rule"][0].contains("Input") && command["Rule"][0].contains("Execute"))
    {
        string uuid = genUuid();
        string idMessage = command["Head"]["IDMessage"];
        rule.id = command["Rule"][0]["ID"];
        rule.name = command["Rule"][0]["Name"];
        rule.type = RuleType::RULE;
        rule.priority = 100;
        int activeValue = command["Rule"][0]["Active"];
        rule.activeState = ActiveState::ENABLED;
        if (activeValue == 0)
        {
            rule.activeState = ActiveState::DISABLED;
        }
        rule.triggerState = TriggerState::OFF;
        rule.startTime = 0;
        rule.endTime = END_TIME;
        // rule.startInDayTime = command["Rule"][0]["Input"]["Schedule"]["TimeStart"];
        // rule.endInDayTime = command["Rule"][0]["Input"]["Schedule"]["TimeStop"];
        if (command["Rule"][0]["Input"]["Schedule"].contains("TimeStart"))
        {
            rule.startInDayTime = command["Rule"][0]["Input"]["Schedule"]["TimeStart"];
        }
        else
        {
            rule.startInDayTime = time(0);
        }
        if (command["Rule"][0]["Input"]["Schedule"].contains("TimeStop"))
        {
            rule.endInDayTime = command["Rule"][0]["Input"]["Schedule"]["TimeStop"];
        }
        else
        {
            rule.endInDayTime = 86400;
        }
        vector<string> repeatDays;
        if (command["Rule"][0]["Loop"]["Monday"] == true)
        {
            repeatDays.push_back("Mon");
        }
        if (command["Rule"][0]["Loop"]["Tuesday"] == true)
        {
            repeatDays.push_back("Tue");
        }
        if (command["Rule"][0]["Loop"]["Wednesday"] == true)
        {
            repeatDays.push_back("Wed");
        }
        if (command["Rule"][0]["Loop"]["Thursday"] == true)
        {
            repeatDays.push_back("Thu");
        }
        if (command["Rule"][0]["Loop"]["Friday"] == true)
        {
            repeatDays.push_back("Fri");
        }
        if (command["Rule"][0]["Loop"]["Saturday"] == true)
        {
            repeatDays.push_back("Sat");
        }
        if (command["Rule"][0]["Loop"]["Sunday"] == true)
        {
            repeatDays.push_back("Sun");
        }
        rule.repeatDays = repeatDays;
        vector<uint32_t> timeInDayConditions;
        timeInDayConditions.push_back(rule.startInDayTime);
        rule.timeInDayConditions = timeInDayConditions;
        rule.conditionLogic = (command["Rule"][0]["Input"]["Condition"]["Logic"] == 0) ? RuleLogic::AND : RuleLogic::OR;
        vector<model::DeviceCondition> deviceConditions;
        json inputDevs = command["Rule"][0]["Input"]["Condition"]["Compare"];
        model::DeviceCondition deviceCondition;
        string maxValue = "";

        for (auto &elem : inputDevs)
        {
            deviceCondition.deviceId = elem["ID"];
            uint8_t typeSensor = elem["TypeSensor"];
            cout << "typeSensor" << typeSensor << endl;
            switch (typeSensor)
            {
            case 0:
                deviceCondition.resourceName = "Temperature";
                maxValue = "100";
                break;
            case 1:
                deviceCondition.resourceName = "Humidity";
                maxValue = "100";
                break;
            case 2:
                deviceCondition.resourceName = "SoilMoisture";
                maxValue = "100";
                break;
            case 3:
                deviceCondition.resourceName = "Lux";
                maxValue = "100000";
                break;
            case 4:
                deviceCondition.resourceName = "CO2";
                maxValue = "1000";
                break;
            case 5:
                deviceCondition.resourceName = "EC";
                maxValue = "1000";
                break;
            case 6:
                deviceCondition.resourceName = "PH";
                maxValue = "1000";
                break;
            }
            deviceCondition.compareOperator = CompareOperator::GREATER;
            deviceCondition.value = to_string(elem["minValue"]);
            deviceConditions.push_back(deviceCondition);
            deviceCondition.compareOperator = CompareOperator::LESS;
            deviceCondition.value = to_string(elem["maxValue"]);
            deviceConditions.push_back(deviceCondition);
        }
        rule.deviceConditions = deviceConditions;
        rule.ruleConditions = vector<model::RuleCondition>();
        vector<model::DeviceAction> deviceActions;
        vector<model::RuleAction> ruleActions;
        json outputDevs = command["Rule"][0]["Execute"];
        model::DeviceAction devAction;
        model::RuleAction ruleAction;
        for (auto &elem : outputDevs)
        {
            if (elem["Type"] == 1)
            {
                devAction.deviceId = elem["ID"];
                devAction.delayTime = elem["Delay"];
                devAction.resourceName = "OnDuring";
                devAction.value = elem["Pull"] == 0 ? "0" : to_string(elem["During"]);
                deviceActions.push_back(devAction);
            }
            else
            {
                rule.deviceActions = vector<model::DeviceAction>();
                ruleAction.actionRuleId = elem["ID"];
                ruleAction.delayTime = 0;
                ruleActions.push_back(ruleAction);
            }
        }
        rule.deviceActions = deviceActions;
        rule.ruleActions = ruleActions;
        rule.enableNotify = true;
        rule.notifyLevel = NORMAL;
        rule.notifyContent = "";
        client.updateRule(rqi, rule);
    }
    else
    {
        string ruleId = command["Rule"][0]["ID"];
        int activeValue = command["Rule"][0]["Active"];
        ActiveState activeState = activeValue == 0 ? ActiveState::DISABLED : ActiveState::ENABLED;
        client.updateStateRule(rqi, ruleId, activeState);
    }
}

void handler_rule_delete(Rule client, json command)
{
    string rqi = genUuid();
    string id = command["Rule"][0]["ID"];
    client.deleteRule(rqi, id);
}

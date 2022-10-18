#include "constants.hpp"
#include "models.hpp"

using namespace std;

// Trien khai cac ham chuyen doi enum <=> string

string enumToString(DeviceType e)
{
    switch (e)
    {
    case DeviceType::ACTUATOR:
        return "ACTUATOR";
    case DeviceType::GATEWAY:
        return "GATEWAY";
    case DeviceType::GROUP:
        return "GROUP";
    case DeviceType::SENSOR:
        return "SENSOR";
    default:
        return "GATEWAY";
    }

    // never here
    return "";
}

void enumFormString(const std::string &str, DeviceType &e)
{
    if (str == "ACTUATOR")
        e = DeviceType::ACTUATOR;
    else if (str == "GATEWAY")
        e = DeviceType::GATEWAY;
    else if (str == "GROUP")
        e = DeviceType::GROUP;
    else if (str == "SENSOR")
        e = DeviceType::SENSOR;
    else
        e = DeviceType::GATEWAY;
}

string enumToString(AdminState e)
{
    switch (e)
    {
    case AdminState::LOCKED:
        return "LOCKED";
    default:
        return "UNLOCKED";
    }

    // never here
    return "";
}

void enumFormString(const std::string &str, AdminState &e)
{
    if (str == "LOCKED")
        e = AdminState::LOCKED;
    else
        e = AdminState::UNLOCKED;
}

string enumToString(OperatingState e)
{
    switch (e)
    {
    case OperatingState::ONLINE:
        return "ONLINE";
    default:
        return "OFFLINE";
    }

    // never here
    return "";
}

void enumFormString(const std::string &str, OperatingState &e)
{
    if (str == "ONLINE")
        e = OperatingState::ONLINE;
    else
        e = OperatingState::OFFLINE;
}

string enumToString(ReadWrite e)
{
    switch (e)
    {
    case ReadWrite::READ_ONLY:
        return "R";
    case ReadWrite::WRITE_ONLY:
        return "W";
    default:
        return "RW";
    }

    // never here
    return "";
}

void enumFormString(const std::string &str, ReadWrite &e)
{
    if (str == "R")
        e = ReadWrite::READ_ONLY;
    else if (str == "W")
        e = ReadWrite::WRITE_ONLY;
    else
        e = ReadWrite::READ_WRITE;
}

string enumToString(RuleType e)
{
    switch (e)
    {
    case RuleType::RULE:
        return "RULE";
    case RuleType::SCHEDULE:
        return "SCHEDULE";
    default:
        return "SCENE";
    }

    // never here
    return "";
}

void enumFormString(const std::string &str, RuleType &e)
{
    if (str == "RULE")
        e = RuleType::RULE;
    else if (str == "SCHEDULE")
        e = RuleType::SCHEDULE;
    else
        e = RuleType::SCENE;
}

string enumToString(ActiveState e)
{
    switch (e)
    {
    case ActiveState::ENABLED:
        return "ENABLED";
    default:
        return "DISABLED";
    }

    // never here
    return "";
}

void enumFormString(const std::string &str, ActiveState &e)
{
    if (str == "ENABLED")
        e = ActiveState::ENABLED;
    else
        e = ActiveState::DISABLED;
}

string enumToString(TriggerState e)
{
    switch (e)
    {
    case TriggerState::ON:
        return "ON";
    default:
        return "OFF";
    }

    // never here
    return "";
}

void enumFormString(const std::string &str, TriggerState &e)
{
    if (str == "ON")
        e = TriggerState::ON;
    else
        e = TriggerState::OFF;
}

string enumToString(RuleLogic e)
{
    switch (e)
    {
    case RuleLogic::OR:
        return "OR";
    default:
        return "AND";
    }

    // never here
    return "";
}

void enumFormString(const std::string &str, RuleLogic &e)
{
    if (str == "OR")
        e = RuleLogic::OR;
    else
        e = RuleLogic::AND;
}

string enumToString(CompareOperator e)
{
    switch (e)
    {
    case CompareOperator::EQU:
        return "=";
    case CompareOperator::NOT_EQU:
        return "!=";
    case CompareOperator::GREATER:
        return ">";
    case CompareOperator::LESS:
        return "<";
    case CompareOperator::GREATER_EQU:
        return ">=";
    case CompareOperator::LESS_EQU:
        return "<=";
    default:
        return "=";
    }

    // never here
    return "";
}

void enumFormString(const std::string &str, CompareOperator &e)
{
    if (str == "=")
        e = CompareOperator::EQU;
    else if (str == "!=")
        e = CompareOperator::NOT_EQU;
    else if (str == ">")
        e = CompareOperator::GREATER;
    else if (str == "<")
        e = CompareOperator::LESS;
    else if (str == ">=")
        e = CompareOperator::GREATER_EQU;
    else if (str == "<=")
        e = CompareOperator::LESS_EQU;
    else
        e = CompareOperator::NOT_EQU;
}

string enumToString(NotifyLevel e)
{
    switch (e)
    {
    case NotifyLevel::NORMAL:
        return "NORMAL";
    case NotifyLevel::WARNING:
        return "WARNING";
    case NotifyLevel::DANGER:
        return "DANGER";
    default:
        return "NORMAL";
    }

    // never here
    return "";
}

void enumFormString(const std::string &str, NotifyLevel &e)
{
    if (str == "NORMAL")
        e = NotifyLevel::NORMAL;
    else if (str == "WARNING")
        e = NotifyLevel::WARNING;
    else
        e = NotifyLevel::DANGER;
}

string enumToString(ValueType e)
{
    switch (e)
    {
    case ValueType::BOOL:
        return "bool";
    case ValueType::INT8:
        return "int8";
    case ValueType::UINT8:
        return "uint8";
    case ValueType::INT16:
        return "int16";
    case ValueType::UINT16:
        return "uint16";
    case ValueType::INT32:
        return "int32";
    case ValueType::UINT32:
        return "uint32";
    case ValueType::INT64:
        return "int64";
    case ValueType::UINT64:
        return "uint64";
    case ValueType::FLOAT:
        return "float";
    case ValueType::DOUBLE:
        return "double";
    case ValueType::STRING:
        return "string";
    default:
        return "string";
    }

    // never here
    return "";
}

void enumFormString(const std::string &str, ValueType &e)
{
    if (str == "bool")
        e = ValueType::BOOL;
    else if (str == "int8")
        e = ValueType::INT8;
    else if (str == "uint8")
        e = ValueType::UINT8;
    else if (str == "int16")
        e = ValueType::INT16;
    else if (str == "uint16")
        e = ValueType::UINT16;
    else if (str == "int32")
        e = ValueType::INT32;
    else if (str == "uint32")
        e = ValueType::UINT32;
    else if (str == "int64")
        e = ValueType::INT64;
    else if (str == "uint64")
        e = ValueType::UINT64;
    else if (str == "float")
        e = ValueType::FLOAT;
    else if (str == "double")
        e = ValueType::DOUBLE;
    else
        e = ValueType::STRING;
}

bool checkValidData(ValueType type, std::string value)
{
    switch (type)
    {
    case ValueType::STRING:
        return true;
    case ValueType::BOOL:
        if (value == "true" || value == "false")
        {
            return true;
        }
        else
        {
            return false;
        }
    case ValueType::FLOAT:
    case ValueType::DOUBLE:
        try
        {
            stod(value);
            return true;
        }
        catch (...)
        {
            return false;
        }
    default:
    try
        {
            stoi(value);
            return true;
        }
        catch (...)
        {
            return false;
        }
    }
}
#pragma once

#include <string>

using namespace std;

// Tu khoa 'string:' bieu dien gia tri khi chuyen sang dang string

enum DeviceType
{
    SENSOR,   // string: "SENSOR"
    ACTUATOR, // string: "ACTUATOR"
    GROUP,    // string: "GROUP"
    GATEWAY   // string: "GATEWAY"
};

enum AdminState
{
    UNLOCKED, // string: "UNLOCKED"
    LOCKED    // string: "LOCKED"
};

enum OperatingState
{
    ONLINE, // string: "ONLINE"
    OFFLINE // string: "OFFLINE"
};

enum ReadWrite
{
    READ_ONLY,  // string: "R"
    WRITE_ONLY, // string: "W"
    READ_WRITE  // string: "RW"
};

enum RuleType
{
    RULE,     // string: "RULE"
    SCHEDULE, // string: "SCHEDULE"
    SCENE     // string: "SCENE"
};

enum ActiveState
{
    ENABLED, // string: "ENABLED"
    DISABLED // string: "DISABLED"
};

enum TriggerState
{
    ON, // string: "ON"
    OFF // string: "OFF"
};

enum RuleLogic
{
    OR, // string: "OR"
    AND // string: "AND"
};

enum CompareOperator
{
    EQU,         // string: "="
    NOT_EQU,     // string: "!="
    GREATER,     // string: ">"
    LESS,        // string: "<"
    GREATER_EQU, // string: ">="
    LESS_EQU     // string: "<="
};

enum NotifyLevel
{
    NORMAL,  // string: "NORMAL"
    WARNING, // string: "WARNING"
    DANGER   // string: "DANGER"
};

enum ValueType
{
    BOOL,   // string: "bool"
    INT8,   // string: "int8"
    UINT8,  // string: "uint8"
    INT16,  // string: "int16"
    UINT16, // string: "uint16"
    INT32,  // string: "int32"
    UINT32, // string: "uint32"
    INT64,  // string: "int64"
    UINT64, // string: "uint64"
    FLOAT,  // string: "float"
    DOUBLE, // string: "double"
    STRING  // string: "string"
};

enum Mode
{
    MODE_MANUAL,  // string: "MANUAL"
    MODE_SCHEDULE, // string: "SCHEDULE"
    MODE_RULE   // string: "RULE"
};

string enumToString(DeviceType e);
void enumFormString(const std::string &str, DeviceType &e);
string enumToString(AdminState e);
void enumFormString(const std::string &str, AdminState &e);
string enumToString(OperatingState e);
void enumFormString(const std::string &str, OperatingState &e);
string enumToString(ReadWrite e);
void enumFormString(const std::string &str, ReadWrite &e);
string enumToString(RuleType e);
void enumFormString(const std::string &str, RuleType &e);
string enumToString(ActiveState e);
void enumFormString(const std::string &str, ActiveState &e);
string enumToString(TriggerState e);
void enumFormString(const std::string &str, TriggerState &e);
string enumToString(RuleLogic e);
void enumFormString(const std::string &str, RuleLogic &e);
string enumToString(CompareOperator e);
void enumFormString(const std::string &str, CompareOperator &e);
string enumToString(NotifyLevel e);
void enumFormString(const std::string &str, NotifyLevel &e);
string enumToString(Mode e);
void enumFormString(const std::string &str, Mode &e);
string enumToString(ValueType e);
void enumFormString(const std::string &str, ValueType &e);

bool checkValidData(ValueType type, std::string value);

// Template cac ham chuyen doi enum <=> string

template <typename T>
static string enumToString(T e)
{
    return enumToString(e);
}

template <typename T>
static void enumFromString(const std::string &str, T &e)
{
    enumFormString(str, e);
}

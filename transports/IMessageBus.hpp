#pragma once

#include <string>

using namespace std;

struct MessageBusData
{
    string topic;
    string message;
};

class IMessageBus
{
public:
    virtual bool subscribe(string topic) = 0;
    virtual bool publish(string topic, string message) = 0;
    virtual bool getMessage(string topic, string &data) = 0;
};
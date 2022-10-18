#pragma once

#include <string>
using namespace std;

class IPublisher
{
public:
    virtual bool publish(string topic, string message) = 0;
};
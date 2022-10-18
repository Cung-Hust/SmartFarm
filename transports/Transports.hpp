#include "IMessageBus.hpp"
#include "IPublisher.hpp"
#include "configuration.hpp"

#include "mqtt/Mqtt.h"

class Transports
{
private:
    static string transportType;

public:
    static IPublisher *getPublisher();
    static IMessageBus *getMessager();
    static void init(string configFile);
};

class ServerTransports
{
private:
    static string transportType;

public:
    static IPublisher *getPublisher();
    static IMessageBus *getMessager();
    static void init(string configFile);
};
#include "Transports.hpp"

string Transports::transportType = "";
string ServerTransports::transportType = "";

IPublisher *Transports::getPublisher()
{
    if (!Transports::transportType.compare("mqtt"))
    {
        return Mqtt::getInstance();
    }

    return nullptr;
}

IMessageBus *Transports::getMessager()
{
    if (!Transports::transportType.compare("mqtt"))
    {
        return Mqtt::getInstance();
    }

    return nullptr;
}

void Transports::init(string configFile)
{
    TransportConfiguration config;
    config.loadConfig(configFile);

    Transports::transportType = config.type;
    if (!Transports::transportType.compare("mqtt"))
    {
        return Mqtt::init(config.params);
    }
}

IPublisher *ServerTransports::getPublisher()
{
    if (!ServerTransports::transportType.compare("mqtt"))
    {
        return Mqtt::getServerInstance();
    }

    return nullptr;
}

IMessageBus *ServerTransports::getMessager()
{
    if (!ServerTransports::transportType.compare("mqtt"))
    {
        return Mqtt::getServerInstance();
    }

    return nullptr;
}

void ServerTransports::init(string configFile)
{
    TransportConfiguration config;
    config.loadServerConfig(configFile);

    ServerTransports::transportType = config.type;
    if (!ServerTransports::transportType.compare("mqtt"))
    {
        return Mqtt::initForServer(config.params);
    }
}
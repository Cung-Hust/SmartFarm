/*
 * Mqtt.h
 *
 *  Created on: Jul 15, 2022
 *      Author: rd
 */

#pragma once

#include <mosquittopp.h>
#include <iostream>
#include <cstring>
#include <string>
#include <vector>
#include <map>
#include <queue>
#include <mutex>

#include "../IMessageBus.hpp"
#include "../IPublisher.hpp"

#define TAG "Mqtt.cpp:"
#define MQTT_SIZE_QUEUE_DEFAULT 100

class SafetyQueue
{
private:
    mutex mutex_;
    queue<string> queue_;
    int sizeQueue_;

public:
    SafetyQueue()
    {
        this->sizeQueue_ = MQTT_SIZE_QUEUE_DEFAULT;
    }
    SafetyQueue(int size)
    {
        this->sizeQueue_ = size;
    }

    void push(string data)
    {
        const std::lock_guard<std::mutex> lock(this->mutex_);
        if (this->queue_.size() < (std::size_t)this->sizeQueue_)
        {
            this->queue_.push(data);
        }
    }

    bool pop(string &data)
    {
        const std::lock_guard<std::mutex> lock(this->mutex_);
        if (!this->queue_.empty())
        {
            data = this->queue_.front();
            this->queue_.pop();
            return true;
        }
        return false;
    }
};

class Mqtt : public mosqpp::mosquittopp, public IPublisher, public IMessageBus
{
private:
    static Mqtt *gInstance;
    static Mqtt *gServerInstance;

    string id;
    string host;
    int port;
    int keepalive;
    int sizeQueue_;

    // const int qos = 0;
    const int qos = 2;
    const bool isRetain = false;

    std::map<string, SafetyQueue *> dataStore;

    void on_connect(int rc);
    void on_disconnect(int rc);
    void on_publish(int mid);
    void on_subscribe(int /*mid*/, int /*qos_count*/, const int * /*granted_qos*/);
    void on_message(const struct mosquitto_message *message);

public:
    static Mqtt *getInstance();
    static void init(map<string, string> params);
    static Mqtt *getServerInstance();
    static void initForServer(map<string, string> params);

    Mqtt(string id, string host, int port, string username, string password, int sizeQueue);
    ~Mqtt();

    bool subscribe(string topic);
    bool publish(string topic, string message);
    bool getMessage(string topic, string &data);
};

/*
 * Mqtt.cpp
 *
 *  Created on: Jul 15, 2022
 *      Author: rd
 */

#include "Mqtt.h"
#include <iostream>

#include <memory>
#include <stdexcept>
#include <algorithm>

Mqtt *Mqtt::gInstance = nullptr;
Mqtt *Mqtt::gServerInstance = nullptr;

vector<string> split(string str, string token)
{
    vector<string> result;
    while (str.size())
    {
        int index = str.find(token);
        if (index != string::npos)
        {
            result.push_back(str.substr(0, index));
            str = str.substr(index + token.size());
            if (str.size() == 0)
                result.push_back(str);
        }
        else
        {
            result.push_back(str);
            str = "";
        }
    }
    return result;
}

Mqtt *Mqtt::getInstance()
{
    return Mqtt::gInstance;
}

Mqtt *Mqtt::getServerInstance()
{
    return Mqtt::gServerInstance;
}

void Mqtt::init(map<string, string> params)
{
    if (Mqtt::gInstance == nullptr)
    {
        string id = params["id"];
        string host = params["host"];
        string portStr = params["port"];
        int port = stoi(portStr);
        string sizeStr = params["maxqueue"];
        int sizeQueue = stoi(sizeStr);
        if (sizeQueue == 0)
        {
            sizeQueue = 100;
        }
        string username = params["username"];
        string password = params["password"];

        Mqtt::gInstance = new Mqtt(id, host, port, username, password, sizeQueue);
    }
}

void Mqtt::initForServer(map<string, string> params)
{
    if (Mqtt::gServerInstance == nullptr)
    {
        string id = params["id"];
        string host = params["host"];
        string portStr = params["port"];
        int port = stoi(portStr);
        string sizeStr = params["maxqueue"];
        int sizeQueue = stoi(sizeStr);
        if (sizeQueue == 0)
        {
            sizeQueue = MQTT_SIZE_QUEUE_DEFAULT;
        }
        string username = params["username"];
        string password = params["password"];

        Mqtt::gServerInstance = new Mqtt(id, host, port, username, password, sizeQueue);
    }
}

Mqtt::Mqtt(string id, string host, int port, string username, string password, int sizeQueue)
{
    mosqpp::lib_init();
    this->id = id;
    this->keepalive = 60;
    this->port = port;
    this->host = host;
    this->sizeQueue_ = sizeQueue;

    if (username != "" && password != "")
    {
        mosquittopp::username_pw_set(username.c_str(), password.c_str());
    }

    /*
     * Connect to an MQTT broker. This is a non-blocking call. If you use mosquitto_connect_async your client must use
     * the threaded interface mosquitto_loop_start.
     */
    connect_async(this->host.c_str(), this->port, this->keepalive);
    loop_start();
}

Mqtt::~Mqtt()
{
    disconnect();
    loop_stop();
    mosqpp::lib_cleanup();
}

bool Mqtt::subscribe(string topic)
{
    bool success = true;

    int answer = mosquittopp::subscribe(nullptr, topic.c_str());
    if (answer != MOSQ_ERR_SUCCESS)
    {
        success = false;
        exit(0);
    }

    this->dataStore.insert(std::make_pair(topic, new SafetyQueue(this->sizeQueue_)));

    return success;
}

bool Mqtt::publish(string topic, string message)
{
    int answer = mosqpp::mosquittopp::publish(nullptr, topic.c_str(), message.length(), message.c_str(), this->qos, this->isRetain);
    return (answer == MOSQ_ERR_SUCCESS);
}

void Mqtt::on_subscribe(int, int, const int *)
{
    // cout << TAG << "Subscription succeeded." << endl;
}

void Mqtt::on_message(const struct mosquitto_message *message)
{
    string payload = string(static_cast<char *>(message->payload));
    string topic = string(message->topic);

    auto it = this->dataStore.find(topic);
    if (it == this->dataStore.end())
    {
        return;
    }
    else
    {
        it->second->push(payload);
    }
}

void Mqtt::on_disconnect(int rc)
{
    cout << TAG << "Disconnection(" << rc << ")" << endl;
    reconnect();
    exit(0);
}

void Mqtt::on_connect(int rc)
{
    if (rc == 0)
    {
        cout << TAG << "MQTT connect success" << endl;
    }
    else
    {
        cout << TAG << "Unable to connect MQTT (" << rc << ")" << endl;
    }
}

void Mqtt::on_publish(int mid) {}

bool Mqtt::getMessage(string topic, string &data)
{
    auto it = this->dataStore.find(topic);
    if (it == this->dataStore.end())
    {
        return false;
    }
    else
    {
        return it->second->pop(data);
    }
}

string executeCmd(const string cmd)
{
    std::array<char, 128> buffer;
    std::string result;
    std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd.c_str(), "r"), pclose);
    if (!pipe)
    {
        throw std::runtime_error("popen() failed!");
    }
    while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr)
    {
        result += buffer.data();
    }

    return result;
}

string getMacAddr(void)
{
    std::string str = executeCmd("cat /sys/class/net/eth0/address");
    str.erase(std::remove(str.begin(), str.end(), ':'), str.end());
    str.erase(std::remove(str.begin(), str.end(), '\n'), str.end());

    return str;
}

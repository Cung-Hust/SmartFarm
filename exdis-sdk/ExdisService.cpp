/*
 * Service.cpp
 *
 *  Created on: Aug 2, 2022
 *      Author: phanhai
 */

#include "ExdisService.hpp"
#include "coreservices-sdk/Management.hpp"

#include <iostream>
#include <unistd.h>
#include <iostream>
#include <unistd.h>
#include <thread>
#include <pthread.h>

namespace exdisservice
{
    ExdisServiceConfiguration config;
    void ExdisService::init(string filename)
    {
        this->loadConfig(filename);
        server::Command server(this->name);
        this->server = server;

        Transports::init("edged.json");
        ServerTransports::init("edged.json");

        if (!ServerTransports::getMessager()->subscribe(this->requestGatewayTopic))
        {
            std::cout << "subscribe failed";
            return;
        }
        else
        {
            std::cout << "subscribe server success" << endl;
        };

        this->responder_.init(this->responseServiceTopic);
        this->responder_.registerCallback(OP_READINGS_ADD, bind(&ExdisService::getDeviceStatusHandler, this, placeholders::_1, placeholders::_2));

        this->reporter_.init(this->readingServiceTopic);
        this->reporter_.registerCallback(OP_DEVICE_ADD, bind(&ExdisService::getDeviceDiscoveryHandler, this, placeholders::_1, placeholders::_2));
    }

    void ExdisService::run_thread_1(bool isDetach)
    {
        cout << this->requestGatewayTopic << endl;
        string startMes = this->name + ": started";
        slog_print(SLOG_INFO, 1, startMes.c_str());
        this->commander.commander(isDetach, this->requestGatewayTopic);
    }

    void ExdisService::run_thread_2(bool isDetach)
    {
        cout << this->readingServiceTopic << endl;
        string startMes = this->name + ": started";
        slog_print(SLOG_INFO, 1, startMes.c_str());
        this->reporter_.server(isDetach);
    }

    void ExdisService::run_thread_3(bool isDetach)
    {
        cout << this->responseServiceTopic << endl;
        string startMes = this->name + ": started";
        slog_print(SLOG_INFO, 1, startMes.c_str());
        this->responder_.server(isDetach);
    }

    void ExdisService::getDeviceDiscoveryHandler(dto::HeaderRequest header, string message)
    {
        this->responder.responder(message);
    }

    void ExdisService::getDeviceStatusHandler(dto::HeaderRequest header, string message)
    {
        this->reporter.reporter(message);
    }
}
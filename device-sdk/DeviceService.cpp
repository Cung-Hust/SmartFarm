/*
 * DeviceService.cpp
 *
 *  Created on: Aug 2, 2022
 *      Author: phanhai
 */

#include "DeviceService.hpp"
#include "../database/Database.hpp"

#include <iostream>
#include <thread>
#include <pthread.h>
#include <unistd.h>

namespace deviceservice
{
    string DeviceService::init(string filename, IDriver *driver)
    {
        this->loadConfig(filename);
        this->driver = driver;
        server::DeviceService server(this->name, this->getReponseTopic("command"), this->getRequestTopic("coredata"));
        this->server = server;

        slog_init(this->name.c_str(), SLOG_FLAGS_ALL, 1);

        // Register DeviceService to DB
        string err;
        model::DeviceService ds;
        ds.name = this->name;
        ds.address = this->name;
        Db::getDb()->addDeviceService(ds, err);
        if (err != "")
        {
            slog_print(SLOG_ERROR, 1, err.c_str());
        }

        this->requester.init(this->requestServiceTopic);
        this->requester.registerCallback(OP_DEVICE_GET, bind(&DeviceService::getCommandHandler, this, placeholders::_1, placeholders::_2));
        this->requester.registerCallback(OP_DEVICE_SET, bind(&DeviceService::setCommandHandler, this, placeholders::_1, placeholders::_2));
        this->requester.registerCallback(OP_DEVICE_DISCOVERY, bind(&DeviceService::discoveryCommandHandler, this, placeholders::_1, placeholders::_2));
        this->requester.registerCallback(OP_DEVICECALLBACK_ADD, bind(&DeviceService::addDeviceHandler, this, placeholders::_1, placeholders::_2));
        this->requester.registerCallback(OP_DEVICECALLBACK_UPDATE, bind(&DeviceService::updateDeviceHandler, this, placeholders::_1, placeholders::_2));
        this->requester.registerCallback(OP_DEVICECALLBACK_DELETE, bind(&DeviceService::deleteDeviceHandler, this, placeholders::_1, placeholders::_2));

        this->rdqueue = new ReadingQueue(this->queueSize);
        function<void()> readingServe = bind(&DeviceService::listenAndPushReading, this);
        thread thr(readingServe);
        thr.detach();

        return this->driver->init(this->userConfig, this->rdqueue);
    }

    void DeviceService::run(bool isDetach)
    {
        slog_print(SLOG_INFO, 1, "start");
        this->requester.server(isDetach);
    }

    void DeviceService::getCommandHandler(dto::HeaderRequest header, string message)
    {
        string err;
        string responseTopic = this->getReponseTopic(header.client);
        cout << "responseTopic :: " << responseTopic << endl;
        dto::IssueGetCommandRequest req;
        try
        {
            deserialize<dto::IssueGetCommandRequest>(message, req);
        }
        catch (...)
        {
            err = INVALID_MESSAGE_ERROR;

            slog_print(SLOG_ERROR, 1, err.c_str());
            err = this->server.responseGetCommand(header.rqi, err, vector<model::Reading>{});
            if (err != "")
            {
                slog_print(SLOG_ERROR, 1, err.c_str());
            }

            return;
        }

        vector<model::Reading> readings = this->driver->handleGetCommand(req.id, req.resources, req.options, err);
        if (err != "")
        {
            slog_print(SLOG_ERROR, 1, err.c_str());
        }

        err = this->server.responseGetCommand(header.rqi, err, readings);
        if (err != "")
        {
            slog_print(SLOG_ERROR, 1, err.c_str());
        }

        return;
    }

    void DeviceService::setCommandHandler(dto::HeaderRequest header, string message)
    {
        string err;
        string responseTopic = this->getReponseTopic(header.client);
        cout << "responseTopic :2: " << responseTopic << endl;
        dto::IssueSetCommandRequest req;
        try
        {
            deserialize<dto::IssueSetCommandRequest>(message, req);
        }
        catch (...)
        {
            err = INVALID_MESSAGE_ERROR;
            slog_print(SLOG_ERROR, 1, err.c_str());
            err = this->server.responseSetCommand(header.rqi, err);
            if (err != "")
            {
                slog_print(SLOG_ERROR, 1, err.c_str());
            }

            return;
        }

        this->driver->handleSetCommand(req.id, req.resources, req.options, err);
        if (err != "")
        {
            slog_print(SLOG_ERROR, 1, err.c_str());
        }

        err = this->server.responseSetCommand(header.rqi, err);
        if (err != "")
        {
            slog_print(SLOG_ERROR, 1, err.c_str());
        }

        return;
    }

    void DeviceService::discoveryCommandHandler(dto::HeaderRequest header, string message)
    {
        string err;
        string responseTopic = this->getReponseTopic(header.client);

        dto::IssueDiscoveryCommandRequest req;
        try
        {
            deserialize<dto::IssueDiscoveryCommandRequest>(message, req);
        }
        catch (...)
        {
            err = INVALID_MESSAGE_ERROR;

            slog_print(SLOG_ERROR, 1, err.c_str());
            err = this->server.responseDiscoveryCommand(header.rqi, err, model::Device{});
            if (err != "")
            {
                slog_print(SLOG_ERROR, 1, err.c_str());
            }

            return;
        }

        model::Device dev = dto::toModel(req.device);
        dev = this->driver->handleDiscoveryCommand(dev, req.options, err);
        cout << "dev.name:  " << dev.name << endl;
        if (err != "")
        {
            slog_print(SLOG_ERROR, 1, err.c_str());
        }

        err = this->server.responseDiscoveryCommand(header.rqi, err, dev);
        if (err != "")
        {
            slog_print(SLOG_ERROR, 1, err.c_str());
        }

        return;
    }

    void DeviceService::addDeviceHandler(dto::HeaderRequest header, string message)
    {
        string err;
        string responseTopic = this->getReponseTopic(header.client);

        dto::CallbackDeviceRequest req;
        try
        {
            deserialize<dto::CallbackDeviceRequest>(message, req);
        }
        catch (...)
        {
            err = INVALID_MESSAGE_ERROR;
            slog_print(SLOG_ERROR, 1, err.c_str());

            return;
        }

        model::Device dev = dto::toModel(req.device);
        this->driver->addDeviceCallback(dev, err);
        if (err != "")
        {
            slog_print(SLOG_ERROR, 1, err.c_str());
        }
        slog_print(SLOG_INFO, 1, "Add new device: %s", req.device.id.c_str());
        return;
    }

    void DeviceService::updateDeviceHandler(dto::HeaderRequest header, string message)
    {
        string err;
        string responseTopic = this->getReponseTopic(header.client);

        dto::CallbackDeviceRequest req;
        try
        {
            deserialize<dto::CallbackDeviceRequest>(message, req);
        }
        catch (...)
        {
            err = INVALID_MESSAGE_ERROR;
            slog_print(SLOG_ERROR, 1, err.c_str());

            return;
        }

        model::Device dev = dto::toModel(req.device);
        this->driver->updateDeviceCallback(dev, err);
        if (err != "")
        {
            slog_print(SLOG_ERROR, 1, err.c_str());
        }

        return;
    }

    void DeviceService::deleteDeviceHandler(dto::HeaderRequest header, string message)
    {
        string err;
        string responseTopic = this->getReponseTopic(header.client);

        dto::CallbackDeviceRequest req;
        try
        {
            deserialize<dto::CallbackDeviceRequest>(message, req);
        }
        catch (...)
        {
            err = INVALID_MESSAGE_ERROR;
            slog_print(SLOG_ERROR, 1, err.c_str());

            return;
        }

        model::Device dev = dto::toModel(req.device);
        this->driver->deleteDeviceCallback(dev, err);
        if (err != "")
        {
            slog_print(SLOG_ERROR, 1, err.c_str());
        }

        return;
    }

    string DeviceService::addReadings(vector<model::Reading> readings)
    {
        string rqi = genUuid();
        string err = this->server.publishMultiReadings(rqi, readings);
        if (err != "")
        {
            slog_print(SLOG_ERROR, 1, err.c_str());
        }
        return err;
    }

    void DeviceService::listenAndPushReading()
    {
        vector<ResourceValues> resourceValues;
        while (true)
        {
            if (this->rdqueue->pop(resourceValues))
            {
                std::vector<model::Reading> readings;
                for (auto &rv : resourceValues)
                {
                    model::Reading reading;
                    reading.deviceId = rv.deviceId;
                    reading.value = rv.value;
                    reading.resourceName = rv.resourceName;
                    reading.timestamp = (uint64_t)std::time(nullptr);

                    readings.push_back(reading);
                }

                string err = this->addReadings(readings);
                if (err != "")
                {
                    slog_print(SLOG_ERROR, 1, err.c_str());
                }
            }

            usleep(5000);
        }
    }
}

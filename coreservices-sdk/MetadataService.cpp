/*
 * Service.cpp
 *
 *  Created on: Aug 2, 2022
 *      Author: phanhai
 */

#include "MetadataService.hpp"
#include "clients/Exdis.hpp"

#include <iostream>
#include <unistd.h>
#include <ctime>

namespace coreservices
{
    void MetadataService::init(string filename)
    {
        this->loadConfig(filename);
        server::Metadata server(this->name);
        this->server = server;

        if (this->resourceFile != "")
        {
            std::ifstream f(this->resourceFile);
            std::string str((std::istreambuf_iterator<char>(f)),
                            std::istreambuf_iterator<char>());

            vector<dto::DeviceResource> dtoResources;
            deserialize<vector<dto::DeviceResource>>(str, dtoResources);
            vector<model::DeviceResource> resources = toModel(dtoResources);

            string err;
            Db::getDb()->addResources(resources, err);
            if (err != "")
            {
                cout << err << endl;
            }
        }

        if (this->profileFile != "")
        {
            std::ifstream f(this->profileFile);
            std::string str((std::istreambuf_iterator<char>(f)),
                            std::istreambuf_iterator<char>());

            vector<dto::DeviceProfile> dtoProfiles;
            deserialize<vector<dto::DeviceProfile>>(str, dtoProfiles);

            string err;
            for (auto &dtoProfile : dtoProfiles)
            {
                model::DeviceProfile profile = toModel(dtoProfile);
                Db::getDb()->addProfile(profile, err);
                if (err != "")
                {
                    cout << err << endl;
                }
            }
        }

        this->requester.init(this->requestServiceTopic);
        this->requester.registerCallback(OP_DEVICE_ADD, bind(&MetadataService::addDeviceHandler, this, placeholders::_1, placeholders::_2));
        this->requester.registerCallback(OP_DEVICE_UPDATE, bind(&MetadataService::updateDeviceHandler, this, placeholders::_1, placeholders::_2));
        this->requester.registerCallback(OP_DEVICE_DELETE, bind(&MetadataService::deleteDeviceHandler, this, placeholders::_1, placeholders::_2));
        this->requester.registerCallback(OP_DEVICE_READ, bind(&MetadataService::readDeviceHandler, this, placeholders::_1, placeholders::_2));
        this->requester.registerCallback(OP_DEVICE_LIST, bind(&MetadataService::listDeviceHandler, this, placeholders::_1, placeholders::_2));
        this->requester.registerCallback(OP_DEVICE_COUNT, bind(&MetadataService::countDeviceHandler, this, placeholders::_1, placeholders::_2));
        this->requester.registerCallback(OP_RESOURCE_ADD, bind(&MetadataService::addResourceHandler, this, placeholders::_1, placeholders::_2));
        this->requester.registerCallback(OP_RESOURCE_DELETE, bind(&MetadataService::deleteResourceHandler, this, placeholders::_1, placeholders::_2));
        this->requester.registerCallback(OP_RESOURCE_LIST, bind(&MetadataService::listResourceHandler, this, placeholders::_1, placeholders::_2));
        this->requester.registerCallback(OP_PROFILE_ADD, bind(&MetadataService::addProfileHandler, this, placeholders::_1, placeholders::_2));
        this->requester.registerCallback(OP_PROFILE_DELETE, bind(&MetadataService::deleteProfileHandler, this, placeholders::_1, placeholders::_2));
        this->requester.registerCallback(OP_PROFILE_LIST, bind(&MetadataService::listProfileHandler, this, placeholders::_1, placeholders::_2));
        this->requester.registerCallback(OP_DEVICESERVICE_ADD, bind(&MetadataService::addDeviceServiceHandler, this, placeholders::_1, placeholders::_2));
        this->requester.registerCallback(OP_DEVICESERVICE_DELETE, bind(&MetadataService::deleteDeviceServiceHandler, this, placeholders::_1, placeholders::_2));
        this->requester.registerCallback(OP_DEVICESERVICE_LIST, bind(&MetadataService::listDeviceServiceHandler, this, placeholders::_1, placeholders::_2));
    }

    void MetadataService::run(bool isDetach)
    {
        string startMes = this->name + ": started";
        slog_print(SLOG_INFO, 1, startMes.c_str());
        this->requester.server(isDetach);
    }

    void MetadataService::addDeviceHandler(dto::HeaderRequest header, string message)
    {
        string err;
        string responseTopic = this->getReponseTopic(header.client);
        dto::AddDeviceRequest req;
        try
        {
            deserialize<dto::AddDeviceRequest>(message, req);
        }
        catch (...)
        {
            err = INVALID_MESSAGE_ERROR;
            err = this->name + ": " + err;
            slog_print(SLOG_ERROR, 1, err.c_str());
            err = this->server.response(responseTopic, req.header.rqi, err);
            if (err != "")
            {
                err = this->name + ": " + err;
                cout << "MetadataService::addDeviceHandler :: --> 1 --> " << err.c_str() << endl;
                slog_print(SLOG_ERROR, 1, err.c_str());
            }

            return;
        }

        model::Device device = dto::toModel(req.device);
        if (device.id == "")
        {
            device.id = genUuid();
        }

        Db::getDb()->addDevice(device, err);
        if (err != "")
        {
            err = this->name + ": " + err;
            cout << "MetadataService::addDeviceHandler :: --> 2 --> " << err.c_str() << endl;
            slog_print(SLOG_ERROR, 1, err.c_str());
            err = this->server.response(responseTopic, req.header.rqi, err);
            if (err != "")
            {
                err = this->name + ": " + err;
                cout << "MetadataService::addDeviceHandler :: --> 3 --> " << err.c_str() << endl;
                slog_print(SLOG_ERROR, 1, err.c_str());
            }

            return;
        }

        // Send notify to Server
        model::Notification notify;
        notify.level = NotifyLevel::NORMAL;
        notify.content = "Device {name:'" + device.name + "',id='" + device.id + "'} has been added";
        notify.timestamp = std::time(nullptr);

        Exdis client(this->name, this->getRequestTopic("exdis"));
        cout << this->name << endl;
        err = client.addNotify(req.header.rqi, notify);
        if (err != "")
        {
            err = this->name + ": " + err;
            cout << "MetadataService::addDeviceHandler :: --> 4 --> " << err.c_str() << endl;
            slog_print(SLOG_ERROR, 1, err.c_str());
        }

        // Send notify to Device Service
        if (req.notify)
        {
            string deviceServiceTopic = this->getRequestTopic(device.serviceName);
            err = this->server.publishAddDeviceCallback(deviceServiceTopic, req.header.rqi, device);
            if (err != "")
            {
                err = this->name + ": " + err;
                cout << "MetadataService::addDeviceHandler :: --> 5 --> " << err.c_str() << endl;
                slog_print(SLOG_ERROR, 1, err.c_str());
            }

            err = this->server.response(responseTopic, req.header.rqi, err);
            if (err != "")
            {
                err = this->name + ": " + err;
                cout << "MetadataService::addDeviceHandler :: --> 6 --> " << err.c_str() << endl;
                slog_print(SLOG_ERROR, 1, err.c_str());
            }

            return;
        }

        err = this->server.responseId(responseTopic, req.header.rqi, err, device.id);
        if (err != "")
        {
            err = this->name + ": " + err;
            cout << "MetadataService::addDeviceHandler :: --> 7 --> " << err.c_str() << endl;
            slog_print(SLOG_ERROR, 1, err.c_str());
        }
    }

    void MetadataService::updateDeviceHandler(dto::HeaderRequest header, string message)
    {
        string err;
        string responseTopic = this->getReponseTopic(header.client);

        dto::UpdateDeviceRequest req;
        try
        {
            deserialize<dto::UpdateDeviceRequest>(message, req);
        }
        catch (...)
        {
            err = this->name + ": " + err;
            err = INVALID_MESSAGE_ERROR;
            cout << "MetadataService::updateDeviceHandler :: --> 1 --> " << err.c_str() << endl;
            slog_print(SLOG_ERROR, 1, err.c_str());
            err = this->server.response(responseTopic, req.header.rqi, err);
            if (err != "")
            {
                err = this->name + ": " + err;
                cout << "MetadataService::updateDeviceHandler :: --> 2 --> " << err.c_str() << endl;
                slog_print(SLOG_ERROR, 1, err.c_str());
            }

            return;
        }

        auto originDev = Db::getDb()->readDevice(req.device.id, err);
        if (err != "")
        {
            err = this->name + ": " + err;
            cout << "MetadataService::updateDeviceHandler :: --> 3 --> " << err.c_str() << endl;
            slog_print(SLOG_ERROR, 1, err.c_str());
            err = this->server.response(responseTopic, req.header.rqi, err);
            if (err != "")
            {
                err = this->name + ": " + err;
                cout << "MetadataService::updateDeviceHandler :: --> 4 --> " << err.c_str() << endl;
                slog_print(SLOG_ERROR, 1, err.c_str());
            }

            return;
        }

        dto::replaceDeviceModelFieldsWithDto(&originDev, req.device);
        Db::getDb()->updateDevice(originDev, err);
        if (err != "")
        {
            err = this->name + ": " + err;
            cout << "MetadataService::updateDeviceHandler :: --> 5 --> " << err.c_str() << endl;
            slog_print(SLOG_ERROR, 1, err.c_str());
            err = this->server.response(responseTopic, req.header.rqi, err);
            if (err != "")
            {
                err = this->name + ": " + err;
                cout << "MetadataService::updateDeviceHandler :: --> 6 --> " << err.c_str() << endl;
                slog_print(SLOG_ERROR, 1, err.c_str());
            }

            return;
        }

        // Send notify to Server
        model::Notification notify;
        notify.level = NotifyLevel::NORMAL;
        notify.content = "Device {name:'" + originDev.name + "',id='" + originDev.id + "'} has been updated";
        notify.timestamp = std::time(nullptr);

        Exdis client(this->name, this->getRequestTopic("exdis"));
        err = client.addNotify(req.header.rqi, notify);
        if (err != "")
        {
            err = this->name + ": " + err;
            cout << "MetadataService::updateDeviceHandler :: --> 7 --> " << err.c_str() << endl;
            slog_print(SLOG_ERROR, 1, err.c_str());
        }

        // Send notify to Device Service
        if (req.notify)
        {
            string deviceServiceTopic = this->getRequestTopic(originDev.serviceName);
            err = this->server.publishUpdateDeviceCallback(deviceServiceTopic, req.header.rqi, originDev);
            if (err != "")
            {
                err = this->name + ": " + err;
                cout << "MetadataService::updateDeviceHandler :: --> 8 --> " << err.c_str() << endl;
                slog_print(SLOG_ERROR, 1, err.c_str());
            }

            err = this->server.response(responseTopic, req.header.rqi, err);
            if (err != "")
            {
                err = this->name + ": " + err;
                cout << "MetadataService::updateDeviceHandler :: --> 9 --> " << err.c_str() << endl;
                slog_print(SLOG_ERROR, 1, err.c_str());
            }

            return;
        }

        err = this->server.response(responseTopic, req.header.rqi, err);
        if (err != "")
        {
            err = this->name + ": " + err;
            cout << "MetadataService::updateDeviceHandler :: --> 10 --> " << err.c_str() << endl;
            slog_print(SLOG_ERROR, 1, err.c_str());
        }
    }

    void MetadataService::deleteDeviceHandler(dto::HeaderRequest header, string message)
    {
        string err;
        string responseTopic = this->getReponseTopic(header.client);

        dto::DeleteDeviceRequest req;
        try
        {
            deserialize<dto::DeleteDeviceRequest>(message, req);
        }
        catch (...)
        {
            err = INVALID_MESSAGE_ERROR;
            err = this->name + ": " + err;
            cout << "MetadataService::deleteDeviceHandler :: --> 1 --> " << err.c_str() << endl;
            slog_print(SLOG_ERROR, 1, err.c_str());
            err = this->server.response(responseTopic, req.header.rqi, err);
            if (err != "")
            {
                err = this->name + ": " + err;
                cout << "MetadataService::deleteDeviceHandler :: --> 2 --> " << err.c_str() << endl;
                slog_print(SLOG_ERROR, 1, err.c_str());
            }

            return;
        }

        auto device = Db::getDb()->readDevice(req.id, err);
        if (err != "")
        {
            err = this->name + ": " + err;
            cout << "MetadataService::deleteDeviceHandler :: --> 3 --> " << err.c_str() << endl;
            slog_print(SLOG_ERROR, 1, err.c_str());
            err = this->server.response(responseTopic, req.header.rqi, err);
            if (err != "")
            {
                err = this->name + ": " + err;
                cout << "MetadataService::deleteDeviceHandler :: --> 4 --> " << err.c_str() << endl;
                slog_print(SLOG_ERROR, 1, err.c_str());
            }

            return;
        }

        Db::getDb()->deleteDevice(req.id, err);
        if (err != "")
        {
            err = this->name + ": " + err;
            cout << "MetadataService::deleteDeviceHandler :: --> 5 --> " << err.c_str() << endl;
            slog_print(SLOG_ERROR, 1, err.c_str());
            err = this->server.response(responseTopic, req.header.rqi, err);
            if (err != "")
            {
                err = this->name + ": " + err;
                cout << "MetadataService::deleteDeviceHandler :: --> 6 --> " << err.c_str() << endl;
                slog_print(SLOG_ERROR, 1, err.c_str());
            }

            return;
        }

        // Send notify to Server
        model::Notification notify;
        notify.level = NotifyLevel::NORMAL;
        notify.content = "Device {name:'" + device.name + "',id='" + device.id + "'} has been deleted";
        notify.timestamp = std::time(nullptr);

        Exdis client(this->name, this->getRequestTopic("exdis"));
        err = client.addNotify(req.header.rqi, notify);
        if (err != "")
        {
            err = this->name + ": " + err;
            cout << "MetadataService::deleteDeviceHandler :: --> 7 --> " << err.c_str() << endl;
            slog_print(SLOG_ERROR, 1, err.c_str());
        }

        // Send notify to Device Service
        if (req.notify)
        {
            string deviceServiceTopic = this->getRequestTopic(device.serviceName);
            err = this->server.publishDeleteDeviceCallback(deviceServiceTopic, req.header.rqi, device);
            if (err != "")
            {
                err = this->name + ": " + err;
                cout << "MetadataService::deleteDeviceHandler :: --> 8 --> " << err.c_str() << endl;
                slog_print(SLOG_ERROR, 1, err.c_str());
            }

            err = this->server.response(responseTopic, req.header.rqi, err);
            if (err != "")
            {
                err = this->name + ": " + err;
                cout << "MetadataService::deleteDeviceHandler :: --> 9 --> " << err.c_str() << endl;
                slog_print(SLOG_ERROR, 1, err.c_str());
            }

            return;
        }

        err = this->server.response(responseTopic, req.header.rqi, err);
        if (err != "")
        {
            err = this->name + ": " + err;
            cout << "MetadataService::deleteDeviceHandler :: --> 10 --> " << err.c_str() << endl;
            slog_print(SLOG_ERROR, 1, err.c_str());
        }
    }

    void MetadataService::readDeviceHandler(dto::HeaderRequest header, string message)
    {
        string err;
        string responseTopic = this->getReponseTopic(header.client);

        dto::RequestWithId req;
        try
        {
            deserialize<dto::RequestWithId>(message, req);
        }
        catch (...)
        {
            err = INVALID_MESSAGE_ERROR;
            err = this->name + ": " + err;
            cout << "MetadataService::readDeviceHandler :: --> 1 --> " << err.c_str() << endl;
            slog_print(SLOG_ERROR, 1, err.c_str());
            err = this->server.response(responseTopic, req.header.rqi, err);
            if (err != "")
            {
                err = this->name + ": " + err;
                cout << "MetadataService::readDeviceHandler :: --> 2 --> " << err.c_str() << endl;
                slog_print(SLOG_ERROR, 1, err.c_str());
            }

            return;
        }

        auto device = Db::getDb()->readDevice(req.id, err);
        if (err != "")
        {
            err = this->name + ": " + err;
            slog_print(SLOG_ERROR, 1, err.c_str());
            cout << "MetadataService::readDeviceHandler :: --> 3 --> " << err.c_str() << endl;
            err = this->server.response(responseTopic, req.header.rqi, err);
            if (err != "")
            {
                err = this->name + ": " + err;
                cout << "MetadataService::readDeviceHandler :: --> 4 --> " << err.c_str() << endl;
                slog_print(SLOG_ERROR, 1, err.c_str());
            }

            return;
        }

        err = this->server.responseDevice(responseTopic, req.header.rqi, err, device);
        if (err != "")
        {
            err = this->name + ": " + err;
            cout << "MetadataService::readDeviceHandler :: --> 5 --> " << err.c_str() << endl;
            slog_print(SLOG_ERROR, 1, err.c_str());
        }
    }

    void MetadataService::listDeviceHandler(dto::HeaderRequest header, string message)
    {
        string err;
        string responseTopic = this->getReponseTopic(header.client);

        dto::RequestWithOptions req;
        try
        {
            deserialize<dto::RequestWithOptions>(message, req);
        }
        catch (...)
        {
            err = INVALID_MESSAGE_ERROR;
            err = this->name + ": " + err;
            cout << "MetadataService::listDeviceHandler :: --> 1 --> " << err.c_str() << endl;
            slog_print(SLOG_ERROR, 1, err.c_str());
            err = this->server.response(responseTopic, req.header.rqi, err);
            if (err != "")
            {
                err = this->name + ": " + err;
                cout << "MetadataService::listDeviceHandler :: --> 2 --> " << err.c_str() << endl;
                slog_print(SLOG_ERROR, 1, err.c_str());
            }

            return;
        }

        string profileFilter = req.options["profile"], serviceFilter = req.options["service"], typeFilter = req.options["type"];
        auto devices = Db::getDb()->listDevice(profileFilter, serviceFilter, typeFilter);

        err = this->server.responseMultiDevice(responseTopic, req.header.rqi, err, devices);
        if (err != "")
        {
            err = this->name + ": " + err;
            cout << "MetadataService::listDeviceHandler :: --> 3 --> " << err.c_str() << endl;
            slog_print(SLOG_ERROR, 1, err.c_str());
        }
    }

    void MetadataService::countDeviceHandler(dto::HeaderRequest header, string message)
    {
        string err;
        string responseTopic = this->getReponseTopic(header.client);

        dto::RequestWithOptions req;
        try
        {
            deserialize<dto::RequestWithOptions>(message, req);
        }
        catch (...)
        {
            err = INVALID_MESSAGE_ERROR;
            err = this->name + ": " + err;
            cout << "MetadataService::countDeviceHandler :: --> 1 --> " << err.c_str() << endl;
            slog_print(SLOG_ERROR, 1, err.c_str());
            err = this->server.response(responseTopic, req.header.rqi, err);
            if (err != "")
            {
                err = this->name + ": " + err;
                cout << "MetadataService::countDeviceHandler :: --> 2 --> " << err.c_str() << endl;
                slog_print(SLOG_ERROR, 1, err.c_str());
            }

            return;
        }

        string profileFilter = req.options["profile"], serviceFilter = req.options["service"], typeFilter = req.options["type"];
        auto count = Db::getDb()->countDevice(profileFilter, serviceFilter, typeFilter, err);
        if (err != "")
        {
            err = this->name + ": " + err;
            cout << "MetadataService::countDeviceHandler :: --> 3 --> " << err.c_str() << endl;
            slog_print(SLOG_ERROR, 1, err.c_str());
            err = this->server.response(responseTopic, req.header.rqi, err);
            if (err != "")
            {
                err = this->name + ": " + err;
                cout << "MetadataService::countDeviceHandler :: --> 4 --> " << err.c_str() << endl;
                slog_print(SLOG_ERROR, 1, err.c_str());
            }

            return;
        }

        err = this->server.responseCount(responseTopic, req.header.rqi, err, count);
        if (err != "")
        {
            err = this->name + ": " + err;
            cout << "MetadataService::countDeviceHandler :: --> 5 --> " << err.c_str() << endl;
            slog_print(SLOG_ERROR, 1, err.c_str());
        }
    }

    void MetadataService::addResourceHandler(dto::HeaderRequest header, string message)
    {
        string err;
        string responseTopic = this->getReponseTopic(header.client);

        dto::AddResourceRequest req;
        try
        {
            deserialize<dto::AddResourceRequest>(message, req);
        }
        catch (...)
        {
            err = INVALID_MESSAGE_ERROR;
            err = this->name + ": " + err;
            cout << "MetadataService::addResourceHandler :: --> 1 --> " << err.c_str() << endl;
            slog_print(SLOG_ERROR, 1, err.c_str());
            err = this->server.response(responseTopic, req.header.rqi, err);
            if (err != "")
            {
                err = this->name + ": " + err;
                cout << "MetadataService::addResourceHandler :: --> 2 --> " << err.c_str() << endl;
                slog_print(SLOG_ERROR, 1, err.c_str());
            }

            return;
        }

        vector<model::DeviceResource> resources = dto::toModel(req.resources);

        Db::getDb()->addResources(resources, err);
        if (err != "")
        {
            err = this->name + ": " + err;
            cout << "MetadataService::addResourceHandler :: --> 3 --> " << err.c_str() << endl;
            slog_print(SLOG_ERROR, 1, err.c_str());
        }

        err = this->server.response(responseTopic, req.header.rqi, err);
        if (err != "")
        {
            err = this->name + ": " + err;
            cout << "MetadataService::addResourceHandler :: --> 4 --> " << err.c_str() << endl;
            slog_print(SLOG_ERROR, 1, err.c_str());
        }
    }

    void MetadataService::deleteResourceHandler(dto::HeaderRequest header, string message)
    {
        string err;
        string responseTopic = this->getReponseTopic(header.client);

        dto::RequestWithName req;
        try
        {
            deserialize<dto::RequestWithName>(message, req);
        }
        catch (...)
        {
            err = INVALID_MESSAGE_ERROR;
            err = this->name + ": " + err;
            cout << "MetadataService::deleteResourceHandler :: --> 1 --> " << err.c_str() << endl;
            slog_print(SLOG_ERROR, 1, err.c_str());
            err = this->server.response(responseTopic, req.header.rqi, err);
            if (err != "")
            {
                err = this->name + ": " + err;
                cout << "MetadataService::deleteResourceHandler :: --> 2 --> " << err.c_str() << endl;
                slog_print(SLOG_ERROR, 1, err.c_str());
            }

            return;
        }

        Db::getDb()->deleteResource(req.name, err);
        if (err != "")
        {
            err = this->name + ": " + err;
            cout << "MetadataService::deleteResourceHandler :: --> 3 --> " << err.c_str() << endl;
            slog_print(SLOG_ERROR, 1, err.c_str());
        }

        err = this->server.response(responseTopic, req.header.rqi, err);
        if (err != "")
        {
            err = this->name + ": " + err;
            cout << "MetadataService::deleteResourceHandler :: --> 4 --> " << err.c_str() << endl;
            slog_print(SLOG_ERROR, 1, err.c_str());
        }
    }

    void MetadataService::listResourceHandler(dto::HeaderRequest header, string message)
    {
        string err;
        string responseTopic = this->getReponseTopic(header.client);

        auto resources = Db::getDb()->listResource();

        err = this->server.responseMultiResource(responseTopic, header.rqi, err, resources);
        if (err != "")
        {
            err = this->name + ": " + err;
            cout << "MetadataService::listResourceHandler :: --> 1 --> " << err.c_str() << endl;
            slog_print(SLOG_ERROR, 1, err.c_str());
        }
    }

    void MetadataService::addProfileHandler(dto::HeaderRequest header, string message)
    {
        string err;
        string responseTopic = this->getReponseTopic(header.client);

        dto::AddProfileRequest req;
        try
        {
            deserialize<dto::AddProfileRequest>(message, req);
        }
        catch (...)
        {
            err = INVALID_MESSAGE_ERROR;
            err = this->name + ": " + err;
            cout << "MetadataService::addProfileHandler :: --> 1 --> " << err.c_str() << endl;
            slog_print(SLOG_ERROR, 1, err.c_str());
            err = this->server.response(responseTopic, req.header.rqi, err);
            if (err != "")
            {
                err = this->name + ": " + err;
                cout << "MetadataService::addProfileHandler :: --> 2 --> " << err.c_str() << endl;
                slog_print(SLOG_ERROR, 1, err.c_str());
            }

            return;
        }

        model::DeviceProfile profile = dto::toModel(req.profile);

        Db::getDb()->addProfile(profile, err);
        if (err != "")
        {
            err = this->name + ": " + err;
            cout << "MetadataService::addProfileHandler :: --> 3 --> " << err.c_str() << endl;
            slog_print(SLOG_ERROR, 1, err.c_str());
        }

        err = this->server.response(responseTopic, req.header.rqi, err);
        if (err != "")
        {
            err = this->name + ": " + err;
            cout << "MetadataService::addProfileHandler :: --> 4 --> " << err.c_str() << endl;
            slog_print(SLOG_ERROR, 1, err.c_str());
        }
    }

    void MetadataService::deleteProfileHandler(dto::HeaderRequest header, string message)
    {
        string err;
        string responseTopic = this->getReponseTopic(header.client);

        dto::RequestWithName req;
        try
        {
            deserialize<dto::RequestWithName>(message, req);
        }
        catch (...)
        {
            err = INVALID_MESSAGE_ERROR;
            err = this->name + ": " + err;
            cout << "MetadataService::deleteProfileHandler :: --> 1 --> " << err.c_str() << endl;
            slog_print(SLOG_ERROR, 1, err.c_str());
            err = this->server.response(responseTopic, req.header.rqi, err);
            if (err != "")
            {
                err = this->name + ": " + err;
                cout << "MetadataService::deleteProfileHandler :: --> 2 --> " << err.c_str() << endl;
                slog_print(SLOG_ERROR, 1, err.c_str());
            }

            return;
        }

        Db::getDb()->deleteProfile(req.name, err);
        if (err != "")
        {
            err = this->name + ": " + err;
            cout << "MetadataService::deleteProfileHandler :: --> 3 --> " << err.c_str() << endl;
            slog_print(SLOG_ERROR, 1, err.c_str());
        }

        err = this->server.response(responseTopic, req.header.rqi, err);
        if (err != "")
        {
            err = this->name + ": " + err;
            cout << "MetadataService::deleteProfileHandler :: --> 4 --> " << err.c_str() << endl;
            slog_print(SLOG_ERROR, 1, err.c_str());
        }
    }

    void MetadataService::listProfileHandler(dto::HeaderRequest header, string message)
    {
        string err;
        string responseTopic = this->getReponseTopic(header.client);

        auto profiles = Db::getDb()->listProfile();

        err = this->server.responseMultiProfile(responseTopic, header.rqi, err, profiles);
        if (err != "")
        {
            err = this->name + ": " + err;
            cout << "MetadataService::listProfileHandler :: --> 1 --> " << err.c_str() << endl;
            slog_print(SLOG_ERROR, 1, err.c_str());
        }
    }

    void MetadataService::addDeviceServiceHandler(dto::HeaderRequest header, string message)
    {
        string err;
        string responseTopic = this->getReponseTopic(header.client);

        dto::AddServiceRequest req;
        try
        {
            deserialize<dto::AddServiceRequest>(message, req);
        }
        catch (...)
        {
            err = INVALID_MESSAGE_ERROR;
            err = this->name + ": " + err;
            cout << "MetadataService::addDeviceServiceHandler :: --> 1 --> " << err.c_str() << endl;
            slog_print(SLOG_ERROR, 1, err.c_str());
            err = this->server.response(responseTopic, req.header.rqi, err);
            if (err != "")
            {
                err = this->name + ": " + err;
                cout << "MetadataService::addDeviceServiceHandler :: --> 2 --> " << err.c_str() << endl;
                slog_print(SLOG_ERROR, 1, err.c_str());
            }

            return;
        }

        model::DeviceService service = dto::toModel(req.service);

        Db::getDb()->addDeviceService(service, err);
        if (err != "")
        {
            err = this->name + ": " + err;
            cout << "MetadataService::addDeviceServiceHandler :: --> 3 --> " << err.c_str() << endl;
            slog_print(SLOG_ERROR, 1, err.c_str());
        }

        err = this->server.response(responseTopic, req.header.rqi, err);
        if (err != "")
        {
            err = this->name + ": " + err;
            cout << "MetadataService::addDeviceServiceHandler :: --> 4 --> " << err.c_str() << endl;
            slog_print(SLOG_ERROR, 1, err.c_str());
        }
    }

    void MetadataService::deleteDeviceServiceHandler(dto::HeaderRequest header, string message)
    {
        string err;
        string responseTopic = this->getReponseTopic(header.client);

        dto::RequestWithName req;
        try
        {
            deserialize<dto::RequestWithName>(message, req);
        }
        catch (...)
        {
            err = INVALID_MESSAGE_ERROR;
            err = this->name + ": " + err;
            cout << "MetadataService::deleteDeviceServiceHandler :: --> 1 --> " << err.c_str() << endl;
            slog_print(SLOG_ERROR, 1, err.c_str());
            err = this->server.response(responseTopic, req.header.rqi, err);
            if (err != "")
            {
                err = this->name + ": " + err;
                cout << "MetadataService::deleteDeviceServiceHandler :: --> 2 --> " << err.c_str() << endl;
                slog_print(SLOG_ERROR, 1, err.c_str());
            }

            return;
        }

        Db::getDb()->deleteDeviceService(req.name, err);
        if (err != "")
        {
            err = this->name + ": " + err;
            cout << "MetadataService::deleteDeviceServiceHandler :: --> 3 --> " << err.c_str() << endl;
            slog_print(SLOG_ERROR, 1, err.c_str());
        }

        err = this->server.response(responseTopic, req.header.rqi, err);
        if (err != "")
        {
            err = this->name + ": " + err;
            cout << "MetadataService::deleteDeviceServiceHandler :: --> 4 --> " << err.c_str() << endl;
            slog_print(SLOG_ERROR, 1, err.c_str());
        }
    }

    void MetadataService::listDeviceServiceHandler(dto::HeaderRequest header, string message)
    {
        string err;
        string responseTopic = this->getReponseTopic(header.client);

        auto services = Db::getDb()->listDeviceService();

        err = this->server.responseMultiService(responseTopic, header.rqi, err, services);
        if (err != "")
        {
            err = this->name + ": " + err;
            cout << "MetadataService::listDeviceServiceHandler :: --> 1 --> " << err.c_str() << endl;
            slog_print(SLOG_ERROR, 1, err.c_str());
        }
    }
}

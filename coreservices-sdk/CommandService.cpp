/*
 * Service.cpp
 *
 *  Created on: Aug 2, 2022
 *      Author: phanhai
 */

#include "CommandService.hpp"

#include <iostream>
#include <unistd.h>

namespace coreservices
{
    void CommandService::init(string filename)
    {
        this->loadConfig(filename);
        server::Command server(this->name);
        this->server = server;

        this->requester.init(this->requestServiceTopic);
        this->requester.registerCallback(OP_COMMAND_GET, bind(&CommandService::issueGetCommandHandler, this, placeholders::_1, placeholders::_2));
        this->requester.registerCallback(OP_COMMAND_SET, bind(&CommandService::issueSetCommandHandler, this, placeholders::_1, placeholders::_2));
        this->requester.registerCallback(OP_COMMAND_DISCOVERY, bind(&CommandService::issueDiscoveryCommandHandler, this, placeholders::_1, placeholders::_2));

        this->responseter.init(this->responseServiceTopic);
        this->responseter.registerCallback(bind(&CommandService::responseHandler, this, placeholders::_1, placeholders::_2));
    }

    void CommandService::run(bool isDetach)
    {
        string startMes = this->name + ": started";
        slog_print(SLOG_INFO, 1, startMes.c_str());
        this->requester.server(true);
        this->responseter.server(isDetach);
    }

    void CommandService::addContext(RequestContext ctx)
    {
        if (ctx.header.rqi == "")
        {
            return;
        }

        const std::lock_guard<std::mutex> lock(this->mutex_);
        // Xoa cac context bi timeout
        for (auto it = this->ctxMap.cbegin(); it != this->ctxMap.cend();)
        {
            RequestContext c = it->second;

            if (std::time(nullptr) - c.timestamp > this->timeoutSeconds)
            {
                this->ctxMap.erase(it);
            }
            ++it;
        }

        // Them context yeu cau
        ctx.timestamp = std::time(nullptr);
        this->ctxMap.insert(std::make_pair(ctx.header.rqi, ctx));
    }

    RequestContext CommandService::getContextByRqi(string rqi, bool &hasCtx)
    {
        RequestContext ctx;
        hasCtx = false;

        const std::lock_guard<std::mutex> lock(this->mutex_);
        auto it = this->ctxMap.find(rqi);
        if (it == this->ctxMap.end())
        {
            return ctx;
        }
        else
        {
            ctx = it->second;
            this->ctxMap.erase(it);

            if (std::time(nullptr) - ctx.timestamp > this->timeoutSeconds)
            {
                return ctx;
            }
        }

        hasCtx = true;
        return ctx;
    }

    void CommandService::issueGetCommandHandler(dto::HeaderRequest header, string message)
    {
        string err;
        string responseTopic = this->getReponseTopic(header.client);

        dto::IssueGetCommandRequest req;
        try
        {
            deserialize<dto::IssueGetCommandRequest>(message, req);
        }
        catch (...)
        {
            err = INVALID_MESSAGE_ERROR;
            err = this->name + ": " + err;
            slog_print(SLOG_ERROR, 1, err.c_str());
            cout << "CommandService::issueGetCommandHandler :: --> 1 --> " << err.c_str() << endl;
            this->server.response(responseTopic, header.rqi, err);
            if (err != "")
            {
                cout << "CommandService::issueGetCommandHandler :: --> 2 --> " << err.c_str() << endl;
                err = this->name + ": " + err;
                slog_print(SLOG_ERROR, 1, err.c_str());
            }

            return;
        }

        auto device = Db::getDb()->readDevice(req.id, err);
        if (err != "")
        {
            err = this->name + ": " + err;
            cout << "CommandService::issueGetCommandHandler :: --> 3 --> " << err.c_str() << endl;
            slog_print(SLOG_ERROR, 1, err.c_str());
            err = this->server.response(responseTopic, header.rqi, err);
            if (err != "")
            {
                cout << "CommandService::issueGetCommandHandler :: --> 4 --> " << err.c_str() << endl;
                err = this->name + ": " + err;
                slog_print(SLOG_ERROR, 1, err.c_str());
            }

            return;
        }
        string requestTopic = this->getRequestTopic(device.serviceName);

        err = this->server.issueGetCommandToDeviceService(req, requestTopic);
        if (err != "")
        {
            err = this->name + ": " + err;
            cout << "CommandService::issueGetCommandHandler :: --> 5 --> " << err.c_str() << endl;
            slog_print(SLOG_ERROR, 1, err.c_str());
        }
        else
        {
            RequestContext ctx;
            ctx.header = req.header;
            ctx.timestamp = std::time(nullptr);

            this->addContext(ctx);
        }
    }

    void CommandService::issueSetCommandHandler(dto::HeaderRequest header, string message)
    {
        string err;
        string responseTopic = this->getReponseTopic(header.client);

        dto::IssueSetCommandRequest req;
        try
        {
            deserialize<dto::IssueSetCommandRequest>(message, req);
        }
        catch (...)
        {
            err = INVALID_MESSAGE_ERROR;
            err = this->name + ": " + err;
            cout << "CommandService::issueSetCommandHandler :: --> 1 --> " << err.c_str() << endl;
            slog_print(SLOG_ERROR, 1, err.c_str());
            err = this->server.response(responseTopic, header.rqi, err);
            if (err != "")
            {
                err = this->name + ": " + err;
                cout << "CommandService::issueSetCommandHandler :: --> 2 --> " << err.c_str() << endl;
                slog_print(SLOG_ERROR, 1, err.c_str());
            }

            return;
        }
        
        if (req.id == "Gateway")
        {
            cout << "Set Gateway mode" << endl;
            // return;
        }
        auto device = Db::getDb()->readDevice(req.id, err);

        if (err != "")
        {
            err = this->name + ": " + err;
            cout << "CommandService::issueSetCommandHandler :: --> 3 --> " << err.c_str() << endl;
            slog_print(SLOG_ERROR, 1, err.c_str());
            err = this->server.response(responseTopic, header.rqi, err);
            if (err != "")
            {
                err = this->name + ": " + err;
                cout << "CommandService::issueSetCommandHandler :: --> 4 --> " << err.c_str() << endl;
                slog_print(SLOG_ERROR, 1, err.c_str());
            }

            return;
        }
        string requestTopic = this->getRequestTopic(device.serviceName);

        err = this->server.issueSetCommandToDeviceService(req, requestTopic);
        if (err != "")
        {
            err = this->name + ": " + err;
            cout << "CommandService::issueSetCommandHandler :: --> 5 --> " << err.c_str() << endl;
            slog_print(SLOG_ERROR, 1, err.c_str());
        }
        else
        {
            RequestContext ctx;
            ctx.header = req.header;
            ctx.timestamp = std::time(nullptr);

            this->addContext(ctx);
        }
    }

    void CommandService::issueDiscoveryCommandHandler(dto::HeaderRequest header, string message)
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
            err = this->name + ": " + err;
            cout << "CommandService::issueDiscoveryCommandHandler :: --> 1 --> " << err.c_str() << endl;
            slog_print(SLOG_ERROR, 1, err.c_str());
            err = this->server.response(responseTopic, header.rqi, err);
            if (err != "")
            {
                cout << "CommandService::issueDiscoveryCommandHandler :: --> 2 --> " << err.c_str() << endl;
                err = this->name + ": " + err;
                slog_print(SLOG_ERROR, 1, err.c_str());
            }

            return;
        }

        string requestTopic = this->getRequestTopic(req.device.serviceName);

        err = this->server.issueDiscoveryCommandToDeviceService(req, requestTopic);
        if (err != "")
        {
            err = this->name + ": " + err;
            cout << "CommandService::issueDiscoveryCommandHandler :: --> 3 --> " << err.c_str() << endl;
            slog_print(SLOG_ERROR, 1, err.c_str());
        }
        else
        {
            RequestContext ctx;
            ctx.header = req.header;
            ctx.timestamp = std::time(nullptr);

            this->addContext(ctx);
        }
    }

    void CommandService::responseHandler(dto::HeaderResponse header, string message)
    {
        bool hasCtx;
        RequestContext ctx = this->getContextByRqi(header.rqi, hasCtx);
        if (!hasCtx)
        {
            return;
        }

        string err;
        string responseTopic = this->getReponseTopic(ctx.header.client);

        if (ctx.header.op == OP_COMMAND_GET)
        {
            dto::MultiReadingResponse req;
            try
            {
                deserialize<dto::MultiReadingResponse>(message, req);
            }
            catch (...)
            {
                err = INVALID_MESSAGE_ERROR;
                err = this->name + ": " + err;
                cout << "CommandService::responseHandler :: --> 1 --> " << err.c_str() << endl;
                slog_print(SLOG_ERROR, 1, err.c_str());
                err = this->server.response(responseTopic, header.rqi, err);
                if (err != "")
                {
                    err = this->name + ": " + err;
                    cout << "CommandService::responseHandler :: --> 2 --> " << err.c_str() << endl;
                    slog_print(SLOG_ERROR, 1, err.c_str());
                }
                return;
            }

            vector<model::Reading> readings = dto::toModel(req.readings);
            for (auto &reading : readings)
            {
                err = Db::getDb()->validReading(reading);
                if (err != "")
                {
                    err = this->name + ": " + err;
                    cout << "CommandService::responseHandler :: --> 3 --> " << err.c_str() << endl;
                    slog_print(SLOG_ERROR, 1, err.c_str());
                    err = this->server.response(responseTopic, header.rqi, err);
                    if (err != "")
                    {
                        err = this->name + ": " + err;
                        cout << "CommandService::responseHandler :: --> 4 --> " << err.c_str() << endl;
                        slog_print(SLOG_ERROR, 1, err.c_str());
                    }
                    return;
                }
            }

            cout << "responseTopic" << responseTopic << endl;
            err = this->server.responseMultiReadings(responseTopic, ctx.header.rqi, err, readings);
            if (err != "")
            {
                err = this->name + ": " + err;
                cout << "CommandService::responseHandler :: --> 5 --> " << err.c_str() << endl;
                slog_print(SLOG_ERROR, 1, err.c_str());
            }
        }
        else if (ctx.header.op == OP_COMMAND_SET || ctx.header.op == OP_COMMAND_DISCOVERY)
        {
            err = this->server.forwardMessage(responseTopic, ctx.header.rqi, message);
            if (err != "")
            {
                err = this->name + ": " + err;
                cout << "CommandService::responseHandler :: --> 6 --> " << err.c_str() << endl;
                slog_print(SLOG_ERROR, 1, err.c_str());
            }
        }
    }
}
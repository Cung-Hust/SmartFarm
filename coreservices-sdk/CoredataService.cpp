/*
 * Service.cpp
 *
 *  Created on: Aug 2, 2022
 *      Author: phanhai
 */

#include "CoredataService.hpp"

#include <iostream>
#include <unistd.h>

namespace coreservices
{
    void CoredataService::init(string filename)
    {
        this->loadConfig(filename);
        server::Coredata server(this->name, this->publishServiceTopic);
        this->server = server;

        this->requester.init(this->requestServiceTopic);
        this->requester.registerCallback(OP_READINGS_ADD, bind(&CoredataService::addReadingsHandler, this, placeholders::_1, placeholders::_2));
        this->requester.registerCallback(OP_READINGS_LIST, bind(&CoredataService::listReadingHandler, this, placeholders::_1, placeholders::_2));
    }

    void CoredataService::run(bool isDetach)
    {
        string startMes = this->name + ": started";
        slog_print(SLOG_INFO, 1, startMes.c_str());
        this->requester.server(isDetach);
    }

    void CoredataService::addReadingsHandler(dto::HeaderRequest header, string message)
    {
        string err;
        string responseTopic = this->getReponseTopic(header.client);

        dto::AddReadingsRequest req;
        try
        {
            deserialize<dto::AddReadingsRequest>(message, req);
        }
        catch (...)
        {
            err = INVALID_MESSAGE_ERROR;
            err = this->name + ": " + err;
            cout << "CoredataService::addReadingsHandler :: --> 1 --> " << err.c_str() << endl;
            slog_print(SLOG_ERROR, 1, err.c_str());
            err = this->server.response(responseTopic, req.header.rqi, err);
            if (err != "")
            {
                err = this->name + ": " + err;
                cout << "CoredataService::addReadingsHandler :: --> 2 --> " << err.c_str() << endl;
                slog_print(SLOG_ERROR, 1, err.c_str());
            }

            return;
        }
        vector<model::Reading> readings = dto::toModel(req.readings);

        Db::getDb()->validAndAddReading(readings, err);
        if (err != "")
        {
            err = this->name + ": " + err;
            cout << "CoredataService::addReadingsHandler :: --> 3 --> " << err.c_str() << endl;
            slog_print(SLOG_ERROR, 1, err.c_str());
            return;
        }
        
        err = this->server.publishMultiReadings(req.header.rqi, readings);
        if (err != "")
        {
            err = this->name + ": " + err;
            cout << "CoredataService::addReadingsHandler :: --> 4 --> " << err.c_str() << endl;
            slog_print(SLOG_ERROR, 1, err.c_str());
        }
    }

    void CoredataService::listReadingHandler(dto::HeaderRequest header, string message)
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
            cout << "CoredataService::listReadingHandler :: --> 1 --> " << err.c_str() << endl;
            slog_print(SLOG_ERROR, 1, err.c_str());
            err = this->server.response(responseTopic, req.header.rqi, err);
            if (err != "")
            {
                err = this->name + ": " + err;
                cout << "CoredataService::listReadingHandler :: --> 2 --> " << err.c_str() << endl;
                slog_print(SLOG_ERROR, 1, err.c_str());
            }

            return;
        }

        string id = req.options["id"];
        auto readings = Db::getDb()->listReading(id);

        err = this->server.responseMultiReadings(responseTopic, req.header.rqi, err, readings);
        if (err != "")
        {
            err = this->name + ": " + err;
            cout << "CoredataService::listReadingHandler :: --> 3 --> " << err.c_str() << endl;
            slog_print(SLOG_ERROR, 1, err.c_str());
        }
    }
}
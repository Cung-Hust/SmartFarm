/*
 * ManagementService.cpp
 *
 *  Created on: Aug 2, 2022
 *      Author: phanhai
 */

#include "Management.hpp"
#include "common/utils.hpp"

#include <iostream>
#include <unistd.h>

namespace coreservices
{
    void ManagementService::init(string filename)
    {
        this->loadConfig(filename);
        server::Management server(this->name);
        this->server = server;

        slog_init(this->name.c_str(), SLOG_FLAGS_ALL, 1);

        this->requester.init(this->requestServiceTopic);
        this->requester.registerCallback(OP_METRIC_GET, bind(&ManagementService::getMetricHandler, this, placeholders::_1, placeholders::_2));
        this->requester.registerCallback(OP_OPERATION_SET, bind(&ManagementService::setOperationHandler, this, placeholders::_1, placeholders::_2));
    }

    void ManagementService::run(bool isDetach)
    {
        string startMes = this->name + ": started";
        slog_print(SLOG_INFO, 1, startMes.c_str());
        this->requester.server(isDetach);
    }

    map<std::string, std::string> ManagementService::getMetric()
    {
        map<std::string, std::string> metrics;
        string topStr = osExec("top -n 1");
        metrics.insert({"top", topStr});

        return metrics;
    }

    string ManagementService::exeOperation(string serviceName, string operation)
    {
        string err;

        string path = this->getServicePath(serviceName);
        vector<string> paths = splitStr(path, "/");
        string proceesName = "";
        if (!paths.empty())
        {
            string proceesName = paths.back();
        }
        if (proceesName == "")
        {
            return "Path is invalid";
        }

        if (operation == "start")
        {
            // Start in background
            string result = osExec("pidof -s " + proceesName);
            std::size_t found = result.find("No such file");
            if (found != std::string::npos)
            {
                return "File not found";
            }
            return "";
        }
        else if (operation == "stop")
        {
            // check if the service is running
            string pid = osExec("pidof -s " + proceesName);
            if (pid == "")
            {
                return "";
            }

            return osExec("pkill -f " + proceesName);
        }
        else if (operation == "restart")
        {
            // Kill without care
            osExec("pkill -f " + proceesName);

            // Start in background
            string result = osExec("pidof -s " + proceesName);
            std::size_t found = result.find("No such file");
            if (found != std::string::npos)
            {
                return "File not found";
            }
            return "";
        }
        else
        {
            return err = "Unknown operation";
        }
    }

    void ManagementService::getMetricHandler(dto::HeaderRequest header, string message)
    {
        string err;
        string responseTopic = this->getReponseTopic(header.client);

        map<string, string> metrics = this->getMetric();

        err = this->server.responseMetric(responseTopic, header.rqi, err, metrics);
        if (err != "")
        {
            err = this->name + ": " + err;
            cout << "ManagementService::getMetricHandler :: --> 1 --> " << err.c_str() << endl;
            slog_print(SLOG_ERROR, 1, err.c_str());
        }

        return;
    }

    void ManagementService::setOperationHandler(dto::HeaderRequest header, string message)
    {
        string err;
        string responseTopic = this->getReponseTopic(header.client);

        dto::SetOperationRequest req;
        try
        {
            deserialize<dto::SetOperationRequest>(message, req);
        }
        catch (...)
        {
            err = INVALID_MESSAGE_ERROR;
            err = this->name + ": " + err;
            cout << "ManagementService::setOperationHandler :: --> 1 --> " << err.c_str() << endl;
            slog_print(SLOG_ERROR, 1, err.c_str());
            err = this->server.response(responseTopic, header.rqi, err);
            if (err != "")
            {
                err = this->name + ": " + err;
                cout << "ManagementService::setOperationHandler :: --> 2 --> " << err.c_str() << endl;
                slog_print(SLOG_ERROR, 1, err.c_str());
            }

            return;
        }

        for (auto it = req.operations.begin(); it != req.operations.end(); ++it)
        {
            err = this->exeOperation(it->first, it->second);
            if (err != "")
            {
                err = this->name + ": " + err;
                cout << "ManagementService::setOperationHandler :: --> 3 --> " << err.c_str() << endl;
                slog_print(SLOG_ERROR, 1, err.c_str());
                break;
            }
        }

        err = this->server.response(responseTopic, header.rqi, err);
        if (err != "")
        {
            err = this->name + ": " + err;
            cout << "ManagementService::setOperationHandler :: --> 4 --> " << err.c_str() << endl;
            slog_print(SLOG_ERROR, 1, err.c_str());
        }

        return;
    }
}
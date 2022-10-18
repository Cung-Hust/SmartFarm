/*
 * Service.hpp
 *
 *  Created on: Aug 2, 2022
 *      Author: phanhai
 */

#pragma once

#include "configurarion.hpp"
#include "servers/Management.hpp"

#include "common/constants.hpp"
#include "common/utils.hpp"
#include "dtos/requestes.hpp"
#include "dtos/responses.hpp"

#include "controller/Controller.hpp"
#include "logging/slog.h"

namespace coreservices
{
    class ManagementService : private ManagementServiceConfiguration
    {
    private:
        SLogConfig slgCfg;
        server::Management server;
        RequestController requester;

        map<std::string, std::string> getMetric();
        string exeOperation(string serviceName, string operation);
        void getMetricHandler(dto::HeaderRequest header, string message);
        void setOperationHandler(dto::HeaderRequest header, string message);

    public:
        ManagementService(){};
        void init(string filename);
        void run(bool isDetach);
    };
}
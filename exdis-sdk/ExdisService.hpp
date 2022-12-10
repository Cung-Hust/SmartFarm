/*
 * ExdisService.hpp
 *
 *  Created on: Aug 2, 2022
 *      Author: phanhai
 */

#pragma once

#include "configurarion.hpp"
#include "../servers/Command.hpp"
#include "../clients/Command.hpp"

#include "../servers/ExDis.hpp"
#include "../common/constants.hpp"
#include "../common/utils.hpp"
#include "../dtos/requestes.hpp"
#include "../dtos/responses.hpp"

#include "../controller/Controller.hpp"
#include "../database/Database.hpp"
#include "../logging/slog.h"

#include "GetCommandHandler.hpp"
#include "GetResponseHandler.hpp"
#include "GetInfoHandler.hpp"

#include <ctime>
#include <mutex>

#include <iostream>
#include <string>

namespace exdisservice
{
    struct RequestContext
    {
        dto::HeaderRequest header;
        std::time_t timestamp;
    };

    class ExdisService : private ExdisServiceConfiguration
    {
    private:
        server::Command server;
        RequestController reporter_;
        RequestController responder_;
        GetCommandHandler commander;
        GetResponseHandler responder;
        GetInfoHandler reporter;
        map<string, RequestContext> ctxMap;
        std::mutex mutex_;

        void addContext(RequestContext ctx);
        RequestContext getContextByRqi(string rqi, bool &hasCtx);

        void getDeviceStatusHandler(dto::HeaderRequest header, string message);
        void getDeviceDiscoveryHandler(dto::HeaderRequest header, string message);

    public:
        ExdisService(){};
        void init(string filename);
        void run_thread_1(bool isDetach);
        void run_thread_2(bool isDetach);
        void run_thread_3(bool isDetach);
    };
}

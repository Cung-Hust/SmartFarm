 /*
 * CommandService.hpp
 *
 *  Created on: Aug 2, 2022
 *      Author: phanhai
 */

#pragma once

#include "configurarion.hpp"
#include "../servers/Command.hpp"
#include "../clients/Command.hpp"

#include "../common/constants.hpp"
#include "../common/utils.hpp"
#include "../dtos/requestes.hpp"
#include "../dtos/responses.hpp"

#include "../controller/Controller.hpp"
#include "../database/Database.hpp"
#include "../logging/slog.h"

#include <ctime>
#include <mutex>

namespace coreservices
{
    struct RequestContext
    {
        dto::HeaderRequest header;
        std::time_t timestamp;
    };

    class CommandService : private CommandServiceConfiguration
    {
    private:
        server::Command server;
        RequestController requester;
        ResponseController responseter;
        map<string, RequestContext> ctxMap;
        std::mutex mutex_;

        void addContext(RequestContext ctx);
        RequestContext getContextByRqi(string rqi, bool &hasCtx);

        void issueGetCommandHandler(dto::HeaderRequest header, string message);
        void issueSetCommandHandler(dto::HeaderRequest header, string message);
        void issueDiscoveryCommandHandler(dto::HeaderRequest header, string message);

        void responseHandler(dto::HeaderResponse header, string message);

    public:
        CommandService(){};
        void init(string filename);
        void run(bool isDetach);
    };
}

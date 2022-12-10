/*
 * CoredataService.hpp
 *
 *  Created on: Aug 2, 2022
 *      Author: phanhai
 */

#pragma once

#include "configurarion.hpp"
#include "../servers/Coredata.hpp"

#include "../common/constants.hpp"
#include "../common/utils.hpp"
#include "../dtos/requestes.hpp"
#include "../dtos/responses.hpp"

#include "../controller/Controller.hpp"
#include "../database/Database.hpp"

#include "../logging/slog.h"

namespace coreservices
{
    class CoredataService : private CoredataServiceConfiguration
    {
    private:
        server::Coredata server;
        RequestController requester;

        void addReadingsHandler(dto::HeaderRequest header, string message);
        void listReadingHandler(dto::HeaderRequest header, string message);

    public:
        CoredataService(){};
        void init(string filename);
        void run(bool isDetach);        
    };

}

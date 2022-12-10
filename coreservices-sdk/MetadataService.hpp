/*
 * MetadataService.hpp
 *
 *  Created on: Aug 2, 2022
 *      Author: phanhai
 */

#pragma once

#include "configurarion.hpp"
#include "../servers/Metadata.hpp"

#include "../common/constants.hpp"
#include "../common/utils.hpp"
#include "../dtos/requestes.hpp"
#include "../dtos/responses.hpp"

#include "../controller/Controller.hpp"
#include "../database/Database.hpp"
#include "../logging/slog.h"

namespace coreservices
{
    class MetadataService : private MetadataServiceConfiguration
    {
    private:
        server::Metadata server;
        RequestController requester;

        void addDeviceHandler(dto::HeaderRequest header, string message);
        void updateDeviceHandler(dto::HeaderRequest header, string message);
        void deleteDeviceHandler(dto::HeaderRequest header, string message);
        void readDeviceHandler(dto::HeaderRequest header, string message);
        void listDeviceHandler(dto::HeaderRequest header, string message);
        void countDeviceHandler(dto::HeaderRequest header, string message);

        void addResourceHandler(dto::HeaderRequest header, string message);
        void deleteResourceHandler(dto::HeaderRequest header, string message);
        void listResourceHandler(dto::HeaderRequest header, string message);

        void addProfileHandler(dto::HeaderRequest header, string message);
        void deleteProfileHandler(dto::HeaderRequest header, string message);
        void listProfileHandler(dto::HeaderRequest header, string message);

        void addDeviceServiceHandler(dto::HeaderRequest header, string message);
        void deleteDeviceServiceHandler(dto::HeaderRequest header, string message);
        void listDeviceServiceHandler(dto::HeaderRequest header, string message);

    public:
        MetadataService(){};
        void init(string filename);
        void run(bool isDetach);
    };

}

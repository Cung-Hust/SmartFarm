/*
 * Service.hpp
 *
 *  Created on: Aug 2, 2022
 *      Author: phanhai
 */

#pragma once

#include "configurarion.hpp"
#include "../servers/DeviceService.hpp"

#include "../common/constants.hpp"
#include "../common/utils.hpp"
#include "../dtos/requestes.hpp"
#include "../dtos/responses.hpp"

#include "../controller/Controller.hpp"
#include "../database/Database.hpp"
#include "../logging/slog.h"

#include "ReadingQueue.hpp"
#include "IDriver.hpp"

namespace deviceservice
{
    class DeviceService : private DeviceServiceConfiguration
    {
    private:
        SLogConfig slgCfg;
        server::DeviceService server;
        RequestController requester;
        RequestController tester;
        IDriver *driver;
        ReadingQueue *rdqueue;

        void getCommandHandler(dto::HeaderRequest header, string message);
        void setCommandHandler(dto::HeaderRequest header, string message);
        void discoveryCommandHandler(dto::HeaderRequest header, string message);
        void addDeviceHandler(dto::HeaderRequest header, string message);
        void updateDeviceHandler(dto::HeaderRequest header, string message);
        void deleteDeviceHandler(dto::HeaderRequest header, string message);

        void listenAndPushReading();
        string addReadings(vector<model::Reading> readings);

    public:
        DeviceService(){};
        string init(string filename, IDriver *driver);
        void run(bool isDetach);
    };
}
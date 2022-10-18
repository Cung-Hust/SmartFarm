/*
 * MetadataService.hpp
 *
 *  Created on: Aug 2, 2022
 *      Author: phanhai
 */

#pragma once

#include "configurarion.hpp"
#include "servers/Rule.hpp"

#include "common/constants.hpp"
#include "common/utils.hpp"
#include "dtos/requestes.hpp"
#include "dtos/responses.hpp"

#include "controller/Controller.hpp"
#include "database/Database.hpp"
#include "logging/slog.h"

#include "RuleEngine.hpp"
#include "clients/Exdis.hpp"
#include "clients/Command.hpp"

namespace coreservices
{
    class RuleService : private RuleServiceConfiguration
    {
    private:
        server::Rule server;
        RequestController requester;
        RequestController requester_1;
        RuleEngine ruleEngine;
        Exdis exdisClient;
        Command commandClient;

        void addRuleHandler(dto::HeaderRequest header, string message);
        void updateRuleHandler(dto::HeaderRequest header, string message);
        void deleteRuleHandler(dto::HeaderRequest header, string message);
        void readRuleHandler(dto::HeaderRequest header, string message);
        void listRuleHandler(dto::HeaderRequest header, string message);
        void setStateRuleHandler(dto::HeaderRequest header, string message);
        void setTriggerRuleHandler(dto::HeaderRequest header, string message);
        void addReadingsHandler(dto::HeaderRequest header, string message);

    public:
        RuleService(){};
        void init(string filename);
        void run(bool isDetach);
    };

};

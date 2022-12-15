/*
 * Service.cpp
 *
 *  Created on: Aug 2, 2022
 *      Author: phanhai
 */

#include "RuleService.hpp"

#include <iostream>
#include <unistd.h>
#include <ctime>

namespace coreservices
{
    void RuleService::init(string filename)
    {
        this->loadConfig(filename);

        server::Rule server(this->name);
        this->server = server;

        this->commandClient = Command(this->name, this->getRequestTopic("command"));
        this->exdisClient = Exdis(this->name, this->getRequestTopic("exdis"));

        this->ruleEngine.init(this->commandClient);

        this->requester.init(this->requestServiceTopic);
        this->requester.registerCallback(OP_RULE_ADD, bind(&RuleService::addRuleHandler, this, placeholders::_1, placeholders::_2));
        this->requester.registerCallback(OP_RULE_UPDATE, bind(&RuleService::updateRuleHandler, this, placeholders::_1, placeholders::_2));
        this->requester.registerCallback(OP_RULE_DELETE, bind(&RuleService::deleteRuleHandler, this, placeholders::_1, placeholders::_2));
        this->requester.registerCallback(OP_RULE_READ, bind(&RuleService::readRuleHandler, this, placeholders::_1, placeholders::_2));
        this->requester.registerCallback(OP_RULE_LIST, bind(&RuleService::listRuleHandler, this, placeholders::_1, placeholders::_2));
        this->requester.registerCallback(OP_RULE_STATE, bind(&RuleService::setStateRuleHandler, this, placeholders::_1, placeholders::_2));
        this->requester.registerCallback(OP_RULE_STATE_ALL, bind(&RuleService::setStateAllRuleHandler, this, placeholders::_1, placeholders::_2));
        this->requester.registerCallback(OP_RULE_TRIGGER, bind(&RuleService::setTriggerRuleHandler, this, placeholders::_1, placeholders::_2));

        this->requester_1.init(this->readingServiceTopic);
        this->requester_1.registerCallback(OP_READINGS_ADD, bind(&RuleService::addReadingsHandler, this, placeholders::_1, placeholders::_2));
    }

    void RuleService::run(bool isDetach)
    {
        string startMes = this->name + ": started";
        slog_print(SLOG_INFO, 1, startMes.c_str());
        this->requester.server(true);
        this->requester_1.server(isDetach);
    }

    void RuleService::addRuleHandler(dto::HeaderRequest header, string message)
    {
        string err;
        string responseTopic = this->getReponseTopic(header.client);

        dto::AddRuleRequest req;
        try
        {
            deserialize<dto::AddRuleRequest>(message, req);
        }
        catch (...)
        {
            err = INVALID_MESSAGE_ERROR;
            err = this->name + ": " + err;
            cout << "RuleService::addRuleHandler :: --> 1 --> " << err.c_str() << endl;
            slog_print(SLOG_ERROR, 1, err.c_str());
            err = this->server.response(responseTopic, req.header.rqi, err);
            if (err != "")
            {
                err = this->name + ": " + err;
                cout << "RuleService::addRuleHandler :: --> 2 --> " << err.c_str() << endl;
                slog_print(SLOG_ERROR, 1, err.c_str());
            }

            return;
        }

        model::Rule rule = dto::toModel(req.rule);
        if (rule.id == "")
        {
            rule.id = genUuid();
        }

        Db::getDb()->addRule(rule, err);
        if (err != "")
        {
            err = this->name + ": " + err;
            cout << "RuleService::addRuleHandler :: --> 3 --> " << err.c_str() << endl;
            slog_print(SLOG_ERROR, 1, err.c_str());
        }

        this->ruleEngine.addCallback(rule, err);
        if (err != "")
        {
            err = this->name + ": " + err;
            cout << "RuleService::addRuleHandler :: --> 4 --> " << err.c_str() << endl;
            slog_print(SLOG_ERROR, 1, err.c_str());
        }

        err = this->server.responseId(responseTopic, req.header.rqi, err, rule.id);
        if (err != "")
        {
            err = this->name + ": " + err;
            cout << "RuleService::addRuleHandler :: --> 5 --> " << err.c_str() << endl;
            slog_print(SLOG_ERROR, 1, err.c_str());
        }
    }

    void RuleService::updateRuleHandler(dto::HeaderRequest header, string message)
    {
        string err;
        string responseTopic = this->getReponseTopic(header.client);

        dto::UpdateRuleRequest req;
        try
        {
            deserialize<dto::UpdateRuleRequest>(message, req);
        }
        catch (...)
        {
            err = this->name + ": " + err;
            err = INVALID_MESSAGE_ERROR;
            cout << "RuleService::updateRuleHandler :: --> 1 --> " << err.c_str() << endl;
            slog_print(SLOG_ERROR, 1, err.c_str());
            err = this->server.response(responseTopic, req.header.rqi, err);
            if (err != "")
            {
                err = this->name + ": " + err;
                cout << "RuleService::updateRuleHandler :: --> 2 --> " << err.c_str() << endl;
                slog_print(SLOG_ERROR, 1, err.c_str());
            }

            return;
        }

        auto originRule = Db::getDb()->readRule(req.rule.id, err);
        if (err != "")
        {
            err = this->name + ": " + err;
            cout << "RuleService::updateRuleHandler :: --> 3 --> " << err.c_str() << endl;
            slog_print(SLOG_ERROR, 1, err.c_str());
            err = this->server.response(responseTopic, req.header.rqi, err);
            if (err != "")
            {
                err = this->name + ": " + err;
                cout << "RuleService::updateRuleHandler :: --> 4 --> " << err.c_str() << endl;
                slog_print(SLOG_ERROR, 1, err.c_str());
            }

            return;
        }

        dto::ReplaceRuleModelFieldsWithDto(&originRule, req.rule);
        Db::getDb()->updateRule(originRule, err);
        if (err != "")
        {
            err = this->name + ": " + err;
            cout << "RuleService::updateRuleHandler :: --> 5 --> " << err.c_str() << endl;
            slog_print(SLOG_ERROR, 1, err.c_str());
            err = this->server.response(responseTopic, req.header.rqi, err);
            if (err != "")
            {
                err = this->name + ": " + err;
                cout << "RuleService::updateRuleHandler :: --> 6 --> " << err.c_str() << endl;
                slog_print(SLOG_ERROR, 1, err.c_str());
            }

            return;
        }

        this->ruleEngine.updateCallback(originRule, err);
        if (err != "")
        {
            err = this->name + ": " + err;
            cout << "RuleService::updateRuleHandler :: --> 7 --> " << err.c_str() << endl;
            slog_print(SLOG_ERROR, 1, err.c_str());
        }

        // Send notify to Server
        model::Notification notify;
        notify.level = NotifyLevel::NORMAL;
        notify.content = "Rule {name:'" + originRule.name + "',id='" + originRule.id + "'} has been updated";
        notify.timestamp = std::time(nullptr);

        err = this->exdisClient.addNotify(req.header.rqi, notify);
        if (err != "")
        {
            err = this->name + ": " + err;
            cout << "RuleService::updateRuleHandler :: --> 8 --> " << err.c_str() << endl;
            slog_print(SLOG_ERROR, 1, err.c_str());
        }

        err = this->server.response(responseTopic, req.header.rqi, err);
        if (err != "")
        {
            err = this->name + ": " + err;
            cout << "RuleService::updateRuleHandler :: --> 9 --> " << err.c_str() << endl;
            slog_print(SLOG_ERROR, 1, err.c_str());
        }
    }

    void RuleService::deleteRuleHandler(dto::HeaderRequest header, string message)
    {
        string err;
        string responseTopic = this->getReponseTopic(header.client);

        dto::RequestWithId req;
        try
        {
            deserialize<dto::RequestWithId>(message, req);
        }
        catch (...)
        {
            err = INVALID_MESSAGE_ERROR;
            err = this->name + ": " + err;
            cout << "RuleService::deleteRuleHandler :: --> 1 --> " << err.c_str() << endl;
            slog_print(SLOG_ERROR, 1, err.c_str());
            err = this->server.response(responseTopic, req.header.rqi, err);
            if (err != "")
            {
                err = this->name + ": " + err;
                cout << "RuleService::deleteRuleHandler :: --> 2 --> " << err.c_str() << endl;
                slog_print(SLOG_ERROR, 1, err.c_str());
            }

            return;
        }

        Db::getDb()->deleteRule(req.id, err);
        if (err != "")
        {
            err = this->name + ": " + err;
            cout << "RuleService::deleteRuleHandler :: --> 3 --> " << err.c_str() << endl;
            slog_print(SLOG_ERROR, 1, err.c_str());
            err = this->server.response(responseTopic, req.header.rqi, err);
            if (err != "")
            {
                err = this->name + ": " + err;
                cout << "RuleService::deleteRuleHandler :: --> 4 --> " << err.c_str() << endl;
                slog_print(SLOG_ERROR, 1, err.c_str());
            }

            return;
        }

        this->ruleEngine.deleteCallback(req.id, err);
        if (err != "")
        {
            err = this->name + ": " + err;
            cout << "RuleService::deleteRuleHandler :: --> 5 --> " << err.c_str() << endl;
            slog_print(SLOG_ERROR, 1, err.c_str());
        }

        // Send notify to Server
        model::Notification notify;
        notify.level = NotifyLevel::NORMAL;
        notify.content = "Rule {id:'" + req.id + "'} has been deleted";
        notify.timestamp = std::time(nullptr);

        err = this->exdisClient.addNotify(req.header.rqi, notify);
        if (err != "")
        {
            err = this->name + ": " + err;
            cout << "RuleService::deleteRuleHandler :: --> 6 --> " << err.c_str() << endl;
            slog_print(SLOG_ERROR, 1, err.c_str());
        }

        err = this->server.response(responseTopic, req.header.rqi, err);
        if (err != "")
        {
            err = this->name + ": " + err;
            cout << "RuleService::deleteRuleHandler :: --> 7 --> " << err.c_str() << endl;
            slog_print(SLOG_ERROR, 1, err.c_str());
        }
    }

    void RuleService::readRuleHandler(dto::HeaderRequest header, string message)
    {
        string err;
        string responseTopic = this->getReponseTopic(header.client);

        dto::RequestWithId req;
        try
        {
            deserialize<dto::RequestWithId>(message, req);
        }
        catch (...)
        {
            err = INVALID_MESSAGE_ERROR;
            err = this->name + ": " + err;
            cout << "RuleService::readRuleHandler :: --> 1 --> " << err.c_str() << endl;
            slog_print(SLOG_ERROR, 1, err.c_str());
            err = this->server.response(responseTopic, req.header.rqi, err);
            if (err != "")
            {
                err = this->name + ": " + err;
                cout << "RuleService::readRuleHandler :: --> 2 --> " << err.c_str() << endl;
                slog_print(SLOG_ERROR, 1, err.c_str());
            }

            return;
        }

        auto rule = Db::getDb()->readRule(req.id, err);
        if (err != "")
        {
            err = this->name + ": " + err;
            cout << "RuleService::readRuleHandler :: --> 3 --> " << err.c_str() << endl;
            slog_print(SLOG_ERROR, 1, err.c_str());
            err = this->server.response(responseTopic, req.header.rqi, err);
            if (err != "")
            {
                err = this->name + ": " + err;
                cout << "RuleService::readRuleHandler :: --> 4 --> " << err.c_str() << endl;
                slog_print(SLOG_ERROR, 1, err.c_str());
            }

            return;
        }

        err = this->server.responseRule(responseTopic, req.header.rqi, err, rule);
        if (err != "")
        {
            err = this->name + ": " + err;
            cout << "RuleService::readRuleHandler :: --> 5 --> " << err.c_str() << endl;
            slog_print(SLOG_ERROR, 1, err.c_str());
        }
    }

    void RuleService::listRuleHandler(dto::HeaderRequest header, string message)
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
            cout << "RuleService::listRuleHandler :: --> 1 --> " << err.c_str() << endl;
            slog_print(SLOG_ERROR, 1, err.c_str());
            err = this->server.response(responseTopic, req.header.rqi, err);
            if (err != "")
            {
                err = this->name + ": " + err;
                cout << "RuleService::listRuleHandler :: --> 2 --> " << err.c_str() << endl;
                slog_print(SLOG_ERROR, 1, err.c_str());
            }

            return;
        }

        string profileFilter = req.options["profile"], serviceFilter = req.options["service"], typeFilter = req.options["type"];
        auto rules = Db::getDb()->listRule(typeFilter, err);

        err = this->server.responseMultiRule(responseTopic, req.header.rqi, err, rules);
        if (err != "")
        {
            err = this->name + ": " + err;
            cout << "RuleService::listRuleHandler :: --> 3 --> " << err.c_str() << endl;
            slog_print(SLOG_ERROR, 1, err.c_str());
        }
    }

    void RuleService::setStateRuleHandler(dto::HeaderRequest header, string message)
    {
        string err;
        string responseTopic = this->getReponseTopic(header.client);

        dto::SetStateRuleRequest req;
        try
        {
            deserialize<dto::SetStateRuleRequest>(message, req);
        }
        catch (...)
        {
            err = INVALID_MESSAGE_ERROR;
            err = this->name + ": " + err;
            cout << "RuleService::setStateRuleHandler :: --> 1 --> " << err.c_str() << endl;
            slog_print(SLOG_ERROR, 1, err.c_str());
            err = this->server.response(responseTopic, req.header.rqi, err);
            if (err != "")
            {
                err = this->name + ": " + err;
                cout << "RuleService::setStateRuleHandler :: --> 2 --> " << err.c_str() << endl;
                slog_print(SLOG_ERROR, 1, err.c_str());
            }

            return;
        }
        ActiveState activeState = req.activestate;

        cout << "ActiveState activeState = req.activestate :: " << activeState << endl;

        Db::getDb()->updateRuleActiveState(req.id, activeState, err);
        if (err != "")
        {
            err = this->name + ": " + err;
            cout << "RuleService::setStateRuleHandler :: --> 3 --> " << err.c_str() << endl;
            slog_print(SLOG_ERROR, 1, err.c_str());
            err = this->server.response(responseTopic, req.header.rqi, err);
            if (err != "")
            {
                err = this->name + ": " + err;
                cout << "RuleService::setStateRuleHandler :: --> 4 --> " << err.c_str() << endl;
                slog_print(SLOG_ERROR, 1, err.c_str());
            }

            return;
        }

        this->ruleEngine.setStateCallback(req.id, activeState, err);
        if (err != "")
        {
            err = this->name + ": " + err;
            cout << "RuleService::setStateRuleHandler :: --> 5 --> " << err.c_str() << endl;
            slog_print(SLOG_ERROR, 1, err.c_str());
        }

        err = this->server.response(responseTopic, req.header.rqi, err);
        if (err != "")
        {
            err = this->name + ": " + err;
            cout << "RuleService::setStateRuleHandler :: --> 6 --> " << err.c_str() << endl;
            slog_print(SLOG_ERROR, 1, err.c_str());
        }
    }

    void RuleService::setStateAllRuleHandler(dto::HeaderRequest header, string message)
    {
        string err;
        string responseTopic = this->getReponseTopic(header.client);

        dto::SetStateRuleRequest req;
        try
        {
            deserialize<dto::SetStateRuleRequest>(message, req);
        }
        catch (...)
        {
            err = INVALID_MESSAGE_ERROR;
            err = this->name + ": " + err;
            cout << "RuleService::setStateRuleHandler :: --> 1 --> " << err.c_str() << endl;
            slog_print(SLOG_ERROR, 1, err.c_str());
            err = this->server.response(responseTopic, req.header.rqi, err);
            if (err != "")
            {
                err = this->name + ": " + err;
                cout << "RuleService::setStateRuleHandler :: --> 2 --> " << err.c_str() << endl;
                slog_print(SLOG_ERROR, 1, err.c_str());
            }

            return;
        }
        ActiveState activeState = req.activestate;

        cout << "ActiveState activeState = req.activestate :: " << activeState << endl;

        auto ruleSchedules = Db::getDb()->listRule("1", err);
        for (auto rule : ruleSchedules) 
        {
            Db::getDb()->updateRuleActiveState(req.id, activeState, err);
            if (err != "")
            {
                err = this->name + ": " + err;
                cout << "RuleService::setStateRuleHandler :: --> 3 --> " << err.c_str() << endl;
                slog_print(SLOG_ERROR, 1, err.c_str());
                err = this->server.response(responseTopic, req.header.rqi, err);
                if (err != "")
                {
                    err = this->name + ": " + err;
                    cout << "RuleService::setStateRuleHandler :: --> 4 --> " << err.c_str() << endl;
                    slog_print(SLOG_ERROR, 1, err.c_str());
                }

                return;
            }

            this->ruleEngine.setStateCallback(req.id, activeState, err);
            if (err != "")
            {
                err = this->name + ": " + err;
                cout << "RuleService::setStateRuleHandler :: --> 5 --> " << err.c_str() << endl;
                slog_print(SLOG_ERROR, 1, err.c_str());
            }

            err = this->server.response(responseTopic, req.header.rqi, err);
            if (err != "")
            {
                err = this->name + ": " + err;
                cout << "RuleService::setStateRuleHandler :: --> 6 --> " << err.c_str() << endl;
                slog_print(SLOG_ERROR, 1, err.c_str());
            }
        }
    }

    void RuleService::setTriggerRuleHandler(dto::HeaderRequest header, string message)
    {
        string err;
        string responseTopic = this->getReponseTopic(header.client);

        dto::TriggerRuleRequest req;
        try
        {
            deserialize<dto::TriggerRuleRequest>(message, req);
        }
        catch (...)
        {
            err = INVALID_MESSAGE_ERROR;
            err = this->name + ": " + err;
            cout << "RuleService::setTriggerRuleHandler :: --> 1 --> " << err.c_str() << endl;
            slog_print(SLOG_ERROR, 1, err.c_str());
            err = this->server.response(responseTopic, req.header.rqi, err);
            if (err != "")
            {
                err = this->name + ": " + err;
                cout << "RuleService::setTriggerRuleHandler :: --> 2 --> " << err.c_str() << endl;
                slog_print(SLOG_ERROR, 1, err.c_str());
            }

            return;
        }

        TriggerState triggerState = req.triggerstate;
        Db::getDb()->updateTriggerStateRule(req.header.rqi, triggerState, err);
        if (err != "")
        {
            err = this->name + ": " + err;
            cout << "RuleService::setTriggerRuleHandler :: --> 3 --> " << err.c_str() << endl;
            slog_print(SLOG_ERROR, 1, err.c_str());
            err = this->server.response(responseTopic, req.header.rqi, err);
            if (err != "")
            {
                err = this->name + ": " + err;
                cout << "RuleService::setTriggerRuleHandler :: --> 4 --> " << err.c_str() << endl;
                slog_print(SLOG_ERROR, 1, err.c_str());
            }

            return;
        }

        this->ruleEngine.setTriggerCallback(req.id, triggerState, err);
        if (err != "")
        {
            err = this->name + ": " + err;
            cout << "RuleService::setTriggerRuleHandler :: --> 5 --> " << err.c_str() << endl;
            slog_print(SLOG_ERROR, 1, err.c_str());
        }

        err = this->server.response(responseTopic, req.header.rqi, err);
        if (err != "")
        {
            err = this->name + ": " + err;
            cout << "RuleService::setTriggerRuleHandler :: --> 6 --> " << err.c_str() << endl;
            slog_print(SLOG_ERROR, 1, err.c_str());
        }
    }

    void RuleService::addReadingsHandler(dto::HeaderRequest header, string message)
    {
        string responseTopic = this->getReponseTopic(header.client);

        dto::AddReadingsRequest req;
        try
        {
            deserialize<dto::AddReadingsRequest>(message, req);
        }
        catch (...)
        {
            string err = INVALID_MESSAGE_ERROR;
            err = this->name + ": " + err;
            slog_print(SLOG_ERROR, 1, err.c_str());
            err = this->server.response(responseTopic, req.header.rqi, err);
            if (err != "")
            {
                err = this->name + ": " + err;
                slog_print(SLOG_ERROR, 1, err.c_str());
            }

            return;
        }

        vector<model::Reading> readings = dto::toModel(req.readings);
        string err;
        this->ruleEngine.readingCallback(readings, err);
        if (err != "")
        {
            err = this->name + ": " + err;
            cout << "RuleService::addReadingsHandler :: --> 3 --> " << err.c_str() << endl;
            slog_print(SLOG_ERROR, 1, err.c_str());
        }
    }
}

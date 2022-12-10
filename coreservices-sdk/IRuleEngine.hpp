#pragma once

#include <string>
#include "../models/models.hpp"
#include "../clients/Command.hpp"

namespace coreservices
{
    using namespace std;

    class IRuleEngine
    {
    public:
        virtual void init(Command commandClient);
        virtual void addCallback(model::Rule rule, string &err) = 0;
        virtual void updateCallback(model::Rule rule, string &err) = 0;
        virtual void deleteCallback(string id, string &err) = 0;
        virtual void setStateCallback(string id, ActiveState state, string &err) = 0;
        virtual void setTriggerCallback(string id, TriggerState state, string &err) = 0;
        virtual void readingCallback(vector<model::Reading> readings, string &err) = 0;
    };
}
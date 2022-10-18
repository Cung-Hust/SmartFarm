#pragma once

#include "../models/models.hpp"
#include "../transports/IPublisher.hpp"

class Rule
{
private:
	IPublisher *transport;
	string clientName;
	string resquestTopic;

public:
	Rule(){};
	Rule(string clientName, string resquestTopic);
	
	string addRule(string &rqi, model::Rule rule);
	string updateRule(string &rqi, model::Rule rule);
	string getAllRule(string &rqi, map<string, string> options);
	string getRule(string &rqi, string id);
	string deleteRule(string &rqi, string id);
	string updateStateRule(string &rqi, string id, ActiveState state);
	string triggerRule(string &rqi, string id);
};

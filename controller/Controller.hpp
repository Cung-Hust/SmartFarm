#pragma once

#include <string>
#include <functional>
#include <vector>
#include <mutex>
#include <map>

#include "../transports/Transports.hpp"
#include "../dtos/requestes.hpp"
#include "../dtos/responses.hpp"

using namespace std;

typedef std::function<void(dto::HeaderRequest, std::string)> CmdRequestCallbackFunc;
typedef std::function<void(dto::HeaderResponse, std::string)> CmdResponseCallbackFunc;

class RequestController
{
private:
    string api;
    IMessageBus *transport;
    map<string, CmdRequestCallbackFunc> cmdCallbackMap;
    void server_();

public:
    bool init(string api);
    void registerCallback(string cmd, CmdRequestCallbackFunc callback);
    void server(bool isDetach);
};

class ResponseController
{
private:
    string api;
    IMessageBus *transport;
    CmdResponseCallbackFunc callback;
    void server_();

public:
    bool init(string api);
    void registerCallback(CmdResponseCallbackFunc callback);
    void server(bool isDetach);
};
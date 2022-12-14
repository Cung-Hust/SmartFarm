#include "Controller.hpp"
#include <thread>
#include <pthread.h>
#include <unistd.h>
#include <ctime>
#include <cstdlib>

bool RequestController::init(string api)
{
    this->transport = Transports::getMessager();
    this->api = api;

    return this->transport->subscribe(api);
}

void RequestController::registerCallback(string cmd, CmdRequestCallbackFunc callback)
{
    this->cmdCallbackMap.insert(std::make_pair(cmd, callback));
}

void RequestController::server_()
{
    string message;
    CmdRequestCallbackFunc callback;

    while (true)
    {
        if (this->transport->getMessage(this->api, message))
        {
            cout << endl << "\033[0;38mTopic: " << this->api << "\t||\tMessage ::  " << message << endl;
            dto::SimpleRequest req;
            try
            {
                deserialize<dto::SimpleRequest>(message, req);
            }
            catch (...)
            {
                continue;
            }

            if (req.header.rqi == "")
            {
                continue;
            }
            auto it = this->cmdCallbackMap.find(req.header.op);
            if (it == this->cmdCallbackMap.end())
            {
                cout << "Error Here  !" << endl;
                continue;
            }
            else
            {
                callback = it->second;
                callback(req.header, message);
            }
        };
        usleep(10000);
        // usleep(randint);
        // sleep(1);
    }
}

void RequestController::server(bool isDetach)
{
    if (isDetach)
    {
        function<void()> server = bind(&RequestController::server_, this);
        thread thr(server);
        thr.detach();
    }
    else
    {
        this->server_();
    }
}

bool ResponseController::init(string api)
{
    this->transport = Transports::getMessager();
    this->api = api;

    return this->transport->subscribe(api);
}

void ResponseController::registerCallback(CmdResponseCallbackFunc callback)
{
    this->callback = callback;
}

void ResponseController::server_()
{
    string message;
    CmdResponseCallbackFunc callback;

    while (true)
    {
        if (this->transport->getMessage(this->api, message))
        {
            dto::SimpleResponse res;
            try
            {
                deserialize<dto::SimpleResponse>(message, res);
            }
            catch (...)
            {
                continue;
            }

            if (res.header.rqi != "")
            {
                this->callback(res.header, message);
            }
        };
        usleep(10000);
        // sleep(1);
    }
}

void ResponseController::server(bool isDetach)
{
    if (isDetach)
    {
        function<void()> server = bind(&ResponseController::server_, this);
        thread thr(server);
        thr.detach();
    }
    else
    {
        this->server_();
    }
}
#include "Metadata.hpp"
#include "../dtos/requestes.hpp"
#include "../dtos/responses.hpp"
#include "../transports/Transports.hpp"
#include "../common/constants.hpp"
#include "../common/utils.hpp"

namespace server
{
    Metadata::Metadata(string serverName)
    {
        this->serverName = serverName;
        this->transport = Transports::getPublisher();
    };

    string Metadata::response(string toTopic, string rqi, string error)
    {
        dto::HeaderResponse header = dto::createHeaderResponse(this->serverName, rqi, error);

        dto::SimpleResponse response;
        response.header = header;

        string responseStr = serialize<dto::SimpleResponse>(response);
        if (this->transport->publish(toTopic, responseStr))
        {
            return "";
        };

        return "request {" + rqi + "} failed";
    };

    string Metadata::responseId(string toTopic, string rqi, string error, string id)
    {
        dto::HeaderResponse header = dto::createHeaderResponse(this->serverName, rqi, error);

        dto::ResponseWithId response;
        response.header = header;
        response.id = id;

        string responseStr = serialize<dto::ResponseWithId>(response);
        if (this->transport->publish(toTopic, responseStr))
        {
            return "";
        };

        return "request {" + rqi + "} failed";
    };

    string Metadata::responseCount(string toTopic, string rqi, string error, uint64_t count)
    {
        dto::HeaderResponse header = dto::createHeaderResponse(this->serverName, rqi, error);

        dto::CountResponse response;
        response.header = header;
        response.count = count;

        string responseStr = serialize<dto::CountResponse>(response);
        if (this->transport->publish(toTopic, responseStr))
        {
            return "";
        };

        return "request {" + rqi + "} failed";
    };

    string Metadata::responseDevice(string toTopic, string rqi, string error, model::Device device)
    {
        dto::HeaderResponse header = dto::createHeaderResponse(this->serverName, rqi, error);
        dto::Device dtoDev = dto::fromModel(device);

        dto::DeviceResponse response;
        response.header = header;
        response.device = dtoDev;

        string responseStr = serialize<dto::DeviceResponse>(response);
        if (this->transport->publish(toTopic, responseStr))
        {
            return "";
        };

        return "request {" + rqi + "} failed";
    };

    string Metadata::responseMultiDevice(string toTopic, string rqi, string error, vector<model::Device> devices)
    {
        dto::HeaderResponse header = dto::createHeaderResponse(this->serverName, rqi, error);
        vector<dto::Device> dtoDevs;

        for (auto &device : devices)
        {
            dto::Device dtoDev = dto::fromModel(device);
            dtoDevs.push_back(dtoDev);
        }

        dto::MultiDeviceResponse response;
        response.header = header;
        response.devices = dtoDevs;

        string responseStr = serialize<dto::MultiDeviceResponse>(response);
        if (this->transport->publish(toTopic, responseStr))
        {
            return "";
        };

        return "request {" + rqi + "} failed";
    };

    string Metadata::responseMultiResource(string toTopic, string rqi, string error, vector<model::DeviceResource> resources)
    {
        dto::HeaderResponse header = dto::createHeaderResponse(this->serverName, rqi, error);
        vector<dto::DeviceResource> dtoRess = dto::fromModel(resources);

        dto::MultiResourceResponse response;
        response.header = header;
        response.resources = dtoRess;

        string responseStr = serialize<dto::MultiResourceResponse>(response);
        if (this->transport->publish(toTopic, responseStr))
        {
            return "";
        };

        return "request {" + rqi + "} failed";
    };

    string Metadata::responseMultiProfile(string toTopic, string rqi, string error, vector<model::DeviceProfile> profiles)
    {
        dto::HeaderResponse header = dto::createHeaderResponse(this->serverName, rqi, error);
        vector<dto::DeviceProfile> dtoPros;

        for (auto &profile : profiles)
        {
            dto::DeviceProfile dtoPro = dto::fromModel(profile);
            dtoPros.push_back(dtoPro);
        }

        dto::MultiProfileResponse response;
        response.header = header;
        response.profiles = dtoPros;

        string responseStr = serialize<dto::MultiProfileResponse>(response);
        if (this->transport->publish(toTopic, responseStr))
        {
            return "";
        };

        return "request {" + rqi + "} failed";
    };

    string Metadata::responseMultiService(string toTopic, string rqi, string error, vector<model::DeviceService> services)
    {
        dto::HeaderResponse header = dto::createHeaderResponse(this->serverName, rqi, error);
        vector<dto::DeviceService> dtoSers;

        for (auto &service : services)
        {
            dto::DeviceService dtoSer = dto::fromModel(service);
            dtoSers.push_back(dtoSer);
        }

        dto::MultiServiceResponse response;
        response.header = header;
        response.services = dtoSers;

        string responseStr = serialize<dto::MultiServiceResponse>(response);
        if (this->transport->publish(toTopic, responseStr))
        {
            return "";
        };

        return "request {" + rqi + "} failed";
    };

    string Metadata::publishAddDeviceCallback(string toTopic, string rqi, model::Device device)
    {
        dto::HeaderRequest header = dto::createHeaderRequest(this->serverName, OP_DEVICECALLBACK_ADD, rqi);
        dto::Device dtoDevice = dto::fromModel(device);

        dto::CallbackDeviceRequest request;
        request.header = header;
        request.device = dtoDevice;

        string requestStr = serialize<dto::CallbackDeviceRequest>(request);
        if (this->transport->publish(toTopic, requestStr))
        {
            return "";
        };

        return "request {" + rqi + "} failed";
    }

    string Metadata::publishUpdateDeviceCallback(string toTopic, string rqi, model::Device device)
    {
        dto::HeaderRequest header = dto::createHeaderRequest(this->serverName, OP_DEVICECALLBACK_UPDATE, rqi);
        dto::Device dtoDevice = dto::fromModel(device);

        dto::CallbackDeviceRequest request;
        request.header = header;
        request.device = dtoDevice;

        string requestStr = serialize<dto::CallbackDeviceRequest>(request);
        if (this->transport->publish(toTopic, requestStr))
        {
            return "";
        };

        return "request {" + rqi + "} failed";
    }

    string Metadata::publishDeleteDeviceCallback(string toTopic, string rqi, model::Device device)
    {
        dto::HeaderRequest header = dto::createHeaderRequest(this->serverName, OP_DEVICECALLBACK_DELETE, rqi);
        dto::Device dtoDevice = dto::fromModel(device);

        dto::CallbackDeviceRequest request;
        request.header = header;
        request.device = dtoDevice;

        string requestStr = serialize<dto::CallbackDeviceRequest>(request);
        if (this->transport->publish(toTopic, requestStr))
        {
            return "";
        };

        return "request {" + rqi + "} failed";
    }
}
#include "Metadata.hpp"
#include "../dtos/requestes.hpp"
#include "../transports/Transports.hpp"
#include "../common/constants.hpp"
#include "../common/utils.hpp"

Metadata::Metadata(string clientName, string resquestTopic)
{
    this->transport = Transports::getPublisher();
    this->clientName = clientName;
    this->resquestTopic = resquestTopic;
};

string Metadata::addDevice(string &rqi, model::Device device, bool isNotify)
{
    cout << "Test Add device" << endl;
    if (rqi == "")
    {
        rqi = genUuid();
    }

    dto::HeaderRequest header = dto::createHeaderRequest(this->clientName, OP_DEVICE_ADD, rqi);
    dto::Device dtoDev = dto::fromModel(device);

    if (dtoDev.id == "")
    {
        dtoDev.id = genUuid();
    }

    dto::AddDeviceRequest request;
    request.header = header;
    request.device = dtoDev;
    request.notify = isNotify;

    string requestStr = serialize<dto::AddDeviceRequest>(request);
    cout << "Topic 1: " << this->resquestTopic << endl;
    if (this->transport->publish(this->resquestTopic, requestStr))
    {
        return "";
    };

    return "->addDevice<- request {" + rqi + "} failed";
};

string Metadata::updateDevice(string &rqi, model::Device device, bool isNotify)
{
    if (rqi == "")
    {
        rqi = genUuid();
    }

    dto::HeaderRequest header = dto::createHeaderRequest(this->clientName, OP_DEVICE_UPDATE, rqi);
    dto::UpdateDevice dtoDev = dto::fromDeviceModel(device);

    dto::UpdateDeviceRequest request;
    request.header = header;
    request.device = dtoDev;
    request.notify = isNotify;

    string requestStr = serialize<dto::UpdateDeviceRequest>(request);
    dto::deleteUpdateDevice(dtoDev);
    if (this->transport->publish(this->resquestTopic, requestStr))
    {
        return "";
    };

    return "->updateDevice<- request {" + rqi + "} failed";
};

string Metadata::countDevice(string &rqi, std::map<std::string, std::string> options)
{
    if (rqi == "")
    {
        rqi = genUuid();
    }

    dto::HeaderRequest header = dto::createHeaderRequest(this->clientName, OP_DEVICE_COUNT, rqi);

    dto::RequestWithOptions request;
    request.header = header;
    request.options = options;

    string requestStr = serialize<dto::RequestWithOptions>(request);
    if (this->transport->publish(this->resquestTopic, requestStr))
    {
        return "";
    };

    return "->countDevice<- request {" + rqi + "} failed";
};

string Metadata::getAllDevice(string &rqi, std::map<std::string, std::string> options)
{
    if (rqi == "")
    {
        rqi = genUuid();
    }

    dto::HeaderRequest header = dto::createHeaderRequest(this->clientName, OP_DEVICE_LIST, rqi);

    dto::RequestWithOptions request;
    request.header = header;
    request.options = options;

    string requestStr = serialize<dto::RequestWithOptions>(request);
    if (this->transport->publish(this->resquestTopic, requestStr))
    {
        return "";
    };

    return "->getAllDevice<- request {" + rqi + "} failed";
};

string Metadata::getDevice(string &rqi, string id)
{
    if (rqi == "")
    {
        rqi = genUuid();
    }

    dto::HeaderRequest header = dto::createHeaderRequest(this->clientName, OP_DEVICE_READ, rqi);

    dto::RequestWithId request;
    request.header = header;
    request.id = id;

    string requestStr = serialize<dto::RequestWithId>(request);
    if (this->transport->publish(this->resquestTopic, requestStr))
    {
        return "";
    };

    return "->getDevice<- request {" + rqi + "} failed";
};

string Metadata::deleteDevice(string &rqi, string id, bool isNotify)
{
    if (rqi == "")
    {
        rqi = genUuid();
    }

    dto::HeaderRequest header = dto::createHeaderRequest(this->clientName, OP_DEVICE_DELETE, rqi);

    dto::DeleteDeviceRequest request;
    request.header = header;
    request.id = id;
    request.notify = isNotify;

    string requestStr = serialize<dto::DeleteDeviceRequest>(request);
    if (this->transport->publish(this->resquestTopic, requestStr))
    {
        return "";
    };

    return "->deleteDevice<- request {" + rqi + "} failed";
};

string Metadata::updateAdminStateDevice(string &rqi, string id, AdminState state)
{
    if (rqi == "")
    {
        rqi = genUuid();
    }

    dto::HeaderRequest header = dto::createHeaderRequest(this->clientName, OP_DEVICE_UPDATE_STATE, rqi);

    dto::SetAdminStateDeviceRequest request;
    request.header = header;
    request.id = id;
    request.adminState = state;

    string requestStr = serialize<dto::SetAdminStateDeviceRequest>(request);
    if (this->transport->publish(this->resquestTopic, requestStr))
    {
        return "";
    };

    return "->updateAdminStateDevice<- request {" + rqi + "} failed";
};

string Metadata::addResource(string &rqi, vector<model::DeviceResource> resources)
{
    if (rqi == "")
    {
        rqi = genUuid();
    }

    dto::HeaderRequest header = dto::createHeaderRequest(this->clientName, OP_RESOURCE_ADD, rqi);
    vector<dto::DeviceResource> dtoRess = dto::fromModel(resources);

    dto::AddResourceRequest request;
    request.header = header;
    request.resources = dtoRess;

    string requestStr = serialize<dto::AddResourceRequest>(request);
    if (this->transport->publish(this->resquestTopic, requestStr))
    {
        return "";
    };

    return "->addResource<- request {" + rqi + "} failed";
};

string Metadata::getAllResource(string &rqi)
{
    if (rqi == "")
    {
        rqi = genUuid();
    }

    dto::HeaderRequest header = dto::createHeaderRequest(this->clientName, OP_RESOURCE_LIST, rqi);

    dto::SimpleRequest request;
    request.header = header;

    string requestStr = serialize<dto::SimpleRequest>(request);
    if (this->transport->publish(this->resquestTopic, requestStr))
    {
        return "";
    };

    return "->getAllResource<- request {" + rqi + "} failed";
};

string Metadata::deleteResource(string &rqi, string name)
{
    if (rqi == "")
    {
        rqi = genUuid();
    }

    dto::HeaderRequest header = dto::createHeaderRequest(this->clientName, OP_RESOURCE_DELETE, rqi);

    dto::RequestWithName request;
    request.header = header;
    request.name = name;

    string requestStr = serialize<dto::RequestWithName>(request);
    if (this->transport->publish(this->resquestTopic, requestStr))
    {
        return "";
    };

    return "->deleteResource<- request {" + rqi + "} failed";
};

string Metadata::addProfile(string &rqi, model::DeviceProfile profile)
{
    if (rqi == "")
    {
        rqi = genUuid();
    }

    dto::HeaderRequest header = dto::createHeaderRequest(this->clientName, OP_PROFILE_ADD, rqi);
    dto::DeviceProfile dtoPro = dto::fromModel(profile);

    dto::AddProfileRequest request;
    request.header = header;
    request.profile = dtoPro;

    string requestStr = serialize<dto::AddProfileRequest>(request);
    if (this->transport->publish(this->resquestTopic, requestStr))
    {
        return "";
    };

    return "->addProfile<- request {" + rqi + "} failed";
};

string Metadata::getAllProfile(string &rqi)
{
    if (rqi == "")
    {
        rqi = genUuid();
    }

    dto::HeaderRequest header = dto::createHeaderRequest(this->clientName, OP_PROFILE_LIST, rqi);

    dto::SimpleRequest request;
    request.header = header;

    string requestStr = serialize<dto::SimpleRequest>(request);
    if (this->transport->publish(this->resquestTopic, requestStr))
    {
        return "";
    };

    return "->getAllProfile<- request {" + rqi + "} failed";
};

string Metadata::deleteProfile(string &rqi, string name)
{
    if (rqi == "")
    {
        rqi = genUuid();
    }

    dto::HeaderRequest header = dto::createHeaderRequest(this->clientName, OP_PROFILE_DELETE, rqi);

    dto::RequestWithName request;
    request.header = header;
    request.name = name;

    string requestStr = serialize<dto::RequestWithName>(request);
    if (this->transport->publish(this->resquestTopic, requestStr))
    {
        return "";
    };

    return "->deleteProfile<- request {" + rqi + "} failed";
};

string Metadata::addService(string &rqi, model::DeviceService service)
{
    if (rqi == "")
    {
        rqi = genUuid();
    }

    dto::HeaderRequest header = dto::createHeaderRequest(this->clientName, OP_DEVICESERVICE_ADD, rqi);
    dto::DeviceService dtoSer = dto::fromModel(service);

    dto::AddServiceRequest request;
    request.header = header;
    request.service = dtoSer;

    string requestStr = serialize<dto::AddServiceRequest>(request);
    if (this->transport->publish(this->resquestTopic, requestStr))
    {
        return "";
    };

    return "->addService<- request {" + rqi + "} failed";
};

string Metadata::getAllService(string &rqi)
{
    if (rqi == "")
    {
        rqi = genUuid();
    }

    dto::HeaderRequest header = dto::createHeaderRequest(this->clientName, OP_DEVICESERVICE_LIST, rqi);

    dto::SimpleRequest request;
    request.header = header;

    string requestStr = serialize<dto::SimpleRequest>(request);
    if (this->transport->publish(this->resquestTopic, requestStr))
    {
        return "";
    };

    return "->getAllService<- request {" + rqi + "} failed";
};

string Metadata::deleteService(string &rqi, string name)
{
    if (rqi == "")
    {
        rqi = genUuid();
    }

    dto::HeaderRequest header = dto::createHeaderRequest(this->clientName, OP_DEVICESERVICE_DELETE, rqi);

    dto::RequestWithName request;
    request.header = header;
    request.name = name;

    string requestStr = serialize<dto::RequestWithName>(request);
    if (this->transport->publish(this->resquestTopic, requestStr))
    {
        return "";
    };

    return "->deleteService<- request {" + rqi + "} failed";
};
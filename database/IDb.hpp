#pragma once

#include "../models/models.hpp"

using namespace std;

class IDb
{
public:
    virtual void close() = 0;

    // Device
    virtual void addDevice(model::Device device, string &error) = 0;
    virtual void updateDevice(model::Device device, string &error) = 0;
    virtual void updateDeviceOperatingState(string id, OperatingState state, string &error) = 0;
    virtual void deleteDevice(string id, string &error) = 0;
    virtual uint64_t countDevice(string profileName, string serviceName, string type, string &error) = 0;
    virtual vector<model::Device> listDevice(string profileName, string serviceName, string type) = 0;
    virtual model::Device readDevice(string id, string &error) = 0;

    // Resource
    virtual void addResources(vector<model::DeviceResource> resources, string &error) = 0;
    virtual void deleteResource(string name, string &error) = 0;
    virtual vector<model::DeviceResource> listResource(void) = 0;

    // Profile
    virtual void addProfile(model::DeviceProfile profile, string &error) = 0;
    virtual void deleteProfile(string name, string &error) = 0;
    virtual vector<model::DeviceProfile> listProfile() = 0;
    virtual model::DeviceProfile readProfile(string name, string &error) = 0;

    // DeviceService
    virtual void addDeviceService(model::DeviceService service, string &error) = 0;
    virtual void deleteDeviceService(string name, string &error) = 0;
    virtual vector<model::DeviceService> listDeviceService() = 0;
    virtual model::DeviceService readDeviceService(string name, string &error) = 0;

    // Reading
    virtual string validReading(model::Reading &reading) = 0;
    virtual void validAndAddReading(vector<model::Reading> &readings, string &error) = 0;
    virtual vector<model::Reading> listReading(string id) = 0;

    // Rule
    virtual void addRule(model::Rule rule, string &error) = 0;
    virtual void updateRule(model::Rule rule, string &error) = 0;
    virtual void updateRuleActiveState(string id, ActiveState state, string &error) = 0;
    virtual void updateTriggerStateRule(string id, TriggerState state, string &error) = 0;
    virtual void deleteRule(string id, string &error) = 0;
    virtual uint64_t countRule(string type, string &error) = 0;
    virtual vector<model::Rule> listRule(string type, string &error) = 0;
    virtual model::Rule readRule(string id, string &error) = 0;

    // Notification
    virtual void addNotification(model::Notification notification, string &error) = 0;
    virtual void deleteNotification(string id, string &error) = 0;
    virtual vector<model::Notification> listNotification(void) = 0;
};
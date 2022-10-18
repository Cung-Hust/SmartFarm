#pragma once

#include <string>
#include <iostream>
#include <map>
#include <mutex>

#include "../IDb.hpp"

class Sqlite : public IDb
{
public:
  static Sqlite *getInstance();
  static void init(std::map<std::string, std::string> params);

  void close();

  // Device
  void addDevice(model::Device device, string &error);
  void updateDevice(model::Device device, string &error);
  void updateDeviceOperatingState(string id, OperatingState state, string &error);
  void deleteDevice(string id, string &error);
  uint64_t countDevice(string profileName, string serviceName, string type, string &error);
  vector<model::Device> listDevice(string profileName, string serviceName, string type);
  model::Device readDevice(string id, string &error);

  // Resource
  void addResources(vector<model::DeviceResource> resources, string &error);
  void deleteResource(string name, string &error);
  vector<model::DeviceResource> listResource(void);

  // Profile
  void addProfile(model::DeviceProfile profile, string &error);
  void deleteProfile(string name, string &error);
  vector<model::DeviceProfile> listProfile(void);
  model::DeviceProfile readProfile(string name, string &error);

  // DeviceService
  void addDeviceService(model::DeviceService service, string &error);
  void deleteDeviceService(string name, string &error);
  vector<model::DeviceService> listDeviceService();
  model::DeviceService readDeviceService(string name, string &error);

  // Reading
  string validReading(model::Reading &reading);
  void validAndAddReading(vector<model::Reading> &readings, string &error);
  vector<model::Reading> listReading(string id);

  // Rule
  void addRule(model::Rule rule, string &error);
  void updateRule(model::Rule rule, string &error);
  void updateRuleActiveState(string id, ActiveState state, string &error);
  void updateTriggerStateRule(string id, TriggerState state, string &error);
  void deleteRule(string id, string &error);
  uint64_t countRule(string type, string &error);
  vector<model::Rule> listRule(string type, string &error);
  model::Rule readRule(string id, string &error);

  // Notification
  void addNotification(model::Notification notification, string &error);
  void deleteNotification(string id, string &error);
  vector<model::Notification> listNotification(void);

private:
  static Sqlite *gInstance;
  std::mutex mutex_;
};
#include "Sqlite.hpp"
#include "../entity/entity.hpp"
#include "sqlite_orm.h"
#include "../../common/utils.hpp"
#include "../../models/constants.hpp"

#include <string>
#include <stdint.h>
#include <unistd.h>

using namespace entity;
using namespace sqlite_orm;
using namespace std;

static auto initStorage(const std::string &path)
{
    return make_storage(path,
                        make_table("DeviceResource",
                                   make_column("name", &DeviceResource::name, primary_key()),
                                   make_column("valueType", &DeviceResource::valueType),
                                   make_column("readWrite", &DeviceResource::readWrite),
                                   make_column("unit", &DeviceResource::unit),
                                   make_column("minimum", &DeviceResource::minimum),
                                   make_column("maximum", &DeviceResource::maximum)),
                        make_table("DeviceProfile",
                                   make_column("name", &DeviceProfile::name, primary_key()),
                                   make_column("model", &DeviceProfile::model)),
                        make_table("DeviceService",
                                   make_column("name", &DeviceService::name, primary_key()),
                                   make_column("address", &DeviceService::address)),
                        make_table("ResourceInProfile",
                                   make_column("profileName", &ResourceInProfile::profileName),
                                   make_column("resourceName", &ResourceInProfile::resourceName),
                                   make_column("attributes", &ResourceInProfile::attributes),
                                   foreign_key(&ResourceInProfile::profileName).references(&DeviceProfile::name).on_delete.cascade(),
                                   foreign_key(&ResourceInProfile::resourceName).references(&DeviceResource::name).on_delete.cascade(),
                                   primary_key(&ResourceInProfile::profileName, &ResourceInProfile::resourceName)),
                        make_table("Device",
                                   make_column("id", &Device::id, primary_key()),
                                   make_column("name", &Device::name),
                                   make_column("type", &Device::type),
                                   make_column("created", &Device::created),
                                   make_column("modified", &Device::modified),
                                   make_column("adminState", &Device::adminState),
                                   make_column("operatingState", &Device::operatingState),
                                   make_column("lastConnected", &Device::lastConnected),
                                   make_column("profileName", &Device::profileName),
                                   make_column("serviceName", &Device::serviceName),
                                   make_column("protocols", &Device::protocols)),
                        make_table("DeviceInGroup",
                                   make_column("groupId", &DeviceInGroup::groupId),
                                   make_column("deviceId", &DeviceInGroup::deviceId),
                                   make_column("state", &DeviceInGroup::state),
                                   foreign_key(&DeviceInGroup::groupId).references(&Device::id).on_delete.cascade(),
                                   foreign_key(&DeviceInGroup::deviceId).references(&Device::id).on_delete.cascade(),
                                   primary_key(&DeviceInGroup::groupId, &DeviceInGroup::deviceId)),
                        make_table("ResourceValue",
                                   make_column("deviceId", &ResourceValue::deviceId),
                                   make_column("resourceName", &ResourceValue::resourceName),
                                   make_column("value", &ResourceValue::value),
                                   make_column("timestamp", &ResourceValue::timestamp),
                                   foreign_key(&ResourceValue::deviceId).references(&Device::id).on_delete.cascade(),
                                   foreign_key(&ResourceValue::resourceName).references(&DeviceResource::name).on_delete.cascade(),
                                   primary_key(&ResourceValue::deviceId, &ResourceValue::resourceName)),
                        make_table("Rule",
                                   make_column("id", &Rule::id, primary_key()),
                                   make_column("name", &Rule::name),
                                   make_column("type", &Rule::type),
                                   make_column("priority", &Rule::priority),
                                   make_column("activeState", &Rule::activeState),
                                   make_column("triggerState", &Rule::triggerState),
                                   make_column("startTime", &Rule::startTime),
                                   make_column("endTime", &Rule::endTime),
                                   make_column("startInDayTime", &Rule::startInDayTime),
                                   make_column("endInDayTime", &Rule::endInDayTime),
                                   make_column("repeatDays", &Rule::repeatDays),
                                   make_column("conditionLogic", &Rule::conditionLogic),
                                   make_column("timeInDayConditions", &Rule::timeInDayConditions),
                                   make_column("enableNotify", &Rule::enableNotify),
                                   make_column("notifyLevel", &Rule::notifyLevel),
                                   make_column("notifyContent", &Rule::notifyContent)),
                        make_table("DeviceCondition",
                                   make_column("id", &DeviceCondition::id, primary_key()),
                                   make_column("ruleId", &DeviceCondition::ruleId),
                                   make_column("deviceId", &DeviceCondition::deviceId),
                                   make_column("resourceName", &DeviceCondition::resourceName),
                                   make_column("compareOperator", &DeviceCondition::compareOperator),
                                   make_column("value", &DeviceCondition::value),
                                   foreign_key(&DeviceCondition::ruleId).references(&Rule::id).on_delete.cascade()),
                        make_table("RuleCondition",
                                   make_column("ruleId", &RuleCondition::ruleId),
                                   make_column("conditionRuleId", &RuleCondition::conditionRuleId),
                                   make_column("compareOperator", &RuleCondition::compareOperator),
                                   foreign_key(&RuleCondition::ruleId).references(&Rule::id).on_delete.cascade(),
                                   foreign_key(&RuleCondition::conditionRuleId).references(&Rule::id).on_delete.cascade(),
                                   primary_key(&RuleCondition::ruleId, &RuleCondition::conditionRuleId)),
                        make_table("DeviceAction",
                                   make_column("ruleId", &DeviceAction::ruleId),
                                   make_column("deviceId", &DeviceAction::deviceId),
                                   make_column("resourceName", &DeviceAction::resourceName),
                                   make_column("delayTime", &DeviceAction::delayTime),
                                   make_column("value", &DeviceAction::value),
                                   foreign_key(&DeviceAction::ruleId).references(&Rule::id).on_delete.cascade(),
                                   foreign_key(&DeviceAction::deviceId).references(&Device::id).on_delete.cascade(),
                                   //    foreign_key(&DeviceAction::resourceName).references(&DeviceResource::name).on_delete.cascade(),
                                   primary_key(&DeviceAction::ruleId, &DeviceAction::deviceId)),
                        make_table("RuleAction",
                                   make_column("ruleId", &RuleAction::ruleId),
                                   make_column("actionRuleId", &RuleAction::actionRuleId),
                                   make_column("delayTime", &RuleAction::delayTime),
                                   foreign_key(&RuleAction::ruleId).references(&Rule::id).on_delete.cascade(),
                                   foreign_key(&RuleAction::actionRuleId).references(&Rule::id).on_delete.cascade(),
                                   primary_key(&RuleAction::ruleId, &RuleAction::actionRuleId)),
                        make_table("Notification",
                                   make_column("id", &Notification::id, primary_key()),
                                   make_column("timestamp", &Notification::timestamp),
                                   make_column("level", &Notification::level),
                                   make_column("content", &Notification::content)));
}

using Storage = decltype(initStorage(""));
static std::unique_ptr<Storage> gStorage;

Sqlite *Sqlite::gInstance = nullptr;

Sqlite *Sqlite::getInstance()
{
    return Sqlite::gInstance;
}

void Sqlite::init(std::map<std::string, std::string> params)
{
    if (Sqlite::gInstance == nullptr)
    {
        Sqlite::gInstance = new Sqlite();
    }

    string filename = params["filename"];

    gStorage = std::make_unique<Storage>(initStorage(filename));

    //  sync schema in case db/tables do not exist
    gStorage->sync_schema();
}

void Sqlite::close(){};

// Device
void Sqlite::addDevice(model::Device device, string &error)
{
    Device ed = fromModel(device);

    const std::lock_guard<std::mutex> lock(this->mutex_);
    try
    {
        gStorage->replace(ed);
    }
    catch (const std::system_error &e)
    {
        error = e.what();
        return;
    }
    catch (...)
    {
        error = "unknown exeption";
        return;
    }

    // replace deviceList
    for (auto &dig : device.deviceList)
    {
        DeviceInGroup edig = fromModel(device.id, dig);
        try
        {
            gStorage->replace(edig);
        }
        catch (const std::system_error &e)
        {
            error = e.what();
            return;
        }
        catch (...)
        {
            error = "unknown exeption";
            return;
        }
    }

    return;
};

void Sqlite::updateDevice(model::Device device, string &error)
{
    Device ed = fromModel(device);

    const std::lock_guard<std::mutex> lock(this->mutex_);

    try
    {
        gStorage->update(ed);
    }
    catch (const std::system_error &e)
    {
        error = e.what();
        return;
    }
    catch (...)
    {
        error = "unknown exeption";
        return;
    }

    // remove and replace deviceList
    try
    {
        gStorage->remove_all<DeviceInGroup>(where(c(&DeviceInGroup::groupId) == device.id));
    }
    catch (const std::system_error &e)
    {
        error = e.what();
    }
    catch (...)
    {
        error = "unknown exeption";
    }

    for (auto &dig : device.deviceList)
    {
        DeviceInGroup edig = fromModel(device.id, dig);
        try
        {
            gStorage->replace(edig);
        }
        catch (const std::system_error &e)
        {
            error = e.what();
            return;
        }
        catch (...)
        {
            error = "unknown exeption";
            return;
        }
    }

    return;
};

void Sqlite::updateDeviceOperatingState(string id, OperatingState state, string &error)
{
    const std::lock_guard<std::mutex> lock(this->mutex_);

    try
    {
        gStorage->update_all(sqlite_orm::set(c(&Device::operatingState) = (int)state), where(c(&Device::id) == id));
    }
    catch (const std::system_error &e)
    {
        error = e.what();
        return;
    }
    catch (...)
    {
        error = "unknown exeption";
        return;
    }
};

void Sqlite::deleteDevice(string id, string &error)
{
    const std::lock_guard<std::mutex> lock(this->mutex_);

    try
    {
        gStorage->remove<Device>(id);
    }
    catch (const std::system_error &e)
    {
        error = e.what();
    }
    catch (...)
    {
        error = "unknown exeption";
    }

    return;
};

// TODO: ap dung filter dong
uint64_t Sqlite::countDevice(string profileName, string serviceName, string type, string &error)
{
    bool isQueryProfile = (profileName != ""), isQueryService = (serviceName != ""), isQueryType = (type != "");
    DeviceType deviceType;
    enumFormString(type, deviceType);

    uint64_t count = 0;
    try
    {
        count = gStorage->count<Device>(where(
            (not isQueryProfile or (c(&Device::profileName) == profileName)) && (not isQueryService or (c(&Device::serviceName) == serviceName)) && (not isQueryType or (c(&Device::type) == (int)deviceType))));
    }
    catch (const std::system_error &e)
    {
        error = e.what();
    }
    catch (...)
    {
        error = "unknown exeption";
    }

    return count;
};

vector<model::Device> Sqlite::listDevice(string profileName, string serviceName, string type)
{
    bool isQueryProfile = (profileName != ""), isQueryService = (serviceName != ""), isQueryType = (type != "");
    DeviceType deviceType;
    enumFormString(type, deviceType);

    vector<model::Device> mds;

    for (auto &record : gStorage->iterate<Device>(where(
             (not isQueryProfile or (c(&Device::profileName) == profileName)) && (not isQueryService or (c(&Device::serviceName) == serviceName)) && (not isQueryType or (c(&Device::type) == (int)deviceType)))))
    {
        model::Device md = toModel(record);

        // lay danh sach DeviceInGroup cho device
        vector<model::DeviceInGroup> deviceList;
        for (auto &groupRecord : gStorage->iterate<DeviceInGroup>(where(c(&DeviceInGroup::groupId) == md.id)))
        {
            model::DeviceInGroup mg = toModel(groupRecord);
            deviceList.push_back(mg);
        }
        md.deviceList = deviceList;

        mds.push_back(md);
    }

    return mds;
};

model::Device Sqlite::readDevice(string id, string &error)
{
    const std::lock_guard<std::mutex> lock(this->mutex_);
    model::Device md;
    try
    {
        auto record = gStorage->get<Device>(id);
        md = toModel(record);

        // lay danh sach DeviceInGroup cho device
        vector<model::DeviceInGroup> deviceList;
        for (auto &groupRecord : gStorage->iterate<DeviceInGroup>(where(c(&DeviceInGroup::groupId) == md.id)))
        {
            model::DeviceInGroup mg = toModel(groupRecord);
            deviceList.push_back(mg);
        }
        md.deviceList = deviceList;
    }
    catch (std::system_error e)
    {
        error = e.what();
    }
    catch (...)
    {
        error = "unknown exeption";
    }

    return md;
};

// Resource
void Sqlite::addResources(vector<model::DeviceResource> resources, string &error)
{
    const std::lock_guard<std::mutex> lock(this->mutex_);

    for (auto &resource : resources)
    {
        DeviceResource rs = fromModel(resource);

        try
        {
            // gStorage->replace(rs);
            gStorage->insert(
                into<DeviceResource>(),
                columns(&DeviceResource::name, &DeviceResource::valueType, &DeviceResource::readWrite, &DeviceResource::unit, &DeviceResource::minimum, &DeviceResource::maximum),
                values(std::make_tuple(rs.name, rs.valueType, rs.readWrite, rs.unit, rs.minimum, rs.maximum)),
                on_conflict(&DeviceResource::name)
                    .do_update(sqlite_orm::set(
                        c(&DeviceResource::valueType) = rs.valueType,
                        c(&DeviceResource::readWrite) = rs.readWrite,
                        c(&DeviceResource::unit) = rs.unit,
                        c(&DeviceResource::minimum) = rs.minimum,
                        c(&DeviceResource::maximum) = rs.maximum)));
        }
        catch (const std::system_error &e)
        {
            error = e.what();
            return;
        }
        catch (...)
        {
            error = "unknown exeption";
            return;
        }
    }

    return;
};

void Sqlite::deleteResource(string name, string &error)
{
    const std::lock_guard<std::mutex> lock(this->mutex_);

    try
    {
        gStorage->remove<DeviceResource>(name);
    }
    catch (const std::system_error &e)
    {
        error = e.what();
    }
    catch (...)
    {
        error = "unknown exeption";
    }

    return;
};

vector<model::DeviceResource> Sqlite::listResource(void)
{
    vector<model::DeviceResource> mdrs;

    for (auto &record : gStorage->iterate<DeviceResource>())
    {
        model::DeviceResource mdr = toModel(record);
        mdrs.push_back(mdr);
    }

    return mdrs;
};

// Profile
void Sqlite::addProfile(model::DeviceProfile profile, string &error)
{
    DeviceProfile edp = fromModel(profile);

    const std::lock_guard<std::mutex> lock(this->mutex_);
    try
    {
        gStorage->insert(
            into<DeviceProfile>(),
            columns(&DeviceProfile::name, &DeviceProfile::model),
            values(std::make_tuple(edp.name, edp.model)),
            on_conflict(&DeviceProfile::name)
                .do_update(sqlite_orm::set(c(&DeviceProfile::model) = excluded(&DeviceProfile::model))));
    }
    catch (const std::system_error &e)
    {
        error = e.what();
    }
    catch (...)
    {
        error = "unknown exeption";
    }

    // replace resources in profile
    for (auto &rs : profile.resources)
    {
        ResourceInProfile rsip = fromModel(profile.name, rs);
        try
        {
            gStorage->replace(rsip);
        }
        catch (const std::system_error &e)
        {
            error = e.what();
            return;
        }
        catch (...)
        {
            error = "unknown exeption";
            return;
        }
    }

    return;
};

void Sqlite::deleteProfile(string name, string &error)
{
    const std::lock_guard<std::mutex> lock(this->mutex_);

    try
    {
        gStorage->remove<DeviceProfile>(name);
    }
    catch (const std::system_error &e)
    {
        error = e.what();
    }
    catch (...)
    {
        error = "unknown exeption";
    }

    return;
};

vector<model::DeviceProfile> Sqlite::listProfile()
{
    vector<model::DeviceProfile> mdps;

    for (auto &record : gStorage->iterate<DeviceProfile>())
    {
        model::DeviceProfile mdp = toModel(record);
        mdps.push_back(mdp);
    }

    return mdps;
};

model::DeviceProfile Sqlite::readProfile(string name, string &error)
{
    model::DeviceProfile mdp;
    try
    {
        auto record = gStorage->get<DeviceProfile>(name);
        mdp = toModel(record);
    }
    catch (std::system_error e)
    {
        error = e.what();
    }
    catch (...)
    {
        error = "unknown exeption";
    }

    return mdp;
};

// DeviceService
void Sqlite::addDeviceService(model::DeviceService service, string &error)
{
    DeviceService mds = fromModel(service);

    const std::lock_guard<std::mutex> lock(this->mutex_);
    try
    {
        gStorage->insert(
            into<DeviceService>(),
            columns(&DeviceService::name, &DeviceService::address),
            values(std::make_tuple(mds.name, mds.address)),
            on_conflict(&DeviceService::name)
                .do_update(sqlite_orm::set(c(&DeviceService::address) = excluded(&DeviceService::address))));
    }
    catch (const std::system_error &e)
    {
        error = e.what();
    }
    catch (...)
    {
        error = "unknown exeption";
    }

    return;
};

void Sqlite::deleteDeviceService(string name, string &error)
{
    const std::lock_guard<std::mutex> lock(this->mutex_);

    try
    {
        gStorage->remove<DeviceService>(name);
    }
    catch (const std::system_error &e)
    {
        error = e.what();
    }
    catch (...)
    {
        error = "unknown exeption";
    }

    return;
};

vector<model::DeviceService> Sqlite::listDeviceService()
{
    vector<model::DeviceService> mdss;

    for (auto &record : gStorage->iterate<DeviceService>())
    {
        model::DeviceService mds = toModel(record);
        mdss.push_back(mds);
    }

    return mdss;
};

model::DeviceService Sqlite::readDeviceService(string name, string &error)
{
    model::DeviceService mds;
    try
    {
        auto record = gStorage->get<DeviceService>(name);
        mds = toModel(record);
    }
    catch (std::system_error e)
    {
        error = e.what();
    }
    catch (...)
    {
        error = "unknown exeption";
    }

    return mds;
};

// Reading
string Sqlite::validReading(model::Reading &reading)
{
    try
    {
        auto rs = gStorage->get<DeviceResource>(reading.resourceName);
        auto r = toModel(rs);
        if (!checkValidData(r.valueType, reading.value))
        {
            return "Data for resource '" + reading.resourceName + "' is invalid";
        }
        reading.valueType = r.valueType;
        reading.unit = r.unit;
        return "";
    }
    catch (const std::system_error &e)
    {
        return e.what();
    }
    catch (...)
    {
        return "unknown exeption";
    }
}

void Sqlite::validAndAddReading(vector<model::Reading> &readings, string &error)
{
    const std::lock_guard<std::mutex> lock(this->mutex_);
    for (auto &reading : readings)
    {
        error = this->validReading(reading);
        if (error != "")
        {
            cout << "Error: " << error.c_str() << endl;
            return;
        }
    }

    for (auto &reading : readings)
    {
        ResourceValue rv = fromModel(reading);

        try
        {
            gStorage->replace(rv);
        }
        catch (const std::system_error &e)
        {
            error = e.what();
            return;
        }
        catch (...)
        {
            error = "unknown exeption";
            return;
        }
    }

    return;
};

vector<model::Reading> Sqlite::listReading(string id)
{
    usleep(1000);
    const std::lock_guard<std::mutex> lock(this->mutex_);
    vector<model::Reading> result;

    if (id == "")
    {
        for (auto &record : gStorage->iterate<ResourceValue>())
        {
            model::Reading md = toModel(record);
            result.push_back(md);
        }
    }
    else
    {
        for (auto &record : gStorage->iterate<ResourceValue>(where(c(&ResourceValue::deviceId) == id)))
        {
            model::Reading md = toModel(record);
            result.push_back(md);
        }
    }

    return result;
};

// Rule
void Sqlite::addRule(model::Rule rule, string &error)
{
    const std::lock_guard<std::mutex> lock(this->mutex_);
    Rule er = fromModel(rule);

    try
    {
        gStorage->replace(er);
    }
    catch (const std::system_error &e)
    {
        error = e.what();
        return;
    }
    catch (...)
    {
        error = "unknown exeption";
        return;
    }

    // replace deviceConditions
    for (auto &dc : rule.deviceConditions)
    {
        string id = genUuid();
        DeviceCondition edc = fromModel(id, rule.id, dc);
        try
        {
            gStorage->replace(edc);
        }
        catch (const std::system_error &e)
        {
            error = e.what();
            return;
        }
        catch (...)
        {
            error = "unknown exeption";
            return;
        }
    }

    for (auto &rc : rule.ruleConditions)
    {
        RuleCondition erc = fromModel(rule.id, rc);
        try
        {
            gStorage->replace(erc);
        }
        catch (const std::system_error &e)
        {
            error = e.what();
            return;
        }
        catch (...)
        {
            error = "unknown exeption";
            return;
        }
    }

    for (auto &da : rule.deviceActions)
    {
        DeviceAction eda = fromModel(rule.id, da);
        try
        {
            gStorage->replace(eda);
        }
        catch (const std::system_error &e)
        {
            error = e.what();
            return;
        }
        catch (...)
        {
            error = "unknown exeption";
            return;
        }
    }

    for (auto &ra : rule.ruleActions)
    {
        RuleAction era = fromModel(rule.id, ra);
        try
        {
            gStorage->replace(era);
        }
        catch (const std::system_error &e)
        {
            error = e.what();
            return;
        }
        catch (...)
        {
            error = "unknown exeption";
            return;
        }
    }

    return;
};

void Sqlite::updateRule(model::Rule rule, string &error)
{
    const std::lock_guard<std::mutex> lock(this->mutex_);
    Rule ed = fromModel(rule);

    try
    {
        gStorage->update(ed);
    }
    catch (const std::system_error &e)
    {
        error = e.what();
        return;
    }
    catch (...)
    {
        error = "unknown exeption";
        return;
    }

    // remove and replace deviceConditions
    try
    {
        gStorage->remove_all<DeviceCondition>(where(c(&DeviceCondition::ruleId) == rule.id));
    }
    catch (const std::system_error &e)
    {
        error = e.what();
    }
    catch (...)
    {
        error = "unknown exeption";
    }

    for (auto &dc : rule.deviceConditions)
    {
        string id = genUuid();
        DeviceCondition edc = fromModel(id, rule.id, dc);
        try
        {
            gStorage->replace(edc);
        }
        catch (const std::system_error &e)
        {
            error = e.what();
            return;
        }
        catch (...)
        {
            error = "unknown exeption";
            return;
        }
    }

    // remove and replace ruleConditions
    try
    {
        gStorage->remove_all<RuleCondition>(where(c(&RuleCondition::ruleId) == rule.id));
    }
    catch (const std::system_error &e)
    {
        error = e.what();
    }
    catch (...)
    {
        error = "unknown exeption";
    }

    for (auto &rc : rule.ruleConditions)
    {
        RuleCondition erc = fromModel(rule.id, rc);
        try
        {
            gStorage->replace(erc);
        }
        catch (const std::system_error &e)
        {
            error = e.what();
            return;
        }
        catch (...)
        {
            error = "unknown exeption";
            return;
        }
    }

    // remove and replace deviceActions
    try
    {
        gStorage->remove_all<DeviceAction>(where(c(&DeviceAction::ruleId) == rule.id));
    }
    catch (const std::system_error &e)
    {
        error = e.what();
    }
    catch (...)
    {
        error = "unknown exeption";
    }

    // update or insert deviceActions
    for (auto &da : rule.deviceActions)
    {
        DeviceAction eda = fromModel(rule.id, da);
        try
        {
            gStorage->replace(eda);
        }
        catch (const std::system_error &e)
        {
            error = e.what();
            return;
        }
        catch (...)
        {
            error = "unknown exeption";
            return;
        }
    }

    // remove and replace ruleActions
    try
    {
        gStorage->remove_all<RuleAction>(where(c(&RuleAction::ruleId) == rule.id));
    }
    catch (const std::system_error &e)
    {
        error = e.what();
    }
    catch (...)
    {
        error = "unknown exeption";
    }

    // update or insert ruleActions
    for (auto &ra : rule.ruleActions)
    {
        RuleAction era = fromModel(rule.id, ra);
        try
        {
            gStorage->replace(era);
        }
        catch (const std::system_error &e)
        {
            error = e.what();
            return;
        }
        catch (...)
        {
            error = "unknown exeption";
            return;
        }
    }

    return;
};

void Sqlite::updateRuleActiveState(string id, ActiveState state, string &error)
{
    const std::lock_guard<std::mutex> lock(this->mutex_);
    // cout << "Sqlite::updateRuleActiveState : " << id << " -> state : " << state << endl;
    try
    {
        gStorage->update_all(sqlite_orm::set(c(&Rule::activeState) = (int)state), where(c(&Rule::id) == id));
    }
    catch (const std::system_error &e)
    {
        error = e.what();
        return;
    }
    catch (...)
    {
        error = "unknown exeption";
        return;
    }
};

void Sqlite::updateTriggerStateRule(string id, TriggerState state, string &error)
{
    const std::lock_guard<std::mutex> lock(this->mutex_);

    try
    {
        gStorage->update_all(sqlite_orm::set(c(&Rule::triggerState) = (int)state), where(c(&Rule::id) == id));
    }
    catch (const std::system_error &e)
    {
        error = e.what();
        return;
    }
    catch (...)
    {
        error = "unknown exeption";
        return;
    }
};

void Sqlite::deleteRule(string id, string &error)
{
    cout << "DeleteRule :: " << id << endl;
    const std::lock_guard<std::mutex> lock(this->mutex_);

    try
    {
        gStorage->remove<Rule>(id);
    }
    catch (const std::system_error &e)
    {
        error = e.what();
    }
    catch (...)
    {
        error = "unknown exeption";
    }

    return;
};

uint64_t Sqlite::countRule(string type, string &error)
{
    bool isQueryType = (type != "");
    RuleType ruleType;
    enumFormString(type, ruleType);

    uint64_t count = 0;
    try
    {
        count = gStorage->count<Rule>((where(
            (not isQueryType or (c(&Rule::type) == (int)ruleType)))));
    }
    catch (const std::system_error &e)
    {
        error = e.what();
    }
    catch (...)
    {
        error = "unknown exeption";
    }

    return count;
};

vector<model::Rule> Sqlite::listRule(string type, string &error)
{
    bool isQueryType = (type != "");
    RuleType ruleType;
    enumFormString(type, ruleType);

    vector<model::Rule> mds;

    for (auto &record : gStorage->iterate<Rule>((where(
             (not isQueryType or (c(&Rule::type) == (int)ruleType))))))
    {
        model::Rule md = toModel(record);

        // lay danh sach DeviceCondition
        vector<model::DeviceCondition> dcList;
        for (auto &dcRecord : gStorage->iterate<DeviceCondition>(where(c(&DeviceCondition::ruleId) == md.id)))
        {
            model::DeviceCondition mdc = toModel(dcRecord);
            dcList.push_back(mdc);
        }
        md.deviceConditions = dcList;

        // lay danh sach RuleCondition
        vector<model::RuleCondition> rcList;
        for (auto &rcRecord : gStorage->iterate<RuleCondition>(where(c(&RuleCondition::ruleId) == md.id)))
        {
            model::RuleCondition mrc = toModel(rcRecord);
            rcList.push_back(mrc);
        }
        md.ruleConditions = rcList;

        // lay danh sach DeviceAction
        vector<model::DeviceAction> daList;
        for (auto &daRecord : gStorage->iterate<DeviceAction>(where(c(&DeviceAction::ruleId) == md.id)))
        {
            model::DeviceAction mda = toModel(daRecord);
            daList.push_back(mda);
        }
        md.deviceActions = daList;

        // lay danh sach RuleAction
        vector<model::RuleAction> raList;
        for (auto &raRecord : gStorage->iterate<RuleAction>(where(c(&RuleAction::ruleId) == md.id)))
        {
            model::RuleAction mra = toModel(raRecord);
            raList.push_back(mra);
        }
        md.ruleActions = raList;
        mds.push_back(md);
    }

    return mds;
};

model::Rule Sqlite::readRule(string id, string &error)
{
    model::Rule md;

    try
    {
        auto record = gStorage->get<Rule>(id);
        md = toModel(record);

        // lay danh sach DeviceCondition
        vector<model::DeviceCondition> dcList;
        for (auto &dcRecord : gStorage->iterate<DeviceCondition>(where(c(&DeviceCondition::ruleId) == md.id)))
        {
            model::DeviceCondition mdc = toModel(dcRecord);
            dcList.push_back(mdc);
        }
        md.deviceConditions = dcList;

        // lay danh sach RuleCondition
        vector<model::RuleCondition> rcList;
        for (auto &rcRecord : gStorage->iterate<RuleCondition>(where(c(&RuleCondition::ruleId) == md.id)))
        {
            model::RuleCondition mrc = toModel(rcRecord);
            rcList.push_back(mrc);
        }
        md.ruleConditions = rcList;

        // lay danh sach DeviceAction
        vector<model::DeviceAction> daList;
        for (auto &daRecord : gStorage->iterate<DeviceAction>(where(c(&DeviceAction::ruleId) == md.id)))
        {
            model::DeviceAction mda = toModel(daRecord);
            daList.push_back(mda);
        }
        md.deviceActions = daList;

        // lay danh sach RuleAction
        vector<model::RuleAction> raList;
        for (auto &raRecord : gStorage->iterate<RuleAction>(where(c(&RuleAction::ruleId) == md.id)))
        {
            model::RuleAction mra = toModel(raRecord);
            raList.push_back(mra);
        }
        md.ruleActions = raList;
    }
    catch (std::system_error e)
    {
        error = e.what();
    }
    catch (...)
    {
        error = "unknown exeption";
    }

    return md;
};

// Notification
void Sqlite::addNotification(model::Notification notification, string &error)
{
    string id = genUuid();
    Notification en = fromModel(id, notification);

    const std::lock_guard<std::mutex> lock(this->mutex_);
    try
    {
        gStorage->replace(en);
    }
    catch (const std::system_error &e)
    {
        error = e.what();
    }
    catch (...)
    {
        error = "unknown exeption";
    }

    return;
};

void Sqlite::deleteNotification(string id, string &error)
{
    const std::lock_guard<std::mutex> lock(this->mutex_);

    try
    {
        gStorage->remove<Notification>(id);
    }
    catch (const std::system_error &e)
    {
        error = e.what();
    }
    catch (...)
    {
        error = "unknown exeption";
    }

    return;
};

vector<model::Notification> Sqlite::listNotification(void)
{
    vector<model::Notification> mns;

    for (auto &record : gStorage->iterate<Notification>())
    {
        model::Notification mn = toModel(record);
        mns.push_back(mn);
    }

    return mns;
};

// Gateway
// void Sqlite::addGateway(model::Gateway gateway, string &error)
// {
//     Gateway en = fromModel(gateway);

//     const std::lock_guard<std::mutex> lock(this->mutex_);
//     try
//     {
//         gStorage->replace(en);
//     }
//     catch (const std::system_error &e)
//     {
//         error = e.what();
//     }
//     catch (...)
//     {
//         error = "unknown exeption";
//     }

//     return;
// };

// void Sqlite::updateGatewayMode(Mode mode, string &error)
// {
//     const std::lock_guard<std::mutex> lock(this->mutex_);
//     // cout << "Sqlite::updateRuleActiveState : " << id << " -> state : " << state << endl;
//     try
//     {
//         gStorage->update_all(sqlite_orm::set(c(&Mode::mode) = (int)mode), where(c(&Gateway::id) == "1"));
//     }
//     catch (const std::system_error &e)
//     {
//         error = e.what();
//         return;
//     }
//     catch (...)
//     {
//         error = "unknown exeption";
//         return;
//     }
// };

//vector<model::DeviceAction> Sqlite::listDeviceAction(string deviceId, string &error)
//{
//    bool isQueryType = (deviceId != "");
//    vector<model::DeviceAction> mds;
//    for (auto &daRecord : gStorage->iterate<DeviceAction>(where(c(&DeviceAction::deviceId) == deviceId)))
//    {
//        model::DeviceAction mda = toModel(daRecord);
//        mds.push_back(mda);
//    }
//
//    return mds;
//}

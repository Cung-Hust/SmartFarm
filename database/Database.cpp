#include "Database.hpp"

string Db::dbType = "";

IDb *Db::getDb()
{
    if (!Db::dbType.compare("sqlite"))
    {
        return Sqlite::getInstance();
    }

    return nullptr;
}

void Db::init(string configFile)
{
    DatabaseConfiguration config;
    config.loadConfig(configFile);

    Db::dbType = config.type;
    if (!Db::dbType.compare("sqlite"))
    {
        return Sqlite::init(config.params);
    }
}
#pragma once

#include "IDb.hpp"
#include "configuration.hpp"
#include "sqlite/Sqlite.hpp"

class Db
{
private:
    static string dbType;

public:
    static IDb *getDb();
    static void init(string configFile);
};
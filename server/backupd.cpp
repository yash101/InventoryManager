#include "backupd.hpp"
#include "../lib/devlib/string.hpp"
#include "ports.hpp"
#include <stdio.h>
#include <string.h>
#include <iostream>
#include <time.h>

std::string randomString(size_t len)
{
    std::string x;
    static const char alphanum[] ="0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
    for (int i = 0; i < len; ++i)
    {
        x += alphanum[rand() % (sizeof(alphanum) - 1)];
    }
    return x;
}

std::string createBackup(std::string name)
{
    std::string cmd = dev::getConfigurationParser()["path_to_zip"] + " -rv " + dev::getConfigurationParser()["backup_location"] + "/" + name + ".zip " + dev::getConfigurationParser()["database_location"];
    system(cmd.c_str());
    return name;
}

std::string createBackup()
{
    return createBackup("backup_" + randomString(10));
}

class bootup_t
{
public:
    bootup_t() { srand(time(NULL)); }
};
bootup_t random_seeder;

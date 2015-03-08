#include "ports.hpp"
#include "../lib/devlib/configuration.hpp"
#include "../lib/devlib/string.hpp"
#include "../lib/devlib/log.hpp"
#include <string>
#if !defined(_WIN32) && !defined(__WIN32) && !defined(_WIN32_) && !defined(__WIN32__)
#include <sys/stat.h>
#else
#include <windows.h>
#endif
int port_web;
dev::ConfigurationParser _users;
dev::ConfigurationParser& users() { return _users; }

std::string getDBLocation(std::string path)
{
    return dev::getConfigurationParser()["database_location"] + path;
}

void port_check()
{
    //Listening port check
    if(dev::getConfigurationParser()["listen"].size() == 0)
    {
        dev::getConfigurationParser()["listen"] = dev::toString(DEFAULT_WEB_PORT);
        dev::getConfigurationParser().flush();
        dev::Log()("Set the default port in the configuration file! Web port is now " + dev::toString(DEFAULT_WEB_PORT));
    }
    port_web = dev::stoi(dev::getConfigurationParser()["listen"]);

    //Administration password check
    if(dev::getConfigurationParser()["admin_password"].size() == 0)
    {
        dev::getConfigurationParser()["admin_password"] = DEFAULT_ADMIN_PASSWD;
        dev::getConfigurationParser().flush();
        dev::Log()("Set the default administration password in the configuration file! The admin password is now " + dev::toString(DEFAULT_ADMIN_PASSWD));
    }

    //Email configuration check
    if(dev::getConfigurationParser()["enable_email"].size() == 0)
    {
        dev::getConfigurationParser()["enable_email"] = DEFAULT_EMAIL_ENABLED;
        dev::getConfigurationParser().flush();
        dev::Log()("Set the email settings in the configuration file!");
    }

    //Sendmail path configuration check
    if(dev::getConfigurationParser()["path_to_sendmail"].size() == 0)
    {
        dev::getConfigurationParser()["path_to_sendmail"] = DEFAULT_PATH_TO_SENDMAIL;
        dev::getConfigurationParser().flush();
        dev::Log()("Set the email settings in the configuration file! The path to sendmail is now " + dev::toString(DEFAULT_PATH_TO_SENDMAIL));
    }

    //ZIP path configuration check
    if(dev::getConfigurationParser()["path_to_zip"].size() == 0)
    {
        dev::getConfigurationParser()["path_to_zip"] = DEFAULT_PATH_TO_ZIP;
        dev::getConfigurationParser().flush();
        dev::Log()("Set the archiving settings in the configuration file! The path to ZIP is now " + dev::toString(DEFAULT_PATH_TO_ZIP));
    }

    //User list location
    if(dev::getConfigurationParser()["userlist"].size() == 0)
    {
        dev::getConfigurationParser()["userlist"] = DEFAULT_USER_LIST;
        dev::getConfigurationParser().flush();
        dev::Log()("Set the path to the userlist");
        _users = dev::ConfigurationParser(dev::getConfigurationParser()["userlist"]);
        _users["user"] = "password";
    }
    else _users = dev::ConfigurationParser(dev::getConfigurationParser()["userlist"]);

    if(dev::getConfigurationParser()["database_location"].size() == 0)
    {
        dev::getConfigurationParser()["database_location"] = DEFAULT_DATABASE_DIRECTORY;
        dev::getConfigurationParser().flush();
        dev::Log()("Set the database path to " + dev::toString(DEFAULT_DATABASE_DIRECTORY));
    }

    if(dev::getConfigurationParser()["access_domain"].size() == 0)
    {
        dev::getConfigurationParser()["access_domain"] = DEFAULT_ACCESS_URL;
        dev::getConfigurationParser().flush();
        dev::Log()("Set the default access domain to " + dev::toString(DEFAULT_ACCESS_URL));
    }

    if(dev::getConfigurationParser()["backup_location"].size() == 0)
    {
        dev::getConfigurationParser()["backup_location"] = DEFAULT_BACKUP_LOCATION;
        dev::getConfigurationParser().flush();
        dev::Log()("Set the backup path to " + dev::toString(DEFAULT_BACKUP_LOCATION));
    }

#if !defined(_WIN32) && !defined(__WIN32) && !defined(_WIN32_) && !defined(__WIN32__)
    mkdir(dev::getConfigurationParser()["database_location"].c_str(), 0700);
    mkdir(dev::getConfigurationParser()["backup_location"].c_str(), 0700);
#else
#endif

    std::ofstream fout(getDBLocation("/index.dat"), std::ios::app);
    if(!fout.is_open())
    {
        std::cout << "Trunctating Index!" << std::endl;
        fout.open(getDBLocation("/index.dat"), std::ios::trunc);
        if(fout.is_open())
        {
            fout.close();
            dev::Log()("Created database index file!");
        }
        else dev::Log()("Failed at creating database index file!");
    }
    else fout.close();
}

//Just used to automate the port setup on app startup
class pchka
{
public:
    pchka() { port_check(); }
};
pchka p;

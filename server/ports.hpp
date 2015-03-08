#ifndef PORTS_HPP
#define PORTS_HPP
#define DEFAULT_WEB_PORT 1234
#define DEFAULT_ADMIN_PASSWD "password"
#define DEFAULT_EMAIL_ENABLED "false"
#define DEFAULT_PATH_TO_SENDMAIL "/var/lib/sendmail -t"
#define DEFAULT_USER_LIST "users.list"
#define DEFAULT_DATABASE_DIRECTORY "database"
#define DEFAULT_ACCESS_URL "localhost"
#define DEFAULT_PATH_TO_ZIP "/usr/bin/zip"
#define DEFAULT_BACKUP_LOCATION "backups"
#include "../lib/devlib/configuration.hpp"
void port_check();
extern int port_web;
dev::ConfigurationParser& users();
std::string getDBLocation(std::string location);
#endif // PORTS_HPP

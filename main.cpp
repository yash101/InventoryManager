#include "server/webserver.hpp"
#include "server/ports.hpp"
#include "lib/devlib/string.hpp"
#include "lib/devlib/log.hpp"
#include "lib/devlib/buildconf.h"
#include "lib/devlib/core.hpp"
#include "lib/devlib/math.hpp"
#include "server/backupd.hpp"
#include "server/inventory.hpp"
#include <iostream>
void cmdline(int argc, char** argv);
int main(int argc, char** argv)
{
    std::cout << std::setprecision(1000) << dev::piCalc<long double>(5000000000) << std::endl;

    cmdline(argc, argv);
    WebServer server;
    server.set_listening_port(port_web);
    server.start();
}

void cmdline(int argc, char** argv)
{
    if(argc <= 1) return;
    bool shouldExit = true;

    std::vector<std::string> args;
    for(int i = 1; i < argc; i++)
    {
        args.push_back(argv[i]);
    }

    try
    {
        for(size_t i = 0; i < args.size(); i++)
        {
            if(args[i].find("configuration") != std::string::npos)           //If the user wants to mess with the configuration
            {
                if(!(++i < args.size())) throw dev::exception("No method was provided for 'configuration'!");
                if(args[i].find("get") != std::string::npos)
                {
                    if(!(++i < args.size())) throw dev::exception("No value for 'key' was provided for 'configuration', with method 'get'!");
                    std::cout << "Query for [" << args[i] << "] resulted with the result [" << dev::getConfigurationParser()[args[i]] << "]" << std::endl;
                }
                else if(args[i].find("set") != std::string::npos)
                {
                    if(!(++i < args.size())) throw dev::exception("No value for 'key' was provided for 'configuration', with method 'set'!");
                    if(!(++i < args.size())) throw dev::exception("No value for 'value' was provided for 'configuration', with method 'set'!");
                    dev::getConfigurationParser()[args[i - 1]] = args[i];
                    dev::getConfigurationParser().flush();
                    std::cout << "Value set successfully!" << std::endl;
                }
                else throw dev::exception("Method provided could not be understood by the parser!");
            }
            else if(args[i].find("log") != std::string::npos)                //If the user wants to mess with the log file
            {
                if(!(++i < args.size())) throw dev::exception("No method was provided for 'log'!");
                if(args[i].find("read") != std::string::npos)
                {
                    std::ifstream fout(LOG_LOCATION);
                    if(fout.is_open()) std::cout << fout.rdbuf() << std::endl;
                    else std::cout << "Unable to read logfile!" << std::endl;
                }
                else if(args[i].find("clear") != std::string::npos)
                {
                    std::ofstream fout(LOG_LOCATION, std::ios::trunc);
                    if(fout.is_open()) std::cout << "Deletion Successful!" << std::endl;
                    else std::cout << "Deletion Unsuccessful!" << std::endl;
                }
            }
            else if(args[i].find("backup") != std::string::npos) createBackup();        //Backups
            else if(args[i].find("inventory") != std::string::npos)                     //Inventory Management
            {
                if(!(++i < args.size())) throw dev::exception("No method was provided for 'inventory'!");
                if(args[i].find("clear") != std::string::npos)
                {
                    std::cout << "Will attempt archival before database destruction!" << std::endl;
                    createBackup();
                    if(getInventory().clear()) std::cout << "Destruction Successful!" << std::endl;
                    else std::cout << "Destruction failed!" << std::endl;
                }
                else if(args[i].find("add") != std::string::npos)
                {
                    if(!(++i < args.size())) throw dev::exception("No value for 'name' was provided for 'inventory', with method 'add'!");
                    getInventory().add(args[i]);
                    std::cout << "Adding: [" << args[i] << "]" << std::endl;
                }
                else if(args[i].find("modify") != std::string::npos)
                {
                    if(!(i + 3 < args.size())) throw dev::exception("Invalid argument count!");
                    if(getInventory().set(args[i + 1], args[i + 2], args[i + 3])) std::cout << "Success!" << std::endl;
                    else std::cout << "Failure!" << std::endl;
                    i += 3;
                }
                else if(args[i].find("backup") != std::string::npos) createBackup();
                else if(args[i].find("decrement") != std::string::npos)
                {
                    if(!(++i < args.size())) throw dev::exception("No names?");
                    getInventory().decrement(args[i]);
                }
            }
            else if(args[i] == "user")
            {
                if(!(++i < args.size())) throw dev::exception("Missing user program arguments!");
                if(args[i] == "add")
                {
                    if(!(++i < args.size())) throw dev::exception("Missing username!");
                    if(!(++i < args.size())) throw dev::exception("Missing password!");
                    users().add(args[i - 1], args[i]);
                    users().flush();
                }
                if(args[i] == "remove")
                {
                    if(!(++i < args.size())) throw dev::exception("Missing username!");
                    users().remove(args[i]);
                    users().flush();
                }
            }
            else throw dev::exception("Unable to parse instruction. Unknown target '" + args[i] + "'!");
        }
    }
    catch(dev::exception& e) { std::cout << e.what() << std::endl; }

    if(shouldExit) exit(EXIT_SUCCESS);
}

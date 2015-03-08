#include "inventory.hpp"
#include <fstream>
#include <iomanip>
#include "../lib/devlib/configuration.hpp"
#include "../lib/devlib/string.hpp"
#include "../lib/devlib/log.hpp"
#include "inventory_back.hpp"


bool Inventory::add(std::string name)
{
    std::string index_location = getDBLocation("/index.dat");       //The location of the index file

    if(exists(name))                                                //Make sure that the object does not already exist!
    {
        _lastError = "E_EXISTS -> " + dev::toString(__FILE__) + " near " + dev::toString(__LINE__); //Set the error as well as where it exists!
        dev::Log()(_lastError);                                     //Log the error
        return false;           //Failure
    }

    //Open the index file and append to it -- the item!
    std::ofstream fout(index_location.c_str(), std::ios::app);
    //Check to see that the file is open. If it isn't, log the error!
    if(!fout.is_open())
    {
        _lastError = "E_FILE_WRITE -> " + dev::toString(__FILE__) + " near " + dev::toString(__LINE__);
        dev::Log()(_lastError);
        return false;           //Failure
    }

    //Write the name, URL-Encoded, to the file!
    fout << encodeURI(name) << "\n";        //Add the item to the database
    fout.close();                           //Close the file access to the database

    //Write the current file number to the database file!
    if(!std::ifstream(getDBLocation("/num.dat")).is_open()) write_file("/num.dat", "0");
    write_file(getDBLocation("/num.dat"), dev::toString(dev::ston<size_t>(read_file(getDBLocation("/num.dat"))) + 1));

    //Stores the filename to the database
    dev::ConfigurationParser fname_dat(getDBLocation("/fname.dat"));
    fname_dat[name] = "/db_" + read_file(getDBLocation("/num.dat")) + ".dat";
    fname_dat.flush();

    //Initialize the data of the object
    initialize(name);           //Initialize the metadata
    return true;                //Success
}

bool Inventory::setCount(std::string name, size_t sz)
{
    if(!exists(name))
    {
        _lastError = "E_OBJECT_NOT_FOUND -> " + dev::toString(__FILE__) + " near " + dev::toString(__LINE__);
        dev::Log()(_lastError);
        return false;
    }
    dev::ConfigurationParser fnames(getDBLocation("/fname.dat"));
    dev::ConfigurationParser cfg(getDBLocation(fnames[name]));
    cfg["quantity"] = dev::toString(sz);
    cfg.flush();
    return true;
}

size_t Inventory::getCount(std::string name) { return dev::ston<size_t>(get(name, "quantity")); }

bool Inventory::increment(std::string name, size_t quantity)
{
    if(!exists(name))
    {
        _lastError = "E_OBJECT_NOT_FOUND -> " + dev::toString(__FILE__) + " near " + dev::toString(__LINE__);
        dev::Log()(_lastError);
        return false;
    }
    dev::ConfigurationParser fnames(getDBLocation("/fname.dat"));
    dev::ConfigurationParser cfg(getDBLocation(fnames[name]));
    cfg["quantity"] = dev::toString(dev::ston<size_t>(cfg["quantity"]) + quantity);
    cfg.flush();
    return true;
}

bool Inventory::increment(std::string name) { return increment(name, 1); }

bool Inventory::decrement(std::string name, size_t quantity)
{
    if(!exists(name))
    {
        _lastError = "E_OBJECT_NOT_FOUND -> " + dev::toString(__FILE__) + " near " + dev::toString(__LINE__);
        dev::Log()(_lastError);
        return false;
    }
    dev::ConfigurationParser fnames(getDBLocation("/fname.dat"));
    dev::ConfigurationParser cfg(getDBLocation(fnames[name]));
    cfg["quantity"] = dev::toString(dev::ston<size_t>(cfg["quantity"]) - quantity);
    if(dev::ston<size_t>(cfg["quantity"]) <= 0) cfg["quantity"] = "0";
    cfg.flush();
    return true;
}

bool Inventory::decrement(std::string name) { return decrement(name, 1); }

bool Inventory::exists(std::string name)
{
    std::ifstream fin(getDBLocation("/index.dat").c_str());
    if(!fin.is_open())
    {
        _lastError = "E_FILE_READ -> " + dev::toString(__FILE__) + " near " + dev::toString(__LINE__);
        dev::Log()(_lastError);
        return false;
    }
    std::string n = dev::itrim(name);
    std::string buffer;
    while(std::getline(fin, buffer, '\n'))
    {
        if(decodeURI(dev::itrim(buffer)).size() != 0 && decodeURI(dev::itrim(buffer)) == n) return true;
    }
    return false;
}

bool Inventory::set(std::string name, std::string key, std::string value)
{
    if(!exists(name))
    {
        _lastError = "E_OBJECT_NOT_FOUND -> " + dev::toString(__FILE__) + " near " + dev::toString(__LINE__);
        dev::Log()(_lastError);
        return false;
    }
    dev::ConfigurationParser fnames(getDBLocation("/fname.dat"));
    dev::ConfigurationParser cfg(getDBLocation(fnames[name]));
    cfg[key] = value;
    cfg.flush();
    return true;
}

std::string Inventory::get(std::string name, std::string key)
{
    if(!exists(name))
    {
        _lastError = "E_OBJECT_NOT_FOUND -> " + dev::toString(__FILE__) + " near " + dev::toString(__LINE__);
        dev::Log()(_lastError);
        return _lastError;
    }
    dev::ConfigurationParser fnames(getDBLocation("/fname.dat"));
    dev::ConfigurationParser cfg(getDBLocation(fnames[name]));
    return cfg[key];
}

std::vector<std::string> Inventory::find(std::string q, std::string method)
{
    std::string query = dev::tolower(q);
    std::vector<std::string> found = getlist();
    if(method == "keywords")
    {
        std::stringstream querysst(query);
        std::string buffer;
        while(std::getline(querysst, buffer, ' '))
        {
            std::vector<std::string> red;
            for(size_t i = 0; i < found.size(); i++)
            {
                if(dev::tolower(found[i]).find(buffer) != std::string::npos) red.push_back(found[i]);
            }
            found = red;
        }
    }
    else
    {
        std::vector<std::string> red;
        for(size_t i = 0; i < found.size(); i++)
        {
            if(dev::tolower(found[i]).find(query) != std::string::npos) red.push_back(found[i]);
        }
        return red;
    }
    return found;
}

std::vector<std::string> Inventory::getlist()
{
    std::vector<std::string> found;
    std::ifstream fin(getDBLocation("/index.dat"));
    if(!fin.is_open())
    {
        _lastError = "E_FILE_READ -> " + dev::toString(__FILE__) + " near " + dev::toString(__LINE__);
        dev::Log()(_lastError);
        found.push_back(_lastError);
        return found;
    }
    std::string buffer;
    while(std::getline(fin, buffer, '\n'))
    {
        if(buffer.size() != 0) found.push_back(decodeURI(buffer));
    }
    return found;
}

void Inventory::initialize(std::string name)
{
    set(name, "quantity", "0");
    set(name, "description", "No description has been added yet!");
}

bool Inventory::remove(std::string name)
{
    if(!exists(name))
    {
        _lastError = "E_OBJECT_NOT_FOUND -> " + dev::toString(__FILE__) + " near " + dev::toString(__LINE__);
        dev::Log()(_lastError);
        return false;
    }
    std::vector<std::string> buffer = getlist();
    std::ofstream out(getDBLocation("/index.dat").c_str());
    if(!out.is_open())
    {
        _lastError = "E_FILE_WRITE -> " + dev::toString(__FILE__) + " near " + dev::toString(__LINE__);
        dev::Log()(_lastError);
        return false;
    }
    for(size_t i = 0; i < buffer.size(); i++)
    {
        if(buffer[i] != name)
        {
            out << encodeURI(buffer[i]) << "\n";
        }
    }
    dev::ConfigurationParser fname(getDBLocation("/fname.dat"));
    if(::remove(getDBLocation(fname[name]).c_str()) != 0)
    {
        _lastError = "E_FILE_DELETE -> " + dev::toString(__FILE__) + " near " + dev::toString(__LINE__);
        dev::Log()(_lastError);
        return false;
    }
    fname.remove(name);
    fname.flush();
    return true;
}

bool Inventory::clear()
{
    std::ofstream(getDBLocation("/index.dat"), std::ios::trunc);
    std::map<std::string, std::string> files = dev::ConfigurationParser(getDBLocation("/fname.dat")).getMap();
    for(std::map<std::string, std::string>::const_iterator it = files.begin(); it != files.end(); ++it)
    {
        if(::remove(getDBLocation(it->second).c_str()))
        {
            _lastError = "E_FILE_DELETE -> " + dev::toString(__FILE__) + " near " + dev::toString(__LINE__);
            dev::Log()(_lastError);
        }
    }
    std::ofstream(getDBLocation("/fname.dat"), std::ios::trunc);
    return true;
}

std::string Inventory::getAccessToken(std::string name)
{
    return dev::ConfigurationParser(getDBLocation("/fname.dat"))[name];
}
std::string Inventory::getByAccessToken(std::string token, std::string key)
{
    return dev::ConfigurationParser(getDBLocation(token))[key];
}
void Inventory::setByAccessToken(std::string token, std::string key, std::string value)
{
    dev::ConfigurationParser cfg(getDBLocation(token));
    cfg[key] = value;
    cfg.flush();
}

std::string Inventory::getName(std::string token)
{
    std::map<std::string, std::string> mp = dev::ConfigurationParser(getDBLocation("/fname.dat")).getMap();
    for(std::map<std::string, std::string>::const_iterator it = mp.begin(); it != mp.end(); ++it)
    {
        if((*it).second == token) return (*it).first;
    }
    _lastError = "E_TOKEN_RESOLVE -> " + dev::toString(__FILE__) + " near " + dev::toString(__LINE__);
    dev::Log()(_lastError);
    return "Unable to resolve token!";
}

Inventory::Inventory() {}

Inventory inventory;
Inventory& getInventory() { return inventory; }

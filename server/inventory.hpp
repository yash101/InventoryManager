#ifndef INVENTORY_HPP
#define INVENTORY_HPP
#include "ports.hpp"
#include <stddef.h>
#include <string>
#include <vector>
class Inventory
{
private:
    std::string _lastError;
public:
    Inventory();
    void initialize(std::string name);
    bool add(std::string name);
    bool setCount(std::string name, size_t sz);
    bool increment(std::string name, size_t quantity);
    bool decrement(std::string name, size_t quantity);
    bool increment(std::string name);
    bool decrement(std::string name);
    bool exists(std::string name);
    bool set(std::string name, std::string key, std::string value);
    bool remove(std::string name);
    bool clear();
    size_t getCount(std::string name);
    std::string get(std::string name, std::string key);
    std::vector<std::string> find(std::string query, std::string method);
    std::vector<std::string> getlist();
    std::string getName(std::string token);

    std::string getAccessToken(std::string name);
    std::string getByAccessToken(std::string token, std::string key);
    void setByAccessToken(std::string token, std::string key, std::string value);

    const std::string lastError() const { return _lastError; }
    void resetError() { _lastError = ""; }
};
Inventory& getInventory();
std::string encodeURI(std::string);
std::string decodeURI(std::string);
#endif // INVENTORY_HPP

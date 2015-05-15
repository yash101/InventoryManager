#include "webserver.hpp"
#include "../lib/devlib/string.hpp"
#include <fstream>
#include "../lib/devlib/log.hpp"
#include "ports.hpp"
#include "inventory.hpp"
#include "automtx.hpp"
std::string readFile(std::string location)
{
    std::fstream fin(location.c_str());
    if(fin.is_open())
    {
        return dev::toString(fin.rdbuf());
    }
    return "";
}

WebServer::WebServer()
{
}

WebServer::WebServer(int port)
{
    set_listening_port(port);
    start_async();
}

bool authenticate(const dlib::incoming_things& incoming)
{
    users().refresh();
    if(incoming.queries["username"].size() == 0 || incoming.queries["password"].size() == 0) return false;
    if(users()[incoming.queries["username"]] == incoming.queries["password"]) return true;
    return false;
}

const std::string WebServer::on_request(const dlib::incoming_things& incoming, dlib::outgoing_things& outgoing)
{
    automtx<dlib::mutex> mutex(&_mtx);
    //Logging
    std::stringstream lg;
    lg << incoming.request_type << "->" << incoming.path << "@" << incoming.foreign_ip << ":" << incoming.foreign_port;
    dev::Log()(lg.str());

    //Processing
    //Sends the index page
    if(incoming.path == "/") return readFile("res/index.html");
    //Echo incoming path
    if(incoming.path == "/getWorkingURL")
    {
        outgoing.headers["content-type"] = "text/plain";
        return dev::getConfigurationParser()["access_domain"] + ":" + dev::toString(get_listening_port());
    }

    //Authenticates a user
    if(incoming.path == "/authenticate")
    {
        outgoing.headers["content-type"] = "text/plain";
        return authenticate(incoming) ? "true" : "false";
    }

    //Searching
    if(incoming.path == "/search")
    {
        outgoing.headers["content-type"] = "text/plain";
        std::stringstream o;
        if(!authenticate(incoming)) return "<h1>Authentication Failure!</h1>\n";

        std::vector<std::string> results = getInventory().find(incoming.queries["query"], incoming.queries["type"]);
        if(results.size() == 0) o << "<div><pre>Search Failed!</pre></div>\n";

        for(size_t i = 0; i < results.size(); i++)
        {
            std::string token = getInventory().getAccessToken(results[i]);
            o << "<div onclick=\"javascript:loadInformation('" << token << "');\"><pre>" << results[i] << "</pre></div>\n";
        }
        return o.str();
    }

    //Get listing
    if(incoming.path == "/getlist")
    {
        outgoing.headers["content-type"] = "text/plain";
        if(!authenticate(incoming)) return "Authentication Failure!";
        std::stringstream sstreama;
#define s sstreama << '\n' <<
        std::vector<std::string> x = getInventory().getlist();
        for(size_t i = 0; i < x.size(); i++)
        {
            s "<tr>";
                s "<td class=\"quantity\">" << i + 1 << "</td>";
                s "<td onclick=\"javascript:loadInformation('" << getInventory().getAccessToken(x[i]) << "');\">" << x[i] << "</td>";
                s "<td class=\"quantity\">" << dev::ston<size_t>(getInventory().get(x[i], "quantity")) << "</td>";
                s "<td class=\"button\" onclick=\"javascript:increment('"<< getInventory().getAccessToken(x[i]) << "', " << i << ");\">+</td>";
                s "<td class=\"button\" onclick=\"javascript:decrement('"<< getInventory().getAccessToken(x[i]) << "', " << i << ");\">-</td>";
            s "</tr>";
        }
#undef s
        return sstreama.str();
    }

    //Incrementing and decrementing quantity available
    if(incoming.path == "/increment")
    {
        outgoing.headers["content-type"] = "text/plain";
        if(!authenticate(incoming)) return "Authentication Failure!";
        if(!getInventory().increment(getInventory().getName(incoming.queries["token"]))) return getInventory().lastError();
        return dev::toString(getInventory().getByAccessToken(incoming.queries["token"], "quantity"));
    }
    if(incoming.path == "/incrementby")
    {
        outgoing.headers["content-type"] = "text/plain";
        if(!authenticate(incoming)) return "Authentication Failure!";
        if(!getInventory().increment(getInventory().getName(incoming.queries["token"]), dev::ston<size_t>(incoming.queries["amount"]))) return getInventory().lastError();
        return dev::toString(getInventory().getByAccessToken(incoming.queries["token"], "quantity"));
    }
    if(incoming.path == "/decrement")
    {
        outgoing.headers["content-type"] = "text/plain";
        if(!authenticate(incoming)) return "Authentication Failure!";
        if(!getInventory().decrement(getInventory().getName(incoming.queries["token"]))) return getInventory().lastError();
        return dev::toString(getInventory().getByAccessToken(incoming.queries["token"], "quantity"));
    }
    if(incoming.path == "/decrementby")
    {
        outgoing.headers["content-type"] = "text/plain";
        if(!authenticate(incoming)) return "Authentication Failure!";
        if(!getInventory().decrement(getInventory().getName(incoming.queries["token"]), dev::ston<size_t>(incoming.queries["amount"]))) return getInventory().lastError();
        return dev::toString(getInventory().getByAccessToken(incoming.queries["token"], "quantity"));
    }


    if(incoming.path == "/get")
    {
        outgoing.headers["content-type"] = "text/plain";
        if(!authenticate(incoming)) return "Authentication Failure!";
        return getInventory().get(incoming.queries["name"], incoming.queries["key"]);
    }
    if(incoming.path == "/add")
    {
        outgoing.headers["content-type"] = "text/plain";
        if(!authenticate(incoming)) return "Authentication Failure!";
        if(getInventory().add(incoming.queries["name"])) return "Success!";
        return getInventory().lastError();
    }

    if(incoming.path == "/getByToken")
    {
        outgoing.headers["content-type"] = "text/plain";
        if(!authenticate(incoming)) return "Authentication Failure!";
        return getInventory().getByAccessToken(incoming.queries["token"], incoming.queries["key"]);
    }

    if(incoming.path == "/setByToken")
    {
        outgoing.headers["content-type"] = "text/plain";
        if(!authenticate(incoming)) return "Authentication Failure!";
        getInventory().setByAccessToken(incoming.queries["token"], incoming.queries["key"], incoming.queries["value"]);
        return getInventory().getByAccessToken(incoming.queries["token"], incoming.queries["key"]);
    }

    if(incoming.path == "/getName")
    {
        outgoing.headers["content-type"] = "text/plain";
        if(!authenticate(incoming)) return "Authentication Failure!";
        return getInventory().getName(incoming.queries["token"]);
    }

    if(incoming.path == "/getDB")
    {
        outgoing.headers["content-type"] = "text/plain";
        if(!authenticate(incoming)) return "Authentication Failure!";
        return getInventory().getDB(incoming.queries["token"]);
    }

    if(incoming.path == "/setDB")
    {
        outgoing.headers["content-type"] = "text/plain";
        if(!authenticate(incoming)) return "Authentication Failure!";
        return getInventory().setDB(incoming.queries["token"], incoming.queries["data"]) ? "true" : "false";
    }

    if(incoming.path.size() > std::string("/qedit/").size() && incoming.path.substr(0, std::string("/qedit/").size()) == "/qedit/")
    {
        return "<script type=\"text/javascript\">var token = \"/db_" + incoming.path.substr(std::string("/qedit/").size(), incoming.path.size()) + "\"</script>" + readFile("res/qedit_stub.html");
    }

    //UI file server
    std::string file_location = "res/" + incoming.path.substr(1, incoming.path.size());
    size_t i;
    for(i = file_location.size(); i != 0; i--)
    {
        if(file_location[i] == '.') break;
    }
    std::string extention = file_location.substr(i + 1, file_location.size());

    if(extention == "html") outgoing.headers["content-type"] = "text/html";
    if(extention == "css") outgoing.headers["content-type"] = "text/css";
    if(extention == "js") outgoing.headers["content-type"] = "text/javascript";
    if(extention == "png") outgoing.headers["content-type"] = "image/png";
    if(extention == "txt") outgoing.headers["content-type"] = "text/plain";

    return readFile(file_location);
}

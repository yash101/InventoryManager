#ifndef WEBSERVER_HPP
#define WEBSERVER_HPP
#include "../lib/dlib/server.h"
class WebServer : public dlib::server_http
{
private:
    dlib::mutex _mtx;
    const std::string on_request(const dlib::incoming_things& incoming, dlib::outgoing_things& outgoing);
public:
    WebServer();
    WebServer(int port);
};
#endif // WEBSERVER_HPP

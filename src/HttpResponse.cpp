#include "HttpResponse.h"

#include <iostream>

using namespace std;


std::string HttpResponse::toString() const
{
    string resp = "HTTP/1.1 " + to_string(statusCode) + " " + statusMessage + "\r\n";

    for(auto& [key,value]:headers){
        resp += key + ": " + value + "\r\n";
    }

    if(headers.find("Connection") == headers.end()) resp += "Connection: close" "\r\n";

    if(headers.find("Content-Length") == headers.end()) resp += "Content-Length: " + to_string(body.size()) + "\r\n" ;

    resp += "\r\n" + body ;

    return resp;
}
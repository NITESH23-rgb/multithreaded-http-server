#ifndef HTTPRESPONSE_H
#define HTTPRESPONSE_H

#include<string>
#include<unordered_map>

class HttpResponse{
    public:
        int statusCode = 200;
        std::string statusMessage = "OK";

        std::string body;
        std::string toString() const;
        
        std::unordered_map<std::string,std::string> headers;
};

#endif
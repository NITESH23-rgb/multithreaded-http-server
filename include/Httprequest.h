#ifndef HTTPREQUEST_H
#define HTTPREQUEST_H
#include<string>
#include<unordered_map>

class HttpRequest{
    public:
        std::string method;
        std::string path;
        std::string version;

        std::unordered_map<std::string,std::string> headers;
        std::unordered_map<std::string,std::string> form;
        std::unordered_map<std::string,std::string> query;
        
        std::string body;

};

#endif

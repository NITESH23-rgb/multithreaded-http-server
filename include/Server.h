#ifndef SERVER_H
#define SERVER_H

#include "Httprequest.h"
#include "HttpResponse.h"

#include<string>
#include<sstream>
#include<unordered_map>

#include<thread>
#include<mutex>
#include<vector>
#include<queue>
#include<condition_variable>

class Server
{
    private:
        int serversocket;
        int port;
    
    public:
        Server(int port);

        std::queue<int> clientQueue;

        std::mutex queueMutex;

        std::condition_variable cv;

        std::vector<std::thread> workers;

        bool stop = false;

        void handleClient(int clientSocket);
        void logRequest(const HttpRequest& request,const HttpResponse& response);
        void sendResponse(int clientSocket,const HttpResponse& response);
        void start();
        void workerThread();
        
        std::string getContentType(const std::string& filepath);
        std::string readfile(const std::string& filepath);
        
        HttpResponse redirect(const std::string& location,bool permanent);
        HttpResponse routeRequest(const HttpRequest& request);
        HttpResponse serve403();
        HttpResponse serveHello();
        HttpResponse serveLogin(const HttpRequest& request);
        HttpResponse serveStaticFiles(const std::string& path);
        HttpResponse serveServerInfo();
        
        std::unordered_map<std::string,std::string> parseHeaders(std::stringstream& ss);
        std::unordered_map<std::string,std::string> parseKeyValuePairs(const std::string& body);

        HttpRequest parseRequest(const std::string& request);
};

#endif
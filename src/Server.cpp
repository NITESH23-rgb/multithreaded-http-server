#include "Server.h"
#include "HttpResponse.h"
#include<iostream>

#include<sys/socket.h>
#include<netinet/in.h>
#include<unistd.h>

#include<sstream>
#include<fstream>
#include<ctime>
#include<iomanip>

#include<filesystem>

using namespace std;

// Initialize server with given port
Server::Server(int port){
    this->port = port;
    serversocket = -1;
}

// Determine MIME type based on requested file extension
string Server::getContentType(const string& filepath){
    size_t pos = filepath.find_last_of('.');

    // Return default binary type for files without an extension
    if(pos == string::npos) return "application/octet-stream";

    string ext = filepath.substr(pos);

    unordered_map<string,string> mimeTypes = {
                                {".html","text/html"},
                                {".css","text/css"},
                                {".js", "application/javascript"},
                                {".png", "image/png"},
                                {".jpg", "image/jpeg"},
                                {".jpeg", "image/jpeg"},
                                {".gif", "image/gif"},
                                {".svg", "image/svg+xml"},
                                {".ico", "image/x-icon"},
                                {".txt", "text/plain"},
                                {".pdf", "application/pdf"}
    };


    if(mimeTypes.find(ext) != mimeTypes.end()) return mimeTypes[ext];

    return "application/octet-stream";
}

// Generate response for /hello page 
HttpResponse Server::serveHello()
{
    HttpResponse response;

    response.body=
    "<html>"
    "<body>"
    "<h1>Hello from C++!</h1>"
    "</body>"
    "</html>";

    response.headers["Content-Type"]="text/html";

    return response;
}

string Server::readfile(const string& filepath){

    // Open requested static file from the public directory
    ifstream file(filepath, ios::binary);

    if (!file.is_open())
    {
        return "";
    }

    stringstream buffer;
    buffer << file.rdbuf();

    return buffer.str();
}

// Generate 403 forbidden response
HttpResponse Server::serve403()
{
    HttpResponse response;

    response.body= readfile("public/403.html");

    response.statusCode = 403;
    response.statusMessage = "Forbidden";
    response.headers["Content-Type"]="text/html";
    
    return response;
}



// generate login response
HttpResponse Server::serveLogin(const HttpRequest& request)
{

    HttpResponse response;

    string username=request.form.at("username");
    string password= request.form.at("password");

    if(username == "Ramesh123" && password == "Ram@123")
    {
        response.body = readfile("public/login-success.html");
    }
    else
    {
        response.body=readfile("public/login-failed.html");
    }

    response.headers["Content-Type"] = "text/html";

    return response;
}


// Generate server information JSON response
HttpResponse Server::serveServerInfo(){
    HttpResponse response;

    response.headers["Content-Type"] = "application/json";
    
    response.body = 
                    "{"
                    "\"server\":\"CppServer\","
                    "\"version\":\"1.0\","
                    "\"author\":\"Nitesh\""
                    "}";
    return response;
}


// Serve requested static files from the public directory
HttpResponse Server::serveStaticFiles(const string& path)
{
    // Prevent directory traversal attacks
    if(path.find("..") != string::npos) return serve403();

    string filepath;

    if(path == "/") filepath = "public/index.html";
    else filepath = "public" + path ;

    HttpResponse response;

    response.body = readfile(filepath);

    // Return 404 response if requested file does not exist
    if(response.body.empty())
    {
        response.body = readfile("public/404.html"); 

        response.statusCode = 404;
        response.statusMessage = "Not Found";
        response.headers["Content-Type"]="text/html";
        
        return response;
    }

    response.headers["Content-Type"] = getContentType(filepath);
    
    return response;
}

// Generate HTTP redirect response
HttpResponse Server::redirect(const string& location,bool permanent)
{
    HttpResponse response;

    if(permanent)
    {
        response.statusCode = 301;
        response.statusMessage = "Moved Permanently";
    }
    else
    {
        response.statusCode = 302;
        response.statusMessage = "Found";
    }

    response.headers["Location"]  = location;

    return response;
}




// Format and send a valid HTTP response
void Server::sendResponse(int clientSocket, const HttpResponse& response){

    string data = response.toString();

    size_t totalsent=0;

    while(totalsent < data.size())
    {
        int bytesSend = send(clientSocket,data.c_str()+totalsent, data.size()-totalsent, 0 );

        if(bytesSend <= 0) break;

        totalsent += bytesSend; 
    }

    return ;
}


// Write HTTP request and response details to server log file
void Server::logRequest(const HttpRequest& request,const HttpResponse& response)
{
    ofstream log("server.log",ios::app);

    time_t now = time(nullptr);
    tm local;
    localtime_r(&now,&local);

    stringstream ss;
    ss << put_time(&local, "%Y-%m-%d %H:%M:%S");

    log << "=====================================\n";
    log << ss.str()<<"\n";
    log << request.method << " " << request.path << "\n";
    log << response.statusCode << " "
        << response.statusMessage << "\n";

    log << "=====================================\n\n";
}



// For parsing the personal data
unordered_map<string,string> Server::parseKeyValuePairs(const string& body){
    
    string line;
    stringstream personal(body);

    unordered_map<string,string> data;
    while(getline(personal,line,'&')){

        if(line.empty()) continue;
        size_t pos=line.find('=');

        // Prevent directory traversal attacks
        if(pos == string::npos) continue;

        string key,value;
        key=line.substr(0,pos);
        value=line.substr(pos+1);

        data[key]=value;
    }

    return data;
}

// For parsing the headers
unordered_map<string,string> Server::parseHeaders(stringstream& ss){
    string line;
    unordered_map<string,string> headers;

    while(getline(ss,line)){

        if(!line.empty() && line.back() == '\r') line.pop_back();
        if(line.empty()) break;

        string key,value;

        size_t pos = line.find(':');

        // Prevent directory traversal attacks
        if(pos == string::npos) continue;

        key=line.substr(0,pos);
        value=line.substr(pos+1);

        if(!value.empty() && value[0] == ' ') value.erase(value.begin());

        headers[key]=value;
    }

    return headers;
}

// to decide what should happen
HttpResponse Server::routeRequest(const HttpRequest& request){

    if(request.method == "POST" && request.path == "/login" ) 
    {
        auto it = request.form.find("username");

        if (it != request.form.end()) return serveLogin(request);
    }

    if(request.method != "GET")
    {
        HttpResponse response;
        response.body= readfile("public/405.html");

        response.statusCode = 405;
        response.statusMessage = "Method Not Allowed";
        response.headers["Content-Type"] = "text/html";
        
        return response;
    }

    else if(request.path == "/hello")
    {
        return serveHello();
    }

    else if(request.path == "/api/server-info")
    {
        return serveServerInfo();
    }
    else if(request.path == "/home")
    {
        return redirect("/",false);
    }
    return serveStaticFiles(request.path);
}


HttpRequest Server::parseRequest(const string& rawRequest)
{
    string firstline;

    stringstream ss(rawRequest);

    getline(ss,firstline);

    if(!firstline.empty() && firstline.back() == '\r' ) firstline.pop_back(); 

    // Now we are parsing first line on behalf of space
    stringstream requestLine(firstline);

    HttpRequest request;

    string url;

    requestLine>>request.method>>url>>request.version;

    request.headers = parseHeaders(ss);

    size_t pos=url.find('?');

    // Extract query parameters from the URL
    string query="";

    if(pos == string::npos) 
    {
        request.path = url;
    }
    else{
        request.path = url.substr(0,pos);
        query = url.substr(pos+1);
    }

    if(!query.empty()) request.query = parseKeyValuePairs(query);

    if(request.headers.find("Content-Length") != request.headers.end()){
        int len=stoi(request.headers["Content-Length"]);

        request.body.resize(len);

        // Read request body based on Content-Length
        ss.read(&request.body[0],len);
    }

    request.form = parseKeyValuePairs(request.body);

    return request;
}

// Receive and parse request
void Server::handleClient(int clientsocket){

    cout << "Thread: " << this_thread::get_id()
     << " handling client " << clientsocket << endl;

    while(true)
    {
        char requestbuffer[4096];

        // sent query from browser is recieved by server as bytes
        int bytesReceived = recv(clientsocket,requestbuffer,sizeof(requestbuffer),0);

        if(bytesReceived <= 0) 
        {
            close(clientsocket);
            return;
        }

        string rawRequest(requestbuffer,bytesReceived);

        HttpRequest request = parseRequest(rawRequest);

        // if(bytesReceived > 0)
        // {
        //     cout<<"Received "<< bytesReceived <<" bytes\n\n";
        //     cout.write(requestbuffer,bytesReceived);
        //     cout<<"\n\n";
        // }
        
        HttpResponse response=routeRequest(request);

        logRequest(request,response);

        sendResponse(clientsocket,response);

        // Close the connection unless the client requests Keep-Alive
        auto it = request.headers.find("Connection");

        if (it == request.headers.end() || it->second != "keep-alive") break;

    }
    return ;
}

// Worker thread waits for client connections and processes requests
void Server::workerThread()
{
    while(true)
    {
        unique_lock<mutex> lock(queueMutex);

        cv.wait(lock,[this]{return !clientQueue.empty();});

        int client = clientQueue.front();

        clientQueue.pop();

        lock.unlock();

        handleClient(client);
    }
}


void Server::start(){
    
    serversocket=socket(AF_INET,SOCK_STREAM,0);

    if(serversocket == -1) {
        cerr<<"Socket Creation Failed.\n";
        return ;
    }

    sockaddr_in serverAddress{};
    
    serverAddress.sin_family= AF_INET;
    serverAddress.sin_port= htons(port);
    serverAddress.sin_addr.s_addr = INADDR_ANY;

    if(bind(serversocket, (sockaddr*) &serverAddress, sizeof(serverAddress)) == -1){
        cout<<"Bind failed.\n";
        close(serversocket);
        return ;
    }

    if(listen(serversocket,10) < 0){
        cerr<<"Listen failed.\n";
        close(serversocket);
        return ;
    }

    cout << "====================================\n";
    cout << "HTTP Server Started\n";
    cout << "Listening on port " << port << '\n';
    cout << "====================================\n";

    // Create worker threads for the thread pool
    for(int i=0;i<4;i++)
    {
        workers.emplace_back(&Server::workerThread,this);
    }

    // Accept incoming client connections and add them to the task queue
    while(true){

        sockaddr_in clientAddr;

        socklen_t len = sizeof(clientAddr);

        int clientsocket = accept(serversocket,(sockaddr*)&clientAddr,&len);

        if(clientsocket == -1)
        {
            cerr<<"Accept failed.\n";
            continue;
        }
        {
            lock_guard<mutex> lock(queueMutex);
            
            clientQueue.push(clientsocket);
        }
        cv.notify_one();
    }
}

#include "Server.h"
#include<iostream>

using namespace std;

int main(){
    
    Server server(8080);
    server.start();

    return 0;
}
//
//  main.cpp
//  TUIO_Mouse
//
//  Created by Jon Copeland on 4/11/18.
//  Copyright © 2018 CrunchyTech. All rights reserved.
//

#include <unistd.h>
#include <iostream>
#include "TUIOListener.hpp"
#include "TCPServer.hpp"
#include "UDPServer.hpp"

void printUsage()
{
    std::cout << "Usage is: \n";
    std::cout << "TUIO_Mouse [options], options are: \n";
    std::cout << "\t-t | --TCP, force tcp mode, tuio is not used and a direct tcp connection is used instead\n";
    std::cout << "\t-p | --PORT port, set port to listen on for both tcp and tuio modes to port, default is 3333\n";
    std::cout << "\t-u | --UDP. Use UDP using a custom protocal, ie. not TUIO\n";
    std::cout << "\t-h | --HELP. print this help menu\n";
    std::cout << "\tIf neither TCP or UDP are specified TUIO will be used as the default protocal\n";
}

int main(int argc, const char * argv[]) {

    bool shouldRun = true;
    bool UseTCP = false;
    bool UseUDP = false;
    int port = 3333;
    
    if(argc > 1)
    {
        for(int index = 1;index<argc;index++)
        {
            if(strcmp(argv[index], "-u") == 0 || strcmp(argv[index], "--UDP") == 0)
            {
                UseUDP = true;
            }
            if(strcmp(argv[index], "-t") == 0 || strcmp(argv[index], "--TCP") == 0)
            {
                UseTCP = true;
            }
            if(strcmp(argv[index], "-p")==0 || strcmp(argv[index], "--PORT")==0 )
            {
                if(index < (argc-1))
                {
                    index++;
                    port = atoi(argv[index]);
                }
            }
            if(strcmp(argv[index], "-h")==0 || strcmp(argv[index], "--HELP")==0 )
            {
                printUsage();
                return 0;
            }
        }
    }
    else
    {
        printUsage();
    }
    if(UseTCP)
    {
        std::cout << "Starting TCP Server\n";
        TCPServer server(port);
        server.runServer(); // blocks
    }
    else if (UseUDP)
    {
        std::cout << "Starting UDP Server\n";
        UDPServer server(port);
        server.runServer();
    }
    else
    {
        std::cout << "Starting TUIO(UDP) Server\n";
        TUIOListener listener(port);
        
        while(shouldRun) // tuio listener doesnt block so we just sleep forever to keep the tuio thread alive
        {
            usleep(100000);
        }
    }
   
    
    return 0;
}

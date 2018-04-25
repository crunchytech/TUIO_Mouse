//
//  UDPClient.cpp
//  TUIO_Mouse
//
//  Created by Jon Copeland on 4/16/18.
//  Copyright © 2018 CrunchyTech. All rights reserved.
//

#include "UDPServer.hpp"
#include <iostream>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>

#include "MouseDataType.h"
#include "MouseController.hpp"

UDPServer::UDPServer(int port)
{
    this->port = port;
    this->shouldRun = true;
    
    struct sockaddr_in server;
    listenSock = socket(AF_INET,SOCK_DGRAM,IPPROTO_UDP);
    if(listenSock == -1)
    {
        std::cerr << "UDPServer Error Creating Listen Socket !" << std::endl;
        this->shouldRun = false;
        return;
    }
    memset(&server,0,sizeof(sockaddr_in));
    server.sin_family = AF_INET;
    server.sin_port = htons(this->port);
    server.sin_addr.s_addr = htonl(INADDR_ANY);
    
    if(bind(listenSock,(sockaddr*)&server,sizeof(sockaddr_in)) == -1)
    {
        std::cerr << "UDPServer Error Binding To Port " << port << std::endl;
    }
}

UDPServer::~UDPServer()
{
    close(listenSock);
}

void UDPServer::runServer()
{
    while(this->shouldRun)
    {
        char buff[64] = {0};
        ssize_t bytes_read = recv(listenSock, buff, 64,0);
        if(bytes_read == -1)
        {
            std::cerr << "UDPServer Error Reading From Socket !\n";
            continue;
        }
        else
        {
            std::cout << "Read " << bytes_read << " Bytes \n";
            MouseData data;
            if(bytes_read == sizeof(MouseData))
            {
                memcpy(&data,buff,sizeof(MouseData));
                static unsigned long long seq = 0;
                if (data.seq < seq && data.seq != -1)
                {
                    std::cout << "Out Of Order Message ! " << seq << " " << data.seq << std::endl;
                    continue;
                }
                seq = data.seq;
                std::cout << "MouseData x " << data.locationX << " Y " << data.locationY << " event " << data.eventType << std::endl;
                
                if(data.eventType == NX_LMOUSEDOWN)
                {
                    MouseController::MouseLeftDown(data.locationX, data.locationY);
                }
                else if(data.eventType == NX_LMOUSEUP)
                {
                    MouseController::MouseLeftUp(data.locationX, data.locationY);
                }
                else if(data.eventType == NX_RMOUSEDOWN)
                {
                    MouseController::MouseRightDown(data.locationX, data.locationY);
                }
                else if(data.eventType == NX_RMOUSEUP)
                {
                    MouseController::MouseRightUp(data.locationX, data.locationY);
                }
                else if(data.eventType == NX_LMOUSEDRAGGED || data.eventType == NX_RMOUSEDRAGGED || data.eventType == NX_MOUSEMOVED)
                {
                    MouseController::MouseMove(data.locationX, data.locationY);
                }
            }
        }
    }
}

//
//  TCPServer.cpp
//  TUIO_Mouse
//
//  Created by Jon Copeland on 4/11/18.
//  Copyright © 2018 CrunchyTech. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <iostream>
#include "TCPServer.hpp"
#include "MouseController.hpp"
#include "MouseDataType.h"

TCPServer::TCPServer(int port)
{
    struct sockaddr_in serv_addr;
    
    this->clientSock = 0;
    this->shouldRun = true;
    this->port = port;
    serverSock = socket(AF_INET, SOCK_STREAM, 0);
    if(serverSock < 0)
    {
        std::cerr << "Error Creating Socket !! \n";
    }
    
    bzero((char *) &serv_addr, sizeof(serv_addr));
    
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(this->port);
    if (bind(serverSock, (struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0)
         std::cerr << "ERROR on binding\n";
}

TCPServer::~TCPServer()
{
    if(serverSock>0)close(clientSock);
}

void TCPServer::runServer()
{
    struct sockaddr_in client;
    socklen_t cl = sizeof(client);
    listen(serverSock, 5);
    while(shouldRun) // accept sockets loop
    {
        clientSock = accept(serverSock, (struct sockaddr *)&client, &cl);
        if(clientSock < 0)
        {
            std::cerr << "ERROR accepting socket !\n";
            continue;
        }
        size_t totalBytes = 0;
        char buff[64] = {0};
        while(shouldRun) // read from client loop
        {
            ssize_t bytesRead = read(clientSock,&buff[totalBytes],64);
            if(bytesRead <= 0)
            {
                std::cerr << "Error Reading From Socket " << bytesRead << std::endl;
                break;
            }
            
            totalBytes += bytesRead;
            std::cout << " Total Bytes Read " << totalBytes << " Partial Bytes Read " << totalBytes << std::endl;
            handleBuffer(buff, bytesRead);
            totalBytes = 0;
        }
    }
    if(clientSock>0)close(clientSock);
}

void TCPServer::handleBuffer(char* buff,ssize_t bytes_read)
{
    ssize_t si = sizeof(MouseData);

    int offset = 0;
    while(bytes_read >= si)
    {
        MouseData data;
        memset(&data,0,si);
        memcpy(&data,&buff[offset],si);
        offset += si;
        
        std::cout << "Mouse X: " << data.locationX << " Mouse Y: " << data.locationY << " Event: " << data.eventType << std::endl;
        
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
        
        bytes_read -= si;
        
        std::cout << " Mouse Action Complete \n";
    }
    
    if(bytes_read > 0)
    {
        std::cout << " LeftOver Bytes !! " << bytes_read << " data may be unaligned ! \n";
    }
}

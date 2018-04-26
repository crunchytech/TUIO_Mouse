//
//  TCPServer.cpp
//  TUIO_Mouse
//
//  Created by Jon Copeland on 4/11/18.
//  Copyright © 2018 CrunchyTech. All rights reserved.
//

#import <Foundation/Foundation.h>

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

CFSocketRef ipv4socket;

static void TCPServerAcceptCallBack(CFSocketRef socket, CFSocketCallBackType type, CFDataRef address, const void *data, void *info) {
    
    if (kCFSocketAcceptCallBack == type) {
        // for an AcceptCallBack, the data parameter is a pointer to a CFSocketNativeHandle
        CFSocketNativeHandle nativeSocketHandle = *(CFSocketNativeHandle *)data;
        uint8_t name[SOCK_MAXADDRLEN];
        socklen_t namelen = sizeof(name);
        NSData *peer = nil;
        if (0 == getpeername(nativeSocketHandle, (struct sockaddr *)name, &namelen)) {
            peer = [NSData dataWithBytes:name length:namelen];
        }
        
        CFReadStreamRef readStream = NULL;
        CFWriteStreamRef writeStream = NULL;
        
        CFStreamCreatePairWithSocket(kCFAllocatorDefault, nativeSocketHandle, &readStream, &writeStream);
        if (readStream && writeStream) {
            CFReadStreamSetProperty(readStream, kCFStreamPropertyShouldCloseNativeSocket, kCFBooleanTrue);
            CFWriteStreamSetProperty(writeStream, kCFStreamPropertyShouldCloseNativeSocket, kCFBooleanTrue);
            
            CFReadStreamOpen(readStream);
            CFWriteStreamOpen(writeStream);

            MouseData mouseData;
            uint8_t readData[sizeof(mouseData)];
            CFIndex read = 0;
            
            

            do{

                read = CFReadStreamRead(readStream,readData,sizeof(MouseData));
                memset(&mouseData,0,sizeof(MouseData));
                memcpy(&mouseData,readData,sizeof(MouseData));
                
                MouseData data = mouseData;
                
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
                
            }while(read > 0);
            


            
        } else {
            // on any failure, need to destroy the CFSocketNativeHandle
            // since we are not going to use it any more
            close(nativeSocketHandle);
        }
        
        if (readStream) CFRelease(readStream);
        if (writeStream) CFRelease(writeStream);
    }
}


TCPServer::TCPServer(int port)
{
    CFSocketContext socketCtxt = {0, nil, NULL, NULL, NULL};
    ipv4socket = CFSocketCreate(kCFAllocatorDefault, PF_INET, SOCK_STREAM, IPPROTO_TCP, kCFSocketAcceptCallBack, (CFSocketCallBack)&TCPServerAcceptCallBack, &socketCtxt);
    
    if (NULL == ipv4socket) {
        if (ipv4socket) CFRelease(ipv4socket);
        ipv4socket = NULL;
        return ;
    }
    
    int yes = 1;
    setsockopt(CFSocketGetNative(ipv4socket), SOL_SOCKET, SO_REUSEADDR, (void *)&yes, sizeof(yes));
    
    // set up the IPv4 endpoint; if port is 0, this will cause the kernel to choose a port for us
    struct sockaddr_in addr4;
    memset(&addr4, 0, sizeof(addr4));
    addr4.sin_len = sizeof(addr4);
    addr4.sin_family = AF_INET;
    addr4.sin_port = htons(port);
    addr4.sin_addr.s_addr = htonl(INADDR_ANY);
    NSData *address4 = [NSData dataWithBytes:&addr4 length:sizeof(addr4)];
    
    if (kCFSocketSuccess != CFSocketSetAddress(ipv4socket, (CFDataRef)address4)) {
        if (ipv4socket) CFRelease(ipv4socket);
        ipv4socket = NULL;
        return;
    }
    
    // set up the run loop sources for the sockets
    CFRunLoopRef cfrl = CFRunLoopGetCurrent();
    
    CFRunLoopSourceRef source4 = CFSocketCreateRunLoopSource(kCFAllocatorDefault, ipv4socket, 0);
    CFRunLoopAddSource(cfrl, source4, kCFRunLoopCommonModes);
    CFRelease(source4);
    
    CFRunLoopRun();
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
//            std::cerr << "ERROR accepting socket " << strerror(errno) << std::endl;
            continue;
        }
        ssize_t totalBytes = 0;
        char buff[64] = {0};
        while(shouldRun) // read from client loopb
        {
            ssize_t bytesRead = read(clientSock,&buff[totalBytes],64);
            if(bytesRead <= 0)
            {
//                std::cerr << "Error Reading From Socket " << strerror(errno) << std::endl;
                close(clientSock);
                break;
            }
            
            totalBytes += bytesRead;
//            std::cout << " Total Bytes Read " << totalBytes << " Partial Bytes Read " << totalBytes << std::endl;
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
        
//        std::cout << "Mouse X: " << data.locationX << " Mouse Y: " << data.locationY << " Event: " << data.eventType << std::endl;
        
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
        
//        std::cout << " Mouse Action Complete \n";
    }
    
    if(bytes_read > 0)
    {
//        std::cout << " LeftOver Bytes !! " << bytes_read << " data may be unaligned ! \n";
    }
}

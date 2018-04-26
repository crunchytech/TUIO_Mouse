//
//  TCPServer.cpp
//  TUIO_Mouse
//
//  Created by Jon Copeland on 4/11/18.
//  Copyright © 2018 CrunchyTech. All rights reserved.
//

#import <CoreFoundation/CoreFoundation.h>
#import <CFNetwork/CFNetwork.h>
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

static const ssize_t MouseDataSize  = sizeof(MouseData);
char* MYCFStringCopyUTF8String(CFStringRef aString);

static void TCPServerAcceptCallBack(CFSocketRef socket, CFSocketCallBackType type, CFDataRef address, const void *data, void *info) {
    
    TCPServer* server = static_cast<TCPServer*>(info);
    
    if (kCFSocketAcceptCallBack == type) {
        // for an AcceptCallBack, the data parameter is a pointer to a CFSocketNativeHandle
        CFSocketNativeHandle nativeSocketHandle = *(CFSocketNativeHandle *)data;
        uint8_t name[SOCK_MAXADDRLEN];
        socklen_t namelen = sizeof(name);
        CFDataRef peer = nil;
        if (getpeername(nativeSocketHandle, (struct sockaddr *)name, &namelen) == 0) {
            peer = CFDataCreate(kCFAllocatorDefault, name, namelen);
        }
        
        CFReadStreamRef readStream = NULL;
        CFWriteStreamRef writeStream = NULL;
        
        CFStreamCreatePairWithSocket(kCFAllocatorDefault, nativeSocketHandle, &readStream, &writeStream);
        if (readStream && writeStream) {
            CFReadStreamSetProperty(readStream, kCFStreamPropertyShouldCloseNativeSocket, kCFBooleanTrue);
            CFWriteStreamSetProperty(writeStream, kCFStreamPropertyShouldCloseNativeSocket, kCFBooleanTrue);
            
            CFReadStreamOpen(readStream);
            CFWriteStreamOpen(writeStream);
            
            uint8_t readData[MouseDataSize];
            CFIndex read = 0;
            CFIndex totalRead = 0;
            do{
                read = CFReadStreamRead(readStream,&readData[totalRead],sizeof(MouseData));
                
                if(read == -1)
                {
                    if (CFReadStreamGetStatus(readStream) == kCFStreamStatusError) {
                        CFErrorRef error = CFReadStreamCopyError (readStream);
                        if(error != NULL)
                        {
                            CFErrorDomain domain = CFErrorGetDomain(error);
                            CFIndex code = CFErrorGetCode(error);

                            std::cerr << "Error Reading From Socket Stream ! (domain: " << domain << " code: " << code << ") " << std::endl;

                            CFRelease(error);
                        }
                    }
                    break;
                }
                else {
                    totalRead += read;
                    if(totalRead >= MouseDataSize)
                    {
                        server->handleBuffer((char*)readData, totalRead);
                        totalRead = 0;
                    }
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
char * MYCFStringCopyUTF8String(CFStringRef aString) {
    if (aString == NULL) {
        return NULL;
    }
    
    CFIndex length = CFStringGetLength(aString);
    CFIndex maxSize =
    CFStringGetMaximumSizeForEncoding(length, kCFStringEncodingUTF8) + 1;
    char *buffer = (char *)malloc(maxSize);
    if (CFStringGetCString(aString, buffer, maxSize,
                           kCFStringEncodingUTF8)) {
        return buffer;
    }
    free(buffer); // If we failed
    return NULL;
}

TCPServer::TCPServer(int port)
{
    CFSocketContext socketCtxt = {0, (void*)this, NULL, NULL, NULL};
    serverSock = CFSocketCreate(kCFAllocatorDefault, PF_INET, SOCK_STREAM, IPPROTO_TCP, kCFSocketAcceptCallBack, (CFSocketCallBack)&TCPServerAcceptCallBack, &socketCtxt);
    
    if (serverSock == NULL) {
        return ;
    }
    
    int yes = 1;
    setsockopt(CFSocketGetNative(serverSock), SOL_SOCKET, SO_REUSEADDR, (void *)&yes, sizeof(yes));
    
    // set up the IPv4 endpoint; if port is 0, this will cause the kernel to choose a port for us
    struct sockaddr_in addr4;
    memset(&addr4, 0, sizeof(addr4));
    addr4.sin_len = sizeof(addr4);
    addr4.sin_family = AF_INET;
    addr4.sin_port = htons(port);
    addr4.sin_addr.s_addr = htonl(INADDR_ANY);
    CFDataRef address4 = CFDataCreate(kCFAllocatorDefault, (UInt8 *)&addr4,sizeof(addr4));
    if (kCFSocketSuccess != CFSocketSetAddress(serverSock, address4)) {
        if (serverSock) CFRelease(serverSock);
        serverSock = NULL;
        return;
    }
    
    // set up the run loop sources for the sockets
    CFRunLoopRef cfrl = CFRunLoopGetCurrent();
    
    CFRunLoopSourceRef source4 = CFSocketCreateRunLoopSource(kCFAllocatorDefault, serverSock, 0);
    CFRunLoopAddSource(cfrl, source4, kCFRunLoopCommonModes);
    CFRelease(source4);
}

TCPServer::~TCPServer()
{
    if(serverSock)CFRelease(serverSock);
}

void TCPServer::runServer()
{
    CFRunLoopRun();
}

void TCPServer::handleBuffer(char* buff,ssize_t bytes_read)
{
    int offset = 0;
    while(bytes_read >= MouseDataSize)
    {
        MouseData data;
        memset(&data,0,MouseDataSize);
        memcpy(&data,&buff[offset],MouseDataSize);
        offset += MouseDataSize;
        
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
        
        bytes_read -= MouseDataSize;
        
//        std::cout << " Mouse Action Complete \n";
    }
    
    if(bytes_read > 0)
    {
        std::cerr << " LeftOver Bytes !! " << bytes_read << " data may be unaligned ! \n";
    }
}

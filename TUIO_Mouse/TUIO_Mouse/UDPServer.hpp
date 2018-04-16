//
//  UDPServer.hpp
//  TUIO_Mouse
//
//  Created by Jon Copeland on 4/16/18.
//  Copyright © 2018 CrunchyTech. All rights reserved.
//

#ifndef UDPClient_hpp
#define UDPClient_hpp

#include <stdio.h>

class UDPServer
{
private:
    int port;
    int listenSock;
    bool shouldRun;
    
public:
    UDPServer(int port);
    ~UDPServer();
    
    void runServer();
};

#endif /* UDPClient_hpp */

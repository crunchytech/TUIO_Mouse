//
//  TCPServer.hpp
//  TUIO_Mouse
//
//  Created by Jon Copeland on 4/11/18.
//  Copyright © 2018 CrunchyTech. All rights reserved.
//

#ifndef TCPServer_hpp
#define TCPServer_hpp

#include <stdio.h>

class TCPServer
{
private:
    int port;
    int serverSock;
    int clientSock;
    bool shouldRun;
    
private:
    void handleBuffer(char* buff,ssize_t bytes_read);

public:
    TCPServer(int port);
    ~TCPServer();
    
    void runServer();
};

#endif /* TCPServer_hpp */

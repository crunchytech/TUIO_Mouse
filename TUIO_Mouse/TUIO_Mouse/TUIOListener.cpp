//
//  TUIOListener.cpp
//  TUIO_Mouse
//
//  Created by Jon Copeland on 4/11/18.
//  Copyright © 2018 CrunchyTech. All rights reserved.
//
#include "TuioClient.h"
#include "TUIOListener.hpp"
#include "MouseController.hpp"

TUIOListener::TUIOListener(int port)
{
    this->ID = -1;
    this->port = port;
    this->tClient = new TUIO::TuioClient(port);
    this->tClient->addTuioListener(this);
    this->tClient->connect();
}

TUIOListener::~TUIOListener()
{
    if(tClient)
    {
        tClient->disconnect();
        delete tClient;
        tClient = 0;
    }
}

void TUIOListener::addTuioCursor(TUIO::TuioCursor *tcur)
{
    if(ID != -1)MouseController::MouseLeftUp(tcur->getX(), tcur->getY());
    MouseController::MouseLeftDown(tcur->getX(), tcur->getY());
    ID = tcur->getCursorID();
}


void TUIOListener::updateTuioCursor(TUIO::TuioCursor *tcur)
{
    if(tcur->getCursorID() != ID)
    {
        if(ID != -1)MouseController::MouseLeftUp(tcur->getX(), tcur->getY());
        MouseController::MouseLeftDown(tcur->getX(), tcur->getY());
        ID = tcur->getCursorID();
    }
    MouseController::MouseMove(tcur->getX(), tcur->getY());
}

void TUIOListener::removeTuioCursor(TUIO::TuioCursor *tcur)
{
    if(tcur->getCursorID() == ID)
    {
        MouseController::MouseLeftUp(tcur->getX(), tcur->getY());
        ID = -1;
    }
}

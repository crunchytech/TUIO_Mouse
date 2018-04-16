//
//  TUIOListener.hpp
//  TUIO_Mouse
//
//  Created by Jon Copeland on 4/11/18.
//  Copyright © 2018 CrunchyTech. All rights reserved.
//

#ifndef TUIOListener_hpp
#define TUIOListener_hpp

#include <stdio.h>
#include "TuioListener.h"

namespace TUIO{
    class TuioClient;
};

class TUIOListener : TUIO::TuioListener
{
private:
    TUIO::TuioClient* tClient;
    int port;
    int ID;
    
public:
    TUIOListener(int port);
    ~TUIOListener();
    
    
    virtual void addTuioCursor(TUIO::TuioCursor *tcur);
    virtual void updateTuioCursor(TUIO::TuioCursor *tcur);
    virtual void removeTuioCursor(TUIO::TuioCursor *tcur);

    // Not Used
    virtual void addTuioObject(TUIO::TuioObject *tobj){};
    virtual void updateTuioObject(TUIO::TuioObject *tobj){};
    virtual void removeTuioObject(TUIO::TuioObject *tobj){};
    virtual void addTuioBlob(TUIO::TuioBlob *tblb){};
    virtual void updateTuioBlob(TUIO::TuioBlob *tblb){};
    virtual void removeTuioBlob(TUIO::TuioBlob *tblb){};
    virtual void refresh(TUIO::TuioTime ftime){};
};

#endif /* TUIOListener_hpp */

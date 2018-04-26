//
//  MouseDataType.h
//  TUIO_Mouse
//
//  Created by Jon Copeland on 4/16/18.
//  Copyright © 2018 CrunchyTech. All rights reserved.
//

#ifndef MouseDataType_h
#define MouseDataType_h

#define NX_LMOUSEDOWN       1    /* left mouse-down event */
#define NX_LMOUSEUP         2    /* left mouse-up event */
#define NX_RMOUSEDOWN       3    /* right mouse-down event */
#define NX_RMOUSEUP         4    /* right mouse-up event */
#define NX_MOUSEMOVED       5    /* mouse-moved event */
#define NX_LMOUSEDRAGGED    6    /* left mouse-dragged event */
#define NX_RMOUSEDRAGGED    7    /* right mouse-dragged event */
#define NX_MOUSEENTERED     8    /* mouse-entered event */
#define NX_MOUSEEXITED      9    /* mouse-exited event */

typedef struct MouseData
{
    double locationX;
    double locationY;
    unsigned int eventType;
    long long seq;
}MouseData;

#endif /* MouseDataType_h */

//
//  MouseController.hpp
//  TUIO_Mouse
//
//  Created by Jon Copeland on 4/11/18.
//  Copyright © 2018 CrunchyTech. All rights reserved.
//

#ifndef MouseController_hpp
#define MouseController_hpp

#include <stdio.h>

class MouseController
{
    static bool LeftMouseDown;
    static bool RightMouseDown;
public:
    
    static void MouseLeftDown(float x,float y, bool isNormed = true);
    static void MouseLeftUp(float x,float y, bool isNormed = true);
    static void MouseRightDown(float x,float y, bool isNormed = true);
    static void MouseRightUp(float x,float y, bool isNormed = true);
    static void MouseMove(float x,float y, bool isNormed = true);
    
};

#endif /* MouseController_hpp */

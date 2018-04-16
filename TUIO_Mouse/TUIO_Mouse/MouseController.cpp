//
//  MouseController.cpp
//  TUIO_Mouse
//
//  Created by Jon Copeland on 4/11/18.
//  Copyright © 2018 CrunchyTech. All rights reserved.
//

#import <CoreGraphics/CoreGraphics.h>
#include "MouseController.hpp"
#include <iostream>
bool MouseController::LeftMouseDown = false;
bool MouseController::RightMouseDown = false;

void MouseController::MouseLeftDown(float x,float y, bool isNormed)
{
    CGPoint point;
    if(isNormed)
    {
        point = CGPointMake(x * CGDisplayPixelsWide(CGMainDisplayID()), y * CGDisplayPixelsHigh(CGMainDisplayID()));
    }
    else
    {
        point = CGPointMake(x, y);
    }
    
    CGEventRef event = CGEventCreateMouseEvent(NULL, kCGEventLeftMouseDown, point, kCGMouseButtonLeft);
    CGEventPost(kCGHIDEventTap, event);
    CFRelease(event);

    LeftMouseDown = true;
    std::cout << "Left Mouse Down " << std::endl;
}

void MouseController::MouseLeftUp(float x,float y, bool isNormed)
{
    CGPoint point;
    if(isNormed)
    {
        point = CGPointMake(x * CGDisplayPixelsWide(CGMainDisplayID()), y * CGDisplayPixelsHigh(CGMainDisplayID()));
    }
    else
    {
        point = CGPointMake(x, y);
    }
    
    CGEventRef event = CGEventCreateMouseEvent(NULL, kCGEventLeftMouseUp, point, kCGMouseButtonLeft);
    CGEventPost(kCGHIDEventTap, event);
    CFRelease(event);
    
    LeftMouseDown = false;
    
    std::cout << "Left Mouse Up " << std::endl;
}

void MouseController::MouseRightDown(float x,float y, bool isNormed)
{
    CGPoint point;
    if(isNormed)
    {
        point = CGPointMake(x * CGDisplayPixelsWide(CGMainDisplayID()), y * CGDisplayPixelsHigh(CGMainDisplayID()));
    }
    else
    {
        point = CGPointMake(x, y);
    }
    
    CGEventRef event = CGEventCreateMouseEvent(NULL, kCGEventRightMouseDown, point, kCGMouseButtonRight);
    CGEventPost(kCGHIDEventTap, event);
    CFRelease(event);
    
    RightMouseDown = true;
}

void MouseController::MouseRightUp(float x,float y, bool isNormed)
{
    CGPoint point;
    if(isNormed)
    {
        point = CGPointMake(x * CGDisplayPixelsWide(CGMainDisplayID()), y * CGDisplayPixelsHigh(CGMainDisplayID()));
    }
    else
    {
        point = CGPointMake(x, y);
    }
    
    CGEventRef event = CGEventCreateMouseEvent(NULL, kCGEventRightMouseUp, point, kCGMouseButtonRight);
    CGEventPost(kCGHIDEventTap, event);
    CFRelease(event);
    
    RightMouseDown = false;
}

void MouseController::MouseMove(float x,float y, bool isNormed)
{
    CGPoint point;
    if(isNormed)
    {
        point = CGPointMake(x * CGDisplayPixelsWide(CGMainDisplayID()), y * CGDisplayPixelsHigh(CGMainDisplayID()));
    }
    else
    {
        point = CGPointMake(x, y);
    }
    
    if(LeftMouseDown)
    {
        CGEventRef event = CGEventCreateMouseEvent(NULL, kCGEventLeftMouseDragged, point, kCGMouseButtonLeft);
        CGEventPost(kCGHIDEventTap, event);
        CFRelease(event);
        
        std::cout << "Left Mouse Dragged " << std::endl;
    }
    if (RightMouseDown)
    {
        CGEventRef event = CGEventCreateMouseEvent(NULL, kCGEventRightMouseDragged, point, kCGMouseButtonLeft);
        CGEventPost(kCGHIDEventTap, event);
        CFRelease(event);
        
        std::cout << "Right Mouse Dragged " << std::endl;
    }
    if(!LeftMouseDown && !RightMouseDown)
    {
        CGEventRef event = CGEventCreateMouseEvent(NULL, kCGEventMouseMoved, point, kCGMouseButtonLeft);
        CGEventPost(kCGHIDEventTap, event);
        CFRelease(event);
        
        std::cout << "Mouse Moved " << std::endl;
    }
}

/*
	TUIO Mouse Driver - part of the reacTIVision project
	http://reactivision.sourceforge.net/

	Copyright (c) 2005-2009 Martin Kaltenbrunner <mkalten@iua.upf.edu>
	Copyright (c) 2011 Andreas Willich <sabotageandi@gmail.com>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

package com.crunchy.MouseDriver;
import java.awt.*;
import java.awt.event.*;
import java.util.Date;
import java.util.concurrent.locks.Lock;
import java.util.concurrent.locks.ReentrantLock;

import TUIO.*;

public class TuioMouse implements TuioListener, Runnable {
	
	private Robot robot = null;
	private int width = 0;
	private int height = 0;
	private long activeCuror = -1;
	private MouseState currentState = MouseState.MouseState_Up;
	private LogLevel logLevel = LogLevel.LogLevel_Error;
	private long lastEvent;
	private boolean shouldRun = true;
	private Date UpdateFPS = new Date();
	public void addTuioObject(TuioObject tobj) {}
	public void updateTuioObject(TuioObject tobj) {}	
	public void removeTuioObject(TuioObject tobj) {}
	public void refresh(TuioTime bundleTime) {}
	Lock lock;
	
	public void addTuioCursor(TuioCursor tcur) {
		if (robot==null) return;
		if(activeCuror != tcur.getCursorID()) {
			robot.mouseMove(tcur.getScreenX(width),tcur.getScreenY(height));
			robot.mouseRelease(InputEvent.BUTTON1_MASK);
		}
		
		activeCuror = tcur.getCursorID();

		robot.mouseMove(tcur.getScreenX(width),tcur.getScreenY(height));
		robot.mousePress(InputEvent.BUTTON1_MASK);
			
		synchronized(this)
		{
			lastEvent = new Date().getTime();
			currentState = MouseState.MouseState_Down;
		}
	}

	public void updateTuioCursor(TuioCursor tcur) {
		if (robot==null)return; 
		if(activeCuror == tcur.getCursorID())
		{
			robot.mouseMove(tcur.getScreenX(width),tcur.getScreenY(height));
			
		}
		else
		{
			robot.mouseMove(tcur.getScreenX(width),tcur.getScreenY(height));
			robot.mouseRelease(InputEvent.BUTTON1_MASK);
			
			activeCuror = tcur.getCursorID();

			robot.mouseMove(tcur.getScreenX(width),tcur.getScreenY(height));
			robot.mousePress(InputEvent.BUTTON1_MASK);
			
		}
		if(this.waitLock())
		{
			lastEvent = new Date().getTime();
			currentState = MouseState.MouseState_Move;
			lock.unlock();
		}
	}
	
	public void removeTuioCursor(TuioCursor tcur) {
		if (robot==null) return;
		if(activeCuror == tcur.getCursorID()) {
			robot.mouseMove(tcur.getScreenX(width),tcur.getScreenY(height));
			robot.mouseRelease(InputEvent.BUTTON1_MASK);
			activeCuror = -1;
			
			if(this.waitLock())
			{
				lastEvent = new Date().getTime();
				currentState = MouseState.MouseState_Up;
				lock.unlock();
				System.out.println("Touch Up !");
			}
		}
	}
	
	public boolean waitLock()
	{
		while(lock.tryLock() == false)
		{
			try {
				Thread.sleep(1);
			}catch(Exception except)
			{
				// dont care about exception
			}
		}
		return true;
	}
	
	public void Update()
	{
		Date current = new Date();
		{
			
			long diff = current.getTime() - UpdateFPS.getTime();
			if(diff > 0 && logLevel == LogLevel.LogLevel_Verbose)
			{
				double fps = 1.0/((double)diff / 1000.0);
				System.out.println("Update FPS is " + fps);
			}
			UpdateFPS = current;
		}

		if(this.waitLock())
		{
			if(currentState != MouseState.MouseState_Up)
			{
				if(logLevel == LogLevel.LogLevel_Verbose)System.out.println("LastEvent in Update = " + lastEvent);
				long diff = current.getTime() - lastEvent;
				if (diff > 133)
				{
					if (robot!=null) robot.mouseRelease(InputEvent.BUTTON1_MASK);
					currentState = MouseState.MouseState_Up;
					activeCuror = -1;
					
					if(logLevel == LogLevel.LogLevel_Info)System.out.println("LastEvent Timeout: diff = " + diff);
				}
			}
			lock.unlock();
		}
	}
	
	public TuioMouse() {
		try { robot = new Robot(); }
		catch (Exception e) {
			System.out.println("failed to initialize mouse robot");
			System.out.println(e.getMessage());
			e.printStackTrace();
			System.exit(0);
		}
		lock = new ReentrantLock();
		width  = (int)Toolkit.getDefaultToolkit().getScreenSize().getWidth();
		height = (int)Toolkit.getDefaultToolkit().getScreenSize().getHeight();
	}
	
	public static enum LogLevel
	{
		LogLevel_Error,
		LogLevel_Info,
		LogLevel_Verbose
	};
	
	public static LogLevel parseLogLevel(String str)
	{
		if(str == "LogLevel_Error")
			return LogLevel.LogLevel_Error;
		else if(str == "LogLevel_Info")
			return LogLevel.LogLevel_Info;
		else if (str == "LogLevel_Verbose")
			return LogLevel.LogLevel_Verbose;
		else
			throw new IllegalArgumentException("parseLogLevel error: argument must bus be one of LogLevel_Error , LogLevel_Info or LogLevel_Verbose");
	}
	
	public static enum MouseState
	{
		MouseState_Down,
		MouseState_Move,
		MouseState_Up
	};

	public static void main(String argv[]) {
	
		int port = 3333;
		LogLevel logLevel = LogLevel.LogLevel_Error;
		if (argv.length==1) {
			try { port = Integer.parseInt(argv[1]); }
			catch (Exception e) { System.out.println("usage: java TuioMouse [port] [LogLevel_Info | LogLevel_Verbose]"); }
		}
		if(argv.length == 2)
		{
			try { 
				port = Integer.parseInt(argv[1]); 
				logLevel = TuioMouse.parseLogLevel(argv[2]);
			}
			catch (Exception e) { System.out.println("usage: java TuioMouse [port] [LogLevel_Info | LogLevel_Verbose]"); }
		
		}

 		TuioMouse mouse = new TuioMouse();
		TuioClient client = new TuioClient(port);

		mouse.logLevel = logLevel;
		
		Thread t = new Thread(mouse);
		t.start();
		System.out.println("listening to TUIO messages at port "+port);
		client.addTuioListener(mouse);
		client.connect();
		
	}
	@Override
	public void run() {
		// TODO Auto-generated method stub
		while(this.shouldRun) {
			this.Update();
		}
	}
}

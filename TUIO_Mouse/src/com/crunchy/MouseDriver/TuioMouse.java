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

import TUIO.*;

public class TuioMouse implements TuioListener {
	
	private Robot robot = null;
	private int width = 0;
	private int height = 0;
	private long activeCuror = -1;
	private MouseState currentState = MouseState.MouseState_Up;
	private Date lastEvent;
	
	public void addTuioObject(TuioObject tobj) {}
	public void updateTuioObject(TuioObject tobj) {}	
	public void removeTuioObject(TuioObject tobj) {}
	public void refresh(TuioTime bundleTime) {}
	
	public void addTuioCursor(TuioCursor tcur) {
		
		if(activeCuror != tcur.getCursorID()) {
			if (robot!=null) robot.mouseMove(tcur.getScreenX(width),tcur.getScreenY(height));
			if (robot!=null) robot.mouseRelease(InputEvent.BUTTON1_MASK);
		}
		
		activeCuror = tcur.getCursorID();
		if (robot!=null) 
		{
			robot.mouseMove(tcur.getScreenX(width),tcur.getScreenY(height));
			robot.mousePress(InputEvent.BUTTON1_MASK);
			currentState = MouseState.MouseState_Down;
			lastEvent = new Date();
		}
		
	}

	public void updateTuioCursor(TuioCursor tcur) {
		if(activeCuror == tcur.getCursorID())
		{
			if (robot!=null) robot.mouseMove(tcur.getScreenX(width),tcur.getScreenY(height));
			currentState = MouseState.MouseState_Move;
		}
		else
		{
			if (robot!=null) robot.mouseMove(tcur.getScreenX(width),tcur.getScreenY(height));
			if (robot!=null) robot.mouseRelease(InputEvent.BUTTON1_MASK);
			
			activeCuror = tcur.getCursorID();
			if (robot!=null) 
			{
				robot.mouseMove(tcur.getScreenX(width),tcur.getScreenY(height));
				robot.mousePress(InputEvent.BUTTON1_MASK);
				currentState = MouseState.MouseState_Down;
			}
		}
		
		lastEvent = new Date();
	}
	
	public void removeTuioCursor(TuioCursor tcur) {
		if(activeCuror == tcur.getCursorID()) {
			if (robot!=null) robot.mouseMove(tcur.getScreenX(width),tcur.getScreenY(height));
			if (robot!=null) robot.mouseRelease(InputEvent.BUTTON1_MASK);
			currentState = MouseState.MouseState_Up;
			activeCuror = -1;
			
			lastEvent = new Date();
		}
	}
	
	public void Update()
	{
		if(currentState != MouseState.MouseState_Up)
		{
			Date currentDate = new Date();
			if (currentDate.getTime() - lastEvent.getTime() > 60)
			{
				if (robot!=null) robot.mouseRelease(InputEvent.BUTTON1_MASK);
				currentState = MouseState.MouseState_Up;
				activeCuror = -1;
			}
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
		
		width  = (int)Toolkit.getDefaultToolkit().getScreenSize().getWidth();
		height = (int)Toolkit.getDefaultToolkit().getScreenSize().getHeight();
	}
	
	public static enum MouseState
	{
		MouseState_Down,
		MouseState_Move,
		MouseState_Up
	};

	public static void main(String argv[]) {
	
		int port = 3333;
 
		if (argv.length==1) {
			try { port = Integer.parseInt(argv[1]); }
			catch (Exception e) { System.out.println("usage: java TuioMouse [port]"); }
		}

 		TuioMouse mouse = new TuioMouse();
		TuioClient client = new TuioClient(port);

		System.out.println("listening to TUIO messages at port "+port);
		client.addTuioListener(mouse);
		client.connect();
	}
}

/*
 * OSGExp - 3D Studio Max plugin for exporting OpenSceneGraph models.
 * Copyright (C) 2003  Rune Schmidt Jensen
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 *
 *	FILE:			EventAdapter.cpp
 *
 *	DESCRIPTION:	Class for adapting events.
 *					osgProducer::EventAdapter.cpp used as template
 *
 *	CREATED BY:		Rune Schmidt Jensen, rune@schmidt-jensen.com
 *
 *	HISTORY:		Created 15.12.2003
 *
 *					04.04.2006 Joran: Made changes so it compiles with the HEAD
 *					revision of OSG.
 *
 *	Copyright 2003 Rune Schmidt Jensen
 */
#include "EventAdapter.h"

#include <osg/Version>

// default to no mouse buttons being pressed.
unsigned int EventAdapter::_s_accumulatedButtonMask = 0;

int EventAdapter::_s_button = 0;
int EventAdapter::_s_modKeyMask = 0;
float EventAdapter::_s_Xmin = 0;
float EventAdapter::_s_Xmax = 1280;
float EventAdapter::_s_Ymin = 0;
float EventAdapter::_s_Ymax = 1024;
float EventAdapter::_s_mx = 0;
float EventAdapter::_s_my = 0;

EventAdapter::EventAdapter(): osgGA::GUIEventAdapter()
{
    _eventType = NONE;           // adaptor does not encapsulate any events.
    _key = -1;                   // set to 'invalid' key value.
    _button = -1;                // set to 'invalid' button value.
    _mx = -1;                    // set to 'invalid' position value.
    _my = -1;                    // set to 'invalid' position value.
    _buttonMask = 0;             // default to no mouse buttons being pressed.
    _modKeyMask = 0;             // default to no mouse buttons being pressed.
    _time = 0.0f;                // default to no time has been set.
    
    setMouseYOrientation(osgGA::GUIEventAdapter::Y_INCREASING_DOWNWARDS);

    copyStaticVariables();
}

void EventAdapter::copyStaticVariables()
{
    setButtonMask(_s_accumulatedButtonMask);
    setModKeyMask(_s_modKeyMask);
    setButton(_s_button);

#if defined(OPENSCENEGRAPH_MAJOR_VERSION) && OPENSCENEGRAPH_MAJOR_VERSION >= 2 && defined(OPENSCENEGRAPH_MINOR_VERSION) && OPENSCENEGRAPH_MINOR_VERSION >= 4
    setInputRange(_s_Xmin, _s_Ymin, _s_Xmax, _s_Ymax);
#else
    setXmin(_s_Xmin);
    setXmax(_s_Xmax);
    setYmin(_s_Ymin);
    setYmax(_s_Ymax);
#endif

    setX(_s_mx);
    setY(_s_my);
}

void EventAdapter::setWindowSize(float Xmin, float Ymin, float Xmax, float Ymax)
{
    _s_Xmin = Xmin;
    _s_Xmax = Xmax;
    _s_Ymin = Ymin;
    _s_Ymax = Ymax;
}

void EventAdapter::adaptResize(double time, float Xmin, float Ymin, float Xmax, float Ymax)
{
    setWindowSize(Xmin,Ymin,Xmax,Ymax);
    _eventType = RESIZE;
    _time = time;
    copyStaticVariables();
}

void EventAdapter::adaptButtonPress(double time,float x, float y, unsigned int button)
{
    _time = time;

    _eventType = PUSH;
    _button = button-1;

    switch(_button)
    {
        case(0): 
	    _s_accumulatedButtonMask = 
		_s_accumulatedButtonMask | LEFT_MOUSE_BUTTON; 
	    _s_button = LEFT_MOUSE_BUTTON;
	    break;
        case(1): 
	    _s_accumulatedButtonMask = 
		_s_accumulatedButtonMask | MIDDLE_MOUSE_BUTTON; 
	    _s_button = MIDDLE_MOUSE_BUTTON;
	    break;
        case(2): 
	    _s_accumulatedButtonMask = 
		_s_accumulatedButtonMask | RIGHT_MOUSE_BUTTON; 
	    _s_button = RIGHT_MOUSE_BUTTON;
	    break;
    }

    _s_mx = x;
    _s_my = y;

    copyStaticVariables();
}

void EventAdapter::adaptButtonRelease(double time,float x, float y, unsigned int button)
{
    _time = time;

    _eventType = RELEASE;
    _button = button-1;

    switch(_button)
    {
        case(0): 
	    _s_accumulatedButtonMask = 
		_s_accumulatedButtonMask & ~LEFT_MOUSE_BUTTON;
	    _s_button = LEFT_MOUSE_BUTTON;
	    break;
        case(1): 
	    _s_accumulatedButtonMask = 
		_s_accumulatedButtonMask & ~MIDDLE_MOUSE_BUTTON; 
	    _s_button = MIDDLE_MOUSE_BUTTON;
	    break;
        case(2): 
	    _s_accumulatedButtonMask = 
		_s_accumulatedButtonMask & ~RIGHT_MOUSE_BUTTON; 
	    _s_button = RIGHT_MOUSE_BUTTON;
	    break;
    }

    _s_mx = x;
    _s_my = y;

    copyStaticVariables();
}

/** method for adapting mouse motion events whilst mouse buttons are pressed.*/
void EventAdapter::adaptMouseMotion(double time, float x, float y)
{
    
    _eventType = (_s_accumulatedButtonMask) ?
                 DRAG :
                 MOVE;

    _time = time;
    _s_mx = x;
    _s_my = y;
    copyStaticVariables();

}

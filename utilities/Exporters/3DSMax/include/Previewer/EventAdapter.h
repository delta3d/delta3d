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
 *	FILE:			EventAdapter.h
 *
 *	DESCRIPTION:	Class for adapting events.
 *					osgProducer::EventAdapter.h used as template
 *
 *	CREATED BY:		Rune Schmidt Jensen, rune@schmidt-jensen.com
 *
 *	HISTORY:		Created 15.12.2003
 *
 *					04.04.2006 Joran: Made changes so it compiles with the HEAD
 *					revision of OSG.
 */
#include "Export.h"
#include <osgGA/GUIEventAdapter>

/** 
 * Class for adapting events so that they can be used as input to osgGA::CameraManipulators.
 */
class PREVIEWER_EXPORT EventAdapter : public osgGA::GUIEventAdapter
{
    public:
        EventAdapter();
        virtual ~EventAdapter() {}

        /** static method for setting window dimensions.*/
        static void setWindowSize(float Xmin, float Ymin, float Xmax, float Ymax);

        /** method for adapting resize events. */
        void adaptResize(double t, float Xmin, float Ymin, float Xmax, float Ymax);

        /** method for adapting mouse motion events whilst mouse buttons are pressed.*/
        void adaptMouseMotion(double t, float x, float y);

        void adaptButtonPress(double t,float x, float y, unsigned int button);
        
        void adaptButtonRelease(double t,float x, float y, unsigned int button);

        void copyStaticVariables();

    public:

        // used to accumulate the button mask state, it represents
        // the current button mask state, which is modified by the
        // adaptMouse() method which then copies it to value _buttonMask
        // which required the mouse buttons state at the time of the event.
        static unsigned int _s_accumulatedButtonMask;
        
		// used to store current button value
		static int		_s_button;

        // used to store window min and max values.
        static float	_s_Xmin;
        static float	_s_Xmax;
        static float	_s_Ymin;
        static float	_s_Ymax;
        static float	_s_mx;
        static float	_s_my;
        static int		_s_modKeyMask;
        
};

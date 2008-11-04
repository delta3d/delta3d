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
 *	FILE:			Previewer.h
 *
 *	DESCRIPTION:	Header file for Previewer class.
 *
 *	CREATED BY:		Rune Schmidt Jensen, rune@schmidt-jensen.com
 *
 *	HISTORY:		Created 28.10.2003
 */

#include "Export.h"
#include "windows.h"

#include <osgUtil/SceneView>
#include <osgUtil/GLObjectsVisitor>
#include <osg/Timer>
#include <osg/Framestamp>

#include <osgGA/KeySwitchMatrixManipulator>
#include <osgGA/TrackballManipulator>
#include "EventAdapter.h"

#include <osg/Switch>
#include <osg/PositionAttitudeTransform>

#define IDM_CAMERA				0x100000
#define IDM_SWITCH				0x200000
#define IDM_SWITCH_ALL_ON		0x210000
#define IDM_SWITCH_ALL_OFF		0x220000
#define IDM_ANIMATION			0x300000

class PREVIEWER_EXPORT Previewer: public osgUtil::SceneView, public osgGA::GUIActionAdapter{

public:
	Previewer();
	void run();
	void setWindowSize(int x, int y, int w, int h);
	void setGLObjectsVisitorMode(osgUtil::GLObjectsVisitor::Mode);


private:
	void initWindow();
	void close();
	void swapBuffers();

	static std::map <HWND, Previewer *>	registry;
	static LRESULT CALLBACK StaticWindowProc(HWND _hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	LRESULT CALLBACK handleEvents(HWND _hWnd, UINT message, WPARAM wParam, LPARAM lParam);

	void initTimeAndFrameCount();
	void updateTimeAndFrameCount();

	double getTime() { return _timer.delta_s(_start_tick, _timer.tick()); }

	// Inherited from osgGA::GUIActionAdapter
    virtual void requestRedraw() {}
    virtual void requestContinuousUpdate(bool) {}
	virtual void requestWarpPointer(float x,float y) {}

	// Method to find special scenegraph nodes like cameras,
	// switches, etc.  and build up menu accordingly
	void findSpecialNodes(osg::Node* node);

	void createSubMenu(const char* subMenuName, const char* itemMenuName, HMENU subMenu, int itemMenuID);

private:
	bool							_exit;
	bool							_init;

	int								_wndX;
	int								_wndY;
	int								_wndWidth;
	int								_wndHeight;

	unsigned int					_frameNumber;
    osg::Timer						_timer;
    osg::Timer_t					_start_tick;
    osg::ref_ptr<osg::FrameStamp>   _frameStamp;

	WNDCLASS						_wndClass;
	HWND							_hWnd;
	HMENU							_mainMenu;
	HMENU							_cameraMenu;
	HMENU							_switchMenu;
	HMENU							_animationMenu;
	HDC								_hDC;
	HGLRC							_glContext;

	osg::ref_ptr<osgGA::KeySwitchMatrixManipulator> _keyswitchManipulator;
	osg::ref_ptr<EventAdapter>						_ea;
	float											_mx;
	float											_my;

	std::vector<osg::PositionAttitudeTransform* >	_cameras;
	std::vector<osg::Switch* >						_switches;
	std::vector<osg::AnimationPath* >				_animations;

	bool							_updateMatrixManipulatorView;

};

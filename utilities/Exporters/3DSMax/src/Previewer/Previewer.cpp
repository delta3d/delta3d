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
 *	FILE:			Previewer.cpp
 *
 *	DESCRIPTION:	Class for previewing osg scenes.
 *
 *	CREATED BY:		Rune Schmidt Jensen, rune@schmidt-jensen.com
 *
 *	HISTORY:		Created 28.10.2003
 */
#include "Previewer.h"
#include <osg/DisplaySettings>
#include <osgDB/ReadFile>

// Static registry to hold previewer class.
std::map <HWND, Previewer *>Previewer::registry;

Previewer::Previewer(){

    _wndX = 100;
    _wndY = 100;
	_wndWidth = 800;
	_wndHeight = 600;
	_exit = false;
	_init = false;
	_updateMatrixManipulatorView = false;
	_mainMenu = _cameraMenu = _switchMenu = _animationMenu = NULL;
	setDefaults();
	setGLObjectsVisitorMode(osgUtil::GLObjectsVisitor::SWITCH_OFF_DISPLAY_LISTS);
}

void Previewer::setWindowSize(int x, int y, int w, int h){
    _wndX = x;
    _wndY = y;
	_wndWidth = w;
	_wndHeight = h;
}

void Previewer::run(){

	if(!_init){
		setViewport(0, 0, _wndWidth, _wndHeight);
		initTimeAndFrameCount();
		// Initialize motionmodel
		_keyswitchManipulator = new osgGA::KeySwitchMatrixManipulator();
		_keyswitchManipulator->addNumberedMatrixManipulator(new osgGA::TrackballManipulator());
		_keyswitchManipulator->setNode(getSceneData());
		_keyswitchManipulator->setByInverseMatrix(getViewMatrix());
		// Search for cameras, switches, ect. in scenegraph and build menu accordingly.
		findSpecialNodes(getSceneData());
		// Init the window and append the menu if any.
		initWindow();
		_init = true;
	}

	while(!_exit){

		MSG msg;
		if ( PeekMessage(&msg, _hWnd, 0, 0, PM_REMOVE) ) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		updateTimeAndFrameCount();

		update();
		cull();
		draw();

		swapBuffers();
	}

	close();
}

void Previewer::initWindow(){

	static TCHAR className[] = TEXT("Previewer");

	// Register the window.
	memset(&_wndClass, 0, sizeof(WNDCLASS));
	_wndClass.style         = CS_OWNDC;
	_wndClass.lpfnWndProc   = (WNDPROC)StaticWindowProc;
	_wndClass.cbClsExtra    = 0;
	_wndClass.cbWndExtra    = 0;
	_wndClass.hInstance     = GetModuleHandle(NULL);
	_wndClass.hIcon         = LoadIcon(NULL, IDI_APPLICATION);
	_wndClass.hCursor       = LoadCursor(NULL, IDC_ARROW);
	_wndClass.hbrBackground = NULL;
	_wndClass.lpszMenuName  = NULL;
	_wndClass.lpszClassName = className;
	RegisterClass(&_wndClass);

	// Create window.
	_hWnd = CreateWindow(	className,				// window class name
							className,				// window caption
	                        WS_OVERLAPPEDWINDOW,	// window style
							_wndX,					// initial x position
							_wndY,					// initial y postion
							_wndWidth,				// initital x size
							_wndHeight,				// initial y size
							(HWND)NULL,				// parent window handle
							_mainMenu,				// window menu handle
							GetModuleHandle(NULL),	// program instance handle
							NULL);					// creation parameters

	ShowWindow(_hWnd, SW_SHOW); 
	SetForegroundWindow(_hWnd);
	ShowCursor(TRUE);
  
	_hDC = GetDC(_hWnd);

	// Define pixel format descriptor.
	PIXELFORMATDESCRIPTOR pfd = { sizeof(PIXELFORMATDESCRIPTOR),								// size
	                              1,															// version
	                              PFD_SUPPORT_OPENGL | PFD_DRAW_TO_WINDOW | PFD_DOUBLEBUFFER,	// support double-buffering
	                              PFD_TYPE_RGBA,												// color type
	                              32,															// prefered color depth
	                              0, 0, 0, 0, 0, 0,												// color bits (ignored)
	                              0,															// no alpha buffer
	                              0,															// alpha bits (ignored)
	                              0,															// no accumulation buffer
	                              0, 0, 0, 0,													// accum bits (ignored)
	                              24,															// depth buffer
	                              0,															// no stencil buffer
	                              0,															// no auxiliary buffers
	                              PFD_MAIN_PLANE,												// main layer
	                              0,															// reserved
	                              0, 0, 0,														// no layer, visible, damage masks
	                            };
															
	// Get the and set pixelformat.
	int pixelFormat = ChoosePixelFormat(_hDC, &pfd);
	SetPixelFormat(_hDC, pixelFormat, &pfd);
  
	// Create OpenGL render context and make it the current.
	_glContext = wglCreateContext(_hDC);
	wglMakeCurrent(_hDC, _glContext);

	// Add this previewer to the registry.
	registry.insert(std::pair<HWND, Previewer *>(_hWnd, this));
}

void Previewer::swapBuffers(){
	::SwapBuffers(_hDC);
}

void Previewer::initTimeAndFrameCount(){
	// set up the time and frame counter.
	_frameNumber = 0;
    _start_tick = _timer.tick();

    if (!_frameStamp) _frameStamp = new osg::FrameStamp;
}

void Previewer::updateTimeAndFrameCount(){
    // set the frame stamp for the new frame.
    double time_since_start = _timer.delta_s(_start_tick,_timer.tick());
    _frameStamp->setFrameNumber(_frameNumber++);
    _frameStamp->setReferenceTime(time_since_start);
	setFrameStamp(_frameStamp.get());
}    

void Previewer::close(){
	wglMakeCurrent(NULL, NULL);
	wglDeleteContext(_glContext);
	::ReleaseDC(_hWnd, _hDC);
	DestroyWindow(_hWnd);
}

void Previewer::setGLObjectsVisitorMode(osgUtil::GLObjectsVisitor::Mode mode){
	if(dynamic_cast<osgUtil::GLObjectsVisitor*>(_initVisitor.get())){
		osgUtil::GLObjectsVisitor* dplv = static_cast<osgUtil::GLObjectsVisitor*>(_initVisitor.get());
		dplv->setMode(mode);
	}
}

LRESULT CALLBACK Previewer::StaticWindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam){

    std::map <HWND, Previewer *>::iterator p;
    p = registry.find( hWnd );
    if( p == registry.end() )
        return DefWindowProc( hWnd, message, wParam, lParam );
    else
        return p->second->handleEvents( hWnd, message, wParam, lParam );
}

LRESULT CALLBACK Previewer::handleEvents(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam){

	_ea = new EventAdapter();
	_ea->setWindowSize(_wndX, _wndY, _wndX + _wndWidth, _wndY + _wndHeight);

	switch(message){
		case WM_LBUTTONDOWN:
		case WM_LBUTTONUP:
		case WM_RBUTTONDOWN:
		case WM_RBUTTONUP:
		case WM_MOUSEMOVE:
			_mx = LOWORD(lParam) + _wndX ;
			_my = HIWORD(lParam) + _wndY;
			if(message == WM_LBUTTONDOWN)
				_ea->adaptButtonPress(	getTime(), _mx, _my, 1);
			else if(message == WM_LBUTTONUP)
				_ea->adaptButtonRelease(getTime(), _mx, _my, 1);
			else if(message == WM_MBUTTONDOWN)
				_ea->adaptButtonPress(	getTime(), _mx, _my, 2);
			else if(message == WM_MBUTTONUP)
				_ea->adaptButtonRelease(getTime(), _mx, _my, 2);
			else if(message == WM_RBUTTONDOWN)
				_ea->adaptButtonPress(	getTime(), _mx, _my, 3);
			else if(message == WM_RBUTTONUP)
				_ea->adaptButtonRelease(getTime(), _mx, _my, 3);
			else if(message == WM_MOUSEMOVE)
				_ea->adaptMouseMotion(	getTime(), _mx, _my);
			break;
		case WM_KEYDOWN:
			switch(wParam){
				case VK_ESCAPE:
					_exit = true;
                    break;
				case VK_SPACE:
					_keyswitchManipulator->home(*_ea, *this);
					break;
				case 0X31:	// Key 1 pressed
					_keyswitchManipulator->selectMatrixManipulator(0);
					break;
  				default :
					break;
			}
			break;
/*		case WM_COMMAND:
			if(wParam & IDM_CAMERA){
				int i = LOWORD(wParam) - IDM_CAMERA;
				_cameras[i];
			}
			if(wParam & IDM_ANIMATION){
				int i = LOWORD(wParam) - IDM_ANIMATION;
			}
			if(wParam & IDM_SWITCH){
				int i = LOWORD(wParam) - IDM_SWITCH;
			}
			break;
*/
		case WM_CLOSE:
			_exit = true;
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
	}

	if(_updateMatrixManipulatorView){
		_keyswitchManipulator->setByInverseMatrix(getViewMatrix());
		_updateMatrixManipulatorView = false;
	}
	_keyswitchManipulator->handle(*_ea, *this);
	setViewMatrix(_keyswitchManipulator->getInverseMatrix());
  
  return 0;
}

/**
 * Find all special nodes in the scenegraph and build up menu.
 */
void Previewer::findSpecialNodes(osg::Node* node){
	// Find animations
	if(dynamic_cast<osg::AnimationPathCallback*>(node->getUpdateCallback())){
		_animations.push_back(dynamic_cast<osg::AnimationPathCallback*>(node->getUpdateCallback())->getAnimationPath());
		createSubMenu("Animations", node->getName().c_str(), _animationMenu, IDM_ANIMATION + _animations.size());
	}
	// Find cameras
	if(dynamic_cast<osg::PositionAttitudeTransform*>(node)){
		_cameras.push_back(static_cast<osg::PositionAttitudeTransform*>(node));
		createSubMenu("Cameras", node->getName().c_str(), _cameraMenu, IDM_CAMERA + _cameras.size());
	}
	// Find switches
	else if(dynamic_cast<osg::Switch*>(node)){
		_switches.push_back(static_cast<osg::Switch*>(node));
		createSubMenu("Switches", node->getName().c_str(), _switchMenu, IDM_SWITCH + _switches.size());
	}
	// Traverse any group node
	else if(dynamic_cast<osg::Group*>(node)){
		osg::Group* group = static_cast<osg::Group*>(node);
		for(unsigned int i=0; i<group->getNumChildren(); i++)
			findSpecialNodes(group->getChild(i));
	}
}

void Previewer::createSubMenu(const char* subMenuName, const char* itemMenuName, HMENU subMenu, int itemMenuID){
	if(!_mainMenu){
		_mainMenu=CreateMenu();
		SetMenu(_hWnd, _mainMenu);	
	}
	if(!subMenu){
		subMenu = CreateMenu();
		AppendMenu(_mainMenu, MF_POPUP, (UINT_PTR)subMenu, subMenuName);
	}
	AppendMenu(subMenu, MF_STRING, itemMenuID, itemMenuName);
}

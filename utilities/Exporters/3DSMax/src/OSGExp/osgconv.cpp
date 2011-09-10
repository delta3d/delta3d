/*
 * OSGExp - 3D Studio Max plugin for exporting OpenSceneGraph models.
 * Copyright (C) 2003  VR-C
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
 *	FILE:			osgconv
 *
 *	DESCRIPTION:	Routines inspired on osgconv
 *
 *	CREATED BY:		Joran Jessurun, A.J.Jessurun@tue.nl
 *
 *	HISTORY:		Created 05.10.2005
 */

#include "osgconv.h"

GraphicsContext::GraphicsContext()
{
    WNDCLASS wndClass;

    memset(&wndClass, 0, sizeof(WNDCLASS));
    wndClass.style         = CS_OWNDC;
	wndClass.lpfnWndProc   = (WNDPROC)StaticWindowProc;
	wndClass.cbClsExtra    = 0;
	wndClass.cbWndExtra    = 0;
	wndClass.hInstance     = GetModuleHandle(NULL);
	wndClass.hIcon         = NULL;
	wndClass.hCursor       = NULL;
	wndClass.hbrBackground = NULL;
	wndClass.lpszMenuName  = NULL;
	wndClass.lpszClassName = "GraphicsContext";
	RegisterClass(&wndClass);

    hwnd=CreateWindow(
        "GraphicsContext",
        "GraphicsContext",
        WS_POPUP,
        0,0,1,1,
        0,0,
        GetModuleHandle(NULL),
        NULL);

    hdc=GetDC(hwnd);

	// Define pixel format descriptor.
	PIXELFORMATDESCRIPTOR pfd = { 
        sizeof(PIXELFORMATDESCRIPTOR),				// size
	    1,											// version
	    PFD_SUPPORT_OPENGL | PFD_DRAW_TO_WINDOW,	// support double-buffering
	    PFD_TYPE_RGBA,								// color type
	    32,											// prefered color depth
	    0, 0, 0, 0, 0, 0,							// color bits (ignored)
	    0,											// no alpha buffer
	    0,											// alpha bits (ignored)
	    0,											// no accumulation buffer
	    0, 0, 0, 0,									// accum bits (ignored)
	    24,											// depth buffer
	    0,											// no stencil buffer
	    0,											// no auxiliary buffers
	    PFD_MAIN_PLANE,								// main layer
	    0,											// reserved
	    0, 0, 0,									// no layer, visible, damage masks
    };

  	int pixelFormat = ChoosePixelFormat(hdc,&pfd);
	SetPixelFormat(hdc,pixelFormat,&pfd);

    hglrc=wglCreateContext(hdc);
    hglrcOld=wglGetCurrentContext();
    hdcOld=wglGetCurrentDC();
    wglMakeCurrent(hdc,hglrc);
}

GraphicsContext::~GraphicsContext()
{
    wglMakeCurrent(hdcOld,hglrcOld);
    wglDeleteContext(hglrc);
    ReleaseDC(hwnd,hdc);
    DestroyWindow(hwnd);
    UnregisterClass("GraphicsContext",GetModuleHandle(NULL));
}

LRESULT CALLBACK GraphicsContext::StaticWindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    return DefWindowProc( hWnd, message, wParam, lParam );
}

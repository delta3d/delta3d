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
 *	FILE:			Toolbar.h
 *
 *	DESCRIPTION:	Header file for the utility plugin adding a toolbar
 *					to MAX.
 *
 *	CREATED BY:		Rune Schmidt Jensen, rune@schmidt-jensen.com
 *
 *	HISTORY:		Created 06.03.2003
 */


#include "Max.h"
#include "resource.h"
#include "utilapi.h"

TCHAR *GetString(int id);
extern ClassDesc* GetOSGToolbarDesc();
#define OSGTOOLBAR_CLASS_ID	Class_ID(0x1a368bc7, 0x773d0e2b)

/**
 * Definitions for the toolbar items themselves
 */
#define TBITEM(type, pIcon, cmd) \
		ToolButtonItem(type,pIcon,GetCUIFrameMgr()->GetImageSize(),GetCUIFrameMgr()->GetImageSize(),GetCUIFrameMgr()->GetButtonWidth(),GetCUIFrameMgr()->GetButtonHeight(),cmd,0)

// Here a large value is used so it won't conflict with the IDs used
// by MAX.  If the ProcessMessage() method of the handler returns FALSE
// MAX will use the default CUI toolbar processing and this could invoke
// a MAX command unintentionally.  Note that this is only the case if
// it returns FALSE.
#define ID_TB_0 10000
#define ID_TB_1 10001
#define ID_TB_2 10002
#define ID_TB_3 10003
#define ID_TB_4 10004
#define ID_TB_5 10005

/**
 * This is the main plug-in class.
 */
class OSGToolbar : public UtilityObj {
  public:
    ICUIFrame* GetICUIFrame();

	OSGToolbar() { _iu = NULL; _ip = NULL; hToolbarWnd = NULL; };
	~OSGToolbar() { };

	void BeginEditParams(Interface* ip,IUtil* iu);
	void EndEditParams(Interface* ip,IUtil* iu);
	void DeleteThis() {}
	Interface* getInterface(){return _ip;};

private:
	IUtil* _iu;
	Interface* _ip;

	HWND hToolbarWnd; // CUIFrameWindow handle
	
};

static OSGToolbar theOSGToolbar;

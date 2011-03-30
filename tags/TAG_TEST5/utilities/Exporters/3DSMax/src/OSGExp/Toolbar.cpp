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
 *	FILE:			Toolbar.cpp
 *
 *	DESCRIPTION:	Utility plugin to the OSG exporter, which add a toolbar
 *					to 3ds max and can do the OSG export when buttons are pressed.
 *
 *	CREATED BY:		Rune Schmidt Jensen, rune@schmidt-jensen.com
 *
 *	HISTORY:		Created 06.03.2003
 */

#include "OSGExp.h"

#include "Toolbar.h"
#include "MaxIcon.h"



/**
 * Class Descriptor for the OSGToolbar plugin.
 */
class OSGToolbarClassDesc:public ClassDesc {
	public:
	int 			IsPublic() {return 1;}
	void *			Create(BOOL loading = FALSE) {return &theOSGToolbar;}
	const TCHAR *	ClassName() {return _T("OSG Export Toolbar");}
	SClass_ID		SuperClassID() {return UTILITY_CLASS_ID;}
	Class_ID		ClassID() {return OSGTOOLBAR_CLASS_ID;}
	const TCHAR* 	Category() {return _T("Utility Toolbar");}
};
static OSGToolbarClassDesc OSGToolbarDesc;
ClassDesc* GetOSGToolbarDesc() {return &OSGToolbarDesc;}


/**
 * This class is the custom message handler installed by the method
 * ICUIFrame::InstallMsgHandler(tbMsgHandler).  It has one method,
 * ProcessMessage() which, uh, processes the messages.
 */
class TBMsgHandler : public CUIFrameMsgHandler {
	OSGToolbar *tb;
  public:
	TBMsgHandler(OSGToolbar *tb)	{ this->tb = tb; }
	int ProcessMessage(UINT message, WPARAM wParam, LPARAM lParam);
};	
static TBMsgHandler *tbMsgHandler;

/**
 * About dialog callback function.
 */
INT_PTR CALLBACK AboutDialogProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam){
	switch(message) {
		case WM_CLOSE:
			EndDialog(hWnd, 0);
			return TRUE;
		case WM_COMMAND:
			switch (LOWORD(wParam)) {
				case IDOK:
					EndDialog(hWnd, 0);
					return TRUE;
			}
	}
	return FALSE;
}

int TBMsgHandler::ProcessMessage(UINT message, WPARAM wParam, LPARAM lParam) {
	BOOL suppressPrompts = TRUE;
	DWORD MAXOptions = 0;
	Interface *ip = tb->getInterface();
	OSGExp* osgExp = NULL;
	Options* options = NULL;
	TSTR cfgfilename;
	TSTR filename;

	switch(message) {

		case WM_NOTIFY: 
		// This is where we provide the tooltip text for the
		// toolbar buttons... 
			if(((LPNMHDR)lParam)->code == TTN_NEEDTEXT) {
				LPTOOLTIPTEXT lpttt;
				lpttt = (LPTOOLTIPTEXT)lParam;
				switch (lpttt->hdr.idFrom) {
					case ID_TB_0:
						lpttt->lpszText = _T("Export and preview");
						break;
					case ID_TB_1:
						lpttt->lpszText = _T("Export selected and preview");
						break;
					case ID_TB_2:
						lpttt->lpszText = _T("Export and save as OSG");
						break;
					case ID_TB_3:
						lpttt->lpszText = _T("Export and save as IVE");
						break;
					case ID_TB_4:
						lpttt->lpszText = _T("Options dialog");
						break;
					case ID_TB_5:
						lpttt->lpszText = _T("About OSGExp");
						break;
				}
			}
			break;

	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case ID_TB_0:
		case ID_TB_1:
		case ID_TB_2:
		case ID_TB_3:
			// Make filename to config file.
			cfgfilename += ip->GetDir(APP_PLUGCFG_DIR);
			cfgfilename += _T("\\OSGEXP.CFG");
			// Make filename to out put file.
			filename += ip->GetDir(APP_EXPORT_DIR);

			// If button 2 save as OSG
			if(LOWORD(wParam)==ID_TB_2){
				filename += _T("\\exported.osg");
			}
			// Else just save as IVE.
			else{
				filename += _T("\\exported.ive");
			}

			// Create an instance of the exporter.
			osgExp = new OSGExp();
			// Get the options for the exporter.
			options = osgExp->getOptions();

			// Load options from config file.
			options->load(cfgfilename);
			// Assure always to write textures to disk if saving as OSG
			if(LOWORD(wParam)==ID_TB_2){
				options->setWriteTexture(TRUE);
				options->setIncludeImageDataInIveFile(FALSE);
			}
			else{
				options->setWriteTexture(FALSE);
				options->setIncludeImageDataInIveFile(TRUE);
			}

			// If button 0 or 1 then set options to quick view but not save file.
			if(LOWORD(wParam) == ID_TB_0 || LOWORD(wParam) == ID_TB_1){
				options->setSaveFile(FALSE);
				options->setQuickView(TRUE);
			}
			// If button 2 or 3 then set options to save file but not quick view.
			else{
				options->setSaveFile(TRUE);
				options->setQuickView(FALSE);
			}
			// Write options to config file before exporting.
			options->write(cfgfilename);

			// If button 1 then export selected.
			if(LOWORD(wParam) == ID_TB_1 )
				MAXOptions = SCENE_EXPORT_SELECTED;				

			// Do the export.
			osgExp->DoExport(filename, NULL, ip, suppressPrompts, MAXOptions);

			// Delete the instance of the exporter.
			delete osgExp;
			return TRUE;
		case ID_TB_4:
			// Make filename to config file.
			cfgfilename += ip->GetDir(APP_PLUGCFG_DIR);
			cfgfilename += _T("\\OSGEXP.CFG");
			// Create an instance of the options class.
			options = new Options();
			// Load options from config file.
			options->load(cfgfilename);
			// Show options dialog. Write config file if ok.
			if(DialogBoxParam(hInstance, MAKEINTRESOURCE(IDD_EXPORTBOX), 
				           GetActiveWindow(), OptionsDlgProc, (LPARAM)options))
				options->write(cfgfilename);
			delete options;
			return TRUE;
		case ID_TB_5:
			// Show about dialog.
			DialogBox(hInstance, MAKEINTRESOURCE(IDD_ABOUTBOX), 
				      GetActiveWindow(), AboutDialogProc);
			return TRUE;
		default: // ID not recognized -- use default CUI processing
			return FALSE;
		}
	}
	return FALSE;
}



/**
 * The Begin/EndEditParams calls, which create and destroy the toolbar
 */
void OSGToolbar::BeginEditParams(Interface *ip,IUtil *iu) {
	_iu = iu;
	_ip = ip;

    ICUIFrame* iFrame = GetICUIFrame();
	if (iFrame) {
		// We have the toolbar already, just show it...(EndEditParams() hid it)
		iFrame->Hide(FALSE);
		// If the frame is floating there is no reason to 
		// recalc the layout, but if it's docked we need to do so
		if (!iFrame->IsFloating()) {
			GetCUIFrameMgr()->RecalcLayout(TRUE);
		}
	}
	else {
		// Create a simple toolbar
		// -- First create the frame
		HWND hParent = ip->GetMAXHWnd();
		HWND hWnd = CreateCUIFrameWindow(hParent, _T("OSG Export Toolbar"), 0, 0, 100, 100);
		// iFrame->SetName();
		ICUIFrame* iFrame = ::GetICUIFrame(hWnd);
		iFrame->SetContentType(CUI_TOOLBAR);
		iFrame->SetPosType(CUI_HORIZ_DOCK | CUI_VERT_DOCK | CUI_FLOATABLE | CUI_SM_HANDLES);

		// -- Now create the toolbar window
		HWND hToolbar = CreateWindow(
				CUSTTOOLBARWINDOWCLASS,
				NULL,
				WS_CHILD | WS_VISIBLE,
				0, 0, 250, 100,
				hWnd,
				NULL,
				hInstance,
				NULL);
        hToolbarWnd = hToolbar;

		// -- Now link the toolbar to the CUI frame
		ICustToolbar *iToolBar = GetICustToolbar(hToolbar);
		iToolBar->LinkToCUIFrame(hWnd, NULL);
		iToolBar->SetBottomBorder(FALSE);
		iToolBar->SetTopBorder(FALSE);

		// Install the message handler to process the controls we'll add...
		tbMsgHandler = new TBMsgHandler(this);
		iFrame->InstallMsgHandler(tbMsgHandler);

        // Get the 0th icon from the CUITest icon file.
        // Note that the index is 1-based.
        MaxBmpFileIcon* pIcon = new MaxBmpFileIcon(_T("osgexp_simple"), 1);
		// Add a push button for export and view.
		iToolBar->AddTool(TBITEM(CTB_PUSHBUTTON, pIcon, ID_TB_0));
		pIcon = new MaxBmpFileIcon(_T("osgexp_simple"), 2);
		// Add a push button for export selected and view.
		iToolBar->AddTool(TBITEM(CTB_PUSHBUTTON, pIcon, ID_TB_1));

		// Add a separator
		iToolBar->AddTool(ToolSeparatorItem(8)); 

		// Add a push button for export and save.
		pIcon = new MaxBmpFileIcon(_T("osgexp_simple"), 3);
		iToolBar->AddTool(TBITEM(CTB_PUSHBUTTON, pIcon, ID_TB_2));
		// Add a push button for export selected and save.
		pIcon = new MaxBmpFileIcon(_T("osgexp_simple"), 4);
		iToolBar->AddTool(TBITEM(CTB_PUSHBUTTON, pIcon, ID_TB_3));

		// Add a separator
		iToolBar->AddTool(ToolSeparatorItem(8)); 

		// Add a push button for export dialog.
		pIcon = new MaxBmpFileIcon(_T("osgexp_simple"), 5);
		iToolBar->AddTool(TBITEM(CTB_PUSHBUTTON, pIcon, ID_TB_4));

		// Add a push button for about dialog.
		pIcon = new MaxBmpFileIcon(_T("osgexp_simple"), 6);
		iToolBar->AddTool(TBITEM(CTB_PUSHBUTTON, pIcon, ID_TB_5));

		// -- Set the initial floating position
		SIZE sz; RECT rect;
		iToolBar->GetFloatingCUIFrameSize(&sz);
		rect.top = 100; rect.left = 100;
		rect.right = rect.left+sz.cx; rect.bottom = rect.top+sz.cy;
		GetCUIFrameMgr()->FloatCUIWindow(hWnd, &rect);
		MoveWindow(hWnd, rect.left, rect.right, sz.cx, sz.cy, TRUE);

		// We are done, release the toolbar and frame handles
		ReleaseICustToolbar(iToolBar);
        iToolBar = NULL;
		ReleaseICUIFrame(iFrame);
	}
}
	
void OSGToolbar::EndEditParams(Interface *ip,IUtil *iu) {
/*	this->iu = NULL;
	this->ip = NULL;
	// Hide the toolbar so the user can't execute anything 
	// when we're not active.
    ICUIFrame* iFrame = GetICUIFrame();
    if (iFrame) {
        iFrame->Hide(TRUE);
        // If the toolbar was docked we need to recalc the layout
        // so the toolbar will be removed.
        if (!iFrame->IsFloating()) {
            GetCUIFrameMgr()->RecalcLayout(TRUE);
        }
    }
	ReleaseICUIFrame(iFrame);
	*/
}

ICUIFrame* OSGToolbar::GetICUIFrame(){
    HWND hFrameWnd = GetParent(hToolbarWnd);
    return  ::GetICUIFrame(hFrameWnd);
}

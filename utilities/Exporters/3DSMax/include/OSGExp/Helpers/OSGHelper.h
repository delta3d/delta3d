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
 *	FILE:			OSGHelper.h
 *
 *	DESCRIPTION:	Includes for Plugins
 *
 *  CREATED BY:		Rune Schmidt jensen, rune@schmidt-jensen.com
 *
 *	HISTORY:		Created 03.02.2003
 *
 *	Copyright 2003 VR-C
 */

#ifndef __OSGHELPER__H
#define __OSGHELPER__H

#include "Max.h"
#include "resource.h"
#include "istdplug.h"
#include "iparamb2.h"
#include "iparamm2.h"

#include "helpers.h" 

extern TCHAR *GetString(int id);
extern HINSTANCE hInstance;




#define PBLOCK_REF	0

class NodeEventCallback;

class OSGHelper : public HelperObject {
	public:
      typedef HelperObject BaseClass;

      INode* mNode;

		IObjParam *ip;

		// Parameter block for holding parameters.
		IParamBlock2	*pblock2;	//ref 0

      int mLinksCount;

		// The name of the helper object.
		TSTR name;

		// From BaseObject
		int HitTest(TimeValue t, INode* inode, int type, int crossing, int flags, IPoint2 *p, ViewExp *vpt);
		int Display(TimeValue t, INode* inode, ViewExp *vpt, int flags);
		CreateMouseCallBack* GetCreateMouseCallBack();
		void BeginEditParams( IObjParam *ip, ULONG flags,Animatable *prev);
		void EndEditParams( IObjParam *ip, ULONG flags,Animatable *next);
		TCHAR *GetObjectName() {return name;}

      ULONG GetCurrentNodeHandle(Interface* ip);


		// From Object
		ObjectState Eval(TimeValue time);
		void InitNodeName(TSTR& s) { s = name;}
		ObjectHandle ApplyTransform(Matrix3& matrix) {return this;}
		int CanConvertToType(Class_ID obtype) {return FALSE;}
		Object* ConvertToType(TimeValue t, Class_ID obtype) {assert(0);return NULL;}		
		void GetWorldBoundBox(TimeValue t, INode *mat, ViewExp *vpt, Box3& box );
		void GetLocalBoundBox(TimeValue t, INode *mat, ViewExp *vpt, Box3& box );
		int DoOwnSelectHilite()	{ return 1; }
		Interval ObjectValidity(TimeValue t);
		int UsesWireColor() {return TRUE;}

		// Animatable methods
		Class_ID ClassID() {return OSGHELPER_CLASS_ID;}		
		SClass_ID SuperClassID() { return HELPER_CLASS_ID; }
		void GetClassName(TSTR& s) {s = name;}
		void DeleteThis() { delete this; }
		int IsKeyable(){ return 0;}
		int NumSubs() { return 1; }  
		Animatable* SubAnim(int i) { return pblock2; }
		TSTR SubAnimName(int i) { return TSTR(_T("Parameters"));}
		IParamArray*  GetParamBlock(){	return (IParamArray*)pblock2;}
		int GetParamBlockIndex(int id){	if (pblock2 && id>=0 && id<pblock2->NumParams()) return id;
										else return -1;	}
		int	NumParamBlocks() { return 1; }// return number of ParamBlocks in this instance
		IParamBlock2* GetParamBlock(int i) { return pblock2; }
		IParamBlock2* GetParamBlockByID(BlockID id) { return (pblock2->ID() == id) ? pblock2 : NULL; } // return id'd ParamBlock

		// From ref
#if MAX_RELEASE >= 9000
		RefTargetHandle Clone(RemapDir& remap = DefaultRemapDir());
#else
		RefTargetHandle Clone(RemapDir& remap = NoRemap());
#endif
/*		IOResult Load(ILoad *iload);
		IOResult Save(ISave *isave);
*/		int NumRefs() {return 1;}
		RefTargetHandle GetReference(int i) {return pblock2;}
		void SetReference(int i, RefTargetHandle rtarg) {pblock2=(IParamBlock2*)rtarg;}
		RefResult NotifyRefChanged( Interval changeInt, RefTargetHandle hTarget, 
         PartID& partID, RefMessage message );

		//Constructor/Destructor
		OSGHelper(TSTR name);
		~OSGHelper();		

		virtual int DrawAndHit(TimeValue t, INode *inode, ViewExp *vpt);

      void AttachReferencedObjects();
      void DetachUnreferencedObjects();
      bool IsObjectReferenced(const INode& node, IParamBlock2& pblock, ParamID nodesParamID);
      void UpdateReferencesByLinks();

      void ClearNodeWidgets(IParamBlock2& pblock2, ParamID paramID, int newListSize);

      void SetNodeParameter(IParamBlock2& pblock2, ParamID paramID,
         int nodeIndex, INode* node);
      INode* GetNodeParameter(IParamBlock2& pblock2, ParamID paramID,
         int nodeIndex);

      void MoveNodeItem(HWND hWnd, int offset);
      INode* GetNodeFromSelectedItem(HWND hWnd, IParamBlock2& pblock2, ParamID paramID);
      void SetSelectedNodeItem(HWND hWnd, int index);

      bool IsHierarchalType();

      static bool NodeHasChild(INode& potentialParent, INode& potentialChild);

      static bool GetParamIDForNodeList(ParamBlockDesc2& desc, ParamID& outParamID);
};

class OSGHelperClassDesc:public ClassDesc2 {
	public:
	int 			IsPublic() { return TRUE; }
	void *			Create(BOOL loading = FALSE) { return new OSGHelper(_T("Helper")); }
	const TCHAR *	ClassName() { return _T("Helper"); }
	SClass_ID		SuperClassID() { return HELPER_CLASS_ID; }
	Class_ID		ClassID() { return OSGHELPER_CLASS_ID; }
	const TCHAR* 	Category() { return GetString(IDS_CATEGORY); }

	const TCHAR*	InternalName() { return _T("OSGHelper"); }	// returns fixed parsable name (scripter-visible name)
	HINSTANCE		HInstance() { return hInstance; }				// returns owning module handle

};



////////////////////////////////////////////////////////////////////////////////
// MULTIPLE SELECT DIALOG CALLBACK
////////////////////////////////////////////////////////////////////////////////
class MultipleSelectDlg : public HitByNameDlgCallback
{
	private:
		IParamMap2* map;
      OSGHelper* mCurrentHelper;

	public:
		MultipleSelectDlg(IParamMap2* map) { this->map=map;} 

      void SetCurrentHelper(OSGHelper* helper) {mCurrentHelper = helper;}
		TCHAR		*dialogTitle()				{ return _T("Multiple Selection"); }
		TCHAR		*buttonText() 				{ return _T("Ok"); }
		BOOL		singleSelect()				{ return FALSE; }
		BOOL		useFilter()					{ return TRUE; }
		BOOL		useProc()					{ return TRUE; }

      virtual int filter(INode *node);

		virtual void proc(INodeTab &nodeTab);
};



////////////////////////////////////////////////////////////////////////////////
// NODE PICK FILTER CALLBACK
////////////////////////////////////////////////////////////////////////////////
class PickNodeFilterCallback : public PickNodeCallback
{
   public:
      PickNodeFilterCallback();

      void SetCurrentNode(INode* node);

      virtual BOOL Filter(INode* node);

   private:
      INode* mCurrentNode;
};



////////////////////////////////////////////////////////////////////////////////
// PICK OBJECT CALLBACK
////////////////////////////////////////////////////////////////////////////////
class PickObjectCallback : public PickModeCallback
{
   public:
      PickObjectCallback();
      virtual ~PickObjectCallback();

      void SetCurrentHelper(OSGHelper* helper);
      OSGHelper* GetCurrentHelper();
      const OSGHelper* GetCurrentHelper() const;

      INode* GetCurrentNode();

      void SetWindowHandle(HWND hWnd);

      void SetButtonHandle(int buttonHandle);

      bool IsEnabled() const;

      virtual void EnterMode(IObjParam* ip);
      virtual void ExitMode(IObjParam* ip);
      virtual BOOL Pick(IObjParam* ip, ViewExp* viewport);
      virtual BOOL HitTest(IObjParam* ip, HWND hWnd, ViewExp* viewport, IPoint2 point, int flags);

   private:
      bool mEnabled;
      OSGHelper* mCurrentHelper;
      PickNodeFilterCallback* mPickNodeCallback;
      HWND mWinHandle;
      int mButtonHandle;
};



////////////////////////////////////////////////////////////////////////////////
// HELPER DIALOG PROCEDURE
////////////////////////////////////////////////////////////////////////////////
// This is used to provide special processing of controls in the rollup page.
class HelperDlgProc : public ParamMap2UserDlgProc 
{
      OSGHelper* mCurrentHelper;
      PickObjectCallback* mPickObjectCallback;

	public:
      HelperDlgProc();
      virtual ~HelperDlgProc();

		BOOL DlgProc(TimeValue t,IParamMap2* map,HWND hWnd,UINT msg,WPARAM wParam,LPARAM lParam);
      void SetCurrentOSGHelper(OSGHelper* helper) {mCurrentHelper = helper;}
      void DeleteThis() {}
};

static HelperDlgProc theHelperProc;

#endif // __OSGHELPER__H

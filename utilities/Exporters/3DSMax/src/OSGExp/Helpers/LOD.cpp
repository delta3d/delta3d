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
 *	FILE:			LOD.cpp
 *
 *	DESCRIPTION:	Class for the LOD helper objects. Through
 *					this helper object the user will be able to make
 *					use of the OSG::LOD class.
 *
 *	CREATED BY:		Rune Schmidt Jensen, rune@schmidt-jensen.com
 *
 *	HISTORY:		Created 11.02.2003
 *
 *                  28.06.2006 Joakim Simonsson: It is possible to 
 *                  select all kind of nodes for LODHelpers
 */
#include "OSGHelper.h"

extern ParamBlockDesc2 lod_param_blk;

class LOD:public OSGHelper{
	public:
		LOD(TSTR name) : OSGHelper(name){	pblock2 = CreateParameterBlock2(&lod_param_blk,0);};
		void BeginEditParams(IObjParam *ip, ULONG flags,Animatable *prev);
		void EndEditParams(IObjParam *ip, ULONG flags,Animatable *next);
		Class_ID ClassID() {return LOD_CLASS_ID;}
		RefTargetHandle Clone(RemapDir& remap);
		int DrawAndHit(TimeValue t, INode *inode, ViewExp *vpt);
		int HitTest(TimeValue t, INode *inode, int type, int crossing, int flags, IPoint2 *p, ViewExp *vpt);
};

class LODClassDesc:public OSGHelperClassDesc {
	public:
	void *			Create(BOOL loading = FALSE) { return new LOD(_T("OSG_LOD")); }
	const TCHAR *	ClassName() { return _T("LOD"); }
	Class_ID		ClassID() { return LOD_CLASS_ID; }
	const TCHAR*	InternalName() { return _T("OSGLOD"); }	// returns fixed parsable name (scripter-visible name)
};


static LODClassDesc LODDesc;
ClassDesc2* GetLODDesc() { return &LODDesc; }


enum { 
	lod_params
};


static ParamBlockDesc2 lod_param_blk ( lod_params, _T("lod_params"),  0, &LODDesc, 
	P_AUTO_CONSTRUCT + P_AUTO_UI, PBLOCK_REF , 
	// rollout
   IDD_LOD, IDS_LOD, 0, 0, &theHelperProc,		
      lod_node_array,		_T("NODES"),		TYPE_INODE_TAB,	0,	P_AUTO_UI|P_VARIABLE_SIZE,	IDS_DOF_NODES,
      p_ui,			TYPE_NODELISTBOX, IDC_LIST,0,0,IDC_SWITCH_REMNODE,
      p_prompt,		IDS_PICK_GEOM_OBJECT,
      end,
	// center x value
	lod_center_x, 		_T("CenterX"),	TYPE_FLOAT, 	P_ANIMATABLE, 	IDS_LOD_CENTER, 
		p_default, 		0.0f,
		p_range, 		-1000000.0f, 1000000.0f, 
		p_ui, 			TYPE_SPINNER,	EDITTYPE_FLOAT, IDC_LOD_CENTER_X_EDIT,	IDC_LOD_CENTER_X_SPIN, 0.01f, 
		end,
	// center y value
	lod_center_y, 		_T("CenterY"),	TYPE_FLOAT, 	P_ANIMATABLE, 	IDS_LOD_CENTER, 
		p_default, 		0.0f, 
		p_range, 		-1000000.0f, 1000000.0f, 
		p_ui, 			TYPE_SPINNER,	EDITTYPE_FLOAT, IDC_LOD_CENTER_Y_EDIT,	IDC_LOD_CENTER_Y_SPIN, 0.01f, 
		end,
	// center z value
	lod_center_z, 		_T("CenterZ"),	TYPE_FLOAT, 	P_ANIMATABLE, 	IDS_LOD_CENTER, 
		p_default, 		0.0f, 
		p_range, 		-1000000.0f, 1000000.0f, 
		p_ui, 			TYPE_SPINNER,	EDITTYPE_FLOAT, IDC_LOD_CENTER_Z_EDIT,	IDC_LOD_CENTER_Z_SPIN, 0.01f, 
		end,
	// min values in a table.
	lod_min,			_T("LODMinValues"),	TYPE_FLOAT_TAB, 	NUM_LOD_OBJECTS,	P_ANIMATABLE, 	IDS_LOD_MIN, 
		p_default, 		0.0f,	
		p_range, 		-1000000.0f, 1000000.0f, 
		p_ui, 			TYPE_SPINNER, EDITTYPE_FLOAT, 
						IDC_LOD_MIN_EDIT1,IDC_LOD_MIN_SPIN1,
						IDC_LOD_MIN_EDIT2,IDC_LOD_MIN_SPIN2,
						IDC_LOD_MIN_EDIT3,IDC_LOD_MIN_SPIN3,
						IDC_LOD_MIN_EDIT4,IDC_LOD_MIN_SPIN4,
						IDC_LOD_MIN_EDIT5,IDC_LOD_MIN_SPIN5,
						0.1f, 
		end,
	// max values in a table.
	lod_max,			_T("LODMaxValues"),	TYPE_FLOAT_TAB, 	NUM_LOD_OBJECTS,	P_ANIMATABLE, 	IDS_LOD_MAX, 
		p_default, 		50.0f,	
		p_range, 		-1000000.0f, 1000000.0f, 
		p_ui, 			TYPE_SPINNER, EDITTYPE_FLOAT, 
						IDC_LOD_MAX_EDIT1,IDC_LOD_MAX_SPIN1,
						IDC_LOD_MAX_EDIT2,IDC_LOD_MAX_SPIN2,
						IDC_LOD_MAX_EDIT3,IDC_LOD_MAX_SPIN3,
						IDC_LOD_MAX_EDIT4,IDC_LOD_MAX_SPIN4,
						IDC_LOD_MAX_EDIT5,IDC_LOD_MAX_SPIN5,
						0.1f, 
		end,
	// use paged lod
	lod_usepaged,	_T("USEPAGEDLOD"),	TYPE_BOOL,	0,	IDS_LOD_USEPAGED, 	
		p_default, 		FALSE,
		p_ui, 			TYPE_SINGLECHEKBOX, IDC_LOD_USEPAGED,
		end,
	// paged filename
	lod_paged_filename,	 	_T("LODPAGEDFILENAME"),	TYPE_STRING, 	0,	IDS_PAGEDLOD_FILENAME,
		p_default, 		"PagedLOD", 
		p_ui, 			TYPE_EDITBOX,	IDC_PAGEDLOD_FILENAME, 
		end,
	// lod geometry nodes. 
	// DAMN THE MAX PARAMBLOCK TEAM!!! 
	// Not able to do a TYPE_INODE_TAB with TYPE_PICKNODEBUTTON,
	// must declare NUM_LOD_OBJECTS (5 params) of TYPE_INODE, or make
	// an NODE_LISTBOX and implement all logic myself.
	/*lod_node+0,			_T("NODES"),	TYPE_INODE,	0,	IDS_LOD_NODES,
		p_ui,			TYPE_PICKNODEBUTTON,	IDC_LOD_PICKNODE1,
		p_prompt,		IDS_PICK_GEOM_OBJECT,
		end,
	lod_node+1,			_T("NODES"),	TYPE_INODE,	0,	IDS_LOD_NODES,
		p_ui,			TYPE_PICKNODEBUTTON,	IDC_LOD_PICKNODE2,
		p_prompt,		IDS_PICK_GEOM_OBJECT,
		end,
	lod_node+2,			_T("NODES"),	TYPE_INODE,	0,	IDS_LOD_NODES,
		p_ui,			TYPE_PICKNODEBUTTON,	IDC_LOD_PICKNODE3,
		p_prompt,		IDS_PICK_GEOM_OBJECT,
		end,
	lod_node+3,			_T("NODES"),	TYPE_INODE,	0,	IDS_LOD_NODES,
		p_ui,			TYPE_PICKNODEBUTTON,	IDC_LOD_PICKNODE4,
		p_prompt,		IDS_PICK_GEOM_OBJECT,		
		end,
	lod_node+4,			_T("NODES"),	TYPE_INODE,	0,	IDS_LOD_NODES,
		p_ui,			TYPE_PICKNODEBUTTON,	IDC_LOD_PICKNODE5,
		p_prompt,		IDS_PICK_GEOM_OBJECT,
		end,*/
	end
	);

void LOD::BeginEditParams(IObjParam *ip, ULONG flags,Animatable *prev)
{
	this->ip = ip;
   theHelperProc.SetCurrentOSGHelper(this);
   LODDesc.BeginEditParams(ip, this, flags, prev);	
}

void LOD::EndEditParams(IObjParam *ip, ULONG flags,Animatable *next)
{
	this->ip = NULL;
   theHelperProc.SetCurrentOSGHelper(NULL);
   LODDesc.EndEditParams(ip, this, flags, next);
	ClearAFlag(A_OBJ_CREATING);
}

RefTargetHandle LOD::Clone(RemapDir& remap){
	LOD* newob = new LOD(name);	
	// newob->data = data;
	newob->ReplaceReference(0, pblock2->Clone(remap));
	BaseClone(this, newob, remap);
	return(newob);
}

/**
 * This method will draw the helpers name, a little cross and assert in hitting testing.
 */
int LOD::DrawAndHit(TimeValue t, INode *inode, ViewExp *vpt){

	Color color(inode->GetWireColor()); 

	Matrix3 tm(1);
	Point3 pt(0,0,0);
	Point3 pts[5];

	vpt->getGW()->setTransform(tm);	
	tm = inode->GetObjectTM(t);
	Point3 p = tm.GetRow(3);
	pblock2->SetValue(lod_center_x, TimeValue(0), p.x);
	pblock2->SetValue(lod_center_y, TimeValue(0), p.y);
	pblock2->SetValue(lod_center_z, TimeValue(0), p.z);
	
	if (inode->Selected()) {
		vpt->getGW()->setColor( TEXT_COLOR, GetUIColor(COLOR_SELECTION) );
		vpt->getGW()->setColor( LINE_COLOR, GetUIColor(COLOR_SELECTION) );
	} 
	else if (!inode->IsFrozen() && !inode->Dependent()) {
		vpt->getGW()->setColor( TEXT_COLOR, color);
		vpt->getGW()->setColor( LINE_COLOR, color);
	}	

	vpt->getGW()->setTransform(tm);

	if (!inode->IsFrozen() && !inode->Dependent() && !inode->Selected()) {
		vpt->getGW()->setColor( LINE_COLOR, color);
	}

	vpt->getGW()->marker(&pt,X_MRKR);
	vpt->getGW()->text( &pt , name );

	return 1;
}

/**
 * This method is called to determine if the specified screen point intersects the item.
 * The method returns nonzero if the item was hit; otherwise 0.
 */
int LOD::HitTest(	TimeValue t, INode *inode, int type, int crossing, int flags, IPoint2 *p, ViewExp *vpt){
	Matrix3 tm(1);	
	HitRegion hitRegion;
	DWORD	savedLimits;
	Point3 pt(0,0,0);

	vpt->getGW()->setTransform(tm);
	GraphicsWindow *gw = vpt->getGW();	
	Material *mtl = gw->getMaterial();

   	tm = inode->GetObjectTM(t);		
	MakeHitRegion(hitRegion, type, crossing, 4, p);

	gw->setRndLimits(((savedLimits = gw->getRndLimits())|GW_PICK)&~GW_ILLUM);
	gw->setHitRegion(&hitRegion);
	gw->clearHitCode();

	DrawAndHit(t, inode, vpt);

	gw->setRndLimits(savedLimits);
	
	if((hitRegion.type != POINT_RGN) && !hitRegion.crossing)
		return TRUE;
	return gw->checkHitCode();
}


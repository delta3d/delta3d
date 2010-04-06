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
 *	FILE:			Billboard.cpp
 *
 *	DESCRIPTION:	Class for the Billboard helper objects.
 *
 *	CREATED BY:		Rune Schmidt Jensen, rune@schmidt-jensen.com
 *
 *	HISTORY:		Created 04.02.2003
 */
#include "OSGHelper.h"

extern ParamBlockDesc2 billboard_param_blk;

class Billboard:public OSGHelper{
	public:
		Billboard(TSTR name) : OSGHelper(name){	pblock2 = CreateParameterBlock2(&billboard_param_blk,0);};
      virtual ClassDesc2& GetClassDesc();
      void BeginEditParams(IObjParam *ip, ULONG flags,Animatable *prev);
		void EndEditParams(IObjParam *ip, ULONG flags,Animatable *next);
		Class_ID ClassID() {return BILLBOARD_CLASS_ID;}
		RefTargetHandle Clone(RemapDir& remap);
};

class BillboardClassDesc:public OSGHelperClassDesc {
	public:
	void *			Create(BOOL loading = FALSE) { return new Billboard(_T("OSG_Billboard")); }
	const TCHAR *	ClassName() { return _T("Billboard"); }
	Class_ID		ClassID() { return BILLBOARD_CLASS_ID; }
	const TCHAR*	InternalName() { return _T("OSGBillboard"); }	// returns fixed parsable name (scripter-visible name)
};


static BillboardClassDesc BillboardDesc;
ClassDesc2* GetBillboardDesc() { return &BillboardDesc; }


enum { 
	billboard_params
};

//ParamMap2UserDlgProc* dlgProc;


static ParamBlockDesc2 billboard_param_blk ( billboard_params, _T("billboard_params"),  0, &BillboardDesc, 
	P_AUTO_CONSTRUCT + P_AUTO_UI, PBLOCK_REF , 
	//rollout
	IDD_BILLBOARD, IDS_BILLBOARD, 0, 0, &theHelperProc,
	// modes
	bilbo_mode,		 	_T("Mode"),		TYPE_INT, 	P_ANIMATABLE,	IDS_BILL_MODE,
		p_default, 		0, 
		p_range, 		0, 2, 
		p_ui, 			TYPE_RADIO, 3, IDC_ROT_AXIAL, IDC_ROT_WORLD, IDC_ROT_EYE,
		end, 
	// x-axis value
	bilbo_axis_x, 		_T("AXIS_X"), 		TYPE_FLOAT, 	P_ANIMATABLE, 	IDS_BILL_AXIS, 
		p_default, 		0.0f, 
		p_range, 		-1.0f,1.0f, 
		p_ui, 			TYPE_SPINNER,		EDITTYPE_FLOAT, IDC_BILL_EDIT1,	IDC_BILL_SPIN1, 0.01f, 
		end,
	// y-axis value
	bilbo_axis_y, 		_T("AXIS_Y"), 		TYPE_FLOAT, 	P_ANIMATABLE, 	IDS_BILL_AXIS, 
		p_default, 		0.0f, 
		p_range, 		-1.0f,1.0f, 
		p_ui, 			TYPE_SPINNER,		EDITTYPE_FLOAT, IDC_BILL_EDIT2,	IDC_BILL_SPIN2, 0.01f, 
		end,
	// z-axis value
	bilbo_axis_z, 		_T("AXIS_Z"), 		TYPE_FLOAT, 	P_ANIMATABLE, 	IDS_BILL_AXIS, 
		p_default, 		1.0f, 
		p_range, 		-1.0f,1.0f, 
		p_ui, 			TYPE_SPINNER,		EDITTYPE_FLOAT, IDC_BILL_EDIT3,	IDC_BILL_SPIN3, 0.01f, 
		end,
	// x-normal value
	bilbo_normal_x,		_T("NORMAL_X"),		TYPE_FLOAT, 	P_ANIMATABLE, 	IDS_BILL_NORMAL, 
		p_default, 		0.0f, 
		p_range, 		-1.0f,1.0f, 
		p_ui, 			TYPE_SPINNER,		EDITTYPE_FLOAT, IDC_BILL_EDIT4,	IDC_BILL_SPIN4, 0.01f, 
		end,
	// y-normal value
	bilbo_normal_y,		_T("NORMAL_Y"),		TYPE_FLOAT, 	P_ANIMATABLE, 	IDS_BILL_NORMAL, 
		p_default, 		-1.0f, 
		p_range, 		-1.0f,1.0f, 
		p_ui, 			TYPE_SPINNER,		EDITTYPE_FLOAT, IDC_BILL_EDIT5,	IDC_BILL_SPIN5, 0.01f, 
		end,
	// z-normal value
	bilbo_normal_z,		_T("NORMAL_Z"),		TYPE_FLOAT, 	P_ANIMATABLE, 	IDS_BILL_NORMAL, 
		p_default, 		0.0f, 
		p_range, 		-1.0f,1.0f, 
		p_ui, 			TYPE_SPINNER,		EDITTYPE_FLOAT, IDC_BILL_EDIT6,	IDC_BILL_SPIN6, 0.01f, 
		end,
	// billboard geometry nodes
	bilbo_nodes,		_T("NODES"),		TYPE_INODE_TAB,	0,	P_AUTO_UI|P_VARIABLE_SIZE,	IDS_BILL_NODES,
		p_ui,			TYPE_NODELISTBOX, IDC_LIST,IDC_PICKNODE,0,IDC_REMOVE_BILL,
		p_prompt,		IDS_PICK_GEOM_OBJECT,
		end,
	end
	);

////////////////////////////////////////////////////////////////////////////////
ClassDesc2& Billboard::GetClassDesc()
{
   return BillboardDesc;
}

void Billboard::BeginEditParams(IObjParam *ip, ULONG flags,Animatable *prev)
{	
	this->ip = ip;
	theHelperProc.SetCurrentOSGHelper(this);
	BillboardDesc.BeginEditParams(ip, this, flags, prev);
}

void Billboard::EndEditParams(IObjParam *ip, ULONG flags,Animatable *next)
{
	this->ip = NULL;
	theHelperProc.SetCurrentOSGHelper(NULL);
	BillboardDesc.EndEditParams(ip, this, flags, next);
	ClearAFlag(A_OBJ_CREATING);
}

RefTargetHandle Billboard::Clone(RemapDir& remap){
	Billboard* newob = new Billboard(name);	
	// newob->data = data;
	newob->ReplaceReference(0, pblock2->Clone(remap));
	BaseClone(this, newob, remap);
	return(newob);
}



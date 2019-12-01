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
 *	FILE:			Switch.cpp
 *
 *	DESCRIPTION:	Class for the OSG Switch helper objects. Through
 *					this helper object the user will be able to make
 *					use of the OSG::Switch class.
 *
 *	CREATED BY:		Rune Schmidt Jensen, rune@schmidt-jensen.com
 *
 *	HISTORY:		Created 09.04.2003
 */
#include "OSGHelper.h"

extern ParamBlockDesc2 switch_param_blk;


class Switch:public OSGHelper{
	public:
		Switch(TSTR name) : OSGHelper(name){	pblock2 = CreateParameterBlock2(&switch_param_blk,0);};
		void BeginEditParams(IObjParam *ip, ULONG flags,Animatable *prev);
		void EndEditParams(IObjParam *ip, ULONG flags,Animatable *next);
		Class_ID ClassID() {return SWITCH_CLASS_ID;}
		RefTargetHandle Clone(RemapDir& remap);
};

class SwitchClassDesc:public OSGHelperClassDesc {
	public:
	void *			Create(BOOL loading = FALSE) { return new Switch(_T("OSG_Switch")); }
	const TCHAR *	ClassName() { return _T("Switch"); }
	Class_ID		ClassID() { return SWITCH_CLASS_ID; }
	const TCHAR*	InternalName() { return _T("OSGSWITCH"); }	// returns fixed parsable name (scripter-visible name)
};


static SwitchClassDesc SwitchDesc;
ClassDesc2* GetSwitchDesc() { return &SwitchDesc; }


enum { 
	switch_params
};


static ParamBlockDesc2 switch_param_blk ( switch_params, _T("switch_params"),  0, &SwitchDesc, 
	P_AUTO_CONSTRUCT + P_AUTO_UI, PBLOCK_REF , 
	// rollout
	IDD_SWITCH, IDS_SWITCH, 0, 0, &theHelperProc,
	// switch geometry nodes
	switch_nodes,		_T("NODES"),		TYPE_INODE_TAB,	0,	P_AUTO_UI|P_VARIABLE_SIZE,	IDS_SWITCH_NODES,
		p_ui,			TYPE_NODELISTBOX, IDC_LIST,0,0,IDC_SWITCH_REMNODE,
		p_prompt,		IDS_PICK_GEOM_OBJECT,
		end,

		//added by banderegg for enabling/disabling switch children
		switch_enable_children1,	_T("SWITCH_ENABLE_CHILDREN"),	TYPE_BOOL,	0,	IDS_SWITCH_ENABLE_CHILDREN, 	
		p_default, 		FALSE,
		p_ui, 			TYPE_SINGLECHEKBOX, IDC_SWITCH_ENABLE_CHILDREN1,
		end,

		switch_enable_children2,	_T("SWITCH_ENABLE_CHILDREN"),	TYPE_BOOL,	0,	IDS_SWITCH_ENABLE_CHILDREN, 	
		p_default, 		FALSE,
		p_ui, 			TYPE_SINGLECHEKBOX, IDC_SWITCH_ENABLE_CHILDREN2,
		end,
																										//there is no three.... :(
		switch_enable_children3,	_T("SWITCH_ENABLE_CHILDREN"),	TYPE_BOOL,	0,	IDS_SWITCH_ENABLE_CHILDREN, 	
		p_default, 		FALSE,
		p_ui, 			TYPE_SINGLECHEKBOX, IDC_SWITCH_ENABLE_CHILDREN4,
		end,

		switch_enable_children4,	_T("SWITCH_ENABLE_CHILDREN"),	TYPE_BOOL,	0,	IDS_SWITCH_ENABLE_CHILDREN, 	
		p_default, 		FALSE,
		p_ui, 			TYPE_SINGLECHEKBOX, IDC_SWITCH_ENABLE_CHILDREN5,
		end,

		switch_enable_children5,	_T("SWITCH_ENABLE_CHILDREN"),	TYPE_BOOL,	0,	IDS_SWITCH_ENABLE_CHILDREN, 	
		p_default, 		FALSE,
		p_ui, 			TYPE_SINGLECHEKBOX, IDC_SWITCH_ENABLE_CHILDREN6,
		end,

		switch_enable_children6,	_T("SWITCH_ENABLE_CHILDREN"),	TYPE_BOOL,	0,	IDS_SWITCH_ENABLE_CHILDREN, 	
		p_default, 		FALSE,
		p_ui, 			TYPE_SINGLECHEKBOX, IDC_SWITCH_ENABLE_CHILDREN7,
		end,

		switch_enable_children7,	_T("SWITCH_ENABLE_CHILDREN"),	TYPE_BOOL,	0,	IDS_SWITCH_ENABLE_CHILDREN, 	
		p_default, 		FALSE,
		p_ui, 			TYPE_SINGLECHEKBOX, IDC_SWITCH_ENABLE_CHILDREN8,
		end,

		switch_enable_children8,	_T("SWITCH_ENABLE_CHILDREN"),	TYPE_BOOL,	0,	IDS_SWITCH_ENABLE_CHILDREN, 	
		p_default, 		FALSE,
		p_ui, 			TYPE_SINGLECHEKBOX, IDC_SWITCH_ENABLE_CHILDREN9,
		end,

		switch_enable_children9,	_T("SWITCH_ENABLE_CHILDREN"),	TYPE_BOOL,	0,	IDS_SWITCH_ENABLE_CHILDREN, 	
		p_default, 		FALSE,
		p_ui, 			TYPE_SINGLECHEKBOX, IDC_SWITCH_ENABLE_CHILDREN10,
		end,
	end
	);

void Switch::BeginEditParams(IObjParam *ip, ULONG flags,Animatable *prev)
{
	this->ip = ip;
	theHelperProc.SetCurrentOSGHelper(this);
	SwitchDesc.BeginEditParams(ip, this, flags, prev);	
}

void Switch::EndEditParams(IObjParam *ip, ULONG flags,Animatable *next)
{
	this->ip = NULL;
	theHelperProc.SetCurrentOSGHelper(NULL);
	SwitchDesc.EndEditParams(ip, this, flags, next);
	ClearAFlag(A_OBJ_CREATING);
}

RefTargetHandle Switch::Clone(RemapDir& remap){
	Switch* newob = new Switch(name);	
	// newob->data = data;
	newob->ReplaceReference(0, pblock2->Clone(remap));
	BaseClone(this, newob, remap);
	return(newob);
}

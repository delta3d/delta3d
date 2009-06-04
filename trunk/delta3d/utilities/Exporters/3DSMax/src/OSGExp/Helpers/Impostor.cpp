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
 *	FILE:			Impostor.cpp
 *
 *	DESCRIPTION:	Class for the Impostor helper objects.
 *
 *	CREATED BY:		Rune Schmidt Jensen, rune@schmidt-jensen.com
 *
 *	HISTORY:		Created 10.04.2003
 */
#include "OSGHelper.h"

extern ParamBlockDesc2 impostor_param_blk;

class Impostor:public OSGHelper{
	public:
		Impostor(TSTR name) : OSGHelper(name){	pblock2 = CreateParameterBlock2(&impostor_param_blk,0);};
		void BeginEditParams(IObjParam *ip, ULONG flags,Animatable *prev);
		void EndEditParams(IObjParam *ip, ULONG flags,Animatable *next);
		Class_ID ClassID() {return IMPOSTOR_CLASS_ID;}
		RefTargetHandle Clone(RemapDir& remap);
};

class ImpostorClassDesc:public OSGHelperClassDesc {
	public:
	void *			Create(BOOL loading = FALSE) { return new Impostor(_T("OSG_Impostor")); }
	const TCHAR *	ClassName() { return _T("Impostor"); }
	Class_ID		ClassID() { return IMPOSTOR_CLASS_ID; }
	const TCHAR*	InternalName() { return _T("OSGImpostor"); }	// returns fixed parsable name (scripter-visible name)
};


static ImpostorClassDesc ImpostorDesc;
ClassDesc2* GetImpostorDesc() { return &ImpostorDesc; }


enum { 
	impostor_params
};


static ParamBlockDesc2 impostor_param_blk ( impostor_params, _T("Impostor_params"),  0, &ImpostorDesc, 
	P_AUTO_CONSTRUCT + P_AUTO_UI, PBLOCK_REF , 
	//rollout
	IDD_IMPOSTOR, IDS_IMPOSTOR, 0, 0, &theHelperProc,
	// modes
	threshold_value,	 	_T("THRESHOLDVALUES"),	TYPE_FLOAT, 	P_ANIMATABLE,	IDS_IMPOSTOR_VALUE,
		p_default, 		5.0f, 
		p_range, 		0.0f, 10000000.0f, 
		p_ui, 			TYPE_SPINNER,		EDITTYPE_POS_FLOAT, IDC_IMPOSTOR_EDIT,	IDC_IMPOSTOR_SPIN, 1.0f, 
		end,
	// impostor nodes
	impostor_nodes,		_T("NODES"),		TYPE_INODE_TAB,	0,	P_AUTO_UI|P_VARIABLE_SIZE,	IDS_IMPOSTOR_NODES,
		p_ui,			TYPE_NODELISTBOX, IDC_LIST,IDC_PICKNODE,0,IDC_IMPOSTOR_REMNODE,
		p_prompt,		IDS_PICK_GEOM_OBJECT,
		end,
	end
	);

void Impostor::BeginEditParams(IObjParam *ip, ULONG flags,Animatable *prev)
{
	this->ip = ip;
	theHelperProc.SetCurrentOSGHelper(this);
	ImpostorDesc.BeginEditParams(ip, this, flags, prev);	
}

void Impostor::EndEditParams(IObjParam *ip, ULONG flags,Animatable *next)
{
	this->ip = NULL;
	theHelperProc.SetCurrentOSGHelper(NULL);
	ImpostorDesc.EndEditParams(ip, this, flags, next);
	ClearAFlag(A_OBJ_CREATING);
}

RefTargetHandle Impostor::Clone(RemapDir& remap){
	Impostor* newob = new Impostor(name);	
	// newob->data = data;
	newob->ReplaceReference(0, pblock2->Clone(remap));
	BaseClone(this, newob, remap);
	return(newob);
}

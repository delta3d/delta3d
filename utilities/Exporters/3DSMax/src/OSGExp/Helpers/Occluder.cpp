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
 *	FILE:			Occluder.cpp
 *
 *	DESCRIPTION:	Class for the Occluder helper objects.
 *
 *	CREATED BY:		Rune Schmidt Jensen, rune@schmidt-jensen.com
 *
 *	HISTORY:		Created 10.04.2003
 */

#include "OSGHelper.h"

extern ParamBlockDesc2 Occluder_param_blk;

class Occluder:public OSGHelper{
	public:
		Occluder(TSTR name) : OSGHelper(name){	pblock2 = CreateParameterBlock2(&Occluder_param_blk,0);};
		void BeginEditParams(IObjParam *ip, ULONG flags,Animatable *prev);
		void EndEditParams(IObjParam *ip, ULONG flags,Animatable *next);
		Class_ID ClassID() {return OCCLUDER_CLASS_ID;}
		RefTargetHandle Clone(RemapDir& remap);
};

class OccluderClassDesc:public OSGHelperClassDesc {
	public:
	void *			Create(BOOL loading = FALSE) { return new Occluder(_T("OSG_Occluder")); }
	const TCHAR *	ClassName() { return _T("Occluder"); }
	Class_ID		ClassID() { return OCCLUDER_CLASS_ID; }
	const TCHAR*	InternalName() { return _T("OSGOccluder"); }	// returns fixed parsable name (scripter-visible name)
};


static OccluderClassDesc OccluderDesc;
ClassDesc2* GetOccluderDesc() { return &OccluderDesc; }


enum { 
	Occluder_params
};

static ParamBlockDesc2 Occluder_param_blk ( Occluder_params, _T("Occluder_params"),  0, &OccluderDesc, 
	P_AUTO_CONSTRUCT + P_AUTO_UI, PBLOCK_REF , 
	//rollout
	IDD_OCCLUDER, IDS_OCCLUDER, 0, 0, NULL,
	// Occluder nodes
	occluder_planes,		_T("PLANES"),		TYPE_INODE_TAB,	0,	P_AUTO_UI|P_VARIABLE_SIZE,	IDS_OCCLUDER_NODES,
		p_ui,			TYPE_NODELISTBOX, IDC_LIST,IDC_PICKNODE,0,IDC_OCCLUDER_REMNODE,
		p_sclassID,		SHAPE_CLASS_ID, // Only allow shape nodes.
		p_prompt,		IDS_PICK_GEOM_OBJECT,
		end,
	end
	);

void Occluder::BeginEditParams(IObjParam *ip, ULONG flags,Animatable *prev)
{
	this->ip = ip;
	theHelperProc.SetCurrentOSGHelper(this);
	OccluderDesc.BeginEditParams(ip, this, flags, prev);	
}

void Occluder::EndEditParams(IObjParam *ip, ULONG flags,Animatable *next)
{
	this->ip = NULL;
	theHelperProc.SetCurrentOSGHelper(NULL);
	OccluderDesc.EndEditParams(ip, this, flags, next);
	ClearAFlag(A_OBJ_CREATING);
}

RefTargetHandle Occluder::Clone(RemapDir& remap){
	Occluder* newob = new Occluder(name);	
	// newob->data = data;
	newob->ReplaceReference(0, pblock2->Clone(remap));
	BaseClone(this, newob, remap);
	return(newob);
}



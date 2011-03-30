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
 *	FILE:			VisibilityGroup.cpp
 *
 *	DESCRIPTION:	Class for the OSG Visibility Group helper objects.
 *					Through this helper object is will be possible to 
 *					make use of the osgSim::VisibilityGroup.
 *
 *	CREATED BY:		Rune Schmidt Jensen, rune@schmidt-jensen.com
 *
 *	HISTORY:		Created 30.12.2003
 */
#include "OSGHelper.h"

extern ParamBlockDesc2 visibilitygroup_param_blk;


class VisibilityGroup:public OSGHelper{
	public:
		VisibilityGroup(TSTR name) : OSGHelper(name){	pblock2 = CreateParameterBlock2(&visibilitygroup_param_blk,0);};
		void BeginEditParams(IObjParam *ip, ULONG flags,Animatable *prev);
		void EndEditParams(IObjParam *ip, ULONG flags,Animatable *next);
		Class_ID ClassID() {return VISIBILITYGROUP_CLASS_ID;}
		RefTargetHandle Clone(RemapDir& remap);
};

class VisibilityGroupClassDesc:public OSGHelperClassDesc {
	public:
	void *			Create(BOOL loading = FALSE) { return new VisibilityGroup(_T("OSG_VisibilityGroup")); }
	const TCHAR *	ClassName() { return _T("VisibilityGroup"); }
	Class_ID		ClassID() { return VISIBILITYGROUP_CLASS_ID; }
	const TCHAR*	InternalName() { return _T("OSGVISIBILITYGROUP"); }	// returns fixed parsable name (scripter-visible name)
};


static VisibilityGroupClassDesc VisibilityGroupDesc;
ClassDesc2* GetVisibilityGroupDesc() { return &VisibilityGroupDesc; }


enum { 
	visibilitygroup_params
};


static ParamBlockDesc2 visibilitygroup_param_blk ( visibilitygroup_params, _T("visibilitygroup_params"),  0, &VisibilityGroupDesc, 
	P_AUTO_CONSTRUCT + P_AUTO_UI, PBLOCK_REF , 
	// rollout
	IDD_VISIBILITYGROUP, IDS_VISIBILITYGROUP, 0, 0, &theHelperProc,
	// visibilitygroup volume node
	visibilitygroup_volume,			_T("VOLUME"),	TYPE_INODE,	0,	IDS_VISIBILITYGROUP_VOLUME,
		p_ui,			TYPE_PICKNODEBUTTON,	IDC_VISIBILITYGROUP_VOLUME,
		p_prompt,		IDS_PICK_GEOM_OBJECT,
		end,
	// visibilitygroup geometry nodes
	visibilitygroup_nodes,		_T("NODES"),		TYPE_INODE_TAB,	0,	P_AUTO_UI|P_VARIABLE_SIZE,	IDS_VISIBILITYGROUP_NODES,
		p_ui,			TYPE_NODELISTBOX, IDC_LIST,IDC_PICKNODE,0,IDC_VISIBILITYGROUP_REMNODE,
		p_prompt,		IDS_PICK_GEOM_OBJECT,
		end,
	end
	);

void VisibilityGroup::BeginEditParams(IObjParam *ip, ULONG flags,Animatable *prev)
{	
	this->ip = ip;
	theHelperProc.SetCurrentOSGHelper(this);
	VisibilityGroupDesc.BeginEditParams(ip, this, flags, prev);	
}

void VisibilityGroup::EndEditParams(IObjParam *ip, ULONG flags,Animatable *next)
{	
	this->ip = NULL;
	theHelperProc.SetCurrentOSGHelper(NULL);
	VisibilityGroupDesc.EndEditParams(ip, this, flags, next);
	ClearAFlag(A_OBJ_CREATING);
}

RefTargetHandle VisibilityGroup::Clone(RemapDir& remap){
	VisibilityGroup* newob = new VisibilityGroup(name);	
	// newob->data = data;
	newob->ReplaceReference(0, pblock2->Clone(remap));
	BaseClone(this, newob, remap);
	return(newob);
}

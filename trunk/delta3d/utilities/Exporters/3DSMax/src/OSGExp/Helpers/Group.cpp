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
*	FILE:			Group.cpp
*
*	DESCRIPTION:	Class for the OSG Group helper objects. 
*
*	CREATED BY:		Bradley Anderegg, bgandere@nps.edu
*
*	HISTORY:		Created 06.16.2006
*/
#include "OSGHelper.h"

extern ParamBlockDesc2 group_param_blk;


class OSGGroup:public OSGHelper{
public:
	OSGGroup(TSTR name) : OSGHelper(name){	pblock2 = CreateParameterBlock2(&group_param_blk,0);};
	void BeginEditParams(IObjParam *ip, ULONG flags,Animatable *prev);
	void EndEditParams(IObjParam *ip, ULONG flags,Animatable *next);
	Class_ID ClassID() {return OSGGROUP_CLASS_ID;}
	RefTargetHandle Clone(RemapDir& remap);
};

class GroupClassDesc:public OSGHelperClassDesc {
public:
	void *			Create(BOOL loading = FALSE) { return new OSGGroup(_T("OSG_Group")); }
	const TCHAR *	ClassName() { return _T("Group"); }
	Class_ID		ClassID() { return OSGGROUP_CLASS_ID; }
	const TCHAR*	InternalName() { return _T("OSGGROUP"); }	// returns fixed parsable name (scripter-visible name)
};


static GroupClassDesc GroupDesc;
ClassDesc2* GetOSGGroupDesc() { return &GroupDesc; }


enum { 
	group_params
};


static ParamBlockDesc2 group_param_blk ( group_params, _T("group_params"),  0, &GroupDesc, 
										 P_AUTO_CONSTRUCT + P_AUTO_UI, PBLOCK_REF , 
										 
										 // rollout
										 IDD_OSGGROUP, IDS_GROUP, 0, 0, &theHelperProc,		
										 osggroup_nodes,		_T("NODES"),		TYPE_INODE_TAB,	0,	P_AUTO_UI|P_VARIABLE_SIZE,	IDS_GROUP_NODES,
										 p_ui,			TYPE_NODELISTBOX, IDC_LIST,0,0,IDC_SWITCH_REMNODE,
										 p_prompt,		IDS_PICK_GEOM_OBJECT,
										 end,

										 end
										 );

void OSGGroup::BeginEditParams(IObjParam *ip, ULONG flags,Animatable *prev)
{
	this->ip = ip;
	theHelperProc.SetCurrentOSGHelper(this);
	GroupDesc.BeginEditParams(ip, this, flags, prev);	
}

void OSGGroup::EndEditParams(IObjParam *ip, ULONG flags,Animatable *next)
{
	this->ip = NULL;
	theHelperProc.SetCurrentOSGHelper(NULL);
	GroupDesc.EndEditParams(ip, this, flags, next);
	ClearAFlag(A_OBJ_CREATING);
}

RefTargetHandle OSGGroup::Clone(RemapDir& remap){
	OSGGroup* newob = new OSGGroup(name);	
	// newob->data = data;
	newob->ReplaceReference(0, pblock2->Clone(remap));
	BaseClone(this, newob, remap);
	return(newob);
}

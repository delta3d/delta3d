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
 *	FILE:			Seqeunce.cpp
 *
 *	DESCRIPTION:	Class for the OSG Sequence helper objects. Through
 *					this helper object the user will be able to make
 *					use of the OSG::Sequence class.
 *
 *	CREATED BY:		Rune Schmidt Jensen, rune@schmidt-jensen.com
 *
 *	HISTORY:		Created 18.02.2003
 */
#include "OSGHelper.h"

extern ParamBlockDesc2 sequence_param_blk;

class Sequence:public OSGHelper{
	public:
		Sequence(TSTR name) : OSGHelper(name){	pblock2 = CreateParameterBlock2(&sequence_param_blk,0);};
		void BeginEditParams(IObjParam *ip, ULONG flags,Animatable *prev);
		void EndEditParams(IObjParam *ip, ULONG flags,Animatable *next);
		Class_ID ClassID() {return SEQUENCE_CLASS_ID;}
		RefTargetHandle Clone(RemapDir& remap);
};

class SequenceClassDesc:public OSGHelperClassDesc {
	public:
	void *			Create(BOOL loading = FALSE) { return new Sequence(_T("OSG_Sequence")); }
	const TCHAR *	ClassName() { return _T("Sequence"); }
	Class_ID		ClassID() { return SEQUENCE_CLASS_ID; }
	const TCHAR*	InternalName() { return _T("OSGSEQUENCE"); }	// returns fixed parsable name (scripter-visible name)
};


static SequenceClassDesc SequenceDesc;
ClassDesc2* GetSequenceDesc() { return &SequenceDesc; }


enum { 
	sequence_params
};


static ParamBlockDesc2 sequence_param_blk ( sequence_params, _T("sequence_params"),  0, &SequenceDesc, 
	P_AUTO_CONSTRUCT + P_AUTO_UI, PBLOCK_REF , 
	// rollout
	IDD_SEQUENCE, IDS_SEQUENCE, 0, 0, NULL,
	// sequence interval
	sequence_begin, 	_T("Begin"),	TYPE_FLOAT, 	P_ANIMATABLE, 	IDS_SEQUENCE_INTERVAL, 
		p_default, 		0.0f,
		p_range, 		-1000000.0f, 1000000.0f, 
		p_ui, 			TYPE_SPINNER,	EDITTYPE_FLOAT, IDC_SEQUENCE_BEGIN_EDIT,	IDC_SEQUENCE_BEGIN_SPIN, 0.01f, 
		end,
	sequence_end, 		_T("End"),	TYPE_FLOAT, 	P_ANIMATABLE, 	IDS_SEQUENCE_INTERVAL, 
		p_default, 		-1.0f, 
		p_range, 		-1000000.0f, 1000000.0f, 
		p_ui, 			TYPE_SPINNER,	EDITTYPE_FLOAT, IDC_SEQUENCE_END_EDIT,	IDC_SEQUENCE_END_SPIN, 0.01f, 
		end,
	sequence_loop, 		_T("Loop"),		TYPE_INT, 	P_ANIMATABLE,	IDS_SEQUENCE_INTERVAL,
		p_default, 		0, 
		p_range, 		0, 1, 
		p_ui, 			TYPE_RADIO, 2, IDC_SEQUENCE_LOOP, IDC_SEQUENCE_SWING,
		end, 
	// sequence duration.
	sequence_speed, 	_T("Speed"),	TYPE_FLOAT, 	P_ANIMATABLE, 	IDS_SEQUENCE_DURATION, 
		p_default, 		1.0f,
		p_range, 		-1000000.0f, 1000000.0f, 
		p_ui, 			TYPE_SPINNER,	EDITTYPE_FLOAT, IDC_SEQUENCE_SPEED_EDIT,	IDC_SEQUENCE_SPEED_SPIN, 0.01f, 
		end,
	sequence_reps, 		_T("Reps"),	TYPE_INT, 	P_ANIMATABLE, 	IDS_SEQUENCE_DURATION, 
		p_default, 		-1, 
		p_range, 		-1000000, 1000000, 
		p_ui, 			TYPE_SPINNER,	EDITTYPE_INT, IDC_SEQUENCE_REPS_EDIT,	IDC_SEQUENCE_REPS_SPIN, 1.0f, 
		end,
	// sequence mode.
	sequence_mode, 		_T("Mode"),		TYPE_INT, 	P_ANIMATABLE,	IDS_SEQUENCE_MODE,
		p_default, 		0, 
		p_range, 		0, 3, 
		p_ui, 			TYPE_RADIO, 4, IDC_SEQUENCE_START, IDC_SEQUENCE_STOP, IDC_SEQUENCE_PAUSE, IDC_SEQUENCE_RESUME,
		end,
	// Sample geometry or use static geometry.
	sequence_approach, 		_T("Approach"),		TYPE_INT, 	P_ANIMATABLE,	IDS_SEQUENCE_APPROACH,
		p_default, 		0, 
		p_range, 		0, 1, 
		p_ui, 			TYPE_RADIO, 2, IDC_SEQUENCE_SAMPLE, IDC_SEQUENCE_STATIC,
		end,
	// Sample geometry
	sequence_sample_node,	_T("Sample node"),	TYPE_INODE,	0,	IDS_SEQUENCE_SAMPLE_NODE,
		p_ui,			TYPE_PICKNODEBUTTON,	IDC_SEQUENCE_SAMPLE_PICKNODE,
		p_prompt,		IDS_PICK_GEOM_OBJECT,
		end,
	// Frames per Sample
	sequence_frame_per_sample, 	_T("Frame per Sample"),	TYPE_INT, 	P_ANIMATABLE, 	IDS_SEQUENCE_FRAME_PER_SAMPLE, 
		p_default, 		1,
		p_range, 		1, 1000000, 
		p_ui, 			TYPE_SPINNER,	EDITTYPE_INT, IDC_SEQUENCE_SAMPLE_EDIT,	IDC_SEQUENCE_SAMPLE_SPIN, 1.0f, 
		end,
	// Static sequence geometry time values in a table.
	sequence_time,		_T("Time"),	TYPE_FLOAT_TAB, 	NUM_SEQUENCE_OBJECTS,	P_ANIMATABLE, 	IDS_SEQUENCE_TIME, 
		p_default, 		0.0f,	
		p_range, 		-1000000.0f, 1000000.0f, 
		p_ui, 			TYPE_SPINNER, EDITTYPE_FLOAT, 
						IDC_SEQUENCE_TIME_EDIT1,IDC_SEQUENCE_TIME_SPIN1,
						IDC_SEQUENCE_TIME_EDIT2,IDC_SEQUENCE_TIME_SPIN2,
						IDC_SEQUENCE_TIME_EDIT3,IDC_SEQUENCE_TIME_SPIN3,
						IDC_SEQUENCE_TIME_EDIT4,IDC_SEQUENCE_TIME_SPIN4,
						IDC_SEQUENCE_TIME_EDIT5,IDC_SEQUENCE_TIME_SPIN5,
						0.01f, 
		end,
	// Static sequence geometry nodes. 
	// DAMN THE MAX PARAMBLOCK TEAM!!! 
	// Not able to do a TYPE_INODE_TAB with TYPE_PICKNODEBUTTON,
	// must declare NUM_LOD_OBJECTS (5 params) of TYPE_INODE, or make
	// an NODE_LISTBOX and implement all logic myself.
	sequence_node+0,	_T("NODES"),	TYPE_INODE,	0,	IDS_SEQUENCE_NODES,
		p_ui,			TYPE_PICKNODEBUTTON,	IDC_SEQUENCE_PICKNODE1,
		p_prompt,		IDS_PICK_GEOM_OBJECT,
		end,
	sequence_node+1,	_T("NODES"),	TYPE_INODE,	0,	IDS_SEQUENCE_NODES,
		p_ui,			TYPE_PICKNODEBUTTON,	IDC_SEQUENCE_PICKNODE2,
		p_prompt,		IDS_PICK_GEOM_OBJECT,
		end,
	sequence_node+2,	_T("NODES"),	TYPE_INODE,	0,	IDS_SEQUENCE_NODES,
		p_ui,			TYPE_PICKNODEBUTTON,	IDC_SEQUENCE_PICKNODE3,
		p_prompt,		IDS_PICK_GEOM_OBJECT,
		end,
	sequence_node+3,	_T("NODES"),	TYPE_INODE,	0,	IDS_SEQUENCE_NODES,
		p_ui,			TYPE_PICKNODEBUTTON,	IDC_SEQUENCE_PICKNODE4,
		p_prompt,		IDS_PICK_GEOM_OBJECT,
		end,
	sequence_node+4,	_T("NODES"),	TYPE_INODE,	0,	IDS_SEQUENCE_NODES,
		p_ui,			TYPE_PICKNODEBUTTON,	IDC_SEQUENCE_PICKNODE5,
		p_prompt,		IDS_PICK_GEOM_OBJECT,
		end,
	end
	);

void Sequence::BeginEditParams(IObjParam *ip, ULONG flags,Animatable *prev)
{
	this->ip = ip;
	SequenceDesc.BeginEditParams(ip, this, flags, prev);	
}

void Sequence::EndEditParams(IObjParam *ip, ULONG flags,Animatable *next)
{
	this->ip = NULL;
	SequenceDesc.EndEditParams(ip, this, flags, next);
	ClearAFlag(A_OBJ_CREATING);
}

RefTargetHandle Sequence::Clone(RemapDir& remap){
	Sequence* newob = new Sequence(name);	
	// newob->data = data;
	newob->ReplaceReference(0, pblock2->Clone(remap));
	BaseClone(this, newob, remap);
	return(newob);
}


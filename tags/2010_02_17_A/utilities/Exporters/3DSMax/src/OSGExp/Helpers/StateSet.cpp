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
 *	FILE:			StateSet.cpp
 *
 *	DESCRIPTION:	Class for the StateSet helper objects. Through
 *					this helper object the user will be able to set
 *					all the properties of an osg::StateSet.
 *
 *	CREATED BY:		Rune Schmidt Jensen, rune@schmidt-jensen.com
 *
 *	HISTORY:		Created 11.02.2003
 *
 *                  12.10.2005 Joran: Modified stateset helper to a super stateset
 *                  helper.
 *
 *                  19.07.2006 Joran: Removed the StateSet::ENCLOSE_RENDERBIN_DETAILS 
 *                  stuff, because it is removed from OSG.
 */
#include "OSGHelper.h"

extern ParamBlockDesc2 stateset_param_blk;

class StateSet:public OSGHelper{
	public:
		StateSet(TSTR name) : OSGHelper(name){	pblock2 = CreateParameterBlock2(&stateset_param_blk,0);};
		void BeginEditParams(IObjParam *ip, ULONG flags,Animatable *prev);
		void EndEditParams(IObjParam *ip, ULONG flags,Animatable *next);
		Class_ID ClassID() {return STATESET_CLASS_ID;}
		RefTargetHandle Clone(RemapDir& remap);
};

class StateSetClassDesc:public OSGHelperClassDesc {
	public:
	void *			Create(BOOL loading = FALSE) { return new StateSet(_T("OSG_StateSet")); }
	const TCHAR *	ClassName() { return _T("StateSet"); }
	Class_ID		ClassID() { return STATESET_CLASS_ID; }
	const TCHAR*	InternalName() { return _T("OSGStateSet"); }	// returns fixed parsable name (scripter-visible name)
};


static StateSetClassDesc StateSetDesc;
ClassDesc2* GetStateSetDesc() { return &StateSetDesc; }


enum { 
	stateset_params
};


static ParamBlockDesc2 stateset_param_blk ( stateset_params, _T("stateset_params"),  0, &StateSetDesc, 
	P_AUTO_CONSTRUCT + P_AUTO_UI + P_MULTIMAP, PBLOCK_REF , 
    // num rollups:
    5,
	// rollups:
	rollup_stateset,            IDD_STATESET,       IDS_STATESET,       0, 0,                   &theHelperProc,
    rollup_renderbin,       IDD_RENDERBIN,      IDS_RENDERBIN,      0, APPENDROLL_CLOSED,   NULL,
    rollup_glmode,          IDD_GLMODE,         IDS_GLMODE,         0, APPENDROLL_CLOSED,   NULL,
	rollup_polygonoffset,   IDD_POLYGONOFFSET,  IDS_POLYGONOFFSET,  0, APPENDROLL_CLOSED,   NULL,
    rollup_polygonmode,     IDD_POLYGONMODE,    IDS_POLYGONMODE,    0, APPENDROLL_CLOSED,   NULL,
    // render bin enable
    render_bin_enable,   _T("RenderBinEnable"),  TYPE_BOOL,  0,  IDS_STATESET_RENDERBIN_ENABLE,
        p_default,      FALSE,
        p_ui,           rollup_renderbin,   TYPE_SINGLECHEKBOX,
            IDC_RENDERBIN_ENABLE,
        end,
	// render bin number
	render_bin_num,		_T("RenderBinNumber"),	TYPE_INT, 	P_ANIMATABLE, 	IDS_STATESET_BIN_NUMBER, 
		p_default, 		1, 
		p_range, 		-100,100, 
		p_ui, 			rollup_renderbin,    TYPE_SPINNER,	
            EDITTYPE_INT, IDC_STATESET_BIN_EDIT,	IDC_STATESET_BIN_SPIN, 1.0f, 
		end,
	// render bin name
	render_bin_name,	_T("RenderBinName"),	TYPE_STRING, 	P_ANIMATABLE, 	IDS_STATESET_BIN_NAME, 
		p_default, 		_T("RenderBin"), 
		p_ui, 			rollup_renderbin,    TYPE_EDITBOX,	
            IDC_STATESET_BIN_NAME,
		end,
	// render bin mode
	render_bin_mode,	_T("RenderBinMode"),	TYPE_INT, 	P_ANIMATABLE,	IDS_STATESET_RENDER_MODE,
		p_default, 		1, 
		p_range, 		0, 2, 
		p_ui, 			rollup_renderbin,    TYPE_RADIO, 
            3, IDC_RENDER_INHERIT, IDC_RENDER_USE, IDC_RENDER_OVERRIDE,
		end,
	// glmode cullface
	glmode_cullface,	_T("GLModeCullFace"),	TYPE_BOOL,	0,	IDS_STATESET_CULLFACE, 	
		p_default, 		TRUE,
		p_ui, 			rollup_glmode,    TYPE_SINGLECHEKBOX, 
            IDC_GLMODE_CULLFACE,
		end,
	// glmode normalize
	glmode_normalize,	_T("GLModeNormalize"),	TYPE_BOOL,	0,	IDS_STATESET_NORMALIZE,
		p_default, 		FALSE,
		p_ui, 			rollup_glmode,    TYPE_SINGLECHEKBOX, 
            IDC_GLMODE_NORMALIZE,
		end, 
	// glmode turn off lighting
	glmode_turnofflighting,	_T("GLModeTurnOffLighting"),	TYPE_BOOL,	0,	IDS_STATESET_TURNOFFLIGHTING,
		p_default, 		FALSE,
		p_ui, 			rollup_glmode,    TYPE_SINGLECHEKBOX, 
            IDC_GLMODE_TURNOFFLIGHTING,
		end, 
	// glmode use alpha blending
	glmode_alphablending,	_T("GLModeAlphaBlending"),	TYPE_BOOL,	0,	IDS_STATESET_USEALPHABLENDING,
		p_default, 		FALSE,
		p_ui, 			rollup_glmode,    TYPE_SINGLECHEKBOX, 
            IDC_GLMODE_ALPHABLENDING,
		end, 
	// geometry nodes to use the stateset on
	stateset_nodes,		_T("NODES"),	TYPE_INODE_TAB,	0,	P_AUTO_UI|P_VARIABLE_SIZE,	IDS_STATESET_NODES,
		p_ui,			rollup_stateset,    TYPE_NODELISTBOX, 
            IDC_STATESET_NODELIST,IDC_PICKNODE,0,IDC_STATESET_REMOVENODE,
		p_prompt,		IDS_PICK_GEOM_OBJECT,
		end,
    // polygonmode enable
    polygonmode_enable, _T("PolygonModeEnable"), TYPE_BOOL, 0, IDS_STATESET_POLYMODE_ENABLE,
        p_default,      FALSE,
        p_ui,           rollup_polygonmode, TYPE_SINGLECHEKBOX,
            IDC_POLYGONMODE_ENABLE,
        end,
    // polyonmode front face
    polygonmode_frontface, _T("PolygonModeFrontFace"), TYPE_INT, 0, IDS_STATESET_POLYMODE_FRONTFACE,
        p_default,      0,
        p_ui,           rollup_polygonmode, TYPE_RADIO,
            3, IDC_PMFF_RADIO1, IDC_PMFF_RADIO2, IDC_PMFF_RADIO3,
        end,
    // polyonmode back face
    polygonmode_backface, _T("PolygonModeBackFace"), TYPE_INT, 0, IDS_STATESET_POLYMODE_BACKFACE,
        p_default,      0,
        p_ui,           rollup_polygonmode, TYPE_RADIO,
            3, IDC_PMBF_RADIO1, IDC_PMBF_RADIO2, IDC_PMBF_RADIO3,
        end,
    // polygonoffset enable
    polygonoffset_enable, _T("PolygonOffsetEnable"), TYPE_BOOL, 0, IDS_STATESET_POLYOFF_ENABLE,
        p_default,      FALSE,
        p_ui,           rollup_polygonoffset, TYPE_SINGLECHEKBOX,
            IDC_POLYGONOFFSET_ENABLE,
        end,
    // polygonoffset factor
    polygonoffset_factor, _T("PolygonOffsetFactor"), TYPE_FLOAT, P_ANIMATABLE, IDS_STATESET_POLYOFF_FACTOR,
   		p_default, 		-1.0f,
        p_range, 		-1000000.0f, 1000000.0f, 
        p_ui, 			rollup_polygonoffset, TYPE_SPINNER,	
            EDITTYPE_FLOAT, IDC_POLYOFF_FACTOR_EDIT, IDC_POLYOFF_FACTOR_SPIN, 0.01f, 
        end,
    // polygonoffset unit
    polygonoffset_unit, _T("PolygonOffsetUnit"), TYPE_FLOAT, P_ANIMATABLE, IDS_STATESET_POLYOFF_UNIT,
   		p_default, 		-1.0f,
        p_range, 		-1000000.0f, 1000000.0f, 
        p_ui, 			rollup_polygonoffset, TYPE_SPINNER,	
            EDITTYPE_FLOAT, IDC_POLYOFF_UNIT_EDIT, IDC_POLYOFF_UNIT_SPIN, 0.01f, 
        end,
    // polygonoffset depthfunc
    polygonoffset_depthfunc, _T("PolygonOffsetDepthFunc"), TYPE_BOOL, 0, IDS_STATESET_POLYOFF_DEPTHFUNC,
        p_default,      TRUE,
        p_ui,           rollup_polygonoffset, TYPE_SINGLECHEKBOX,
            IDC_POLYGONOFFSET_DEPTHFUNC,
        end,

	end
	);

void StateSet::BeginEditParams(IObjParam *ip, ULONG flags,Animatable *prev)
{
	this->ip = ip;
	theHelperProc.SetCurrentOSGHelper(this);
	StateSetDesc.BeginEditParams(ip, this, flags, prev);	
}

void StateSet::EndEditParams(IObjParam *ip, ULONG flags,Animatable *next)
{
	this->ip = NULL;
	theHelperProc.SetCurrentOSGHelper(NULL);
	StateSetDesc.EndEditParams(ip, this, flags, next);
	ClearAFlag(A_OBJ_CREATING);
}

RefTargetHandle StateSet::Clone(RemapDir& remap){
	StateSet* newob = new StateSet(name);	
	// newob->data = data;
	newob->ReplaceReference(0, pblock2->Clone(remap));
	BaseClone(this, newob, remap);
	return(newob);
}

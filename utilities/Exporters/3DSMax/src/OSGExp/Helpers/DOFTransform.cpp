/*
* OSGExp - 3D Studio Max plugin for exporting OpenSceneGraph models.
* Copyright (C) 2006  Bradley Anderegg
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
*	FILE:			DOFTransform.cpp
*
*	DESCRIPTION:	Class for the DOFTransfor mophelper objects. 
*
*	CREATED BY:		Bradley Anderegg, bgandere@nps.edu
*
*	HISTORY:		Created 06.16.2006
*/
#include "OSGHelper.h"

extern ParamBlockDesc2 doftrans_param_blk;


class DOFTransform:public OSGHelper{
public:
	DOFTransform(TSTR name) : OSGHelper(name){	pblock2 = CreateParameterBlock2(&doftrans_param_blk,0);};
   virtual ClassDesc2& GetClassDesc();
   void BeginEditParams(IObjParam *ip, ULONG flags,Animatable *prev);
	void EndEditParams(IObjParam *ip, ULONG flags,Animatable *next);
	Class_ID ClassID() {return DOFTRANSFORM_CLASS_ID;}
	RefTargetHandle Clone(RemapDir& remap);
   /*virtual*/ int DrawAndHit(TimeValue t, INode *inode, ViewExp *vpt);
   void DrawAnAxis( ViewExp *vpt, Point3 axis );
   void DrawAxis(ViewExp *vpt, const Matrix3 &tm, float length);
   void Text( ViewExp *vpt, TCHAR *str, Point3 &pt );
};

class DOFTransformClassDesc:public OSGHelperClassDesc {
public:
	void *			Create(BOOL loading = FALSE) { return new DOFTransform(_T("DOFTransform")); }
	const TCHAR *	ClassName() { return _T("DOFTransform"); }
	Class_ID		ClassID() { return DOFTRANSFORM_CLASS_ID; }
	const TCHAR*	InternalName() { return _T("DOFTransform"); }	// returns fixed parsable name (scripter-visible name)
};


static DOFTransformClassDesc DOFTransDesc;
ClassDesc2* GetDOFTransDesc() { return &DOFTransDesc; }


enum { 
	doftrans_params
};


static ParamBlockDesc2 doftrans_param_blk ( doftrans_params, _T("doftrans_params"),  0, &DOFTransDesc, 
										 P_AUTO_CONSTRUCT + P_AUTO_UI, PBLOCK_REF , 
										 
										 // rollout
										 IDD_DOFTRANSFORM, IDS_DOF, 0, 0, &theHelperProc,		
										 doftrans_nodes,		_T("NODES"),		TYPE_INODE_TAB,	0,	P_AUTO_UI|P_VARIABLE_SIZE,	IDS_DOF_NODES,
                               p_ui,			TYPE_NODELISTBOX, IDC_LIST,0,0,IDC_SWITCH_REMNODE,
										 p_prompt,		IDS_PICK_GEOM_OBJECT,
										 end,

										 // mult order
										 dof_mult_order,		 	_T("Mult Order"),		TYPE_INT, 	P_ANIMATABLE,	IDS_BILL_MODE,
										 p_default, 		0, 
										 p_range, 		0, 2, 
										 p_ui, 			TYPE_RADIO, 6, IDC_PRH, IDC_PHR, IDC_HPR, IDC_HRP, IDC_RPH, IDC_RHP,
										 end, 

										 // hpr min -h axis value
										 dof_min_hpr_h, 		_T("Heading"), 		TYPE_FLOAT, 	P_ANIMATABLE, 	IDS_BILL_AXIS, 
										 p_default, 		0.0f, 
										 p_range, 		-180.0f,180.0f, 
										 p_ui, 			TYPE_SPINNER,		EDITTYPE_FLOAT, IDC_BILL_EDIT7,	IDC_BILL_SPIN7, 1.0f, 
										 end,

										 // hpr min -p axis value
										 dof_min_hpr_p, 		_T("Pitch"), 		TYPE_FLOAT, 	P_ANIMATABLE, 	IDS_BILL_AXIS, 
										 p_default, 		0.0f, 
										 p_range, 		-180.0f,180.0f, 
										 p_ui, 			TYPE_SPINNER,		EDITTYPE_FLOAT, IDC_BILL_EDIT8,	IDC_BILL_SPIN8, 1.0f, 
										 end,

										 // hpr min -r axis value
										 dof_min_hpr_r, 		_T("Roll"), 		TYPE_FLOAT, 	P_ANIMATABLE, 	IDS_BILL_AXIS, 
										 p_default, 		0.0f, 
										 p_range, 		-180.0f,180.0f, 
										 p_ui, 			TYPE_SPINNER,		EDITTYPE_FLOAT, IDC_BILL_EDIT9,	IDC_BILL_SPIN9, 1.0f, 
										 end,

										 // hpr max -h axis value
										 dof_max_hpr_h, 		_T("Heading"), 		TYPE_FLOAT, 	P_ANIMATABLE, 	IDS_BILL_AXIS, 
										 p_default, 		0.0f, 
										 p_range, 		-180.0f,180.0f, 
										 p_ui, 			TYPE_SPINNER,		EDITTYPE_FLOAT, IDC_BILL_EDIT10,	IDC_BILL_SPIN10, 1.0f, 
										 end,

										 // hpr max -p axis value
										 dof_max_hpr_p, 		_T("Pitch"), 		TYPE_FLOAT, 	P_ANIMATABLE, 	IDS_BILL_AXIS, 
										 p_default, 		0.0f, 
										 p_range, 		-180.0f,180.0f, 
										 p_ui, 			TYPE_SPINNER,		EDITTYPE_FLOAT, IDC_BILL_EDIT11,	IDC_BILL_SPIN11, 1.0f, 
										 end,

										 // hpr max -r axis value
										 dof_max_hpr_r, 		_T("Roll"), 		TYPE_FLOAT, 	P_ANIMATABLE, 	IDS_BILL_AXIS, 
										 p_default, 		0.0f, 
										 p_range, 		-180.0f,180.0f, 
										 p_ui, 			TYPE_SPINNER,		EDITTYPE_FLOAT, IDC_BILL_EDIT12,	IDC_BILL_SPIN12, 1.0f, 
										 end,


										 // translate min -x axis value
										 dof_min_translate_x, 		_T("Axis X"), 		TYPE_FLOAT, 	P_ANIMATABLE, 	IDS_BILL_AXIS, 
										 p_default, 		0.0f, 
										 p_range, 		-1000.0f,1000.0f, 
										 p_ui, 			TYPE_SPINNER,		EDITTYPE_FLOAT, IDC_BILL_EDIT1,	IDC_BILL_SPIN1, 1.0f, 
										 end,										 

										 // translate min -y axis value
										 dof_min_translate_y, 		_T("Axis Y"), 		TYPE_FLOAT, 	P_ANIMATABLE, 	IDS_BILL_AXIS, 
										 p_default, 		0.0f, 
										 p_range, 		-1000.0f,1000.0f, 
										 p_ui, 			TYPE_SPINNER,		EDITTYPE_FLOAT, IDC_BILL_EDIT2,	IDC_BILL_SPIN2, 1.0f, 
										 end,					

										 // translate min -y axis value
										 dof_min_translate_z, 		_T("Axis Z"), 		TYPE_FLOAT, 	P_ANIMATABLE, 	IDS_BILL_AXIS, 
										 p_default, 		0.0f, 
										 p_range, 		-1000.0f,1000.0f, 
										 p_ui, 			TYPE_SPINNER,		EDITTYPE_FLOAT, IDC_BILL_EDIT3,	IDC_BILL_SPIN3, 1.0f, 
										 end,					

										 // translate max -x axis value
										 dof_max_translate_x, 		_T("Axis X"), 		TYPE_FLOAT, 	P_ANIMATABLE, 	IDS_BILL_AXIS, 
										 p_default, 		0.0f, 
										 p_range, 		-1000.0f,1000.0f, 
										 p_ui, 			TYPE_SPINNER,		EDITTYPE_FLOAT, IDC_BILL_EDIT4,	IDC_BILL_SPIN4, 1.0f, 
										 end,										 

										 // translate max -y axis value
										 dof_max_translate_y, 		_T("Axis Y"), 		TYPE_FLOAT, 	P_ANIMATABLE, 	IDS_BILL_AXIS, 
										 p_default, 		0.0f, 
										 p_range, 		-1000.0f,1000.0f, 
										 p_ui, 			TYPE_SPINNER,		EDITTYPE_FLOAT, IDC_BILL_EDIT5,	IDC_BILL_SPIN5, 1.0f, 
										 end,					

										 // translate max -y axis value
										 dof_max_translate_z, 		_T("Axis Z"), 		TYPE_FLOAT, 	P_ANIMATABLE, 	IDS_BILL_AXIS, 
										 p_default, 		0.0f, 
										 p_range, 		-1000.0f,1000.0f, 
										 p_ui, 			TYPE_SPINNER,		EDITTYPE_FLOAT, IDC_BILL_EDIT6,	IDC_BILL_SPIN6, 1.0f, 
										 end,		

										 // scale min -x axis value
										 dof_min_scale_x, 		_T("Axis X"), 		TYPE_FLOAT, 	P_ANIMATABLE, 	IDS_BILL_AXIS, 
										 p_default, 		1.0f, 
										 p_range, 		-1000.0f,1000.0f, 
										 p_ui, 			TYPE_SPINNER,		EDITTYPE_FLOAT, IDC_BILL_EDIT13,	IDC_BILL_SPIN13, 1.0f, 
										 end,

										 // scale min -y axis value
										 dof_min_scale_y, 		_T("Axis Y"), 		TYPE_FLOAT, 	P_ANIMATABLE, 	IDS_BILL_AXIS, 
										 p_default, 		1.0f, 
										 p_range, 		-1000.0f,1000.0f, 
										 p_ui, 			TYPE_SPINNER,		EDITTYPE_FLOAT, IDC_BILL_EDIT14,	IDC_BILL_SPIN14, 1.0f, 
										 end,

										 // scale min -z axis value
										 dof_min_scale_z, 		_T("Axis Z"), 		TYPE_FLOAT, 	P_ANIMATABLE, 	IDS_BILL_AXIS, 
										 p_default, 		1.0f, 
										 p_range, 		-1000.0f,1000.0f, 
										 p_ui, 			TYPE_SPINNER,		EDITTYPE_FLOAT, IDC_BILL_EDIT15,	IDC_BILL_SPIN15, 1.0f, 
										 end,

										 // scale max -x axis value
										 dof_max_scale_x, 		_T("Axis X"), 		TYPE_FLOAT, 	P_ANIMATABLE, 	IDS_BILL_AXIS, 
										 p_default, 		1.0f, 
										 p_range, 		-1000.0f,1000.0f, 
										 p_ui, 			TYPE_SPINNER,		EDITTYPE_FLOAT, IDC_BILL_EDIT16,	IDC_BILL_SPIN16, 1.0f, 
										 end,

										 // scale max -y axis value
										 dof_max_scale_y, 		_T("Axis Y"), 		TYPE_FLOAT, 	P_ANIMATABLE, 	IDS_BILL_AXIS, 
										 p_default, 		1.0f, 
										 p_range, 		-1000.0f,1000.0f, 
										 p_ui, 			TYPE_SPINNER,		EDITTYPE_FLOAT, IDC_BILL_EDIT17,	IDC_BILL_SPIN17, 1.0f, 
										 end,

										 // scale max -z axis value
										 dof_max_scale_z, 		_T("Axis Z"), 		TYPE_FLOAT, 	P_ANIMATABLE, 	IDS_BILL_AXIS, 
										 p_default, 		1.0f, 
										 p_range, 		-1000.0f,1000.0f, 
										 p_ui, 			TYPE_SPINNER,		EDITTYPE_FLOAT, IDC_BILL_EDIT18,	IDC_BILL_SPIN18, 1.0f, 
										 end,

										 end
										 );


////////////////////////////////////////////////////////////////////////////////
ClassDesc2& DOFTransform::GetClassDesc()
{
 return DOFTransDesc;
}

void DOFTransform::BeginEditParams(IObjParam *ip, ULONG flags,Animatable *prev)
{
   this->ip = ip;
   theHelperProc.SetCurrentOSGHelper(this);
	DOFTransDesc.BeginEditParams(ip, this, flags, prev);
}

void DOFTransform::EndEditParams(IObjParam *ip, ULONG flags,Animatable *next)
{
	this->ip = NULL;
	theHelperProc.SetCurrentOSGHelper(NULL);
	DOFTransDesc.EndEditParams(ip, this, flags, next);
	ClearAFlag(A_OBJ_CREATING);
}

RefTargetHandle DOFTransform::Clone(RemapDir& remap){
	DOFTransform* newob = new DOFTransform(name);	
	// newob->data = data;
	newob->ReplaceReference(0, pblock2->Clone(remap));
	BaseClone(this, newob, remap);
	return(newob);
}


void DOFTransform::DrawAnAxis( ViewExp *vpt, Point3 axis )
{
   Point3 v1, v2, v[3];	
   v1 = axis * (float)0.9;
   if ( axis.x != 0.0 || axis.y != 0.0 ) {
      v2 = Point3( axis.y, -axis.x, axis.z ) * (float)0.1;
   } else {
      v2 = Point3( axis.x, axis.z, -axis.y ) * (float)0.1;
   }

   v[0] = Point3(0.0,0.0,0.0);
   v[1] = axis;
   vpt->getGW()->polyline( 2, v, NULL, NULL, FALSE, NULL );	
   v[0] = axis;
   v[1] = v1+v2;
   vpt->getGW()->polyline( 2, v, NULL, NULL, FALSE, NULL );
   v[0] = axis;
   v[1] = v1-v2;
   vpt->getGW()->polyline( 2, v, NULL, NULL, FALSE, NULL );
}

void DOFTransform::Text( ViewExp *vpt, TCHAR *str, Point3 &pt )
{	
   vpt->getGW()->text( &pt, str );	
}


void DOFTransform::DrawAxis(ViewExp *vpt, const Matrix3 &tm, float length)
{
   Matrix3 tmn = tm;
   float zoom;

   //scale to always be the same size on screen
   zoom = vpt->GetScreenScaleFactor(tmn.GetTrans()) * 0.005f;
   tmn.Scale( Point3(zoom,zoom,zoom) );
   

   vpt->getGW()->setTransform( tmn );		

   Text( vpt, _T("x"), Point3(length,0.0f,0.0f) ); 
   DrawAnAxis( vpt, Point3(length,0.0f,0.0f) );	

   Text( vpt, _T("y"), Point3(0.0f,length,0.0f) ); 
   DrawAnAxis( vpt, Point3(0.0f,length,0.0f) );	

   Text( vpt, _T("z"), Point3(0.0f,0.0f,length) ); 
   DrawAnAxis( vpt, Point3(0.0f,0.0f,length) );
}

int DOFTransform::DrawAndHit(TimeValue t, INode *inode, ViewExp *vpt){

   Color color(inode->GetWireColor()); 

   Matrix3 tm(1);
   Point3 pt(0,0,0);
   Point3 pts[5];

   vpt->getGW()->setTransform(tm);	
   tm = inode->GetObjectTM(t);

   if (inode->Selected()) {
      vpt->getGW()->setColor( TEXT_COLOR, GetUIColor(COLOR_SELECTION) );
      vpt->getGW()->setColor( LINE_COLOR, GetUIColor(COLOR_SELECTION) );
   } 
   else if (!inode->IsFrozen() && !inode->Dependent()) {
      vpt->getGW()->setColor( TEXT_COLOR, color);
      vpt->getGW()->setColor( LINE_COLOR, color);
   }	

   DrawAxis(vpt, tm, 30.0f);

   vpt->getGW()->setTransform(tm);

   if (!inode->IsFrozen() && !inode->Dependent() && !inode->Selected()) {
      vpt->getGW()->setColor( LINE_COLOR, color);
   }

   vpt->getGW()->marker(&pt,X_MRKR);
   vpt->getGW()->text( &pt , name );


   return 1;
}

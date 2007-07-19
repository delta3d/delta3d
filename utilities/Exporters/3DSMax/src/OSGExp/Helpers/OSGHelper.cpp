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
 *	FILE:			OSGHelper.cpp
 *
 *	DESCRIPTION:	This is a basic class for the OSG helper objects.
 *
 *	CREATED BY:		Rune Schmidt Jensen, rune@schmidt-jensen.com
 *
 *	HISTORY:		Created 03.02.2003
 */

#include "OSGHelper.h"

static OSGHelperClassDesc OSGHelperDesc;
ClassDesc2* GetOSGHelperDesc() { return &OSGHelperDesc; }


enum { 
	osghelper_params,
};


//TODO: Add enums for various parameters
enum { 
	pb_spin,
};


static ParamBlockDesc2 osghelper_param_blk ( osghelper_params, _T("params"),  0, &OSGHelperDesc, 
	P_AUTO_CONSTRUCT + P_AUTO_UI, PBLOCK_REF, 
	//rollout
	IDD_PANEL, IDS_PARAMS, 0, 0, NULL,
	// params
	pb_spin, 			_T("spin"), 		TYPE_FLOAT, 	P_ANIMATABLE, 	IDS_SPIN, 
		p_default, 		0.1f, 
		p_range, 		0.0f,1000.0f, 
		p_ui, 			TYPE_SPINNER,		EDITTYPE_FLOAT, IDC_EDIT,	IDC_SPIN, 0.01f, 
		end,



	end
	);


/**
 * This method is called by the system when the user may edit the item's 
 * (object, modifier, controller, etc.) parameters.
 */
void OSGHelper::BeginEditParams(IObjParam *ip, ULONG flags,Animatable *prev){	
	this->ip = ip;
	editOb   = this;
	OSGHelperDesc.BeginEditParams(ip, this, flags, prev);	
}

/**
 * This method is called when the user is finished editing an objects parameters.
 * The system passes a flag into the EndEditParams() method to indicate if the rollup
 * page should be removed. If this flag is TRUE, the plug-in must un-register the 
 * rollup page, and delete it from the panel.
 */
void OSGHelper::EndEditParams(IObjParam *ip, ULONG flags,Animatable *next){	
	editOb   = NULL;
	this->ip = NULL;
	OSGHelperDesc.EndEditParams(ip, this, flags, next);
	ClearAFlag(A_OBJ_CREATING);
}

OSGHelper::OSGHelper(TSTR name){	
	this->name = name;
	OSGHelperDesc.MakeAutoParamBlocks(this);
	SetAFlag(A_OBJ_CREATING);
}

OSGHelper::~OSGHelper(){
	DeleteAllRefsFromMe();
}

/**
 * Class for the needed for the OSGHelper::GetCreateMouseCallBack() method.
 */
class OSGHelperCreateCallBack: public CreateMouseCallBack {
	OSGHelper *ob;
	public:
		int proc( ViewExp *vpt,int msg, int point, int flags, IPoint2 m, Matrix3& mat );
		void SetObj(OSGHelper *obj) { ob = obj; }
};

/** 
 * This is the method where the developer defines the user / mouse interaction
 * that takes place during the creation phase of an object.
 */
int OSGHelperCreateCallBack::proc(ViewExp *vpt,int msg, int point, int flags, IPoint2 m, Matrix3& mat ) {	

	// Show a preview snap in the viewport prior to the 
	// first point event.
	if (msg == MOUSE_FREEMOVE)
		vpt->SnapPreview(m,m,NULL, SNAP_IN_3D);

	if (msg==MOUSE_POINT||msg==MOUSE_MOVE) {
		switch(point) {
			// First click with mouse.
			case 0:
				mat.SetTrans(vpt->SnapPoint(m,m,NULL,SNAP_IN_3D));
				break;
			// Second click with mouse.
			case 1:
				mat.SetTrans(vpt->SnapPoint(m,m,NULL,SNAP_IN_3D));
				if (msg==MOUSE_POINT) {
					return 0;
				}
				break; 
		}
	} 
	else if(msg == MOUSE_ABORT) { 
		return CREATE_ABORT;
	}
	return 1;
}

/** This is the callback object for handling the creation process of a plug-in object.*/
static OSGHelperCreateCallBack OSGHelperCreateCB;

/**
 * This method allows the system to retrieve a callback object used in creating
 * an object in the 3D viewports. This method returns a pointer to an instance 
 * of a class derived from CreateMouseCallBack. This class has a method proc() 
 * which is where the programmer defines the user/mouse interaction during the 
 * object creation phase.
 */
CreateMouseCallBack* OSGHelper::GetCreateMouseCallBack() {
	OSGHelperCreateCB.SetObj(this);
	return(&OSGHelperCreateCB);
}


/**
 * This is the object space bounding box, the box in the object's local coordinates. 
 * The system expects that requesting the object space bounding box will be fast.
 */
void OSGHelper::GetLocalBoundBox(TimeValue t, INode* inode, ViewExp* vpt, Box3& box ){
	
	float size = 1.0f;
	float zoom = 1.0f;

	box =  Box3(Point3(0,0,0), Point3(0,0,0));
	box += Point3(size*0.5f,  0.0f, 0.0f);
	box += Point3( 0.0f, size*0.5f, 0.0f);
	box += Point3( 0.0f, 0.0f, size*0.5f);
	box += Point3(-size*0.5f,   0.0f,  0.0f);
	box += Point3(  0.0f, -size*0.5f,  0.0f);
	box += Point3(  0.0f,  0.0f, -size*0.5f);

	box.EnlargeBy(10.0f);
}

/**
 * This method returns the world space bounding box. The bounding box returned by
 * this method does not need to be precise. It should however be calculated rapidly.
 * The object can handle this by transforming the 8 points of its local bounding box 
 * into world space and take the minimums and maximums of the result. Although this isn't
 * necessarily the tightest bounding box of the objects points in world space, it is close enough.
 */
void OSGHelper::GetWorldBoundBox(TimeValue t, INode* inode, ViewExp* vpt, Box3& box ){
	Matrix3 tm;
	tm = inode->GetObjectTM(t);
	Box3 lbox;

	GetLocalBoundBox(t, inode, vpt, lbox);
	box = Box3(tm.GetTrans(), tm.GetTrans());
	for (int i=0; i<8; i++) {
		box += lbox * tm;
	}
}

/**
 * This method will draw the helpers name, a little cross and assert in hitting testing.
 */
int OSGHelper::DrawAndHit(TimeValue t, INode *inode, ViewExp *vpt){

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
int OSGHelper::HitTest(	TimeValue t, INode *inode, int type, int crossing, int flags, IPoint2 *p, ViewExp *vpt){
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

/**
 * This is called by the system to have the item display itself 
 * (perform a quick render in viewport, using the current TM). 
 */
int OSGHelper::Display(TimeValue t, INode* inode, ViewExp *vpt, int flags) {
	DrawAndHit(t, inode, vpt);
	return(0);
}


/**
 * A plug-in which makes references must implement a method to receive and 
 * respond to messages broadcast by its dependents. 
 * This is the method will handle it.
 */
RefResult OSGHelper::NotifyRefChanged(Interval changeInt, RefTargetHandle hTarget, 
										PartID& partID, RefMessage message ){
	switch (message) {
		case REFMSG_CHANGE:
			//if (editOb==this) InvalidateUI();
			break;
		}
	return(REF_SUCCEED);
}

/**
 * This method returns the validity interval of the object as a whole at the specified time.
 */
Interval OSGHelper::ObjectValidity(TimeValue t){
	Interval ivalid = FOREVER;
	return ivalid;
}

/**
 * This method is called to evaluate the object and return the result as an ObjectState. 
 * When the system has a pointer to an object it doesn't know if it's a procedural object
 * or a derived object. So it calls Eval() on it and gets back an ObjectState. A derived 
 * object managed by the system may have to call Eval() on its input for example. 
 * A plug-in (like a procedural object) typically just returns itself.
 */
ObjectState OSGHelper::Eval(TimeValue t){
	return ObjectState(this);
}

/**
 * This method is called to have the plug-in clone itself. 
 * This method should copy both the data structure and all the data residing in the data
 * structure of this reference target. 
 * The plug-in should clone all its references as well. 
 */
RefTargetHandle OSGHelper::Clone(RemapDir& remap){
	OSGHelper* newob = new OSGHelper(name);	
	// newob->data = data;
	newob->ReplaceReference(0, pblock2->Clone(remap));
	BaseClone(this, newob, remap);
	return(newob);
}

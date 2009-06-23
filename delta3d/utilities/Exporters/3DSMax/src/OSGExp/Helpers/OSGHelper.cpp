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
 *					
 *					24.09.2007 Farshid Lashkari: Fixed bug that would crash
 *                  Max 9 when using the helpers.
 */

#include "OSGHelper.h"
#include <vector>

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
void OSGHelper::BeginEditParams(IObjParam *ip, ULONG flags,Animatable *prev)
{
	this->ip = ip;
   OSGHelperDesc.BeginEditParams(ip, this, flags, prev);	
}

/**
 * This method is called when the user is finished editing an objects parameters.
 * The system passes a flag into the EndEditParams() method to indicate if the rollup
 * page should be removed. If this flag is TRUE, the plug-in must un-register the 
 * rollup page, and delete it from the panel.
 */
void OSGHelper::EndEditParams(IObjParam *ip, ULONG flags,Animatable *next)
{
	this->ip = NULL;
	OSGHelperDesc.EndEditParams(ip, this, flags, next);
	ClearAFlag(A_OBJ_CREATING);
}

/**
 * This method returns the handle to the current node being operated on through the
 * supplied interface.
 */
ULONG OSGHelper::GetCurrentNodeHandle(Interface* ip)
{
   ULONG handle = 0;

   if( ip != NULL && ip->GetSelNodeCount() > 0 )
   {
      INode* node = ip->GetSelNode(0);
      if( node != NULL )
      {
         handle = node->GetHandle();
      }
   }

   return handle;
}

OSGHelper::OSGHelper(TSTR name)
   : mNode(NULL)
   , ip(NULL)
   , pblock2(NULL)
   , mLinksCount(0)
{	
	this->name = name;
	SetAFlag(A_OBJ_CREATING);
}

OSGHelper::~OSGHelper()
{
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
int OSGHelper::Display(TimeValue t, INode* inode, ViewExp *vpt, int flags)
{
   // HACK: Allow the object to initialize with a reference its own Node.
   // There does not seem to be any other way to do this through the Max API.
   if(mNode == NULL)
   {
      mNode = inode;
      AttachReferencedObjects();
   }

   // HACK: Update references in both the object and the UI if the link count
   // has changed.
   if(mNode != NULL)
   {
      int linkCount = mNode->NumberOfChildren();
      if(mLinksCount != linkCount)
      {
         mLinksCount = linkCount;
         UpdateReferencesByLinks();
      }
   }

	DrawAndHit(t, inode, vpt);
	return(0);
}


/**
 * A plug-in which makes references must implement a method to receive and 
 * respond to messages broadcast by its dependents. 
 * This is the method will handle it.
 */
RefResult OSGHelper::NotifyRefChanged(Interval changeInt, RefTargetHandle hTarget, 
										PartID& partID, RefMessage message )
{
	switch (message)
   {
		case REFMSG_NODE_LINK:
			break;
      default:
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

////////////////////////////////////////////////////////////////////////////////
void OSGHelper::AttachReferencedObjects()
{
   IParamBlock2* pblock2 = GetParamBlock(osghelper_params);

   if( ! IsHierarchalType() || mNode == NULL || pblock2 == NULL)
   {
      return;
   }

   ParamBlockDesc2* pdesc2 = pblock2->GetDesc();
   INode* rootNode = GetCOREInterface()->GetRootNode();

   // Get the parameter ID for the node list referenced by this object.
   ParamID nodesParamID = 0;
   if(GetParamIDForNodeList(*pdesc2, nodesParamID))
   {
      INode* curNode = NULL;
      Interval curInterval;
      typedef std::vector<INode*> NodeArray;
      NodeArray nodeArray;

      int size = pblock2->Count(nodesParamID);
      for(int i = 0; i < size; ++i)
      {
         curNode = GetNodeParameter(*pblock2, nodesParamID, i);

         if(curNode != NULL && curNode != mNode && curNode->GetParentNode() == rootNode)
         {
            nodeArray.push_back(curNode);
         }
      }

      ClearNodeWidgets(*pblock2, nodesParamID, nodeArray.size());
      NodeArray::iterator curNodeIter = nodeArray.begin();
      NodeArray::iterator endNodeArray = nodeArray.end();
      for(int i = 0; curNodeIter != endNodeArray; ++curNodeIter, ++i)
      {
         curNode = *curNodeIter;
         mNode->AttachChild(curNode);
         SetNodeParameter(*pblock2, nodesParamID, i, curNode);
      }

      UpdateReferencesByLinks();
   }
}

////////////////////////////////////////////////////////////////////////////////
void OSGHelper::DetachUnreferencedObjects()
{
   IParamBlock2* pblock2 = GetParamBlock(osghelper_params);

   if( ! IsHierarchalType() || mNode == NULL || pblock2 == NULL)
   {
      return;
   }

   ParamBlockDesc2* pdesc2 = pblock2->GetDesc();
   INode* rootNode = GetCOREInterface()->GetRootNode();

   // Get the parameter ID for the node list referenced by this object.
   ParamID nodesParamID = 0;
   if(GetParamIDForNodeList(*pdesc2, nodesParamID))
   {
      typedef std::vector<INode*> NodeArray;
      NodeArray removableNodes;

      // Gather all the nodes not referenced by this objects node list.
      INode* curNode = NULL;
      int childCount = mNode->NumberOfChildren();
      for(int i = 0; i < childCount; ++i)
      {
         curNode = mNode->GetChildNode(i);

         if( ! IsObjectReferenced(*curNode, *pblock2, nodesParamID))
         {
            removableNodes.push_back(curNode);
         }
      }

      // Detach the nodes that did not match.
      NodeArray::iterator curNodeIter = removableNodes.begin();
      NodeArray::iterator endNodeArray = removableNodes.end();
      for( ; curNodeIter != endNodeArray; ++curNodeIter)
      {
         (*curNodeIter)->Detach(0);
      }
   }
}

////////////////////////////////////////////////////////////////////////////////
bool OSGHelper::IsObjectReferenced(const INode& node, IParamBlock2& pblock, ParamID nodesParamID)
{
   bool isReferenced = false;
   INode* curNode = NULL;

   int size = pblock.Count(nodesParamID);
   for(int i = 0; i < size; ++i)
   {
      curNode = GetNodeParameter(pblock, nodesParamID, i);

      if( &node == curNode )
      {
         isReferenced = true;
         break;
      }
   }

   return isReferenced;
}

////////////////////////////////////////////////////////////////////////////////
void OSGHelper::UpdateReferencesByLinks()
{
   IParamBlock2* pblock2 = GetParamBlock(osghelper_params);

   if( ! IsHierarchalType() || mNode == NULL || pblock2 == NULL)
   {
      return;
   }

   ParamBlockDesc2* pdesc2 = pblock2->GetDesc();
   INode* rootNode = GetCOREInterface()->GetRootNode();

   // Get the parameter ID for the node list referenced by this object.
   ParamID nodesParamID = 0;
   if(GetParamIDForNodeList(*pdesc2, nodesParamID))
   {
      // Add references to the parameter block based on the objected that are linked.
      INode* curNode = NULL;
      int childCount = mNode->NumberOfChildren();
      ClearNodeWidgets(*pblock2, nodesParamID, childCount); // Resize list to match number of children.
      for(int i = 0; i < childCount; ++i)
      {
         curNode = mNode->GetChildNode(i);
         SetNodeParameter(*pblock2, nodesParamID, i, curNode);
      }

      // Update the listbox.
      pdesc2->InvalidateUI();
   }
}

////////////////////////////////////////////////////////////////////////////////
void OSGHelper::ClearNodeWidgets(IParamBlock2& pblock2, ParamID paramID, int newListSize)
{
   pblock2.Resize(paramID, 0); // Clear current list.
   pblock2.Resize(paramID, newListSize); // Resize list to match number of children.
}

////////////////////////////////////////////////////////////////////////////////
void OSGHelper::SetNodeParameter(IParamBlock2& pblock2, ParamID paramID,
   int nodeIndex, INode* node)
{
   pblock2.SetValue(paramID, 0, node, nodeIndex);
}

////////////////////////////////////////////////////////////////////////////////
INode* OSGHelper::GetNodeParameter(IParamBlock2& pblock2, ParamID paramID, int nodeIndex)
{
   INode* node = NULL;
   Interval curInterval;

   if(nodeIndex >= 0 && pblock2.Count(paramID) > nodeIndex)
   {
      pblock2.GetValue(paramID, 0, node, curInterval, nodeIndex);
   }

   return node;
}

////////////////////////////////////////////////////////////////////////////////
bool OSGHelper::GetParamIDForNodeList(ParamBlockDesc2& desc, ParamID& outParamID)
{
   Class_ID cid = desc.cd->ClassID();

   bool success = true;
   
   if(cid == BILLBOARD_CLASS_ID)	
      outParamID = bilbo_nodes;
   else if(cid == STATESET_CLASS_ID)
      outParamID = stateset_nodes;
   else if(cid == NODEMASK_CLASS_ID)
      outParamID = nodemask_nodes;
   else if(cid == SWITCH_CLASS_ID)
      outParamID = switch_nodes;
   else if(cid == IMPOSTOR_CLASS_ID)
      outParamID = impostor_nodes;
   else if(cid == OCCLUDER_CLASS_ID)
      outParamID = occluder_planes;
   else if(cid == VISIBILITYGROUP_CLASS_ID)
      outParamID = visibilitygroup_nodes;
   else if(cid == OSGGROUP_CLASS_ID)
      outParamID = osggroup_nodes;
   else if(cid == DOFTRANSFORM_CLASS_ID)
      outParamID = doftrans_nodes;
   else if(cid == LOD_CLASS_ID)
      outParamID = lod_node_array;
   else
      success = false;

   return success;
}

////////////////////////////////////////////////////////////////////////////////
bool OSGHelper::IsHierarchalType()
{
   Class_ID cid = ClassID();
   return cid != STATESET_CLASS_ID
      && cid != NODEMASK_CLASS_ID
      && cid != VISIBILITYGROUP_CLASS_ID
      && cid != OCCLUDER_CLASS_ID
      && cid != IMPOSTOR_CLASS_ID;
}

////////////////////////////////////////////////////////////////////////////////
bool OSGHelper::NodeHasChild(INode& potentialParent, INode& potentialChild)
{
   bool isChildNode = false;

   INode* curNode = &potentialChild;
   while(curNode != NULL)
   {
      if( &potentialParent == curNode )
      {
         isChildNode = true;
         break;
      }
      curNode = curNode->GetParentNode();
   }

   return isChildNode;
}

////////////////////////////////////////////////////////////////////////////////
void OSGHelper::MoveNodeItem(HWND hWnd, int offset)
{
   IParamBlock2* pblock2 = GetParamBlock(osghelper_params);

   if(offset == 0 || mNode == NULL || pblock2 == NULL)
   {
      return;
   }

   ParamBlockDesc2* pdesc2 = pblock2->GetDesc();

   // Get the parameter ID for the node list referenced by this object.
   ParamID nodesParamID = 0;
   if(GetParamIDForNodeList(*pdesc2, nodesParamID))
   {
      typedef std::vector<INode*> NodeArray;
      NodeArray nodes;

      // Get the selected item.
      INode* selectedNode = GetNodeFromSelectedItem(hWnd, *pblock2, nodesParamID);

      if(selectedNode != NULL)
      {
         // Get all node references.
         int foundIndex = -1;
         INode* curNode = NULL;
         int childCount = mNode->NumberOfChildren();
         for(int i = 0; i < childCount; ++i)
         {
            curNode = GetNodeParameter(*pblock2, nodesParamID, i);
            curNode->Detach(0);
            nodes.push_back(curNode);

            // Is this the selected node?
            if(curNode == selectedNode)
            {
               foundIndex = i;
            }
         }

         // Move up
         if(offset < 0)
         {
            if(foundIndex > 0)
            {
               INode* a = nodes[foundIndex-1];
               INode* b = nodes[foundIndex];
               nodes[foundIndex-1] = b;
               nodes[foundIndex] = a;

               --foundIndex;
            }
         }
         // Move down
         else if(foundIndex >= 0 && foundIndex < int(nodes.size() - 1))
         {
            INode* a = nodes[foundIndex];
            INode* b = nodes[foundIndex+1];
            nodes[foundIndex] = b;
            nodes[foundIndex+1] = a;
            
            ++foundIndex;
         }

         // Remove all list references so that they do not conflict
         // when re-attaching the child nodes.
         ClearNodeWidgets(*pblock2, nodesParamID, childCount); // Resize list to match number of children.

         // Re-reference the nodes in the specified order.
         NodeArray::iterator curNodeIter = nodes.begin();
         NodeArray::iterator endNodeArray = nodes.end();
         for( int index = 0; curNodeIter != endNodeArray; ++curNodeIter, ++index)
         {
            curNode = *curNodeIter;
            mNode->AttachChild(curNode);
            SetNodeParameter(*pblock2, nodesParamID, index, curNode);
         }

         // Keep the select highlight on the moved item, if it was moved.
         if(foundIndex >= 0)
         {
            SetSelectedNodeItem(hWnd, foundIndex);
         }

         // Refresh the list box.
         pdesc2->InvalidateUI();
      }
   }
}

////////////////////////////////////////////////////////////////////////////////
INode* OSGHelper::GetNodeFromSelectedItem(HWND hWnd, IParamBlock2& pblock2, ParamID paramID)
{
   HWND hList = GetDlgItem(hWnd,IDC_LIST);

   // Get selected index.
   int itemIndex = (int)SendMessage(hList, LB_GETCURSEL, 0, 0);

   return GetNodeParameter(pblock2, paramID, itemIndex);
}

////////////////////////////////////////////////////////////////////////////////
void OSGHelper::SetSelectedNodeItem(HWND hWnd, int index)
{
   HWND hList = GetDlgItem(hWnd,IDC_LIST);
   SendMessage(hList, LB_SETCURSEL, index, 0);
}


////////////////////////////////////////////////////////////////////////////////
// MULTIPLE SELECT DIALOG CALLBACK
////////////////////////////////////////////////////////////////////////////////
int MultipleSelectDlg::filter(INode *node)
{
   bool allowThrough = mCurrentHelper == NULL
      || mCurrentHelper->mNode == NULL;

   if( ! allowThrough && node != mCurrentHelper->mNode)
   {
      // Do a hierarchy search.
      if(mCurrentHelper->IsHierarchalType())
      {
         allowThrough = ! OSGHelper::NodeHasChild(*mCurrentHelper->mNode, *node);
      }
      else // Do a search through the parameter block.
      {
         IParamBlock2* pblock2 = mCurrentHelper->GetParamBlock(osghelper_params);
         ParamBlockDesc2* pdesc2 = pblock2->GetDesc();

         ParamID nodesParamID = 0;
         if(OSGHelper::GetParamIDForNodeList(*pdesc2, nodesParamID))
         {
            allowThrough = ! mCurrentHelper->IsObjectReferenced(*node, *pblock2, nodesParamID);
         }
         else
         {
            allowThrough = true;
         }
      }
   }

   return allowThrough;
}

////////////////////////////////////////////////////////////////////////////////
void MultipleSelectDlg::proc(INodeTab &nodeTab)
{
   if(map)
   {
      if(nodeTab.Count())
      {
         IParamBlock2* pblock2 = map->GetParamBlock();
         ParamBlockDesc2* pdesc2 = pblock2->GetDesc();

         // Figure out which helper object the multiple button is used on.
         // Get the parameter ID for the node list referenced by this object.
         ParamID nodesParamID = 0;
         bool allowAddNodes = OSGHelper::GetParamIDForNodeList(*pdesc2, nodesParamID);

         if( allowAddNodes && mCurrentHelper != NULL && mCurrentHelper->mNode != NULL)
         {
            INode* currentNode = mCurrentHelper->mNode;
            if(mCurrentHelper->IsHierarchalType())
            {
               // Attempt attachment of the selected nodes.
               int size = pblock2->Count(nodesParamID);
               for(int i=0;i<nodeTab.Count();i++)
               {
                  currentNode->AttachChild(nodeTab[i]);
               }

               // Update the UI list.
               int nodeCount = currentNode->NumberOfChildren();
               pblock2->Resize(nodesParamID, 0);
               pblock2->Resize(nodesParamID, nodeCount);
               for(int i = 0; i < nodeCount; ++i)
               {
                  pblock2->SetValue(nodesParamID, 0, currentNode->GetChildNode(i), i);
               }
            }
            else // Non-hierarchal type
            {
               int size = pblock2->Count(nodesParamID);
               pblock2->Resize(nodesParamID, size + nodeTab.Count());
               for(int i = 0; i < nodeTab.Count(); ++i)
               {
                  pblock2->SetValue(nodesParamID, 0, nodeTab[i], size+i);
               }
            }

            pdesc2->InvalidateUI();
         }

         // Do not hold onto the node.
         // Let the next call to this procedure set it again.
         mCurrentHelper = NULL;
      }
   }
}


////////////////////////////////////////////////////////////////////////////////
// NODE PICK FILTER CALLBACK
////////////////////////////////////////////////////////////////////////////////
PickNodeFilterCallback::PickNodeFilterCallback()
   : mCurrentNode(NULL)
{
}

////////////////////////////////////////////////////////////////////////////////
void PickNodeFilterCallback::SetCurrentNode(INode* node)
{
   mCurrentNode = node;
}

////////////////////////////////////////////////////////////////////////////////
BOOL PickNodeFilterCallback::Filter(INode* node)
{
   return mCurrentNode != NULL
      && ! OSGHelper::NodeHasChild(*mCurrentNode, *node) ? TRUE : FALSE;
}



////////////////////////////////////////////////////////////////////////////////
// PICK OBJECT CALLBACK
////////////////////////////////////////////////////////////////////////////////
PickObjectCallback::PickObjectCallback()
   : mEnabled(false)
   , mCurrentHelper(NULL)
   , mPickNodeCallback(new PickNodeFilterCallback)
   , mWinHandle(0)
   , mButtonHandle(0)
{
}

////////////////////////////////////////////////////////////////////////////////
PickObjectCallback::~PickObjectCallback()
{
   delete mPickNodeCallback;
}

////////////////////////////////////////////////////////////////////////////////
void PickObjectCallback::SetCurrentHelper(OSGHelper* helper)
{
   mCurrentHelper = helper;
   mPickNodeCallback->SetCurrentNode(helper==NULL?NULL:helper->mNode);
}

////////////////////////////////////////////////////////////////////////////////
OSGHelper* PickObjectCallback::GetCurrentHelper()
{
   return mCurrentHelper;
}

////////////////////////////////////////////////////////////////////////////////
const OSGHelper* PickObjectCallback::GetCurrentHelper() const
{
   return mCurrentHelper;
}

////////////////////////////////////////////////////////////////////////////////
INode* PickObjectCallback::GetCurrentNode()
{
   return mCurrentHelper == NULL ? NULL : mCurrentHelper->mNode;
}

////////////////////////////////////////////////////////////////////////////////
void PickObjectCallback::SetWindowHandle(HWND hWnd)
{
   mWinHandle = hWnd;
}

////////////////////////////////////////////////////////////////////////////////
void PickObjectCallback::SetButtonHandle(int buttonHandle)
{
   mButtonHandle = buttonHandle;
}

////////////////////////////////////////////////////////////////////////////////
bool PickObjectCallback::IsEnabled() const
{
   return mEnabled;
}

////////////////////////////////////////////////////////////////////////////////
void PickObjectCallback::EnterMode(IObjParam* ip)
{
   mEnabled = true;
   if(mWinHandle != 0)
   {
      ICustButton* pickButton = GetICustButton(GetDlgItem(mWinHandle,mButtonHandle));
      if(pickButton != NULL)
      {
         pickButton->SetType(CBT_CHECK);
         pickButton->SetCheck(TRUE);
         ReleaseICustButton(pickButton);
      }
   }
}

////////////////////////////////////////////////////////////////////////////////
void PickObjectCallback::ExitMode(IObjParam* ip)
{
   mEnabled = false;
   if(mWinHandle != 0)
   {
      ICustButton* pickButton = GetICustButton(GetDlgItem(mWinHandle,mButtonHandle));
      if(pickButton != NULL)
      {
         pickButton->SetCheck(FALSE);
         ReleaseICustButton(pickButton);
      }
   }

   if( mCurrentHelper != NULL )
   {
      mCurrentHelper->UpdateReferencesByLinks();
   }
}

////////////////////////////////////////////////////////////////////////////////
BOOL PickObjectCallback::Pick(IObjParam* ip, ViewExp* viewport)
{
   INode* node = GetCurrentNode();

   if(node != NULL)
   {
      INode* curNode = NULL;
      int hitCount = viewport->HitCount();
      for(int i = 0; i < hitCount; ++i)
      {
         curNode = viewport->GetHit(i);
         if( ! OSGHelper::NodeHasChild(*node, *curNode))
         {
            // Add node to current helper object.
            node->AttachChild(curNode);
         }
      }
   }
   return TRUE;
}

////////////////////////////////////////////////////////////////////////////////
BOOL PickObjectCallback::HitTest(IObjParam* ip, HWND hWnd, ViewExp* viewport,
   IPoint2 point, int flags)
{
   return ip->PickNode(hWnd,point,mPickNodeCallback)? TRUE:FALSE;
}



////////////////////////////////////////////////////////////////////////////////
// HELPER DIALOG PROCEDURE
////////////////////////////////////////////////////////////////////////////////
HelperDlgProc::HelperDlgProc()
   : mCurrentHelper(NULL)
   , mPickObjectCallback(new PickObjectCallback)
{
}

////////////////////////////////////////////////////////////////////////////////
HelperDlgProc::~HelperDlgProc()
{
   delete mPickObjectCallback;
}

////////////////////////////////////////////////////////////////////////////////
BOOL HelperDlgProc::DlgProc(TimeValue t,IParamMap2* map,HWND hWnd,UINT msg,
                            WPARAM wParam,LPARAM lParam)
{
   if(mCurrentHelper == NULL || mCurrentHelper->ip == NULL)
   {
      return FALSE;
   }

   OSGHelper& helper = *mCurrentHelper;
   IObjParam& ip = *helper.ip;

   ICustButton* pickButton;
   ICustButton* mpickButton;
   switch (msg) 
   {
   case WM_INITDIALOG:
      {
         pickButton = GetICustButton(GetDlgItem(hWnd,IDC_PICKNODE));
         mpickButton = GetICustButton(GetDlgItem(hWnd,IDC_MULTIPLE_PICKNODE));
         
         if(pickButton != NULL && mpickButton != NULL)
         {
            if(pickButton->IsEnabled())
               mpickButton->Enable(TRUE);
            else 
               mpickButton->Enable(FALSE);
         }

         ReleaseICustButton(pickButton);
         ReleaseICustButton(mpickButton);
      }
      break;
   case WM_DESTROY:
      break;
   case WM_COMMAND:
      {
         int command = LOWORD(wParam);

         switch (command)
         {
         case IDC_PICKNODE:
            if(helper.IsHierarchalType())
            {
               // Is the pick mode EXITING the custom pick mode callback?
               if(mPickObjectCallback->IsEnabled())
               {
                  // Clear all references to other objects to prevent any potential problems.
                  mPickObjectCallback->SetButtonHandle(0);
                  mPickObjectCallback->SetWindowHandle(0);
                  mPickObjectCallback->SetCurrentHelper(NULL);

                  // This should happen after the IF-statement for this block, and NOT before.
                  // Otherwise the call to IsEnabled will not pass; the mode exits and changes
                  // Enabled to FALSE before the program flow reaches this method.
                  ip.ClearPickMode();
               }
               else // Entering the custom pick mode callback.
               {
                  // Exit the default pick mode so that the custom one can be executed.
                  // This should happen before setting the custom callback, otherwise
                  // ExitMode will be called on the custom callback rather than on the
                  // default callback; this would lead to out-of-synch state for the button.
                  ip.ClearPickMode();

                  // Workaround: The callback will toggle the button state manually.
                  // Pass handles to access the specific button.
                  mPickObjectCallback->SetButtonHandle(command);
                  mPickObjectCallback->SetWindowHandle(hWnd);

                  // Set the custom callback and pass it a reference to the
                  // current helper object.
                  ip.SetPickMode(mPickObjectCallback);
                  mPickObjectCallback->SetCurrentHelper(&helper);
               }
            }
            break;

         case IDC_NODE_MOVE_UP:
            if(helper.IsHierarchalType())
            {
               helper.MoveNodeItem(hWnd,-1);
            }
            break;

         case IDC_NODE_MOVE_DOWN:
            if(helper.IsHierarchalType())
            {
               helper.MoveNodeItem(hWnd,1);
            }
            break;

         case IDC_SWITCH_REMNODE:
            if(helper.IsHierarchalType())
            {
               mCurrentHelper->DetachUnreferencedObjects();
            }
            break;
         case IDC_MULTIPLE_PICKNODE:
            if(map != NULL)
            {
               MultipleSelectDlg* multiSelectDialog = new MultipleSelectDlg(map);
               multiSelectDialog->SetCurrentHelper(mCurrentHelper);
               ip.DoHitByNameDialog(multiSelectDialog);
            }
            break;
         default:
            break;
         }
      }
      break;
   default:
      break;
   }
   return FALSE;
}

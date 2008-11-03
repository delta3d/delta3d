#include <prefix/dtcoreprefix-src.h>
#include <dtCore/pointaxis.h>
#include <dtCore/scene.h>
#include <dtCore/camera.h>//due to include of scene.h
#include <dtCore/keyboardmousehandler.h> //due to include of scene.h
#include <dtCore/odegeomwrap.h>
#include <dtCore/transformable.h>
#include <dtCore/collisioncategorydefaults.h>
#include <dtUtil/log.h>
#include <dtUtil/matrixutil.h>

#include <osg/Geode>
#include <osg/MatrixTransform>
#include <osg/NodeVisitor>
#include <osg/Material>
#include <osg/PolygonMode>
#include <osg/PolygonOffset>
#include <osg/ShapeDrawable>
#include <osg/StateSet>
#include <osg/Version> // For #ifdef


#if defined(OSG_VERSION_MAJOR) && defined(OSG_VERSION_MINOR) && OSG_VERSION_MAJOR == 1 && OSG_VERSION_MINOR == 0
#include <osg/CameraNode>
#endif

#include <cassert>

using namespace dtCore;
using namespace dtUtil;

IMPLEMENT_MANAGEMENT_LAYER(Transformable)

const std::string Transformable::COLLISION_GEODE_ID("__DELTA3D_COLLISION_GEOMETRY__");

///////////////////////////////////////////////////////////////////////////////
IMPLEMENT_ENUM(Transformable::CollisionGeomType)
Transformable::CollisionGeomType
Transformable::CollisionGeomType::NONE("NONE");
Transformable::CollisionGeomType
Transformable::CollisionGeomType::SPHERE("SPHERE");
Transformable::CollisionGeomType
Transformable::CollisionGeomType::CYLINDER("CYLINDER");
Transformable::CollisionGeomType
Transformable::CollisionGeomType::CUBE("CUBE");
Transformable::CollisionGeomType
Transformable::CollisionGeomType::RAY("RAY");
Transformable::CollisionGeomType
Transformable::CollisionGeomType::MESH("MESH");


namespace dtCore
{

   /** Custom NodeVisitor used to collect the parent NodePaths of a Node.
     * This Visitor will only traverse osg::Transforms and is used to
     * calculate a Node's absolute coordinate.  It sets a node mask
     * override in case any of the parent Nodes' masks are un-traversable.
     * Modified from osg::CollectParentPaths class.
     */
   class CollectParentPaths : public osg::NodeVisitor
   {
   public:
      CollectParentPaths(osg::Node* haltTraversalAtNode=0)
         : osg::NodeVisitor(osg::NodeVisitor::TRAVERSE_PARENTS)
         , _haltTraversalAtNode(haltTraversalAtNode)
      {
         //this will force the traversal over nodes that have a mask of 0x0
         this->setNodeMaskOverride(0xffffffff);
      }

      virtual void apply(osg::Node& node)
      {
         if (node.getNumParents()==0 || &node==_haltTraversalAtNode)
         {
            _nodePaths.push_back(getNodePath());
         }
         else
         {
            traverse(node);
         }
      }

      osg::Node*        _haltTraversalAtNode;
      osg::NodePathList _nodePaths;
   };
}

/////////////////////////////////////////////////////////////
Transformable::Transformable(const std::string& name)
   : DeltaDrawable(name)
   , mGeomWrap(new ODEGeomWrap())
   , mGeomGeod(NULL)
   , mNode(new TransformableNode)
   , mRenderingGeometry(false)
   , mRenderProxyNode(false)
{
   //set the name of the node
   mNode->setName(GetName());

   Ctor();
}

/////////////////////////////////////////////////////////////
Transformable::Transformable(TransformableNode& node, const std::string& name)
   : DeltaDrawable(name)
   , mGeomWrap(new ODEGeomWrap())
   , mGeomGeod(NULL)
   , mNode(&node)
   , mRenderingGeometry(false)
   , mRenderProxyNode(false)
{
   Ctor();
}

/////////////////////////////////////////////////////////////
void Transformable::Ctor()
{
   RegisterInstance(this);

   SetNormalRescaling( true );

   SetCollisionCategoryBits(COLLISION_CATEGORY_MASK_TRANSFORMABLE);

   // By default, collide with all categories.
   SetCollisionCollideBits(COLLISION_CATEGORY_MASK_ALL);
}

/////////////////////////////////////////////////////////////
Transformable::~Transformable()
{
   mGeomWrap = NULL;

   DeregisterInstance(this);
}

/////////////////////////////////////////////////////////////
osg::Node* Transformable::GetOSGNode()
{
   return mNode.get();
}

/////////////////////////////////////////////////////////////
const osg::Node* Transformable::GetOSGNode() const
{
   return mNode.get();
}

/////////////////////////////////////////////////////////////
void Transformable::ReplaceMatrixNode(TransformableNode* matrixTransform)
{
   if (matrixTransform == NULL)
   {
      LOG_ERROR("The matrix node may not be set to NULL.");
      return;
   }

   // Save off the parent
   RefPtr<DeltaDrawable> oldParent;
   RefPtr<Scene> oldScene;
   if(GetParent() != NULL)
   {
      oldParent = GetParent();
      GetParent()->RemoveChild(this);
   }
   else if(GetSceneParent() != NULL)
   {
      // If the parent is NULL, but the Scene is not, then we
      // know this has been added directly to the scene root.
      oldScene = GetSceneParent();
      GetSceneParent()->RemoveDrawable(this);
   }

   // Save off each of the children
   typedef std::vector<RefPtr<DeltaDrawable> > DrawableVector;
   DrawableVector children;
   for(unsigned i = 0; i < GetNumChildren(); ++i)
   {
      children.push_back(GetChild(i));
   }

   // Then actually remove them from the old node. This is not done
   // in the previous loop or else GetChild(++i) will return a bad value.
   for(DrawableVector::iterator iter = children.begin();
       iter != children.end();
       ++iter)
   {
      RemoveChild( iter->get() );
   }

   // Save preseve normal rescaling property
   bool normalRescaling = GetNormalRescaling();

   // Save whether or not we are rendering the proxy node
   bool usingProxyNode(false);
   if(GetMatrixNode()->containsNode(GetProxyNode()))
   {
      usingProxyNode = true;
      RenderProxyNode(false);
   }

   // Replace the node pointer
   mNode = matrixTransform;

   // Preseve normal rescaling property
   SetNormalRescaling(normalRescaling);

   // Re-enable proxy node if it was on
   if(usingProxyNode)
   {
      RenderProxyNode(true);
   }

   // Now enable the rendering of collision geometry if that was on
   RenderCollisionGeometry(GetRenderCollisionGeometry());

   // Next, add back all the children
   for(DrawableVector::iterator iter = children.begin();
       iter != children.end();
       ++iter)
   {
      AddChild(iter->get());
   }

   // Finally, add this as a child of its old parent. Delta3D enforces a
   // single-parent hierarchy.
   if(oldParent.valid())
   {
      oldParent->AddChild(this);
   }
   else if(oldScene.valid())
   {
      oldScene->AddDrawable(this);
   }
}

/////////////////////////////////////////////////////////////
bool Transformable::GetAbsoluteMatrix(const osg::Node* node, osg::Matrix& wcMatrix)
{
   if(node != NULL)
   {
      dtCore::CollectParentPaths cpp(NULL);
      const_cast<osg::Node*>(node)->accept(cpp);
      osg::NodePathList nodePathList = cpp._nodePaths;

      if(!nodePathList.empty())
      {
         osg::NodePath nodePath = nodePathList[0];

         #if defined(OSG_VERSION_MAJOR) && defined(OSG_VERSION_MINOR) && OSG_VERSION_MAJOR == 1 && OSG_VERSION_MINOR == 0
         // Luckily, this behavior is redundant with OSG 1.1
         if (dynamic_cast<osg::CameraNode*>(nodePath[0]) != NULL)
         {
            nodePath = osg::NodePath( nodePath.begin()+1, nodePath.end() );
         }
         #endif // OSG 1.1

         wcMatrix.set(osg::computeLocalToWorld(nodePath));
         return true;
      }
   }

   return false;
}

////////////////////////////////////////////////////////////////////////////
void Transformable::SetTransform(const Transform& xform, CoordSysEnum cs)
{
   osg::Matrix newMat;
   xform.Get(newMat);

   if(cs == ABS_CS)
   {
      //convert the xform into a Relative CS as the MatrixNode is always
      //in relative coords

      //if this has a parent
      if (!GetOSGNode()->getParents().empty())
      {
         //get the parent's world position
         osg::Matrix parentMat;
         GetAbsoluteMatrix(GetOSGNode()->getParent(0), parentMat);

         //calc the difference between xform and the parent's world position
         //child * parent^-1
         osg::Matrix relMat = newMat * osg::Matrix::inverse(parentMat);

         //pass the rel matrix to this node
         GetMatrixNode()->setMatrix(relMat);
      }
      else
      {
         //pass the xform to the this node
         GetMatrixNode()->setMatrix(newMat);
      }
   }
   else if(cs == REL_CS)
   {
     GetMatrixNode()->setMatrix(newMat);
   }

   PrePhysicsStepUpdate();
}

////////////////////////////////////////////////////////////////////////////
void Transformable::GetTransform(Transform& xform, CoordSysEnum cs) const
{
   const TransformableNode* mt = GetMatrixNode();

   if(cs == ABS_CS)
   {
      osg::Matrix newMat;
      GetAbsoluteMatrix(mt, newMat);
      xform.Set( newMat );
   }
   else if(cs == REL_CS)
   {
     xform.Set(mt->getMatrix());
   }

}

////////////////////////////////////////////////////////////////////////////
const osg::Matrix& Transformable::GetMatrix() const
{
   return mNode->getMatrix();
}

////////////////////////////////////////////////////////////////////////////
void Transformable::SetMatrix(const osg::Matrix& mat)
{
   mNode->setMatrix(mat);
}

////////////////////////////////////////////////////////////////////////////
bool Transformable::AddChild(DeltaDrawable* child)
{
   // Add the child's node to our's
   if (DeltaDrawable::AddChild(child))
   {
      GetMatrixNode()->addChild(child->GetOSGNode());
      return true;
   }
   else
   {
      return false;
   }
}

////////////////////////////////////////////////////////////////////////////
void Transformable::RemoveChild(DeltaDrawable* child)
{
   GetMatrixNode()->removeChild(child->GetOSGNode());
   DeltaDrawable::RemoveChild(child);
}

////////////////////////////////////////////////////////////////////////////
void Transformable::RenderProxyNode(const bool enable)
{
   if(enable)
   {
      // Make sphere
      float radius = 0.5f;

      osg::Sphere* sphere = new osg::Sphere(osg::Vec3( 0.0, 0.0, 0.0 ), radius);

      osg::Geode* proxyGeode = new osg::Geode();
      SetProxyNode(proxyGeode);

      osg::TessellationHints* hints = new osg::TessellationHints;
      hints->setDetailRatio(0.5f);

      osg::ShapeDrawable* sd = new osg::ShapeDrawable(sphere, hints);

      proxyGeode->addDrawable(sd);

      osg::Material* mat = new osg::Material();
      mat->setDiffuse(osg::Material::FRONT_AND_BACK, osg::Vec4(1.0f, 0.0f, 1.0f, 0.5f));
      mat->setAmbient(osg::Material::FRONT_AND_BACK, osg::Vec4(1.0f, 0.0f, 1.0f, 1.0f));
      mat->setEmission(osg::Material::FRONT_AND_BACK, osg::Vec4(0.0f, 0.0f, 0.0f, 1.0f));

      osg::PolygonOffset* polyoffset = new osg::PolygonOffset;
      polyoffset->setFactor(-1.0f);
      polyoffset->setUnits(-1.0f);

      osg::StateSet *ss = GetProxyNode()->getOrCreateStateSet();
      ss->setAttributeAndModes(mat, osg::StateAttribute::OVERRIDE|osg::StateAttribute::ON);
      ss->setMode(GL_BLEND, osg::StateAttribute::ON);
      ss->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);
      ss->setAttributeAndModes(polyoffset, osg::StateAttribute::OVERRIDE | osg::StateAttribute::ON);

      GetMatrixNode()->addChild(GetProxyNode());

      PointAxis* paxis = new PointAxis();
      paxis->Enable(PointAxis::X);
      paxis->Enable(PointAxis::Y);
      paxis->Enable(PointAxis::Z);
      paxis->Enable(PointAxis::LABEL_X);
      paxis->Enable(PointAxis::LABEL_Y);
      paxis->Enable(PointAxis::LABEL_Z);

      AddChild(paxis);
   }
   else
   {
      GetMatrixNode()->removeChild(GetProxyNode());
   }

   mRenderProxyNode = enable;
}

////////////////////////////////////////////////////////////////////////////
void Transformable::SetNormalRescaling(const bool enable)
{
   osg::StateAttribute::GLModeValue state;
   state = (enable) ? osg::StateAttribute::ON : osg::StateAttribute::OFF;

   if(GetOSGNode() != NULL)
   {
      GetOSGNode()->getOrCreateStateSet()->setMode(GL_RESCALE_NORMAL, state);
   }
}

////////////////////////////////////////////////////////////////////////////
bool Transformable::GetNormalRescaling() const
{
   if(GetOSGNode() == NULL)
   {
      return false;
   }

   osg::StateAttribute::GLModeValue state = GetOSGNode()->getStateSet()->getMode(GL_RESCALE_NORMAL);
   return (state & osg::StateAttribute::ON) ? true : false;
}

/////////////////////////////////////////////////////////////////////////////////
Transformable::CollisionGeomType* Transformable::GetCollisionGeomType() const
{
   //ugly bit of code used to convert the ODEGeomWrap enums to Transformable enums.
   //This is here to keep from breaking existing Transformable clients.

   if (mGeomWrap->GetCollisionGeomType() == &dtCore::CollisionGeomType::NONE)
   {
      return &dtCore::Transformable::CollisionGeomType::NONE;
   }
   else if (mGeomWrap->GetCollisionGeomType() == &dtCore::CollisionGeomType::SPHERE)
   {
      return &dtCore::Transformable::CollisionGeomType::SPHERE;
   }
   else if (mGeomWrap->GetCollisionGeomType() == &dtCore::CollisionGeomType::CYLINDER)
   {
      return &dtCore::Transformable::CollisionGeomType::CYLINDER;
   }
   else if (mGeomWrap->GetCollisionGeomType() == &dtCore::CollisionGeomType::CUBE)
   {
      return &dtCore::Transformable::CollisionGeomType::CUBE;
   }
   else if (mGeomWrap->GetCollisionGeomType() == &dtCore::CollisionGeomType::RAY)
   {
      return &dtCore::Transformable::CollisionGeomType::RAY;
   }
   else if (mGeomWrap->GetCollisionGeomType() == &dtCore::CollisionGeomType::MESH)
   {
      return &dtCore::Transformable::CollisionGeomType::MESH;
   }
   else
   {
      return &dtCore::Transformable::CollisionGeomType::NONE;
   }
}

///////////////////////////////////////////////////////////////////////////////////////
void Transformable::GetCollisionGeomDimensions(std::vector<float>& dimensions)
{
   // Sync up ODE with our OSG transforms.
   PrePhysicsStepUpdate();

   mGeomWrap->GetCollisionGeomDimensions(dimensions);
 }

///////////////////////////////////////////////////////////////////////
void Transformable::SetCollisionCategoryBits(unsigned long bits)
{
   mGeomWrap->SetCollisionCategoryBits(bits);
}

////////////////////////////////////////////////////////////////////////
unsigned long Transformable::GetCollisionCategoryBits() const
{
   return mGeomWrap->GetCollisionCategoryBits();
}

//////////////////////////////////////////////////////////////////////////////
void Transformable::SetCollisionCollideBits(unsigned long bits)
{
   mGeomWrap->SetCollisionCollideBits(bits);
}

////////////////////////////////////////////////////////////////////////////////
unsigned long Transformable::GetCollisionCollideBits() const
{
   return mGeomWrap->GetCollisionCollideBits();
}


/////////////////////////////////////////////////////////////////////////////////
void Transformable::SetCollisionDetection(bool enabled)
{
   mGeomWrap->SetCollisionDetection(enabled);
}

////////////////////////////////////////////////////////////////
bool Transformable::GetCollisionDetection() const
{
   return mGeomWrap->GetCollisionDetection();
}


dGeomID Transformable::GetGeomID() const
{
   return mGeomWrap->GetGeomID();
}


////////////////////////////////////////////////////////////////
void Transformable::SetCollisionGeom(dGeomID geom)
{
   mGeomWrap->SetCollisionGeom(geom);

   RenderCollisionGeometry(mRenderingGeometry);

   // Sync-up the transforms on mGeomID
   PrePhysicsStepUpdate();
}

////////////////////////////////////////////////////////////////
void Transformable::SetCollisionSphere(float radius)
{
   mGeomWrap->SetCollisionSphere(radius);

   RenderCollisionGeometry(mRenderingGeometry);

   mGeomWrap->SetCollisionDetection(true);

   // Sync-up the transforms on mGeomID
   PrePhysicsStepUpdate();
}


////////////////////////////////////////////////////////////////
void Transformable::SetCollisionSphere(osg::Node* node)
{
   if(node == 0)
   {
      node = GetOSGNode();
   }

   if(node)
   {
      // Hmm, do we even need this here? I think this is a hack
      // to overcome a bug in DrawableVisitor... -osb
      osg::Matrix oldMatrix = GetMatrixNode()->getMatrix();
      GetMatrixNode()->setMatrix(osg::Matrix::identity());

      mGeomWrap->SetCollisionSphere(node);

      GetMatrixNode()->setMatrix(oldMatrix);

      RenderCollisionGeometry(mRenderingGeometry);

      mGeomWrap->SetCollisionDetection(true);

      // Sync-up the transforms on mGeomID
      PrePhysicsStepUpdate();
   }
}

/////////////////////////////////////////////////////////////////////
void Transformable::SetCollisionBox(float lx, float ly, float lz)
{
   mGeomWrap->SetCollisionBox(lx, ly, lz);

   RenderCollisionGeometry(mRenderingGeometry);
   SetCollisionDetection(true);

   // Sync-up the transforms on mGeomID
   PrePhysicsStepUpdate();
}

/////////////////////////////////////////////////////////////////////
void Transformable::SetCollisionBox(osg::Node* node)
{
   if(node == 0)
   {
      node = GetOSGNode();
   }

   if(node != 0)
   {
      osg::Matrix oldMatrix = GetMatrixNode()->getMatrix();
      GetMatrixNode()->setMatrix(osg::Matrix::identity());

      mGeomWrap->SetCollisionBox(node);

      GetMatrixNode()->setMatrix(oldMatrix);

      RenderCollisionGeometry(mRenderingGeometry);
      SetCollisionDetection(true);

      PrePhysicsStepUpdate();
   }
}

/////////////////////////////////////////////////////////////////////
void Transformable::SetCollisionCappedCylinder(float radius, float length)
{
   mGeomWrap->SetCollisionCappedCylinder(radius, length);

   RenderCollisionGeometry(mRenderingGeometry);
   SetCollisionDetection(true);

   // Sync-up the transforms on mGeomID
   PrePhysicsStepUpdate();
}


/////////////////////////////////////////////////////////////////////
void Transformable::SetCollisionCappedCylinder(osg::Node* node)
{
   if(node == 0)
   {
      node = this->GetOSGNode();
   }

   if(node)
   {
      osg::Matrix oldMatrix = GetMatrixNode()->getMatrix();
      GetMatrixNode()->setMatrix(osg::Matrix::identity());

      mGeomWrap->SetCollisionCappedCylinder(node);

      GetMatrixNode()->setMatrix(oldMatrix);

      RenderCollisionGeometry(mRenderingGeometry);
      SetCollisionDetection(true);

      // Sync-up the transforms on mGeomID
      PrePhysicsStepUpdate();
   }
}

/////////////////////////////////////////////////////////////////////
void Transformable::SetCollisionRay(float length)
{
   mGeomWrap->SetCollisionRay(length);

   RenderCollisionGeometry(mRenderingGeometry);
   SetCollisionDetection(true);

   // Sync-up the transforms on mGeomID
   PrePhysicsStepUpdate();
}


/////////////////////////////////////////////////////////////////////
void Transformable::SetCollisionMesh(osg::Node* node)
{
   if(node == 0)
   {
      node = GetOSGNode();
   }

   if(node)
   {
      //the following is a workaround to temporarily bypass this Physical's Transform
      //At this point, we'll set it temporarily to the Identity so it doesn't affect
      //our creation of the collision mesh.  This Transform will be accounted
      //for later in PrePhysicsUpdate().
      osg::Matrix oldMatrix = GetMatrixNode()->getMatrix();
      GetMatrixNode()->setMatrix(osg::Matrix::identity());

      mGeomWrap->SetCollisionMesh(node);

      GetMatrixNode()->setMatrix(oldMatrix);

      RenderCollisionGeometry(mRenderingGeometry);
      SetCollisionDetection(true);
   }
}

/////////////////////////////////////////////////////////////
void Transformable::ClearCollisionGeometry()
{
   mGeomWrap->ClearCollisionGeometry();

   //If the collision geometry is valid, this implies the user has
   //enabled render collision geometry.  Therefore, we just remove
   //the drawables from the geode.  When the user turns off render
   //collision geometry, that will remove the geode from this node.
   if(mGeomGeod.valid())
   {
      #if defined(OSG_VERSION_MAJOR) && defined(OSG_VERSION_MINOR) && OSG_VERSION_MAJOR >= 1 && OSG_VERSION_MINOR >= 1
      mGeomGeod->removeDrawables(0,mGeomGeod->getNumDrawables());
      #else
      mGeomGeod->removeDrawable(0,mGeomGeod->getNumDrawables());
      #endif
   }
}

/////////////////////////////////////////////////////////////
void Transformable::PrePhysicsStepUpdate()
{
   if (mGeomWrap->GetCollisionDetection() == false) {return;}

   Transform transform;

   this->GetTransform(transform, Transformable::ABS_CS);

   mGeomWrap->UpdateGeomTransform(transform);
}

/////////////////////////////////////////////////////////////
void Transformable::RenderCollisionGeometry(bool enable)
{
   TransformableNode *xform = this->GetMatrixNode();

   if(!xform)
   {
      return;
   }

   mRenderingGeometry = enable;

   if(enable)
   {
      //If there is already an existing rendering of the collision geometry,
      //remove it before adding a new one.
      if (mGeomGeod.valid())
      {
         RemoveRenderedCollisionGeometry();
      }

      mGeomGeod = mGeomWrap->CreateRenderedCollisionGeometry();

      if (mGeomGeod.valid())
      {
         mGeomGeod->setName(Transformable::COLLISION_GEODE_ID);

         xform->addChild(mGeomGeod.get());
      }
   } //end if enabled==true
   else
   {
      this->RemoveRenderedCollisionGeometry();
   }
}

/////////////////////////////////////////////////////////////
void Transformable::AddedToScene(Scene* scene)
{
   if(scene)
   {
      //remove us from our existing parent scene, if we already have one.
      //TODO This ends up calling AddedToScene again with a 0.  Is this bad?
      if(GetSceneParent())
      {
         GetSceneParent()->RemoveDrawable(this);
      }

      DeltaDrawable::AddedToScene(scene);
      scene->RegisterCollidable(this);
   }
   else
   {
      if(GetSceneParent())
      {
         GetSceneParent()->UnRegisterCollidable(this);
      }
      DeltaDrawable::AddedToScene(NULL);
   }
}

///////////////////////////////////////////////////////////////
void Transformable::RemoveRenderedCollisionGeometry()
{
   if(mGeomGeod.valid())
   {
      GetMatrixNode()->removeChild(mGeomGeod.get());
      mGeomGeod = 0;
   }
}

//////////////////////////////////////////////////////////////////////////
const ODEGeomWrap* Transformable::GetGeomWrapper() const
{
   return mGeomWrap.get();
}

//////////////////////////////////////////////////////////////////////////
ODEGeomWrap* Transformable::GetGeomWrapper()
{
   return mGeomWrap.get();
}

#include <prefix/dtcoreprefix.h>
#include <dtCore/pointaxis.h>
#include <dtCore/scene.h>
#include <dtCore/odegeomwrap.h>
#include <dtCore/transformable.h>
#include <dtCore/transform.h>
#include <dtCore/collisioncategorydefaults.h>
#include <dtUtil/log.h>
#include <dtUtil/matrixutil.h>
#include <dtUtil/nodemask.h>

#include <osg/Geode>
#include <osg/MatrixTransform>
#include <osg/NodeVisitor>
#include <osg/Material>
#include <osg/PolygonMode>
#include <osg/PolygonOffset>
#include <osg/ShapeDrawable>
#include <osg/StateSet>
#include <osg/Version> // For #ifdef

#include <cassert>

using namespace dtCore;
using namespace dtUtil;

IMPLEMENT_MANAGEMENT_LAYER(Transformable)

const std::string Transformable::COLLISION_GEODE_ID("__DELTA3D_COLLISION_GEOMETRY__");


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
         this->setNodeMaskOverride(dtUtil::NodeMask::EVERYTHING);
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
/////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////
namespace dtCore
{
   class TransformableImpl
   {
   public:
      TransformableImpl(Transformable::TransformableNode& node)
      : mGeomWrap(new ODEGeomWrap())
      , mGeomGeod(NULL)
      , mNode(&node)
      , mRenderingGeometry(false)
      , mRenderProxyNode(false)
      {

      }
      dtCore::RefPtr<ODEGeomWrap> mGeomWrap;


      /**
       *  Pointer to the collision geometry representation
       */
      RefPtr<osg::Geode> mGeomGeod;

      /**
      * The node passed on GetOSGNode()
      */
      RefPtr<Transformable::TransformableNode> mNode;

      /**
       * If we're rendering the collision geometry.
       */
      bool mRenderingGeometry;

      /**
       * If we're rendering the proxy node
       */
      bool mRenderProxyNode;

      ///used for the rendering of the proxy node
      dtCore::RefPtr<PointAxis> mPointAxis;

   };
}
/////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
Transformable::Transformable(const std::string& name)
   : DeltaDrawable(name)
   , mImpl(new TransformableImpl(*new TransformableNode))
{
   //set the name of the node
   mImpl->mNode->setName(GetName());

   Ctor();
}

////////////////////////////////////////////////////////////////////////////////
Transformable::Transformable(TransformableNode& node, const std::string& name)
   : DeltaDrawable(name)
   , mImpl(new TransformableImpl(node))
{
   //Should we set the name on the node now?
   Ctor();
}

////////////////////////////////////////////////////////////////////////////////
void Transformable::Ctor()
{
   RegisterInstance(this);

   SetNormalRescaling(true);

   SetCollisionCategoryBits(COLLISION_CATEGORY_MASK_TRANSFORMABLE);

   // By default, collide with all categories.
   SetCollisionCollideBits(COLLISION_CATEGORY_MASK_ALL);
}

////////////////////////////////////////////////////////////////////////////////
Transformable::~Transformable()
{
   mImpl->mGeomWrap = NULL;
   if (mImpl->mPointAxis.valid())
   {
      SetProxyNode(NULL);
      RemoveChild(mImpl->mPointAxis.get());
      mImpl->mPointAxis = NULL;
   }

   DeregisterInstance(this);

   delete mImpl;
   mImpl = NULL;
}

////////////////////////////////////////////////////////////////////////////////
osg::Node* Transformable::GetOSGNode()
{
   return mImpl->mNode.get();
}

////////////////////////////////////////////////////////////////////////////////
const osg::Node* Transformable::GetOSGNode() const
{
   return mImpl->mNode.get();
}

////////////////////////////////////////////////////////////////////////////////
bool GetAbsoluteMatrixOld(const osg::Node* node, osg::Matrix& wcMatrix)
{
   if(node != NULL)
   {
      dtCore::CollectParentPaths cpp(NULL);
      const_cast<osg::Node*>(node)->accept(cpp);
      const osg::NodePathList& nodePathList = cpp._nodePaths;

      if(!nodePathList.empty())
      {
         const osg::NodePath& nodePath = nodePathList[0];

         wcMatrix.set(osg::computeLocalToWorld(nodePath));
         return true;
      }
   }

   return false;
}

// Debug class that lets you get statistics on exit for how many nodes you have in the tree.
//class DepthStorage
//{
//public:
//   std::vector<size_t> vectorSizes;
//   ~DepthStorage()
//   {
//      for (unsigned i = 0; i < vectorSizes.size(); ++i)
//      {
//         printf("Number of objects with \"%u\" depth: %lu \n", i, vectorSizes[i]);
//      }
//   }
//};
//
//static DepthStorage depthStorage;

////////////////////////////////////////////////////////////////////////////////
bool GetAbsoluteMatrixNew(const osg::Node* node, osg::Matrix& wcMatrix)
{
   if (node != NULL)
   {
      wcMatrix.makeIdentity();

      std::vector<osg::Transform*>  nodePath;
      // most things have very small depths, but allocating 15 didn't seem to be any slower
      // than allocating 5, and it's just pointers, so I made it 15 to account for some pretty
      // deep trees.
      nodePath.reserve(15U);
      nodePath.clear();

      osg::Node* curNode = const_cast<osg::Node*>(node);
      while (curNode != NULL)
      {
         osg::Transform* txNode = curNode->asTransform();
         if (txNode != NULL)
         {
#if defined(OSG_VERSION_GREATER_OR_EQUAL)
  #if OSG_VERSION_GREATER_OR_EQUAL(3, 1, 0)
            // This depends on a submission to osg, but it's much faster than the
            // dynamic cast below
            osg::Camera* camera = txNode->asCamera();
  #else
            osg::Camera* camera = dynamic_cast<osg::Camera*>(curNode);
  #endif
#else
            osg::Camera* camera = dynamic_cast<osg::Camera*>(curNode);
#endif

            if (camera != NULL && (camera->getReferenceFrame() != osg::Transform::RELATIVE_RF || camera->getNumParents() == 0))
            {
               curNode = NULL;
            }
            else
            {
               // only put transforms in the node path.
               nodePath.push_back(txNode);
            }
         }

         if (curNode != NULL && curNode->getNumParents() > 0U)
         {
            curNode = curNode->getParent(0);
            // Stop when you get to a camera.
         }
         else
         {
            curNode = NULL;
         }
      }

// This is debug code to get statistics on the depth of hierarchies.
//      if (nodePath.size() > depthStorage.vectorSizes.size() + 1U)
//      {
//         depthStorage.vectorSizes.resize(nodePath.size() + 1, 0U);
//         printf("New larger reserve size %lu\n", nodePath.size());
//      }

//      ++depthStorage.vectorSizes[nodePath.size()];

      if (!nodePath.empty())
      {
         std::vector<osg::Transform*>::reverse_iterator i = nodePath.rbegin(), iend = nodePath.rend();
         for (; i != iend; ++i)
         {
            osg::Transform* curTran = *i;
            // Print out the transform hierarchy.
            //printf("node: %s - ", curTran->getName().c_str());
            if (curTran != NULL && !curTran->computeLocalToWorldMatrix(wcMatrix, NULL))
            {
               break;
            }
         }
         // Print out the transform hierarchy.
         //printf("\n");

         return true;
      }
   }

   return false;
}

namespace dtCore
{
   bool DT_CORE_EXPORT UseNewAbsoluteMatrixCode = true;
}

////////////////////////////////////////////////////////////////////////////////
bool Transformable::GetAbsoluteMatrix(const osg::Node* node, osg::Matrix& wcMatrix)
{
   if (UseNewAbsoluteMatrixCode)
   {
      return GetAbsoluteMatrixNew(node, wcMatrix);
   }
   else
   {
      return GetAbsoluteMatrixOld(node, wcMatrix);
   }
}


////////////////////////////////////////////////////////////////////////////////
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

////////////////////////////////////////////////////////////////////////////////
void Transformable::GetTransform(Transform& xform, CoordSysEnum cs) const
{
   const TransformableNode* mt = GetMatrixNode();

   if(cs == ABS_CS)
   {
      osg::Matrix newMat;
      GetAbsoluteMatrix(mt, newMat);
      xform.Set(newMat);
   }
   else if(cs == REL_CS)
   {
     xform.Set(mt->getMatrix());
   }

}

////////////////////////////////////////////////////////////////////////////////
Transformable::TransformableNode* Transformable::GetMatrixNode()
{
   return mImpl->mNode.get();
}

////////////////////////////////////////////////////////////////////////////////
const Transformable::TransformableNode* Transformable::GetMatrixNode() const
{
   return mImpl->mNode.get();
}

////////////////////////////////////////////////////////////////////////////////
const osg::Matrix& Transformable::GetMatrix() const
{
   return mImpl->mNode->getMatrix();
}

////////////////////////////////////////////////////////////////////////////////
void Transformable::SetMatrix(const osg::Matrix& mat)
{
   mImpl->mNode->setMatrix(mat);
}

////////////////////////////////////////////////////////////////////////////////
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

////////////////////////////////////////////////////////////////////////////////
void Transformable::RemoveChild(DeltaDrawable* child)
{
   GetMatrixNode()->removeChild(child->GetOSGNode());
   DeltaDrawable::RemoveChild(child);
}

////////////////////////////////////////////////////////////////////////////////
void Transformable::RenderProxyNode(const bool enable)
{
   if (enable == GetIsRenderingProxyNode())
   {
      return; //nothing to do here
   }

   if(enable)
   {
      mImpl->mPointAxis = new PointAxis();
      mImpl->mPointAxis->Enable(PointAxis::X);
      mImpl->mPointAxis->Enable(PointAxis::Y);
      mImpl->mPointAxis->Enable(PointAxis::Z);
      mImpl->mPointAxis->Enable(PointAxis::LABEL_X);
      mImpl->mPointAxis->Enable(PointAxis::LABEL_Y);
      mImpl->mPointAxis->Enable(PointAxis::LABEL_Z);

      // Make sphere
      float radius = 0.5f;

      osg::Sphere* sphere = new osg::Sphere(osg::Vec3(0.0f, 0.0f, 0.0f), radius);

      osg::Geode* proxyGeode = new osg::Geode();
      mImpl->mPointAxis->GetMatrixNode()->addChild(proxyGeode);

      osg::TessellationHints* hints = new osg::TessellationHints;
      hints->setDetailRatio(0.5f);

      osg::ShapeDrawable* sd = new osg::ShapeDrawable(sphere, hints);
      sd->setColor(osg::Vec4(1.0f, 0.0f, 1.0f, 0.5f));

      proxyGeode->addDrawable(sd);

      osg::StateSet* ss = proxyGeode->getOrCreateStateSet();
      ss->setMode(GL_BLEND, osg::StateAttribute::PROTECTED | osg::StateAttribute::ON);
      ss->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);
      ss->setMode(GL_LIGHTING, osg::StateAttribute::PROTECTED | osg::StateAttribute::OFF);

      AddChild(mImpl->mPointAxis.get());
      SetProxyNode(mImpl->mPointAxis->GetOSGNode());
   }
   else
   {
      if (mImpl->mPointAxis.valid())
      {
         RemoveChild(mImpl->mPointAxis.get());
         mImpl->mPointAxis = NULL;
      }
   }

   mImpl->mRenderProxyNode = enable;
}

////////////////////////////////////////////////////////////////////////////////
bool Transformable::GetIsRenderingProxyNode() const
{
   return mImpl->mRenderProxyNode;
}


////////////////////////////////////////////////////////////////////////////////
void Transformable::SetNormalRescaling(const bool enable)
{
   osg::StateAttribute::GLModeValue state;
   state = (enable) ? osg::StateAttribute::ON : osg::StateAttribute::OFF;

   if(GetOSGNode() != NULL)
   {
      GetOSGNode()->getOrCreateStateSet()->setMode(GL_RESCALE_NORMAL, state);
   }
}

////////////////////////////////////////////////////////////////////////////////
bool Transformable::GetNormalRescaling() const
{
   if (GetOSGNode() != NULL && GetOSGNode()->getStateSet() != NULL)
   {
      osg::StateAttribute::GLModeValue state = GetOSGNode()->getStateSet()->getMode(GL_RESCALE_NORMAL);
      return (state & osg::StateAttribute::ON) ? true : false;
   }
   return false;
}

////////////////////////////////////////////////////////////////////////////////
CollisionGeomType* Transformable::GetCollisionGeomType() const
{
   //ugly bit of code used to convert the ODEGeomWrap enums to Transformable enums.
   //This is here to keep from breaking existing Transformable clients.

   if (mImpl->mGeomWrap->GetCollisionGeomType() == &dtCore::CollisionGeomType::NONE)
   {
      return &dtCore::CollisionGeomType::NONE;
   }
   else if (mImpl->mGeomWrap->GetCollisionGeomType() == &dtCore::CollisionGeomType::SPHERE)
   {
      return &dtCore::CollisionGeomType::SPHERE;
   }
   else if (mImpl->mGeomWrap->GetCollisionGeomType() == &dtCore::CollisionGeomType::CYLINDER)
   {
      return &dtCore::CollisionGeomType::CYLINDER;
   }
   else if (mImpl->mGeomWrap->GetCollisionGeomType() == &dtCore::CollisionGeomType::CCYLINDER)
   {
      return &dtCore::CollisionGeomType::CCYLINDER;
   }
   else if (mImpl->mGeomWrap->GetCollisionGeomType() == &dtCore::CollisionGeomType::CUBE)
   {
      return &dtCore::CollisionGeomType::CUBE;
   }
   else if (mImpl->mGeomWrap->GetCollisionGeomType() == &dtCore::CollisionGeomType::RAY)
   {
      return &dtCore::CollisionGeomType::RAY;
   }
   else if (mImpl->mGeomWrap->GetCollisionGeomType() == &dtCore::CollisionGeomType::MESH)
   {
      return &dtCore::CollisionGeomType::MESH;
   }
   else
   {
      return &dtCore::CollisionGeomType::NONE;
   }
}

////////////////////////////////////////////////////////////////////////////////
void Transformable::GetCollisionGeomDimensions(std::vector<float>& dimensions)
{
   // Sync up ODE with our OSG transforms.
   PrePhysicsStepUpdate();

   mImpl->mGeomWrap->GetCollisionGeomDimensions(dimensions);
 }

////////////////////////////////////////////////////////////////////////////////
void Transformable::SetCollisionCategoryBits(unsigned long bits)
{
   mImpl->mGeomWrap->SetCollisionCategoryBits(bits);

   unsigned int count = GetNumChildren();
   for (unsigned int index = 0; index < count; ++index)
   {
      Transformable* child = dynamic_cast<Transformable*>(GetChild(index));
      if (child)
      {
         child->SetCollisionCategoryBits(bits);
      }
   }
}

////////////////////////////////////////////////////////////////////////////////
unsigned long Transformable::GetCollisionCategoryBits() const
{
   return mImpl->mGeomWrap->GetCollisionCategoryBits();
}

////////////////////////////////////////////////////////////////////////////////
void Transformable::SetCollisionCollideBits(unsigned long bits)
{
   mImpl->mGeomWrap->SetCollisionCollideBits(bits);

   unsigned int count = GetNumChildren();
   for (unsigned int index = 0; index < count; ++index)
   {
      Transformable* child = dynamic_cast<Transformable*>(GetChild(index));
      if (child)
      {
         child->SetCollisionCollideBits(bits);
      }
   }
}

////////////////////////////////////////////////////////////////////////////////
unsigned long Transformable::GetCollisionCollideBits() const
{
   return mImpl->mGeomWrap->GetCollisionCollideBits();
}


////////////////////////////////////////////////////////////////////////////////
void Transformable::SetCollisionDetection(bool enabled)
{
   mImpl->mGeomWrap->SetCollisionDetection(enabled);

   unsigned int count = GetNumChildren();
   for (unsigned int index = 0; index < count; ++index)
   {
      Transformable* child = dynamic_cast<Transformable*>(GetChild(index));
      if (child)
      {
         child->SetCollisionDetection(enabled);
      }
   }
}

////////////////////////////////////////////////////////////////////////////////
bool Transformable::GetCollisionDetection() const
{
   return mImpl->mGeomWrap->GetCollisionDetection();
}

////////////////////////////////////////////////////////////////////////////////
dGeomID Transformable::GetGeomID() const
{
   return mImpl->mGeomWrap->GetGeomID();
}

////////////////////////////////////////////////////////////////////////////////
void Transformable::SetCollisionGeom(dGeomID geom)
{
   mImpl->mGeomWrap->SetCollisionGeom(geom);

   RenderCollisionGeometry(mImpl->mRenderingGeometry);

   // Sync-up the transforms on mGeomID
   PrePhysicsStepUpdate();
}

////////////////////////////////////////////////////////////////////////////////
void Transformable::SetCollisionSphere(float radius)
{
   mImpl->mGeomWrap->SetCollisionSphere(radius);

   RenderCollisionGeometry(mImpl->mRenderingGeometry);

   mImpl->mGeomWrap->SetCollisionDetection(true);

   // Sync-up the transforms on mGeomID
   PrePhysicsStepUpdate();
}


////////////////////////////////////////////////////////////////////////////////
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

      mImpl->mGeomWrap->SetCollisionSphere(node);

      GetMatrixNode()->setMatrix(oldMatrix);

      RenderCollisionGeometry(mImpl->mRenderingGeometry);

      mImpl->mGeomWrap->SetCollisionDetection(true);

      // Sync-up the transforms on mGeomID
      PrePhysicsStepUpdate();
   }
}

////////////////////////////////////////////////////////////////////////////////
void Transformable::SetCollisionBox(float lx, float ly, float lz)
{
   mImpl->mGeomWrap->SetCollisionBox(lx, ly, lz);

   RenderCollisionGeometry(mImpl->mRenderingGeometry);
   SetCollisionDetection(true);

   // Sync-up the transforms on mGeomID
   PrePhysicsStepUpdate();
}

////////////////////////////////////////////////////////////////////////////////
void Transformable::SetCollisionBox(osg::Node* node)
{
   if(node == NULL)
   {
      node = GetOSGNode();
   }

   if(node != NULL)
   {
      osg::Matrix oldMatrix = GetMatrixNode()->getMatrix();
      GetMatrixNode()->setMatrix(osg::Matrix::identity());

      mImpl->mGeomWrap->SetCollisionBox(node);

      GetMatrixNode()->setMatrix(oldMatrix);

      RenderCollisionGeometry(mImpl->mRenderingGeometry);
      SetCollisionDetection(true);

      PrePhysicsStepUpdate();
   }
}

////////////////////////////////////////////////////////////////////////////////
void Transformable::SetCollisionCylinder(float radius, float length)
{
   mImpl->mGeomWrap->SetCollisionCylinder(radius, length);

   RenderCollisionGeometry(mImpl->mRenderingGeometry);
   SetCollisionDetection(true);

   // Sync-up the transforms on mGeomID
   PrePhysicsStepUpdate();
}

////////////////////////////////////////////////////////////////////////////////
void Transformable::SetCollisionCylinder(osg::Node* node)
{
   if(node == NULL)
   {
      node = this->GetOSGNode();
   }

   if(node != NULL)
   {
      osg::Matrix oldMatrix = GetMatrixNode()->getMatrix();
      GetMatrixNode()->setMatrix(osg::Matrix::identity());

      mImpl->mGeomWrap->SetCollisionCylinder(node);

      GetMatrixNode()->setMatrix(oldMatrix);

      RenderCollisionGeometry(mImpl->mRenderingGeometry);
      SetCollisionDetection(true);

      // Sync-up the transforms on mGeomID
      PrePhysicsStepUpdate();
   }
}

////////////////////////////////////////////////////////////////////////////////
void Transformable::SetCollisionCappedCylinder(float radius, float length)
{
   mImpl->mGeomWrap->SetCollisionCappedCylinder(radius, length);

   RenderCollisionGeometry(mImpl->mRenderingGeometry);
   SetCollisionDetection(true);

   // Sync-up the transforms on mGeomID
   PrePhysicsStepUpdate();
}


////////////////////////////////////////////////////////////////////////////////
void Transformable::SetCollisionCappedCylinder(osg::Node* node)
{
   if(node == NULL)
   {
      node = this->GetOSGNode();
   }

   if(node != NULL)
   {
      osg::Matrix oldMatrix = GetMatrixNode()->getMatrix();
      GetMatrixNode()->setMatrix(osg::Matrix::identity());

      mImpl->mGeomWrap->SetCollisionCappedCylinder(node);

      GetMatrixNode()->setMatrix(oldMatrix);

      RenderCollisionGeometry(mImpl->mRenderingGeometry);
      SetCollisionDetection(true);

      // Sync-up the transforms on mGeomID
      PrePhysicsStepUpdate();
   }
}

////////////////////////////////////////////////////////////////////////////////
void Transformable::SetCollisionRay(float length)
{
   mImpl->mGeomWrap->SetCollisionRay(length);

   RenderCollisionGeometry(mImpl->mRenderingGeometry);
   SetCollisionDetection(true);

   // Sync-up the transforms on mGeomID
   PrePhysicsStepUpdate();
}


////////////////////////////////////////////////////////////////////////////////
void Transformable::SetCollisionMesh(osg::Node* node)
{
   if (node == NULL)
   {
      node = GetOSGNode();
   }

   if (node != NULL)
   {
      //the following is a workaround to temporarily bypass this Physical's Transform
      //At this point, we'll set it temporarily to the Identity so it doesn't affect
      //our creation of the collision mesh.  This Transform will be accounted
      //for later in PrePhysicsUpdate().
      osg::Matrix oldMatrix = GetMatrixNode()->getMatrix();
      GetMatrixNode()->setMatrix(osg::Matrix::identity());

      mImpl->mGeomWrap->SetCollisionMesh(node);

      GetMatrixNode()->setMatrix(oldMatrix);

      RenderCollisionGeometry(mImpl->mRenderingGeometry);
      SetCollisionDetection(true);
   }
}

////////////////////////////////////////////////////////////////////////////////
void Transformable::ClearCollisionGeometry()
{
   mImpl->mGeomWrap->ClearCollisionGeometry();

   //If the collision geometry is valid, this implies the user has
   //enabled render collision geometry.  Therefore, we just remove
   //the drawables from the geode.  When the user turns off render
   //collision geometry, that will remove the geode from this node.
   if(mImpl->mGeomGeod.valid())
   {
      mImpl->mGeomGeod->removeDrawables(0,mImpl->mGeomGeod->getNumDrawables());
   }
}

////////////////////////////////////////////////////////////////////////////////
void Transformable::PrePhysicsStepUpdate()
{
   if (mImpl->mGeomWrap->GetCollisionDetection() == false)
   {
      return;
   }

   Transform transform;

   this->GetTransform(transform, Transformable::ABS_CS);

   mImpl->mGeomWrap->UpdateGeomTransform(transform);
}

////////////////////////////////////////////////////////////////////////////////
void Transformable::RenderCollisionGeometry(bool enable /* = true */,
   bool wireFrame /*= false */)
{
   TransformableNode* xform = this->GetMatrixNode();

   if(!xform)
   {
      return;
   }

   mImpl->mRenderingGeometry = enable;

   if(enable)
   {
      //If there is already an existing rendering of the collision geometry,
      //remove it before adding a new one.
      if (mImpl->mGeomGeod.valid())
      {
         RemoveRenderedCollisionGeometry();
      }

      mImpl->mGeomGeod =
         mImpl->mGeomWrap->CreateRenderedCollisionGeometry(wireFrame);

      if (mImpl->mGeomGeod.valid())
      {
         mImpl->mGeomGeod->setName(Transformable::COLLISION_GEODE_ID);

         xform->addChild(mImpl->mGeomGeod.get());
      }
   } //end if enabled==true
   else
   {
      this->RemoveRenderedCollisionGeometry();
   }

   unsigned int count = GetNumChildren();
   for (unsigned int index = 0; index < count; ++index)
   {
      Transformable* child = dynamic_cast<Transformable*>(GetChild(index));
      if (child)
      {
         child->RenderCollisionGeometry(enable);
      }
   }
}

////////////////////////////////////////////////////////////////////////////////
bool Transformable::GetRenderCollisionGeometry() const
{
   return mImpl->mRenderingGeometry;
}

////////////////////////////////////////////////////////////////////////////////
void Transformable::AddedToScene(Scene* scene)
{
   if(scene)
   {
      //remove us from our existing parent scene, if we already have one.
      //TODO This ends up calling AddedToScene again with a 0.  Is this bad?
      if(GetSceneParent())
      {
         GetSceneParent()->RemoveChild(this);
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

////////////////////////////////////////////////////////////////////////////////
void Transformable::RemoveRenderedCollisionGeometry()
{
   if(mImpl->mGeomGeod.valid())
   {
      GetMatrixNode()->removeChild(mImpl->mGeomGeod.get());
      mImpl->mGeomGeod = 0;
   }
}

////////////////////////////////////////////////////////////////////////////////
const ODEGeomWrap* Transformable::GetGeomWrapper() const
{
   return mImpl->mGeomWrap.get();
}

////////////////////////////////////////////////////////////////////////////////
ODEGeomWrap* Transformable::GetGeomWrapper()
{
   return mImpl->mGeomWrap.get();
}

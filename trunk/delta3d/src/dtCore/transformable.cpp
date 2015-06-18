#include <prefix/dtcoreprefix.h>
#include <dtCore/pointaxis.h>
#include <dtCore/scene.h>
#include <dtCore/transformable.h>
#include <dtCore/transform.h>
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
      : mGeomGeod(nullptr)
      , mNode(&node)
      , mRenderingGeometry(false)
      , mRenderProxyNode(false)
      {

      }
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

}

////////////////////////////////////////////////////////////////////////////////
Transformable::~Transformable()
{
   if (mImpl->mPointAxis.valid())
   {
      SetProxyNode(nullptr);
      RemoveChild(mImpl->mPointAxis.get());
      mImpl->mPointAxis = nullptr;
   }

   DeregisterInstance(this);

   delete mImpl;
   mImpl = nullptr;
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
bool Transformable::GetAbsoluteMatrix(const osg::Node* node, osg::Matrix& wcMatrix, const osg::Node* stopNode)
{
   if (node != nullptr)
   {
      wcMatrix.makeIdentity();

      std::vector<osg::Transform*>  nodePath;
      // most things have very small depths, but allocating 15 didn't seem to be any slower
      // than allocating 5, and it's just pointers, so I made it 15 to account for some pretty
      // deep trees.
      nodePath.reserve(15U);
      nodePath.clear();

      osg::Node* curNode = const_cast<osg::Node*>(node);
      while (curNode != nullptr && curNode != stopNode)
      {
         osg::Transform* txNode = curNode->asTransform();
         if (txNode != nullptr)
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

            if (camera != nullptr && (camera->getReferenceFrame() != osg::Transform::RELATIVE_RF || camera->getNumParents() == 0))
            {
               curNode = nullptr;
            }
            else
            {
               // only put transforms in the node path.
               nodePath.push_back(txNode);
            }
         }

         if (curNode != nullptr && curNode->getNumParents() > 0U)
         {
            curNode = curNode->getParent(0);
            // Stop when you get to a camera.
         }
         else
         {
            curNode = nullptr;
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
            if (curTran != nullptr && !curTran->computeLocalToWorldMatrix(wcMatrix, nullptr))
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
         mImpl->mPointAxis = nullptr;
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

   if(GetOSGNode() != nullptr)
   {
      GetOSGNode()->getOrCreateStateSet()->setMode(GL_RESCALE_NORMAL, state);
   }
}

////////////////////////////////////////////////////////////////////////////////
bool Transformable::GetNormalRescaling() const
{
   if (GetOSGNode() != nullptr && GetOSGNode()->getStateSet() != nullptr)
   {
      osg::StateAttribute::GLModeValue state = GetOSGNode()->getStateSet()->getMode(GL_RESCALE_NORMAL);
      return (state & osg::StateAttribute::ON) ? true : false;
   }
   return false;
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
   }
   else
   {
      DeltaDrawable::AddedToScene(nullptr);
   }
}

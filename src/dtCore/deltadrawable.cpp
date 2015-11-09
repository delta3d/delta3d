
////////////////////////////////////////////////////////////////////////////////
// INCLUDE DIRECTIVES
////////////////////////////////////////////////////////////////////////////////
#include <prefix/dtcoreprefix.h>
#include <cassert>
// DELTA3D
#include <dtCore/deltadrawable.h>
#include <dtCore/scene.h>
#include <dtCore/shadergroup.h>
#include <dtCore/shadermanager.h>
#include <dtCore/shaderprogram.h>
#include <dtCore/project.h>  //for binding editor shader in edit mode
#include <dtUtil/log.h>
// OSG
#include <osg/ComputeBoundsVisitor>
#include <osg/Node>
#include <osg/Switch>

// For Tangents
#include <dtUtil/geometrycollector.h>
#include <osg/Geometry>
#include <osgUtil/TangentSpaceGenerator>


using namespace dtUtil;


namespace dtCore
{
   IMPLEMENT_MANAGEMENT_LAYER(DeltaDrawable)
         class DeltaDrawablePrivate
         {
         public:
      DeltaDrawablePrivate();
      ~DeltaDrawablePrivate();

      void SetOwner(DeltaDrawable* owner);

      void SetParent(DeltaDrawable* parent);
      DeltaDrawable* GetParent();
      const DeltaDrawable* GetParent() const;

      void AddedToScene(Scene* scene, osg::Node* node);
      void RemovedFromScene(Scene* scene, osg::Node* node);

      Scene* GetSceneParent();
      const Scene* GetSceneParent() const;

      bool AddChild(DeltaDrawable* child, DeltaDrawable* parent);
      void RemoveChild(DeltaDrawable* child);

      void Emancipate(DeltaDrawable* that);

      unsigned int GetNumChildren() const;

      DeltaDrawable* GetChild(unsigned int idx);
      const DeltaDrawable* GetChild(unsigned int idx) const;

      unsigned int GetChildIndex(const DeltaDrawable* child) const;

      bool CanBeChild(DeltaDrawable* child) const;

      void RenderProxyNode(bool enable = true);
      bool GetIsRenderingProxyNode() const;

      void GetBoundingSphere(osg::Vec3& center, float& radius, osg::Node* node);

      void SetActive(bool enable, osg::Node* node);
      bool GetActive() const;

      osg::Node* GetProxyNode();
      const osg::Node* GetProxyNode() const;
      void SetProxyNode(osg::Node* proxyNode);

      void OnOrphaned();

      void SetDescription(const std::string& description);
      const std::string& GetDescription() const;

      void SetShaderGroup(const std::string& groupName);
      std::string GetShaderGroup() const;

      void ApplyShaderGroup();

         private:
      ///Insert a new Switch Node above GetOSGNode() and below it's parents
      void InsertSwitchNode(osg::Node *node);

      ///Remove Switch Node above GetOSGNode()
      void RemoveSwitchNode(osg::Node* node);

      DeltaDrawable* mOwner;
      DeltaDrawable* mParent; ///< Any immediate parent of this instance (Weak pointer to prevent circular reference).

      typedef std::vector< RefPtr<DeltaDrawable> > ChildList;
      ChildList mChildList; ///< List of children DeltaDrawable added
      Scene* mParentScene; ///< The Scene this Drawable was added to (Weak pointer to prevent circular reference).
      RefPtr<osg::Node> mProxyNode; ///< Handle to the rendered proxy node (or NULL)
      bool mIsActive; ///<Is this DeltaDrawable active (rendering)
      std::string mDescription; ///<description string
      std::string mShaderGroup;
         };

   ////////////////////////////////////////////////////////////////////////////////
   void DeltaDrawablePrivate::SetOwner(DeltaDrawable* owner)
   {
      mOwner = owner;
   }

   ////////////////////////////////////////////////////////////////////////////////
   void DeltaDrawablePrivate::SetParent(DeltaDrawable* parent)
   {
      mParent = parent;
   }

   ////////////////////////////////////////////////////////////////////////////////
   DeltaDrawable* DeltaDrawablePrivate::GetParent()
   {
      return mParent;
   }

   ////////////////////////////////////////////////////////////////////////////////
   const DeltaDrawable* DeltaDrawablePrivate::GetParent() const
   {
      return mParent;
   }

   ////////////////////////////////////////////////////////////////////////////////
   Scene* DeltaDrawablePrivate::GetSceneParent()
   {
      return mParentScene;
   }

   ////////////////////////////////////////////////////////////////////////////////
   const Scene* DeltaDrawablePrivate::GetSceneParent() const
   {
      return mParentScene;
   }

   ////////////////////////////////////////////////////////////////////////////////
   void DeltaDrawablePrivate::RemoveChild(DeltaDrawable* child)
   {
      if (!child) return;

      unsigned int pos = GetChildIndex(child);
      if (pos < mChildList.size())
      {
         child->SetParent(NULL);
         if (GetSceneParent() != NULL)
         {
            child->AddedToScene(NULL);
            child->RemovedFromScene(GetSceneParent());
         }

         mChildList.erase(mChildList.begin() + pos);
         child = NULL;
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void DeltaDrawablePrivate::Emancipate(DeltaDrawable* that)
   {
      if (mParent != NULL)
      {
         mParent->RemoveChild(that);
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   unsigned int DeltaDrawablePrivate::GetNumChildren() const
   {
      return mChildList.size();
   }

   ////////////////////////////////////////////////////////////////////////////////
   DeltaDrawable* DeltaDrawablePrivate::GetChild(unsigned int idx)
   {
      if (idx >= GetNumChildren())
      {
         return NULL;
      }

      return mChildList[idx].get();
   }

   ////////////////////////////////////////////////////////////////////////////////
   const DeltaDrawable* DeltaDrawablePrivate::GetChild(unsigned int idx) const
   {
      if (idx >= GetNumChildren())
      {
         return NULL;
      }

      return mChildList[idx].get();
   }

   ////////////////////////////////////////////////////////////////////////////////
   unsigned int DeltaDrawablePrivate::GetChildIndex(const DeltaDrawable* child) const
   {
      for (unsigned int childNum = 0; childNum < mChildList.size(); ++childNum)
      {
         if (mChildList[childNum] == child)
         {
            return childNum;
         }
      }

      return mChildList.size(); // node not found.
   }

   ////////////////////////////////////////////////////////////////////////////////
   void DeltaDrawablePrivate::RenderProxyNode(bool enable)
   {
      if (!mProxyNode.valid())
      {
         LOG_WARNING("Proxy node is not implemented, overwrite RenderProxyNode." );
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   bool DeltaDrawablePrivate::GetIsRenderingProxyNode() const
   {
      return mProxyNode.valid();
   }

   ////////////////////////////////////////////////////////////////////////////////
   void DeltaDrawablePrivate::GetBoundingSphere(osg::Vec3& center,
         float& radius,
         osg::Node* node)
   {
      if (node != NULL)
      {
         osg::BoundingSphere bs = node->getBound();
         center.set(bs.center());
         radius = bs.radius();
      }
      else
      {
         LOG_WARNING("Can't calculate Bounding Sphere, there is no geometry associated with this DeltaDrawable");
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void DeltaDrawablePrivate::SetActive(bool enable, osg::Node* node)
   {
      if (mIsActive == enable)
      {
         return; //nothing to do here
      }

      mIsActive = enable;

      if (mParentScene == NULL)
      {
         // if we haven't been added to a Scene yet, then we are already effectively
         // inactive.  Once we get added to a Scene, we'll make sure we remain inactive.
         return;
      }

      if (mIsActive == false)
      {
         InsertSwitchNode(node);
      }
      else
      {
         RemoveSwitchNode(node);
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   bool DeltaDrawablePrivate::GetActive() const
   {
      return mIsActive;
   }

   ////////////////////////////////////////////////////////////////////////////////
   void DeltaDrawablePrivate::SetProxyNode(osg::Node* proxyNode)
   {
      mProxyNode = proxyNode;
   }

   ////////////////////////////////////////////////////////////////////////////////
   void DeltaDrawablePrivate::OnOrphaned()
   {
      mParent = NULL;
   }

   ////////////////////////////////////////////////////////////////////////////////
   void DeltaDrawablePrivate::InsertSwitchNode(osg::Node *node)
   {
      // save off all parents of the Node
      osg::Node::ParentList parents = node->getParents();

      // remove the Node from all its parents
      osg::Node::ParentList::iterator parentItr = parents.begin();
      while (parentItr != parents.end())
      {
         (*parentItr)->removeChild(node);
         ++parentItr;
      }

      osg::ref_ptr<osg::Switch> parentSwitch = new osg::Switch();
      parentSwitch->setAllChildrenOff();

      // add the Node as a child of parentSwitch
      parentSwitch->addChild(node);

      // add parentSwitch to all of the Node's parents
      parentItr = parents.begin();
      while (parentItr != parents.end())
      {
         (*parentItr)->addChild(parentSwitch.get());
         ++parentItr;
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void DeltaDrawablePrivate::RemoveSwitchNode(osg::Node* node)
   {
      if (node == NULL) { return; }  // no geometry?

      if (node->getNumParents() == 0) { return; }  // no parents?

      osg::Switch* parentSwitch = dynamic_cast<osg::Switch*>(node->getParent(0));
      if (parentSwitch == NULL) { return; }

      // save off all parents of the Switch Node
      osg::Node::ParentList parents = parentSwitch->getParents();

      // remove the Switch node from all its parents
      osg::Node::ParentList::iterator parentItr = parents.begin();
      while (parentItr != parents.end())
      {
         (*parentItr)->removeChild(parentSwitch);
         ++parentItr;
      }

      // Add the Node as a child to what was the Switch node's parents
      parentItr = parents.begin();
      while (parentItr != parents.end())
      {
         (*parentItr)->addChild(node);
         ++parentItr;
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   osg::Node* DeltaDrawablePrivate::GetProxyNode()
   {
      return mProxyNode.get();
   }

   ////////////////////////////////////////////////////////////////////////////////
   const osg::Node* DeltaDrawablePrivate::GetProxyNode() const
   {
      return mProxyNode.get();
   }

   ////////////////////////////////////////////////////////////////////////////////
   void DeltaDrawablePrivate::SetDescription(const std::string& description)
   {
      mDescription = description;
   }

   ////////////////////////////////////////////////////////////////////////////////
   const std::string& DeltaDrawablePrivate::GetDescription() const
   {
      return mDescription;
   }

   /////////////////////////////////////////////////////////////////////////////
   void DeltaDrawablePrivate::SetShaderGroup(const std::string& groupName)
   {
      // Setting the shader group, when it didn't change can cause a massive
      // hit on performance because it unassigns everything and will make a new
      // instance of the shader and all its params. Could also cause anomalies with
      // oscilating shader params.
      if (groupName != mShaderGroup)
      {
         mShaderGroup = groupName;

         ApplyShaderGroup();

         mOwner->OnShaderGroupChanged();
      }
   }

   /////////////////////////////////////////////////////////////////////////////
   std::string DeltaDrawablePrivate::GetShaderGroup() const
   {
      return mShaderGroup;
   }

   ///////////////////////////////////////////////////////////////////////////////////////////////////////
   void DeltaDrawablePrivate::ApplyShaderGroup()
   {
      if (mShaderGroup.empty())
         return; // Do nothing, since there is nothing to load.

      osg::Node* node = mOwner->GetOSGNode();

      if (node == NULL)
      {
         LOG_ERROR("Cannot assign shader group \"" + mShaderGroup
               + "\" to actor \"" + mOwner->GetName()
               + "\" because it has no drawable.");
         return;
      }

      // Unassign any old setting on this, if any - works regardless if there's a node or not
      ShaderManager::GetInstance().UnassignShaderFromNode(*node);

      //First get the shader group assigned to this actor.
      const ShaderGroup* shaderGroup =
            ShaderManager::GetInstance().FindShaderGroupPrototype(mShaderGroup);

      if (shaderGroup == NULL)
      {
         LOG_INFO("Could not find shader group \""
               + mShaderGroup + "\" for actor \"" + mOwner->GetName() +"\"");
         return;
      }

      // TODO: Find an explicit shader instead of just the default;
      //shaderGroup->FindShader();

      bool editMode = Project::GetInstance().GetEditMode();

      const ShaderProgram* defaultShader = NULL;

      if(editMode)
      {
         defaultShader = shaderGroup->GetEditorShader();
      }

      //if we arent in edit mode or there is no specific shader for editors
      if(defaultShader == NULL)
      {
         defaultShader = shaderGroup->GetDefaultShader();
      }

      try
      {
         if (defaultShader != NULL)
         {
            ShaderManager::GetInstance().AssignShaderFromPrototype(*defaultShader, *node);
         }
         else
         {
            LOG_WARNING("Could not find a default shader in shader group \"" + mShaderGroup +"\"");
            return;
         }
      }
      catch (const dtUtil::Exception& e)
      {
         LOG_WARNING("Caught Exception while assigning shader group \""
               + mShaderGroup + "\": " + e.ToString());
         return;
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void DeltaDrawablePrivate::AddedToScene(Scene* scene, osg::Node* node)
   {
      if (mParentScene == scene) { return; } // nothing to do here.

      mParentScene = scene;

      for (ChildList::iterator itr = mChildList.begin();
            itr != mChildList.end();
            ++itr)
      {
         (*itr)->AddedToScene(scene);
      }

      // If we've been set to inactive before being added to a Scene,
      // then we need to do add in our Switch node.
      if (mParentScene != NULL)
      {
         if (GetActive() == false)
         {
            InsertSwitchNode(node);
         }
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void DeltaDrawablePrivate::RemovedFromScene(Scene* scene, osg::Node* node)
   {
      // for now don't include this check, since we're calling
      //   DeltaDrawablePrivate::AddedToScene() first
      //if (mParentScene != scene) { return; } // we're not even in this scene; nothing to do here.

      mParentScene = NULL;

      for (ChildList::iterator itr = mChildList.begin();
            itr != mChildList.end();
            ++itr)
      {
         (*itr)->RemovedFromScene(scene);
      }

      // we've just been removed from a scene
      // if we're inactive, we should remove our Switch
      if (GetActive() == false)
      {
         RemoveSwitchNode(node);
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   bool DeltaDrawablePrivate::AddChild(DeltaDrawable* child, DeltaDrawable* parent)
   {
      child->SetParent(parent);
      mChildList.push_back(child);

      if (mParentScene)
      {
         child->AddedToScene(mParentScene);
      }

      return true;
   }

   ////////////////////////////////////////////////////////////////////////////////
   bool DeltaDrawablePrivate::CanBeChild(DeltaDrawable* child) const
   {
      if (child->GetParent() != NULL)
      {
         return false;
      }

      //loop through parent's parents and make sure they're not == child
      RefPtr<const DeltaDrawable> t = GetParent();
      while (t != NULL)
      {
         if (t == child)
         {
            return false;
         }
         t = t->GetParent();
      }

      return true;

   }

   ////////////////////////////////////////////////////////////////////////////////
   DeltaDrawablePrivate::DeltaDrawablePrivate()
   : mOwner(NULL)
   , mParent(NULL)
   , mParentScene(NULL)
   , mIsActive(true)
   {
   }

   ////////////////////////////////////////////////////////////////////////////////
   DeltaDrawablePrivate::~DeltaDrawablePrivate()
   {
   }





   //////////////////////////////////////////////////////////////////////////
   DeltaDrawable::DeltaDrawable(const std::string& name)
   : Base(name)
   , mPvt(new DeltaDrawablePrivate())
   {
      mPvt->SetOwner(this);

      RegisterInstance(this);
   }

   ////////////////////////////////////////////////////////////////////////////////
   DeltaDrawable::~DeltaDrawable()
   {
      DeregisterInstance(this);

      for (unsigned int i = 0; i < mPvt->GetNumChildren(); ++i)
      {
         DeltaDrawable* child = mPvt->GetChild(i);
         child->OnOrphaned();
      }

      mPvt->SetOwner(NULL);
      delete mPvt;
   }

   /** This virtual method can be overwritten
    *  to perform specific functionality.  The default method will
    *  store the child in a list and set the child's parent.
    * @param child : The child to add to this Drawable
    * @return : Successfully added this child or not
    */
   bool DeltaDrawable::AddChild(DeltaDrawable* child)
   {
      if (CanBeChild(child))
      {
         return mPvt->AddChild(child, this);
      }
      else
      {
         Log::GetInstance().LogMessage(Log::LOG_WARNING, __FILE__,
               "DeltaDrawable: '%s' cannot be added as a child to '%s'",
               child->GetName().c_str(), this->GetName().c_str());
         return false;
      }
   }

   //////////////////////////////////////////////////////////////////////////
   DeltaDrawable* DeltaDrawable::GetChild(unsigned int idx)
   {
      return mPvt->GetChild(idx);
   }


   //////////////////////////////////////////////////////////////////////////
   const DeltaDrawable* DeltaDrawable::GetChild(unsigned int idx) const
   {
      return mPvt->GetChild(idx);
   }

   /*!
    * Remove a child from this DeltaDrawable.  This will detach the child from its
    * parent so that its free to be repositioned on its own.
    *
    * @param *child : The child DeltaDrawable to be removed
    */
   void DeltaDrawable::RemoveChild(DeltaDrawable* child)
   {
      if (child->GetParent() != this && child->GetParent() != NULL)
      {
         return;
      }

      return mPvt->RemoveChild(child);
   }

   /**
    * Return a value between
    * 0 and the number of children-1 if found, if not found then
    * return the number of children.
    */
   unsigned int DeltaDrawable::GetChildIndex(const DeltaDrawable* child) const
   {
      return mPvt->GetChildIndex(child);
   }

   bool DeltaDrawable::HasChild(const DeltaDrawable* child) const
   {
      return GetChildIndex(child) < GetNumChildren();
   }

   /*!
    * Check to see if the supplied DeltaDrawable can be a child to this instance.
    * To be valid, it can't already have a parent, can't be this instance, and
    * can't be the parent of this instance.
    *
    * @param *child : The candidate child to be tested
    *
    * @return bool  : True if it can be a child, false otherwise
    */
   bool DeltaDrawable::CanBeChild(DeltaDrawable* child) const
   {
      if (this == child)
      {
         return false;
      }
      else
      {
         return mPvt->CanBeChild(child);
      }
   }

   void DeltaDrawable::RenderProxyNode(bool enable)
   {
      mPvt->RenderProxyNode(enable);
   }

   //////////////////////////////////////////////////////////////////////////
   bool DeltaDrawable::GetIsRenderingProxyNode() const
   {
      return mPvt->GetIsRenderingProxyNode();
   }

   /**
    * Notifies this drawable object that it has been added to
    * a scene.  This is typically called from Scene::AddChild().
    *
    * This method will iterate through the list of children DeltaDrawable's (if any)
    * and call AddedToScene() with the supplied Scene.
    *
    * @param scene the scene to which this drawable object has
    * been added.  Note: NULL indicates removal.
    */
   void DeltaDrawable::AddedToScene(Scene* scene)
   {
      mPvt->AddedToScene(scene, GetOSGNode());
   }

   /**
    * Notifies this drawable object that it has been removed from
    * a scene.  This is typically called from Scene::RemoveDrawable().
    *
    * This method will iterate through the list of children DeltaDrawable's (if any)
    * and call RemovedFromScene() with the supplied Scene.
    *
    * @param scene the scene to which this drawable object has
    * been added.  Note: scene should not be NULL.
    */
   void DeltaDrawable::RemovedFromScene(Scene* scene)
   {
      assert(scene);
      mPvt->RemovedFromScene(scene, GetOSGNode());
   }

   /** Remove this DeltaDrawable from it's parent DeltaDrawable if it has one.
    * Each DeltaDrawable may have only one parent and it must be removed from
    * it's parent before adding it as a child to another.
    * @see RemoveChild()
    */
   void DeltaDrawable::Emancipate()
   {
      mPvt->Emancipate(this);
   }


   //////////////////////////////////////////////////////////////////////////
   void DeltaDrawable::SetProxyNode(osg::Node* proxyNode)
   {
      mPvt->SetProxyNode(proxyNode);
   }

   //////////////////////////////////////////////////////////////////////////
   void DeltaDrawable::OnOrphaned()
   {
      mPvt->OnOrphaned();
   }

   //////////////////////////////////////////////////////////////////////////
   void DeltaDrawable::GetBoundingSphere(osg::Vec3& center, float& radius)
   {
      mPvt->GetBoundingSphere(center, radius, GetOSGNode());
   }

   //////////////////////////////////////////////////////////////////////////
   osg::BoundingBox DeltaDrawable::GetBoundingBox()
   {
      osg::Node* topNode = this->GetOSGNode();

      if (topNode == NULL)
      {
         LOG_WARNING("Can't calculate Bounding Box, there is no geometry associated with this DeltaDrawable");
         osg::BoundingBox bb;
         return bb;
      }

      osg::ComputeBoundsVisitor cbv;
      topNode->accept(cbv);

      return cbv.getBoundingBox();
   }

   //////////////////////////////////////////////////////////////////////////
   void DeltaDrawable::SetActive(bool enable)
   {
      mPvt->SetActive(enable, GetOSGNode());
   }

   //////////////////////////////////////////////////////////////////////////
   bool DeltaDrawable::GetActive() const
   {
      return mPvt->GetActive();
   }

   //////////////////////////////////////////////////////////////////////////
   void DeltaDrawable::SetParent(DeltaDrawable* parent)
   {
      mPvt->SetParent(parent);
   }

   //////////////////////////////////////////////////////////////////////////
   DeltaDrawable* DeltaDrawable::GetParent()
   {
      return mPvt->GetParent();
   }

   //////////////////////////////////////////////////////////////////////////
   const DeltaDrawable* DeltaDrawable::GetParent() const
   {
      return mPvt->GetParent();
   }

   //////////////////////////////////////////////////////////////////////////
   Scene* DeltaDrawable::GetSceneParent()
   {
      return mPvt->GetSceneParent();
   }

   //////////////////////////////////////////////////////////////////////////
   const Scene* DeltaDrawable::GetSceneParent() const
   {
      return mPvt->GetSceneParent();
   }

   //////////////////////////////////////////////////////////////////////////
   unsigned int DeltaDrawable::GetNumChildren() const
   {
      return mPvt->GetNumChildren();
   }

   /////////////////////////////////////////////////////////////////////////////
   unsigned int DeltaDrawable::GetChildren(DeltaDrawable::DrawableList& outDrawables)
   {
      unsigned int count = mPvt->GetNumChildren();
      for (unsigned int i = 0; i < count; ++i)
      {
         outDrawables.push_back(GetChild(i));
      }

      return count;
   }

   //////////////////////////////////////////////////////////////////////////
   osg::Node* DeltaDrawable::GetProxyNode()
   {
      return mPvt->GetProxyNode();
   }

   //////////////////////////////////////////////////////////////////////////
   const osg::Node* DeltaDrawable::GetProxyNode() const
   {
      return mPvt->GetProxyNode();
   }

   ////////////////////////////////////////////////////////////////////////////////
   void DeltaDrawable::SetDescription(const std::string& description)
   {
      mPvt->SetDescription(description);
   }

   ////////////////////////////////////////////////////////////////////////////////
   const std::string& DeltaDrawable::GetDescription() const
   {
      return mPvt->GetDescription();
   }

   /////////////////////////////////////////////////////////////////////////////
   void DeltaDrawable::SetShaderGroup(const std::string& groupName)
   {
      mPvt->SetShaderGroup(groupName);
   }

   /////////////////////////////////////////////////////////////////////////////
   std::string DeltaDrawable::GetShaderGroup() const
   {
      return mPvt->GetShaderGroup();
   }

   /////////////////////////////////////////////////////////////////////////////
   void DeltaDrawable::ApplyShaderGroup()
   {
      return mPvt->ApplyShaderGroup();
   }

   ///////////////////////////////////////////////////////////////////////////////////////////////////////
   void DeltaDrawable::OnShaderGroupChanged()
   {
      // OVERRIDE:
   }

   /////////////////////////////////////////////////////////////////////////////
   bool DeltaDrawable::GenerateTangents()
   {
      bool success = false;

      // Get all geometry in the graph to apply the shader to
      osg::ref_ptr<dtUtil::GeometryCollector> geomCollector = new dtUtil::GeometryCollector;
      GetOSGNode()->accept(*geomCollector);

      // Calculate tangent vectors for all faces and store them as vertex attributes
      for (size_t geomIndex = 0; geomIndex < geomCollector->mGeomList.size(); ++geomIndex)
      {
         osg::Geometry* geom = geomCollector->mGeomList[geomIndex];

         // Force display lists to OFF and VBO's to ON so that vertex
         // attributes can be set without disturbing the graphics driver
         geom->setSupportsDisplayList(false);
         geom->setUseDisplayList(false);
         geom->setUseVertexBufferObjects(true);

         osg::ref_ptr<osgUtil::TangentSpaceGenerator> tsg = new osgUtil::TangentSpaceGenerator;
         tsg->generate(geom, 0);
         osg::Array* tangentArray = tsg->getTangentArray();

         if ( ! geom->getTexCoordArray(1))
         {
            if (tangentArray != NULL)
            {
               geom->setTexCoordArray(1, tangentArray);

               success = true;
            }
            else
            {
               LOGN_WARNING("deltadrawable.cpp", "Could not generate tangent space for object: " + GetName()
                     + " - Geometry: " + geom->getName());
            }
         }
         else
         {
            LOGN_INFO("deltadrawable.cpp", "Tangent space data may already exist for object: " + GetName()
                  + " - Geometry: " + geom->getName());
         }
      }

      return success;
   }

}//namespace dtCore

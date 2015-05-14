
////////////////////////////////////////////////////////////////////////////////
// INCLUDE DIRECTIVES
////////////////////////////////////////////////////////////////////////////////
#include <dtAnim/osgmodeldata.h>
#include <dtAnim/constants.h>
#include <dtAnim/osgmodelresourcefinder.h>
#include <dtAnim/osgloader.h>
#include <dtUtil/log.h>
#include <osg/Geode>
#include <osgAnimation/BasicAnimationManager>
#include <osgAnimation/MorphGeometry>
#include <osgAnimation/RigGeometry>
#include <osgAnimation/Skeleton>



namespace dtAnim
{
   /////////////////////////////////////////////////////////////////////////////
   // CLASS CODE
   /////////////////////////////////////////////////////////////////////////////
   class OsgModelResourceCloner : public osg::CopyOp
   {
   public:
      typedef osg::CopyOp BaseClass;

      OsgModelResourceCloner(osg::CopyOp::CopyFlags flags, dtAnim::ModelResourceType mode)
         : BaseClass(flags)
         , mMode(mode)
      {}

      virtual ~OsgModelResourceCloner()
      {}
      
      virtual osg::Node* operator() (const osg::Node* node) const
      {
         bool copyNode = false;

         if (mMode == dtAnim::MIXED_FILE)
         {
            copyNode = true;
         }
         else if (mMode == dtAnim::MESH_FILE)
         {
            if (strcmp(node->className(), "Geode") == 0)
            {
               copyNode = true;
            }
         }
         else if (mMode == dtAnim::SKEL_FILE)
         {
            if (strcmp(node->className(), "MatrixTransform") == 0
               || strcmp(node->className(), "Bone") == 0
               || strcmp(node->className(), "Skeleton") == 0)
            {
               copyNode = true;
            }
         }

         if (copyNode)
         {
             return BaseClass::operator()(node);
         }

         return NULL;
      }

      virtual osg::StateSet* operator() (const osg::StateSet* stateset) const
      {
         if (mMode == dtAnim::MIXED_FILE || mMode == dtAnim::MAT_FILE)
         {
            return BaseClass::operator()(stateset);
         }

         return NULL;
      }

      virtual osg::Callback* operator() (const osg::Callback* nodecallback) const
      {
         if (nodecallback == NULL)
         {
            return NULL;
         }

         if (mMode == dtAnim::MIXED_FILE || mMode == dtAnim::ANIM_FILE || mMode == dtAnim::SKEL_FILE)
         {
            bool allowCopy = true;

            // Prevent copying the animation manager if one exists on a skeleton
            if (mMode == dtAnim::SKEL_FILE)
            {
               allowCopy = ! (strcmp(nodecallback->className(), "BasicAnimationManager") == 0
                  || strcmp(nodecallback->className(), "TimelineManager") == 0);
            }

            if (allowCopy)
            {
               return BaseClass::operator()(nodecallback);
            }
         }

         return NULL;
      }

   protected:
      osg::CopyOp mOptions;
      dtAnim::ModelResourceType mMode;
   };



   /////////////////////////////////////////////////////////////////////////////
   // CLASS CODE
   /////////////////////////////////////////////////////////////////////////////
   class NodeAttacher : public osg::NodeVisitor
   {
   public:
      typedef osg::NodeVisitor BaseClass;

      NodeAttacher()
         : BaseClass(osg::NodeVisitor::TRAVERSE_ALL_CHILDREN)
      {}

      static int GetDepthFromSkeleton(osg::Node& node)
      {
         int depth = 0;

         bool skeletonFound = false;
         osg::Group* parent = node.getNumParents() > 0 ? node.getParent(0) : NULL;
         while (parent != NULL)
         {
            if (strcmp(parent->className(), "Skeleton") == 0)
            {
               skeletonFound = true;
               break;
            }
            else
            {
               ++depth;
            }

            if (parent->getNumParents() > 0)
            {
               parent = parent->getParent(0);
            }
            else
            {
               parent = NULL;
            }
         }

         if ( ! skeletonFound)
         {
            depth = -1;
         }

         return depth;
      }

      class ParentChildPair : public osg::Referenced
      {
      public:
         ParentChildPair(osg::Group& parent, osg::Node& child)
            : mParent(&parent)
            , mChild(&child)
            , mNodeDepth(0)
         {
            mNodeDepth = GetDepthFromSkeleton(child);
         }

         dtCore::RefPtr<osg::Group> mParent;
         dtCore::RefPtr<osg::Node> mChild;
         int mNodeDepth;

      protected:
         virtual ~ParentChildPair()
         {}
      };

      // Predicat method
      struct IsNullPred
      {
         bool operator()(ParentChildPair* pair)
         {
            return ! pair->mParent.valid();
         }
      };

      /**
       * This method takes a parent argument in case the node has
       * multiple parents and may need to be attached to multiple parents.
       */
      bool AddNode(osg::Group& parent, osg::Node& child)
      {
         child.getStateSet()->setTextureAttributeAndModes(0,NULL,osg::StateAttribute::OFF);

         mNameParentChildMap[parent.getName()].push_back(new ParentChildPair(parent, child));
         return true;
      }

      bool Process(osg::Group& node)
      {
         NameParentChildMap::iterator foundIter = mNameParentChildMap.find(node.getName());
         if (foundIter != mNameParentChildMap.end())
         {
            ParentChildPair* curPair = NULL;
            ParentChildArray* pairsArray = &foundIter->second;
            ParentChildArray::iterator curIter = pairsArray->begin();

            while (curIter != pairsArray->end())
            {
               curPair = curIter->get();

               if (curPair != NULL && curPair->mParent.valid())
               {
                  // Determine if the current node is a true match
                  // with the parent in the current pair, even though
                  // the current node matches by name.
                  int nodeDepth = GetDepthFromSkeleton(node);
                  bool isMatch = strcmp(curPair->mParent->className(), node.className()) == 0
                     && nodeDepth == curPair->mNodeDepth - 1;

                  if (isMatch)
                  {
                     // Attach the geode to the current node.
                     node.addChild(curPair->mChild.get());

                     // Remove the current pair since the associated
                     // node has been attached.
                     curPair->mParent = NULL;
                     curPair->mChild = NULL;
                  }
               }

               ++curIter;
            }

            // Remove NULL entries from the current array.
            if ( ! pairsArray->empty())
            {
               IsNullPred pred;
               std::remove_if(pairsArray->begin(), pairsArray->end(), pred);
            }

            // Remove the node array from the map since they have been attached.
            if (pairsArray->empty())
            {
               mNameParentChildMap.erase(foundIter);
            }
         }

         // Continue traversal only if there are more nodes to be attached.
         return ! mNameParentChildMap.empty();
      }

      virtual void apply(osg::Group& node)
      {
         if (Process(node))
         {
            traverse(node);
         }
      }

      virtual void apply(osg::MatrixTransform& node)
      {
         if (Process(node))
         {
            traverse(node);
         }
      }

   protected:
      virtual ~NodeAttacher()
      {}

      typedef std::vector<dtCore::RefPtr<ParentChildPair> > ParentChildArray;
      typedef std::map<std::string, ParentChildArray> NameParentChildMap;
      NameParentChildMap mNameParentChildMap;
   };



   /////////////////////////////////////////////////////////////////////////////
   // CLASS CODE
   /////////////////////////////////////////////////////////////////////////////
   const osg::CopyOp::Options OsgModelData::DEFAULT_COPY_OPTIONS
      = (osg::CopyOp::Options)(osg::CopyOp::DEEP_COPY_NODES
      | osg::CopyOp::DEEP_COPY_DRAWABLES
      | osg::CopyOp::DEEP_COPY_STATESETS
      | osg::CopyOp::DEEP_COPY_STATEATTRIBUTES
      | osg::CopyOp::DEEP_COPY_UNIFORMS
      | osg::CopyOp::DEEP_COPY_CALLBACKS
      | osg::CopyOp::DEEP_COPY_USERDATA);

   /////////////////////////////////////////////////////////////////////////////
   OsgModelData::OsgModelData(const std::string& modelName, const dtCore::ResourceDescriptor& resource)
      : BaseClass(modelName, resource, Constants::CHARACTER_SYSTEM_OSG)
   {
      // TODO:
   }
      
   /////////////////////////////////////////////////////////////////////////////
   OsgModelData::~OsgModelData()
   {
      mCoreAnims.clear();
      mCoreModel = NULL;
   }

   /////////////////////////////////////////////////////////////////////////////
   void OsgModelData::SetCoreModel(osg::Node* model)
   {
      if (mCoreModel != model)
      {
         mCoreModel = model;

         // Model has changes so ensure resource references match.
         UpdateResources();
      }
   }

   /////////////////////////////////////////////////////////////////////////////
   osg::Node* OsgModelData::GetCoreModel()
   {
      return mCoreModel.get();
   }

   /////////////////////////////////////////////////////////////////////////////
   const osg::Node* OsgModelData::GetCoreModel() const
   {
      return mCoreModel.get();
   }

   /////////////////////////////////////////////////////////////////////////////
   dtCore::RefPtr<osg::Node> OsgModelData::CreateModelClone(osg::CopyOp::Options copyOptions) const
   {
      dtCore::RefPtr<osg::Node> node;

      if (mCoreModel.valid())
      {
         node = dynamic_cast<osg::Node*>(mCoreModel->clone(copyOptions));
      }

      return node;
   }
   
   /////////////////////////////////////////////////////////////////////////////
   const osgAnimation::Animation* OsgModelData::GetCoreAnimation(const std::string& name) const
   {
      OsgAnimationMap::const_iterator foundIter = mCoreAnims.find(name);
      return foundIter == mCoreAnims.end() ? NULL : foundIter->second.get();
   }

   /////////////////////////////////////////////////////////////////////////////
   float OsgModelData::GetAnimationDuration(const std::string& name) const
   {
      float duration = 0.0f;

      const osgAnimation::Animation* anim = GetCoreAnimation(name);
      if (anim != NULL)
      {
         duration = anim->getDuration();
      }

      return duration;
   }

   /////////////////////////////////////////////////////////////////////////////
   float OsgModelData::GetAnimationWeight(const std::string& name) const
   {
      float weight = 1.0f;

      const osgAnimation::Animation* anim = GetCoreAnimation(name);
      if (anim != NULL)
      {
         weight = anim->getWeight();
      }

      return weight;
   }

   /////////////////////////////////////////////////////////////////////////////
   int OsgModelData::LoadResource(dtAnim::ModelResourceType resourceType,
      const std::string& file, const std::string& objectName)
   {
      dtCore::RefPtr<dtAnim::OsgLoader> loader = new dtAnim::OsgLoader;
      
      dtCore::RefPtr<osg::Node> node = loader->LoadResourceFile(file, resourceType);
      if (node.valid())
      {
         if (ApplyNodeToModel(resourceType, *node) > 0)
         {
            RegisterFile(file, objectName, resourceType);
         }
      }

      return node.valid() ? 1 : 0;
   }

   /////////////////////////////////////////////////////////////////////////////
   int OsgModelData::UnloadResource(dtAnim::ModelResourceType resourceType, const std::string& objectName)
   {
      // TODO:
      return 0;
   }

   /////////////////////////////////////////////////////////////////////////////
   dtAnim::ModelResourceType OsgModelData::GetFileType(const std::string& file) const
   {
      // Do nothing. OSG can load many file types so a specific
      // resource type cannot be inferred by the file extension.
      return dtAnim::NO_FILE;
   }

   /////////////////////////////////////////////////////////////////////////////
   int OsgModelData::GetIndexForObjectName(ModelResourceType fileType, const std::string& objectName) const
   {
      // TODO:
      return 0;
   }

   /////////////////////////////////////////////////////////////////////////////
   const OsgModelData::OsgAnimationMap& OsgModelData::GetCoreAnimations() const
   {
      return mCoreAnims;
   }

   /////////////////////////////////////////////////////////////////////////////
   const OsgModelData::OsgMaterialMap& OsgModelData::GetCoreMaterials() const
   {
      return mCoreMaterials;
   }

   /////////////////////////////////////////////////////////////////////////////
   void OsgModelData::ClearResources()
   {
      mCoreAnims.clear();
      mCoreMaterials.clear();
   }

   /////////////////////////////////////////////////////////////////////////////
   void OsgModelData::UpdateResources()
   {
      if (mCoreModel.valid())
      {
         dtCore::RefPtr<OsgModelResourceFinder> resFinder = new OsgModelResourceFinder;
         mCoreModel->accept(*resFinder);

         UpdateCoreAnimations(*resFinder);
         UpdateCoreMaterials(*resFinder);
      }
      else
      {
         ClearResources();
      }
   }

   /////////////////////////////////////////////////////////////////////////////
   void OsgModelData::UpdateCoreAnimations(OsgModelResourceFinder& finder)
   {
      typedef OsgModelResourceFinder::OsgAnimationArray OsgAnimArray;
      OsgAnimArray anims;
      finder.GetAnimations(anims);

      osgAnimation::Animation* anim = NULL;
      OsgAnimArray::iterator curIter = anims.begin();
      OsgAnimArray::iterator endIter = anims.end();
      for (; curIter != endIter; ++curIter)
      {
         anim = curIter->get();
         mCoreAnims.insert(std::make_pair(anim->getName(), anim));
      }

      // Go through all the core animations and ensure their
      // durations are all precalculated.
      OsgAnimationMap::iterator curCoreIter = mCoreAnims.begin();
      OsgAnimationMap::iterator endCoreIter = mCoreAnims.end();
      for (; curCoreIter != endCoreIter; ++curCoreIter)
      {
         anim = curCoreIter->second.get();

         // Ensure that the animation's duration is valid.
         if (anim->getDuration() == 0.0f)
         {
            anim->computeDuration();
         }

         // Ensure that the animation's weight is valid.
         if (anim->getWeight() == 0.0f)
         {
            anim->setWeight(1.0f);
         }
      }
   }

   /////////////////////////////////////////////////////////////////////////////
   void OsgModelData::UpdateCoreMaterials(OsgModelResourceFinder& finder)
   {
      typedef dtAnim::OsgModelResourceFinder::OsgMaterialArray OsgMatArray;
      OsgMatArray::iterator curIter = finder.mMaterials.begin();
      OsgMatArray::iterator endIter = finder.mMaterials.end();
      for (; curIter != endIter; ++curIter)
      {
         AddOrReplaceCoreMaterial(*curIter->get());
      }
   }

   /////////////////////////////////////////////////////////////////////////////
   void OsgModelData::AddOrReplaceCoreMaterial(osg::StateSet& material)
   {
      const std::string& name = material.getName();
      OsgMaterialMap::iterator foundIter = mCoreMaterials.find(name);
      if (foundIter != mCoreMaterials.end())
      {
         foundIter->second = &material;
      }
      else
      {
         mCoreMaterials.insert(std::make_pair(name, &material));
      }
   }

   /////////////////////////////////////////////////////////////////////////////
   int OsgModelData::ApplyAnimationsToModel(OsgModelResourceFinder& finder)
   {
      int results = 0;

      typedef dtAnim::OsgModelResourceFinder::OsgAnimationArray OsgAnimationArray;
      typedef dtAnim::OsgModelResourceFinder::OsgAnimManagerArray OsgAnimManagerArray;

      osg::Node* node = GetOrCreateModelNode();

      // Acquire or create an animation manager in the original model.
      dtCore::RefPtr<OsgModelResourceFinder> finderOfOriginal = new OsgModelResourceFinder;
      node->accept(*finderOfOriginal);

      // Get or create a new animation manager for the current model.
      dtCore::RefPtr<osgAnimation::BasicAnimationManager> originalAnimManager;

      if ( ! finderOfOriginal->mAnimManagers.empty())
      {
         originalAnimManager = finderOfOriginal->mAnimManagers.front();
      }

      // Create the animation manager if it does not yet exist.
      if ( ! originalAnimManager.valid())
      {
         originalAnimManager = new osgAnimation::BasicAnimationManager;
         node->addUpdateCallback(originalAnimManager);
      }

      // Find all the new animations and add them to the original
      // animation manager on the current model.
      osgAnimation::BasicAnimationManager* animManager = NULL;
      OsgAnimManagerArray::iterator curIter = finder.mAnimManagers.begin();
      OsgAnimManagerArray::iterator endIter = finder.mAnimManagers.end();
      for (; curIter != endIter; ++curIter)
      {
         animManager = curIter->get();

         osgAnimation::Animation* anim = NULL;
         const osgAnimation::AnimationList& animList = animManager->getAnimationList();
         osgAnimation::AnimationList::const_iterator curAnimIter = animList.begin();
         osgAnimation::AnimationList::const_iterator endAnimIter = animList.end();
         for (; curAnimIter != endAnimIter; ++curAnimIter)
         {
            anim = curAnimIter->get();

            if (anim->getDuration() == 0.0f)
            {
               anim->computeDuration();
            }
            originalAnimManager->registerAnimation(anim);
            ++results;
         }
      }

      return results;
   }

   /////////////////////////////////////////////////////////////////////////////
   int OsgModelData::ApplyMaterialsToModel(OsgModelResourceFinder& finder)
   {
      int results = (int)(finder.mMaterials.size());

      if (results > 0)
      {
         UpdateCoreMaterials(finder);
      }
      
      return results;
   }

   /////////////////////////////////////////////////////////////////////////////
   int OsgModelData::ApplyMeshesToModel(OsgModelResourceFinder& finder)
   {
      int results = 0;

      osg::Node* coreModel = GetOrCreateModelNode();

      // This should have been set in the previous call at least.
      osgAnimation::Skeleton* skel = mSkeleton.get();

      dtCore::RefPtr<NodeAttacher> attacher = new NodeAttacher;

      dtCore::RefPtr<osg::Geode> geode = NULL;
      typedef OsgModelResourceFinder::OsgGeodeArray OsgGeodeArray;
      OsgGeodeArray::iterator curIter = finder.mMeshes.begin();
      OsgGeodeArray::iterator endIter = finder.mMeshes.end();
      for (; curIter != endIter; ++curIter)
      {
         geode = curIter->get();

         // Remove the mesh from its current parents
         while (geode->getNumParents() > 0)
         {
            osg::Group* parent = geode->getParent(0);
            attacher->AddNode(*parent, *geode);

            parent->removeChild(geode);
         }

         // If there are rig geometries, make sure they have references
         // to the currently loaded skeleton.
         osgAnimation::RigGeometry* rigGeom = NULL;

         int numDrawables = geode->getNumDrawables();
         for (int i = 0; i < numDrawables; ++i)
         {
            osg::Drawable* geom = geode->getDrawable(i)->asGeometry();
            rigGeom = dynamic_cast<osgAnimation::RigGeometry*>(curIter->get());
            if (rigGeom != NULL)
            {
               rigGeom->setSkeleton(skel);
            }
         }

         ++results;
      }

      // Attach the geodes to the current model.
      coreModel->accept(*attacher);

      return results;
   }

   /////////////////////////////////////////////////////////////////////////////
   int OsgModelData::ApplyMorphTargetsToModel(OsgModelResourceFinder& finder)
   {
      // TODO:
      return 0;
   }

   /////////////////////////////////////////////////////////////////////////////
   int OsgModelData::ApplySkeletonToModel(OsgModelResourceFinder& finder)
   {
      int results = 0;

      if (finder.mSkeleton.valid())
      {
         dtCore::RefPtr<osgAnimation::Skeleton> oldSkel = mSkeleton;
         
         // Keep track of the new root skeleton node.
         OsgModelResourceCloner clonerOp(DEFAULT_COPY_OPTIONS, dtAnim::SKEL_FILE);
         mSkeleton = dynamic_cast<osgAnimation::Skeleton*>(finder.mSkeleton->clone(clonerOp));

         // Determine if there is already an existing model root node.
         osg::Node* node = GetCoreModel();
         if (node == NULL)
         {
            // Set the current skeleton node as the root for the model.
            SetCoreModel(mSkeleton.get());
         }
         else // Determine if there is an existing skeleton in the current model.
         {
            dtCore::RefPtr<OsgModelResourceFinder> originalFinder
               = new OsgModelResourceFinder(OsgModelResourceFinder::SEARCH_SKELETON);
            node->accept(*originalFinder);

            if (originalFinder->mSkeleton.valid())
            {
               if (oldSkel.valid() && oldSkel.get() != originalFinder->mSkeleton.get())
               {
                  LOG_WARNING("Old skeleton for model \"" + GetModelName() + "\" not the same as previously created skeleton.");
               }

               oldSkel = originalFinder->mSkeleton;
            }

            // If the old skeleton is the current root node...
            if (oldSkel == node)
            {
               // ...replace it with the new skeleton.
               SetCoreModel(mSkeleton);
            }
         }

         // Replace old skeleton with the new skeleton.
         if (oldSkel.valid())
         {
            // Transfer the children nodes from the old node to the new one.
            osg::Node* child = NULL;
            while (oldSkel->getNumChildren() > 0)
            {
               child = oldSkel->getChild(0);
               mSkeleton->addChild(child);
               oldSkel->removeChild(child);
            }
               
            // Attach the new node to the old node's parent(s).
            // Remove the old node from its parents.
            while (oldSkel->getNumParents() > 0)
            {
               osg::Group* parent = oldSkel->getParent(0);
               parent->removeChild(oldSkel.get());
               parent->addChild(mSkeleton.get());
            }
         }

         ++results;
      }

      return results;
   }

   /////////////////////////////////////////////////////////////////////////////
   int OsgModelData::ApplyAllResourcesToModel(OsgModelResourceFinder& finder)
   {
      int results = 0;

      results += ApplySkeletonToModel(finder);
      results += ApplyMeshesToModel(finder);
      results += ApplyMaterialsToModel(finder);
      results += ApplyAnimationsToModel(finder);
      results += ApplyMorphTargetsToModel(finder);

      return results;
   }

   /////////////////////////////////////////////////////////////////////////////
   int OsgModelData::ApplyResourcesToModel(dtAnim::ModelResourceType resType,
      OsgModelResourceFinder& finder)
   {
      int results = 0;

      switch (resType)
      {
      case dtAnim::ANIM_FILE:
         results += ApplyAnimationsToModel(finder);
         break;

      case dtAnim::MAT_FILE:
         results += ApplyMaterialsToModel(finder);
         break;

      case dtAnim::MESH_FILE:
         results += ApplyMeshesToModel(finder);
         break;

      case dtAnim::MORPH_FILE:
         results += ApplyMorphTargetsToModel(finder);
         break;

      case dtAnim::SKEL_FILE:
         results += ApplySkeletonToModel(finder);
         break;

      case dtAnim::MIXED_FILE:
      default:
         results += ApplyAllResourcesToModel(finder);
         break;
      }

      return results;
   }

   /////////////////////////////////////////////////////////////////////////////
   int OsgModelData::ApplyNodeToModel(dtAnim::ModelResourceType resType, osg::Node& node)
   {
      int results = 0;
      
      // Apply resources from the node but only search for
      // resources if a model node already exists; otherwise,
      // the node will be the new model.

      if (GetCoreModel() == NULL)
      {
         SetCoreModel(&node);
         ++results;
      }
      else // Add new resource nodes to existing model.
      {
         dtCore::RefPtr<OsgModelResourceFinder> newFinder = new OsgModelResourceFinder;
         SetFinderMode(resType, *newFinder);
         node.accept(*newFinder);

         results += ApplyResourcesToModel(resType, *newFinder);
      }

      return results;
   }
   
   /////////////////////////////////////////////////////////////////////////////
   osg::Node* OsgModelData::GetOrCreateModelNode()
   {
      if ( ! mCoreModel.valid())
      {
         dtCore::RefPtr<osg::MatrixTransform> xform = new osg::MatrixTransform;
         mCoreModel = xform.get();
         mCoreModel->setName(GetModelName());

         mSkeleton = new osgAnimation::Skeleton;
         mSkeleton->setName("Skeleton");
         xform->addChild(mSkeleton.get());

         SetCoreModel(mCoreModel.get());
      }

      return mCoreModel.get();
   }

   /////////////////////////////////////////////////////////////////////////////
   osgAnimation::Skeleton* OsgModelData::GetOrCreateSkeleton()
   {
      osgAnimation::Skeleton* skel = NULL;

      if (mSkeleton.valid())
      {
         skel = mSkeleton.get();
      }
      else
      {
         // Determine if a skeleton node is somewhere under the
         // current root model node.
         osg::Node* node = GetCoreModel();
         if (node != NULL)
         {
            dtCore::RefPtr<OsgModelResourceFinder> originalFinder
               = new OsgModelResourceFinder(OsgModelResourceFinder::SEARCH_SKELETON);
            node->accept(*originalFinder);
            mSkeleton = originalFinder->mSkeleton;

            skel = mSkeleton.get();
         }
      }

      // If nothing was found, create a new skeleton node.
      if (skel == NULL)
      {
         GetOrCreateModelNode();

         // The skeleton reference should have been created in GetOrCreateModelNode.
         skel = mSkeleton.get();
      }

      return skel;
   }

   /////////////////////////////////////////////////////////////////////////////
   void OsgModelData::SetFinderMode(dtAnim::ModelResourceType resType, OsgModelResourceFinder& finder)
   {
      switch (resType)
      {
      case dtAnim::ANIM_FILE:
         finder.mMode = OsgModelResourceFinder::SEARCH_ANIMATIONS;
         break;

      case dtAnim::MAT_FILE:
         finder.mMode = OsgModelResourceFinder::SEARCH_MATERIALS;
         break;

      case dtAnim::MESH_FILE:
         finder.mMode = OsgModelResourceFinder::SEARCH_MESHES;
         break;

      case dtAnim::MORPH_FILE:
         finder.mMode = OsgModelResourceFinder::SEARCH_MORPHS;
         break;

      case dtAnim::SKEL_FILE:
         finder.mMode = OsgModelResourceFinder::SEARCH_SKELETON;
         break;

      default:
         finder.mMode = OsgModelResourceFinder::SEARCH_ALL;
         break;
      }
   }

}


////////////////////////////////////////////////////////////////////////////////
// INCLUDE DIRECTIVES
////////////////////////////////////////////////////////////////////////////////
#include <dtAnim/osgmodelresourcefinder.h>
#include <dtUtil/log.h>
#include <osg/CopyOp>
#include <osg/Geode>
#include <osg/MatrixTransform>
#include <osgAnimation/BasicAnimationManager>
#include <osgAnimation/Bone>
#include <osgAnimation/Skeleton>
#include <sstream>


namespace dtAnim
{
   /////////////////////////////////////////////////////////////////////////////
   // CLASS CODE
   /////////////////////////////////////////////////////////////////////////////
   OsgModelResourceFinder::OsgModelResourceFinder(SearchMode mode)
      : BaseClass(BaseClass::TRAVERSE_ALL_CHILDREN)
      , mMode(mode)
   {}

   OsgModelResourceFinder::~OsgModelResourceFinder()
   {}

   void OsgModelResourceFinder::AcquireAnimationManager(osg::Node& node)
   {
      if (mMode == SEARCH_ALL || mMode == SEARCH_ANIMATIONS)
      {
         osg::NodeCallback* ucb = node.getUpdateCallback();

         if (ucb != NULL && (0 == strcmp(ucb->className(), "BasicAnimationManager")))
         {
            osgAnimation::BasicAnimationManager* animManager
               = static_cast<osgAnimation::BasicAnimationManager*>(ucb);
            mAnimManagers.push_back(animManager);
            mAnimNodes.push_back(&node);
         }
      }
   }

   void OsgModelResourceFinder::AcquireMaterial(osg::Geode& geode)
   {
      if (mMode == SEARCH_ALL || mMode == SEARCH_MATERIALS)
      {
         typedef osg::Geode::DrawableList GeometryList;
         GeometryList geoms = geode.getDrawableList();

         GeometryList::iterator curIter = geoms.begin();
         GeometryList::iterator endIter = geoms.end();
         for (; curIter != endIter; ++curIter)
         {
            osg::StateSet* stateSet = (*curIter)->getStateSet();

            // Add the stateset/material if unique.
            if (stateSet != NULL && std::find(mMaterials.begin(), mMaterials.end(), stateSet) == mMaterials.end())
            {
               mMaterials.push_back(stateSet);
            }
         }
      }
   }

   void OsgModelResourceFinder::apply(osg::Node& node)
   {
      AcquireAnimationManager(node);

      traverse(node);
   }

   void OsgModelResourceFinder::apply(osg::MatrixTransform& node)
   {
      bool findBones = mMode == SEARCH_ALL || mMode == SEARCH_BONES;
      if (findBones || mMode == SEARCH_SKELETON)
      {
         if (findBones && 0 == strcmp(node.className(), "Bone"))
         {
            osgAnimation::Bone* bone = static_cast<osgAnimation::Bone*>(&node);
            mBones.push_back(bone);
         }
         else if (0 == strcmp(node.className(), "Skeleton"))
         {
            mSkel = static_cast<osgAnimation::Skeleton*>(&node);
         }
      }

      AcquireAnimationManager(node);

      traverse(node);
   }

   void OsgModelResourceFinder::apply(osg::Geode& node)
   {
      if (mMode == SEARCH_ALL || mMode == SEARCH_MESHES)
      {
         // Add the geode directly if it is not already
         // referenced in the meshes array.
         if (std::find(mMeshes.begin(), mMeshes.end(), &node)
            == mMeshes.end())
         {
            mMeshes.push_back(&node);
         }
      }

      AcquireAnimationManager(node);
      AcquireMaterial(node);

      traverse(node);
   }

   void OsgModelResourceFinder::Clear()
   {
      mAnimNodes.clear();
      mAnimManagers.clear();
      mBones.clear();
      mMeshes.clear();
      mSkel = NULL;
   }

   void OsgModelResourceFinder::Reset()
   {
      Clear();
      mMode = SEARCH_ALL;
   }

   int OsgModelResourceFinder::GetAnimations(OsgAnimationArray& outAnims) const
   {
      int results = 0;

      osgAnimation::BasicAnimationManager* animManager = NULL;
      OsgAnimManagerArray::const_iterator curIter = mAnimManagers.begin();
      OsgAnimManagerArray::const_iterator endIter = mAnimManagers.end();
      for (; curIter != endIter; ++curIter)
      {
         animManager = curIter->get();
         const osgAnimation::AnimationList& animList = animManager->getAnimationList();
         outAnims.insert(outAnims.end(), animList.begin(), animList.end());

         results += (int)(animList.size());
      }

      return results;
   }

}

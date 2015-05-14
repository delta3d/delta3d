
////////////////////////////////////////////////////////////////////////////////
// INCLUDE DIRECTIVES
////////////////////////////////////////////////////////////////////////////////
#include <dtAnim/osgmodelresourcefinder.h>
#include <dtUtil/log.h>
#include <osgAnimation/BasicAnimationManager>
#include <osgAnimation/MorphGeometry>
#include <osgAnimation/Skeleton>
#include <osg/CopyOp>
#include <osg/Geode>
#include <osg/MatrixTransform>
#include <sstream>
#include <algorithm>

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

   void OsgModelResourceFinder::AcquireCommonObjects(osg::Node& node)
   {
      AcquireAnimationManager(node);
      AcquireMaterial(node);
   }

   void OsgModelResourceFinder::AcquireAnimationManager(osg::Node& node)
   {
      osg::Callback* ucb = node.getUpdateCallback();

      if (ucb != NULL)
      {
         AcquireAnimationManagerFromCallback(*ucb, node);
      }
   }

   void OsgModelResourceFinder::AcquireAnimationManagerFromCallback(osg::Callback& callback, osg::Node& node)
   {
      if (mMode == SEARCH_ALL || mMode == SEARCH_ANIMATIONS || mMode == SEARCH_MORPHS)
      {
         osg::Callback* curCallback = &callback;

         while (curCallback != NULL)
         {
            if (0 == strcmp(curCallback->className(), "BasicAnimationManager"))
            {
               if (mMode == SEARCH_ALL || mMode == SEARCH_ANIMATIONS)
               {
                  osgAnimation::BasicAnimationManager* animManager
                     = dynamic_cast<osgAnimation::BasicAnimationManager*>(curCallback);
                  mAnimManagers.push_back(animManager);
                  mAnimNodes.push_back(&node);
               }
            }
            else if (0 == strcmp(curCallback->className(), "UpdateMorph"))
            {
               if (mMode == SEARCH_ALL || mMode == SEARCH_MORPHS)
               {
                  osgAnimation::UpdateMorph* morphManager
                      = dynamic_cast<osgAnimation::UpdateMorph*>(curCallback);
                  mMorphManagers.insert(std::make_pair(morphManager, &node));
               }
            }

            curCallback = curCallback->getNestedCallback();
         }
      }
   }

   void OsgModelResourceFinder::AcquireMaterial(osg::Node& node)
   {
      if (mMode == SEARCH_ALL || mMode == SEARCH_MATERIALS)
      {
         // Determine if the node has material assignment.
         osg::StateSet* stateSet = node.getStateSet();
         if (stateSet != NULL)
         {
            mMaterials.push_back(stateSet);
            mMaterialToObjectMap.insert(std::make_pair(stateSet, &node));
         }
      }
   }

   void OsgModelResourceFinder::AcquireMaterialFromGeometry(osg::Geode& geode)
   {
      if (mMode == SEARCH_ALL || mMode == SEARCH_MATERIALS)
      {
         int numDrawables = geode.getNumDrawables();
         for (int i = 0; i < numDrawables; ++i)
         {
            osg::Drawable* geom = geode.getDrawable(i);
            osg::StateSet* stateSet = geom->getStateSet();

            // Add the stateset/material if unique.
            if (stateSet != NULL && std::find(mMaterials.begin(), mMaterials.end(), stateSet) == mMaterials.end())
            {
               mMaterials.push_back(stateSet);
               mMaterialToObjectMap.insert(std::make_pair(stateSet, geom));
            }
         }
      }
   }

   void OsgModelResourceFinder::AcquireMorphs(osg::Geode& geode)
   {
       int numDrawables = geode.getNumDrawables();
       for (int i = 0; i < numDrawables; ++i)
       {
         osg::Drawable* geom = geode.getDrawable(i);
         if (0 == strcmp(geom->className(), "MorphGeometry"))
         {
            osgAnimation::MorphGeometry* morph
               = static_cast<osgAnimation::MorphGeometry*>(geom);
            mMorphs.push_back(morph);
         }
      }
   }

   void OsgModelResourceFinder::apply(osg::Node& node)
   {
      AcquireCommonObjects(node);

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
            mSkeleton = static_cast<osgAnimation::Skeleton*>(&node);
         }
      }

      AcquireCommonObjects(node);

      traverse(node);
   }

   void OsgModelResourceFinder::apply(osg::Geode& node)
   {
      if (mMode == SEARCH_ALL || mMode == SEARCH_MESHES || mMode == SEARCH_MORPHS)
      {
         if (mMode == SEARCH_ALL || mMode == SEARCH_MORPHS)
         {
            AcquireMorphs(node);
         }

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
      }

      AcquireCommonObjects(node);
      AcquireMaterialFromGeometry(node);

      traverse(node);
   }

   void OsgModelResourceFinder::Clear()
   {
      mAnimNodes.clear();
      mAnimManagers.clear();
      mBones.clear();
      mMaterials.clear();
      mMaterialToObjectMap.clear();
      mMeshes.clear();
      mMorphs.clear();
      mMorphManagers.clear();
      mSkeleton = NULL;

      mMaterialToObjectMap.clear();
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

   osg::Geode* OsgModelResourceFinder::GetGeodeByName(const std::string& name) const
   {
      osg::Geode* geode = NULL;

      OsgGeodeArray::const_iterator curIter = mMeshes.begin();
      OsgGeodeArray::const_iterator endIter = mMeshes.end();
      for (; curIter != endIter; ++curIter)
      {
         geode = curIter->get();

         if (geode->getName() == name)
         {
            break;
         }
         else
         {
            geode = NULL;
         }
      }

      return geode;
   }

}

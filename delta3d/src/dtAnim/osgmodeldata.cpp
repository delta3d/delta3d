
////////////////////////////////////////////////////////////////////////////////
// INCLUDE DIRECTIVES
////////////////////////////////////////////////////////////////////////////////
#include <dtAnim/osgmodeldata.h>
#include <dtAnim/constants.h>
#include <dtAnim/osgmodelresourcefinder.h>



namespace dtAnim
{
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

   OsgModelData::OsgModelData(const std::string& modelName, const std::string& filename)
      : BaseClass(modelName, filename, Constants::CHARACTER_SYSTEM_OSG)
   {
      // TODO:
   }
      
   OsgModelData::~OsgModelData()
   {
      mCoreAnims.clear();
      mCoreModel = NULL;
   }

   void OsgModelData::SetCoreModel(osg::Node* model)
   {
      mCoreModel = model;

      UpdateCoreAnimations();
   }

   const osg::Node* OsgModelData::GetCoreModel() const
   {
      return mCoreModel.get();
   }

   dtCore::RefPtr<osg::Node> OsgModelData::CreateModelClone(osg::CopyOp::Options copyOptions) const
   {
      dtCore::RefPtr<osg::Node> node;

      if (mCoreModel.valid())
      {
         node = dynamic_cast<osg::Node*>(mCoreModel->clone(copyOptions));
      }

      return node;
   }
   
   const osgAnimation::Animation* OsgModelData::GetCoreAnimation(const std::string& name) const
   {
      AnimMap::const_iterator foundIter = mCoreAnims.find(name);
      return foundIter == mCoreAnims.end() ? NULL : foundIter->second.get();
   }

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

   int OsgModelData::LoadResource(dtAnim::ModelResourceType resourceType,
      const std::string& file, const std::string& objectName)
   {
      // TODO:
      return 0;
   }

   int OsgModelData::UnloadResource(dtAnim::ModelResourceType resourceType, const std::string& objectName)
   {
      // TODO:
      return 0;
   }

   dtAnim::ModelResourceType OsgModelData::GetFileType(const std::string& file) const
   {
      // TODO:
      return dtAnim::NO_FILE;
   }

   int OsgModelData::GetIndexForObjectName(ModelResourceType fileType, const std::string& objectName) const
   {
      // TODO:
      return 0;
   }

   void OsgModelData::UpdateCoreAnimations()
   {
      if (mCoreModel.valid())
      {
         OsgModelResourceFinder resFinder;
         mCoreModel->accept(resFinder);

         typedef OsgModelResourceFinder::OsgAnimationArray OsgAnimArray;
         OsgAnimArray anims;
         resFinder.GetAnimations(anims);

         osgAnimation::Animation* anim = NULL;
         OsgAnimArray::iterator curIter = anims.begin();
         OsgAnimArray::iterator endIter = anims.end();
         for (; curIter != endIter; ++curIter)
         {
            anim = curIter->get();
            mCoreAnims.insert(std::make_pair(anim->getName(), anim));
         }
      }

      // Go through all the core animations and ensure their
      // durations are all precalculated.
      osgAnimation::Animation* anim = NULL;
      AnimMap::iterator curIter = mCoreAnims.begin();
      AnimMap::iterator endIter = mCoreAnims.end();
      for (; curIter != endIter; ++curIter)
      {
         anim = curIter->second.get();

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

}

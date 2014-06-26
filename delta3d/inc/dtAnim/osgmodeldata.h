#ifndef __DELTA_OSGMODELDATA_H__
#define __DELTA_OSGMODELDATA_H__

////////////////////////////////////////////////////////////////////////////////
// INCLUDE DIRECTIVES
////////////////////////////////////////////////////////////////////////////////
#include <dtAnim/export.h>
#include <dtAnim/basemodeldata.h>
#include <osg/Node>
#include <osgAnimation/Animation>



namespace osgAnimation
{
   class Skeleton;
}

namespace dtAnim
{
   class OsgModelResourceFinder;



   /////////////////////////////////////////////////////////////////////////////
   // CLASS CODE
   /////////////////////////////////////////////////////////////////////////////
   class DT_ANIM_EXPORT OsgModelData: public dtAnim::BaseModelData
   {
   public:
      typedef dtAnim::BaseModelData BaseClass;

      static const osg::CopyOp::Options DEFAULT_COPY_OPTIONS;

      OsgModelData(const std::string& modelName, const std::string& filename);

      void SetCoreModel(osg::Node* model);
      osg::Node* GetCoreModel();
      const osg::Node* GetCoreModel() const;
   
      dtCore::RefPtr<osg::Node> CreateModelClone(osg::CopyOp::Options copyOptions = DEFAULT_COPY_OPTIONS) const;

      const osgAnimation::Animation* GetCoreAnimation(const std::string& name) const;

      virtual float GetAnimationDuration(const std::string& name) const;

      float GetAnimationWeight(const std::string& name) const;
      
      virtual int LoadResource(dtAnim::ModelResourceType resourceType,
         const std::string& file, const std::string& objectName);

      virtual int UnloadResource(dtAnim::ModelResourceType resourceType, const std::string& objectName);

      /**
       * Convenience method for determining the file type from the specified file.
       */
      virtual dtAnim::ModelResourceType GetFileType(const std::string& file) const;

      virtual int GetIndexForObjectName(ModelResourceType fileType, const std::string& objectName) const;
      
      int ApplyAnimationsToModel(OsgModelResourceFinder& finder);
      int ApplyMaterialsToModel(OsgModelResourceFinder& finder);
      int ApplyMeshesToModel(OsgModelResourceFinder& finder);
      int ApplyMorphTargetsToModel(OsgModelResourceFinder& finder);
      int ApplySkeletonToModel(OsgModelResourceFinder& finder);
      
      int ApplyAllResourcesToModel(OsgModelResourceFinder& finder);
   
      int OsgModelData::ApplyResourcesToModel(dtAnim::ModelResourceType resType,
         OsgModelResourceFinder& finder);

      int ApplyNodeToModel(dtAnim::ModelResourceType resType, osg::Node& node);
      
      osg::Node* GetOrCreateModelNode();

      osgAnimation::Skeleton* GetOrCreateSkeleton();

      void SetFinderMode(dtAnim::ModelResourceType resType, OsgModelResourceFinder& finder);

   protected:
      virtual ~OsgModelData();

      OsgModelData(const OsgModelData&); //not implemented
      OsgModelData& operator=(const OsgModelData&); //not implemented

      void UpdateCoreAnimations();

   private:
      dtCore::RefPtr<osg::Node> mCoreModel;

      typedef std::map<std::string, dtCore::RefPtr<osgAnimation::Animation> > AnimMap;
      AnimMap mCoreAnims;
      dtCore::RefPtr<osgAnimation::Skeleton> mSkeleton;

      typedef std::vector<dtCore::RefPtr<osg::StateSet> > MaterialArray;
      MaterialArray mMaterials;
   };

}

#endif

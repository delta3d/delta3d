#ifndef __DELTA_OSGMODELRESOURCEFINDER_H__
#define __DELTA_OSGMODELRESOURCEFINDER_H__

////////////////////////////////////////////////////////////////////////////////
// INCLUDE DIRECTIVES
////////////////////////////////////////////////////////////////////////////////
#include <dtAnim/export.h>
#include <dtCore/refptr.h>
#include <osg/NodeVisitor>
#include <osgAnimation/Bone>


////////////////////////////////////////////////////////////////////////////////
// FORWARD DECLARATIONS
////////////////////////////////////////////////////////////////////////////////
namespace osgAnimation
{
   class Animation;
   class BasicAnimationManager;
   class MorphGeometry;
   class Skeleton;
   class UpdateMorph;
}



namespace dtAnim
{
   /////////////////////////////////////////////////////////////////////////////
   // CLASS CODE
   /////////////////////////////////////////////////////////////////////////////
   class DT_ANIM_EXPORT OsgModelResourceFinder : public osg::NodeVisitor
   {
   public:
      typedef osg::NodeVisitor BaseClass;

      typedef std::vector<dtCore::RefPtr<osg::Node> > OsgNodeArray;
      typedef std::vector<dtCore::RefPtr<osgAnimation::BasicAnimationManager> > OsgAnimManagerArray;
      typedef std::vector<dtCore::RefPtr<osgAnimation::Animation> > OsgAnimationArray;
      typedef std::vector<dtCore::RefPtr<osgAnimation::Bone> > OsgBoneArray;
      typedef std::vector<dtCore::RefPtr<osg::StateSet> > OsgMaterialArray;
      typedef std::vector<dtCore::RefPtr<osg::Geode> > OsgGeodeArray;
      typedef std::vector<dtCore::RefPtr<osgAnimation::MorphGeometry> > OsgMorphGeometryArray;
      typedef std::map<osgAnimation::UpdateMorph*, dtCore::RefPtr<osg::Object> > OsgMorphManagerMap;

      typedef std::map<osg::StateSet*, dtCore::RefPtr<osg::Object> > MaterialObjectMap;

      typedef enum SearchModeE
      {
         SEARCH_ALL,
         SEARCH_ANIMATIONS,
         SEARCH_BONES,
         SEARCH_MATERIALS,
         SEARCH_MESHES,
         SEARCH_MORPHS,
         SEARCH_SKELETON,
      } SearchMode;

      explicit OsgModelResourceFinder(SearchMode mode = SEARCH_ALL);

      // Find objects that could be assigned to any node type.
      void AcquireCommonObjects(osg::Node& node);
      
      // Finds animation managers
      void AcquireAnimationManager(osg::Node& node);
      void AcquireAnimationManagerFromCallback(osg::Callback& callback, osg::Node& node);

      // Finds materials on nodes
      void AcquireMaterial(osg::Node& geode);

      // Finds materials on geometry
      void AcquireMaterialFromGeometry(osg::Geode& geode);

      // Finds only morph target geometry
      void AcquireMorphs(osg::Geode& geode);

      // Finds resources contained in generic nodes
      virtual void apply(osg::Node& node);

      // Finds skeleton and bones
      virtual void apply(osg::MatrixTransform& node);

      // Finds meshes
      virtual void apply(osg::Geode& node);

      // Removes all references to found objects
      void Clear();

      // Clears all references and resets to default search mode.
      void Reset();

      // Returns all animations for all animation managers.
      int GetAnimations(OsgAnimationArray& outAnims) const;

      osg::Geode* GetGeodeByName(const std::string& name) const;
      
      SearchMode mMode;

      OsgNodeArray mAnimNodes;
      OsgAnimManagerArray mAnimManagers;
      OsgBoneArray mBones;
      OsgMaterialArray mMaterials;
      OsgGeodeArray mMeshes;
      OsgMorphGeometryArray mMorphs;
      OsgMorphManagerMap mMorphManagers;
      dtCore::RefPtr<osgAnimation::Skeleton> mSkeleton;

      MaterialObjectMap mMaterialToObjectMap;

   protected:
      virtual ~OsgModelResourceFinder();
   };

}

#endif

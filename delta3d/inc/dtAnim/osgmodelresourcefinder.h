#ifndef __DELTA_OSGMODELRESOURCEFINDER_H__
#define __DELTA_OSGMODELRESOURCEFINDER_H__

////////////////////////////////////////////////////////////////////////////////
// INCLUDE DIRECTIVES
////////////////////////////////////////////////////////////////////////////////
#include <dtAnim/export.h>
#include <dtCore/refptr.h>
#include <osg/NodeVisitor>


////////////////////////////////////////////////////////////////////////////////
// FORWARD DECLARATIONS
////////////////////////////////////////////////////////////////////////////////
namespace osgAnimation
{
   class Animation;
   class BasicAnimationManager;
   class Bone;
   class Skeleton;
}



namespace dtAnim
{
   /////////////////////////////////////////////////////////////////////////////
   // CLASS CODE
   /////////////////////////////////////////////////////////////////////////////
   class OsgModelResourceFinder : public osg::NodeVisitor
   {
   public:
      typedef osg::NodeVisitor BaseClass;

      typedef std::vector<dtCore::RefPtr<osg::Node> > OsgNodeArray;
      typedef std::vector<dtCore::RefPtr<osgAnimation::BasicAnimationManager> > OsgAnimManagerArray;
      typedef std::vector<dtCore::RefPtr<osgAnimation::Animation> > OsgAnimationArray;
      typedef std::vector<dtCore::RefPtr<osgAnimation::Bone> > OsgBoneArray;
      typedef std::vector<dtCore::RefPtr<osg::StateSet> > OsgMaterialArray;
      typedef std::vector<dtCore::RefPtr<osg::Geode> > OsgGeodeArray;

      enum SearchMode
      {
         SEARCH_ALL,
         SEARCH_ANIMATIONS,
         SEARCH_BONES,
         SEARCH_MATERIALS,
         SEARCH_MESHES,
         SEARCH_SKELETON,
      };

      OsgModelResourceFinder(SearchMode mode = SEARCH_ALL);

      virtual ~OsgModelResourceFinder();
      
      // Finds animation managers
      void AcquireAnimationManager(osg::Node& node);

      // Finds materials
      void AcquireMaterial(osg::Geode& geode);

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
      
      SearchMode mMode;

      OsgNodeArray mAnimNodes;
      OsgAnimManagerArray mAnimManagers;
      OsgBoneArray mBones;
      OsgMaterialArray mMaterials;
      OsgGeodeArray mMeshes;
      dtCore::RefPtr<osgAnimation::Skeleton> mSkel;
   };

}

#endif

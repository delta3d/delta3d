#ifndef _STAGE_VOLUME_EDIT_ACTOR_H__
#define _STAGE_VOLUME_EDIT_ACTOR_H__

#include <dtDAL/plugin_export.h>

#include <dtCore/observerptr.h>
#include <dtCore/transformable.h>
#include <dtDAL/transformableactorproxy.h>

//forward Delta3D declarations
namespace dtCore
{
   class Model;
}

//forward osg declarations
/// @cond DOXYGEN_SHOULD_SKIP_THIS
namespace osg
{   
   class Geode;
   class Group;   
   class Shape;
   class ShapeDrawable;
}
/// @endcond

namespace dtActors
{
/* This class is intended for use with STAGE and defines a volume.  What happens
   with regard to the volume is variable: originally we intend to use it as
   a location where actors can be randomly generated.  However, it could be
   extended later to allow for the volume to be used for a great many things:
   a few volumes could be defined as "tiles" that can rapidly be repeated
   through a map.  A volume could be defined as a proximity trigger and
   also scaled, change its shape, and be duplicated with ease. */

class VolumeEditActorProxy;

class DT_PLUGIN_EXPORT VolumeEditActor : public dtCore::Transformable
{
public:
   class DT_PLUGIN_EXPORT VolumeShapeType : public dtUtil::Enumeration
   {
      DECLARE_ENUM(VolumeShapeType)

      public:
      static VolumeShapeType BOX;
      static VolumeShapeType SPHERE;
      static VolumeShapeType CYLINDER;
      static VolumeShapeType CAPSULE;
      static VolumeShapeType CONE;      

      private:
      VolumeShapeType(const std::string& name) : dtUtil::Enumeration(name)
      {
         AddInstance(this);
      }
   };

   VolumeEditActor();
   virtual ~VolumeEditActor();

   double GetBaseLength();
   double GetBaseRadius();   
   osg::Vec3 GetScale() const;
   VolumeShapeType& GetShape();

   void SetScale(const osg::Vec3& xyz);
   void SetShape(VolumeShapeType& shape);

   void EnableOutline(bool doEnable);

private:
   void SetupWireOutline();
   
   dtCore::RefPtr<osg::Group>          mVolumeGroup;
   dtCore::RefPtr<osg::Group>          mShaderGroup;   
   dtCore::RefPtr<osg::Geode>          mVolumeGeode;
   dtCore::RefPtr<osg::Shape>          mVolumeShape;
   dtCore::RefPtr<osg::ShapeDrawable>  mVolumeDrawable;

   //used so I can scale (see GameMeshActor -- where I stole this idea from)
   dtCore::RefPtr<dtCore::Model>       mModel;

   const double                        mBaseRadius;
   const double                        mBaseLength;
};

class DT_PLUGIN_EXPORT VolumeEditActorProxy : public dtDAL::TransformableActorProxy
{
public:
   VolumeEditActorProxy();
   virtual ~VolumeEditActorProxy();

   void CreateActor();

   void BuildPropertyMap();
   
   VolumeEditActor::VolumeShapeType& GetShape();
   void SetShape(VolumeEditActor::VolumeShapeType& shape);
};

}//end namespace dtActors

#endif //_STAGE_VOLUME_EDIT_ACTOR_H__

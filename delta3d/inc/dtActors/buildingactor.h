#ifndef BuildingActor_h__
#define BuildingActor_h__

#include <dtDAL/plugin_export.h>
#include <dtDAL/transformableactorproxy.h>
#include <dtABC/export.h>
#include <dtDAL/actorproperty.h>
#include <osgText/Text>
#include <dtActors/linkedpointsactorproxy.h>
#include <dtCore/refptr.h>
#include <osg/ref_ptr>
#include <dtCore/object.h>
#include <osg/Geode>
#include <osg/Geometry>

namespace dtDAL
{
   class ResourceDescriptor;
}

namespace dtActors
{
   ////////////////////////////////////////////////////////////////////////////////
   // FENCE POST GEOM DATA
   ////////////////////////////////////////////////////////////////////////////////
   class BuildingGeomData : public LinkedPointsGeomDataBase
   {
   public:
      BuildingGeomData(LinkedPointsGeomNodeBase* parent): 
         LinkedPointsGeomDataBase(parent) {}

      /**
      * Initializes the Geom Data.
      */
      bool Initialize();

      /**
      * Shuts down the Geom Data.
      */
      bool Shutdown();

      // Wall
      osg::ref_ptr<osg::Geometry>      mWallGeom;
      osg::ref_ptr<osg::Geode>         mWallGeode;

      osg::ref_ptr<osg::Vec3Array>     mWallVertexList;
      osg::ref_ptr<osg::Vec2Array>     mWallTextureList;
      osg::ref_ptr<osg::Vec3Array>     mWallNormalList;
   };

   ////////////////////////////////////////////////////////////////////////////////
   // FENCE POST GEOM NODE
   ////////////////////////////////////////////////////////////////////////////////
   class BuildingGeomNode : public LinkedPointsGeomNodeBase
   {
   public:

      /**
      * Default Constructor.
      */
      BuildingGeomNode();

      /**
      * Default Destructor.
      */
      ~BuildingGeomNode();

      /**
      * Creates a new geom data.  This should be overloaded
      * to create the Geom Data of the proper inheritance.
      */
      LinkedPointsGeomDataBase* CreateGeom();

      dtCore::RefPtr<dtCore::Transformable> mOrigin;
      osg::ref_ptr<osg::Vec4Array>          mSegColorList;
   };

   /////////////////////////////////////////////////////////////////////////////
   // ACTOR CODE
   /////////////////////////////////////////////////////////////////////////////
   class DT_PLUGIN_EXPORT BuildingActor : public dtActors::LinkedPointsActor
   {
   public:
      typedef dtActors::LinkedPointsActor BaseClass;

      struct RoofReturnData
      {
         bool bFailed;
         int firstFailedIndex;
         int endFailedIndex;
      };

      BuildingActor(dtActors::LinkedPointsActorProxy* proxy, const std::string& name = "BuildingActor");

      /**
      * Initializes the actor.
      */
      virtual bool Initialize();

      /**
      * This will visualize the current actor.
      */
      virtual void Visualize(void);

      /**
      * This will update the visuals on a single point.
      *
      * @param[in]  pointIndex  The point to update.
      */
      virtual void Visualize(int pointIndex);

      /**
      * Adds a new point into the point array.
      *
      * @param[in]  location  The location of the new point to be added.
      * @param[in]  index     The index to insert the new point, -1 to append.
      */
      void AddPoint(osg::Vec3 location, int index = -1);

      /**
      * Inserts a new point on a segment that is closest to
      * the location specified.
      *
      * @param[in]  location  The location to insert near.
      *
      * @return     Returns the index of the new point created, or -1 if
      *             the point could not be created.
      */
      int AddPointOnSegment(osg::Vec3 location);

      /**
      * Removes a point from the array.
      *
      * @param[in]  index  The index of the point to remove.
      */
      void RemovePoint(int index);

      /**
      * Changes the position or rotation of a point at the given index.
      *
      * @param[in]  index     The index of the point to edit.
      * @param[in]  location  The new location of the point.
      * @param[in]  rotation  The new rotation of the point.
      */
      void SetPointPosition(int index, osg::Vec3 location);
      void SetPointRotation(int index, osg::Vec3 rotation);

      /**
      * Retrieves the index of a point given a transformable.
      *
      * @param[in]  drawable  The drawable to find.
      *
      * @return     Returns the index of the found transformable or -1 if it doesn't exist.
      */
      int GetPointIndex(dtCore::DeltaDrawable* drawable, osg::Vec3 pickPos);

      /**
      * Sets the roof texture.
      */
      void SetRoofTexture(const std::string& fileName);

      /**
      * Sets the wall texture.
      */
      void SetWallTexture(const std::string& fileName);

      /**
      * Gets the width of the segments.
      */
      float GetRoofTextureScale() { return mRoofTextureScale; }

      /**
      * Sets the width of the segments.
      */
      void SetRoofTextureScale(float value);

      /**
      * Gets the width of the segments.
      */
      float GetWallTextureScale() { return mWallTextureScale; }

      /**
      * Sets the width of the segments.
      */
      void SetWallTextureScale(float value);

      /**
      * Gets the height of the building.
      */
      float GetBuildingHeight() { return mBuildingHeight; }

      /**
      * Sets the height of the building.
      */
      void SetBuildingHeight(float value);

      /**
      * Gets the scale of the fence.
      */
      osg::Vec3 GetScale() const;

      /**
      * Sets the scale fo the fence.
      */
      void SetScale(const osg::Vec3& value);

      /**
      * Creates the drawable representation of a new link point.
      *
      * @param[in]  position  The position of the drawable.
      *
      * @return               The new drawable.
      */
      virtual dtCore::Transformable* CreatePointDrawable(osg::Vec3 position);

   protected:
      virtual ~BuildingActor();

   private:

      /**
      * Places a segment.
      *
      * @param[in]  pointIndex  The parent point of the segment.
      * @param[in]  start       The start post transform.
      * @param[in]  end         The end post transform.
      */
      void PlaceWall(int pointIndex, osg::Vec3 start, osg::Vec3 end);

      /**
      * Calculates the height of the roof.
      */
      float CalculateRoofHeight();

      /**
      * Generates a roof with the given list of points.
      * This is a recursive function.
      *
      * @param[in]  roofPoints   The entire list of roof points.
      * @param[in]  roofPortion  The portion of the roof that we are generating for.
      * @param[in]  masterIndex  The current master index.
      * @param[in]  stackCount   Used as a failsafe to count the number of times we have recursed.
      *
      * @return     Returns a list of points that ended up not being used.
      */
      RoofReturnData GenerateRoof(std::vector<osg::Vec3> roofPoints, int masterIndex = 0, int stackCount = 0);

      /**
      * Takes the current roof points, makes sure there are no
      * overhangs, and then draws the roof.
      *
      * @param[in]  roofPoints  The points of the roof to draw.
      * @param[in]  stackCount  Used as a failsafe to count the number of times we have recursed.
      */
      void DrawRoof(std::vector<osg::Vec3> roofPoints, int stackCount = 0);

      /**
      * Draws a roof with the given points.
      * The points must be in clockwise or counter clockwise order
      * and must be convex.
      *
      * @param[in]  roofPoints  The points of the roof to draw.
      */
      void DrawRoofPortion(std::vector<osg::Vec3> roofPoints);

      /**
      * Does a line-to-line collision test between two lines.
      * All Z values are ignored.
      *
      * @param[in]  a1  The start of the first line.
      * @param[in]  a2  The end of the first line.
      * @param[in]  b1  The start of the second line.
      * @param[in]  b2  The end of the second line.
      *
      * @return     True if the lines intersect.
      */
      bool IntersectionTest(osg::Vec3 a1, osg::Vec3 a2, osg::Vec3 b1, osg::Vec3 b2);

      float       mRoofTextureScale;
      float       mWallTextureScale;
      float       mBuildingHeight;

      // Roof.
      bool                             mGenerateRoof;
      std::vector<osg::Vec3>           mRoofPoints;

      osg::ref_ptr<osg::Geometry>      mRoofGeom;
      osg::ref_ptr<osg::Geode>         mRoofGeode;

      osg::ref_ptr<osg::Vec3Array>     mRoofVertexList;
      osg::ref_ptr<osg::Vec2Array>     mRoofTextureList;
      osg::ref_ptr<osg::Vec4Array>     mRoofColorList;
      osg::ref_ptr<osg::Vec3Array>     mRoofNormalList;

      osg::ref_ptr<osg::Texture2D>     mRoofTexture;
      osg::ref_ptr<osg::Texture2D>     mWallTexture;

      dtCore::RefPtr<dtCore::Transformable> mOrigin;
   };

   /////////////////////////////////////////////////////////////////////////////
   // PROXY CODE
   /////////////////////////////////////////////////////////////////////////////
   class DT_PLUGIN_EXPORT BuildingActorProxy : public dtActors::LinkedPointsActorProxy
   {
   public:
      typedef dtActors::LinkedPointsActorProxy BaseClass;

      /**
      * Constructor.
      */
      BuildingActorProxy();

      /**
      * Creates the default actor associated with this proxy.
      */
      void CreateActor();

      /**
      * Builds the property map.
      */
      void BuildPropertyMap();

      /**
      * Called when the SetRotation function is called.  The rotation will
      * be passed in using x,y,z order (p,r,h)
      * @param oldValue The previous value
      * @param newValue The new value
      */
      void OnRotation(const osg::Vec3 &oldValue, const osg::Vec3 &newValue);

   protected:
      virtual ~BuildingActorProxy();

   private:
   };
}
#endif // BuildingActor_h__

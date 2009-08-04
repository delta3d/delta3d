#ifndef linkedpointarrayactorproxy_h__
#define linkedpointarrayactorproxy_h__

#include <dtGame/gameactor.h>
#include <dtDAL/plugin_export.h>
#include <dtDAL/transformableactorproxy.h>
#include <dtABC/export.h>
#include <dtDAL/actorproperty.h>
#include <osgText/Text>
#include <dtCore/transformable.h>
#include <dtDAL/gameevent.h>
#include <osg/Geode>
#include <osg/ShapeDrawable>

namespace dtActors
{
   class LinkedPointsActorProxy;
   class LinkedPointsGeomNodeBase;

   ////////////////////////////////////////////////////////////////////////////////
   // LINKED POINTS GEOM DATA BASE CLASS
   ////////////////////////////////////////////////////////////////////////////////
   /**
   * This is the base class used for all geometry nodes inside a point.
   * This should be overloaded by specific geometry classes.
   */
   class LinkedPointsGeomDataBase : public osg::Referenced
   {
   public:
      LinkedPointsGeomDataBase(LinkedPointsGeomNodeBase* parent): mParent(parent) {}
      virtual ~LinkedPointsGeomDataBase() {}

      /**
      * Initializes the Geom Data.
      */
      virtual bool Initialize() = 0;

      /**
      * Shuts down the Geom Data.
      */
      virtual bool Shutdown() = 0;

      LinkedPointsGeomNodeBase* mParent;
   };

   ////////////////////////////////////////////////////////////////////////////////
   // LINKED POINTS GEOM DATA
   ////////////////////////////////////////////////////////////////////////////////
   /**
   * This is an instantiation of the Geomentry Data Base class
   * used for the Linked Points Actor class, it stores
   * a sphere for the point and a cylinder to represent the
   * connection between points.
   */
   class LinkedPointsGeomData : public LinkedPointsGeomDataBase
   {
   public:
      LinkedPointsGeomData(LinkedPointsGeomNodeBase* parent): 
         LinkedPointsGeomDataBase(parent) {}

      /**
      * Initializes the Geom Data.
      */
      bool Initialize();

      /**
      * Shuts down the Geom Data.
      */
      bool Shutdown();

      // Point
      dtCore::RefPtr<osg::Geode>             mPointGeode;
      osg::ref_ptr<osg::ShapeDrawable>       mPointDrawable;
      osg::ref_ptr<osg::Sphere>              mPointSphere;

      // Segment
      dtCore::RefPtr<dtCore::Transformable>  mSegTransformable;
      osg::ref_ptr<osg::Geode>               mSegGeode;
      osg::ref_ptr<osg::ShapeDrawable>       mSegDrawable;
      osg::ref_ptr<osg::Cylinder>            mSegCylinder;
   };

   ////////////////////////////////////////////////////////////////////////////////
   // LINKED POINTS GEOM NODE BASE
   ////////////////////////////////////////////////////////////////////////////////
   /**
   * This is the base class for the primary Geom Node Transformable
   * used as the drawable of a point.  There is only one of these
   * for each point.  It contains a list of LinkedPointsGeomDataBase
   * to represent the geometry of the point.
   * This should be overloaded by specific geometry classes.
   */
   class LinkedPointsGeomNodeBase : public dtCore::Transformable
   {
   public:

      /**
      * Default Constructor.
      */
      LinkedPointsGeomNodeBase() {}

      /**
      * Default Destructor.
      */
      virtual ~LinkedPointsGeomNodeBase() {}

      /**
      * Sets the current segment/post index.
      * This will ensure the arrays are large enough
      * for the given index but will not shrink the arrays
      * for a smaller index.
      *
      * @param[in]  index  The index to set.
      *
      * @return     Success or failure.
      */
      bool SetIndex(int index);

      /**
      * Creates a new geom data.  This should be overloaded
      * to create the Geom Data of the proper inheritance.
      */
      virtual LinkedPointsGeomDataBase* CreateGeom() = 0;

      /**
      * This will set the current size of the segment/post arrays.
      * Should be used when you know exactly how many posts and
      * segments there will be.
      *
      * @param[in]  size  The new size.
      *
      * @return     Success or failure.
      */
      bool SetSize(int size);

      std::vector<dtCore::RefPtr<LinkedPointsGeomDataBase> > mGeomList;
   };


   ////////////////////////////////////////////////////////////////////////////////
   // LINKED POINTS GEOM NODE
   ////////////////////////////////////////////////////////////////////////////////
   /**
   * This is an instantiation of the primary Geom Node Transformable
   * used for the Linked Points Actor.  It handles the management
   * of the LinkedPointsGeomDataBase class used to represent the
   * geometry of a point.
   */
   class LinkedPointsGeomNode : public LinkedPointsGeomNodeBase
   {
   public:

      /**
      * Default Constructor.
      */
      LinkedPointsGeomNode();

      /**
      * Default Destructor.
      */
      ~LinkedPointsGeomNode();

      /**
      * Creates a new geom data.  This should be overloaded
      * to create the Geom Data of the proper inheritance.
      */
      LinkedPointsGeomDataBase* CreateGeom();
   };

   /////////////////////////////////////////////////////////////////////////////
   // ACTOR CODE
   /////////////////////////////////////////////////////////////////////////////
   class DT_PLUGIN_EXPORT LinkedPointsActor : public dtCore::Transformable
   {
   public:
      typedef dtCore::Transformable BaseClass;

      LinkedPointsActor(dtActors::LinkedPointsActorProxy* proxy, const std::string& name = "LinkedPointsActor");

      /**
      * Initializes the actor.
      */
      virtual bool Initialize();

      /**
      * This is used during the edit process to allow specialized
      * visualization effects that should only be seen during edit in
      * STAGE.
      */
      void SetVisualize(bool enabled) { mVisualize = enabled; }

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
      virtual void AddPoint(osg::Vec3 location, int index = -1);

      /**
      * Inserts a new point on a segment that is closest to
      * the location specified.
      *
      * @param[in]  location  The location to insert near.
      *
      * @return     Returns the index of the new point created, or -1 if
      *             the point could not be created.
      */
      virtual int AddPointOnSegment(osg::Vec3 location);

      /**
      * Removes a point from the array.
      *
      * @param[in]  index  The index of the point to remove.
      */
      virtual void RemovePoint(int index);

      /**
      * Changes the position or rotation of a point at the given index.
      *
      * @param[in]  index     The index of the point to edit.
      * @param[in]  location  The new location of the point.
      * @param[in]  rotation  The new rotation of the point.
      */
      virtual void SetPointPosition(int index, osg::Vec3 location);
      virtual void SetPointRotation(int index, osg::Vec3 rotation);

      /**
      * Retrieves the position or rotation of a point.
      *
      * @param[in]  index  The index of the point.
      *
      * @return            The position or rotation of the point.
      */
      osg::Vec3 GetPointPosition(int index);
      osg::Vec3 GetPointRotation(int index);

      /**
      * Retrieves the transformable of a point.
      */
      dtCore::Transformable* GetPointDrawable(int index);

      /**
      * Retrieves the index of a point given a transformable.
      *
      * @param[in]  drawable  The drawable to find.
      *
      * @return     Returns the index of the found transformable or -1 if it doesn't exist.
      */
      virtual int GetPointIndex(dtCore::DeltaDrawable* drawable);
      virtual int GetPointIndex(dtCore::DeltaDrawable* drawable, osg::Vec3 pickPos);

      /**
      * Retrieves the total number of points in the array.
      *
      * @return  The number of points.
      */
      int GetPointCount(void) { return (int)mPointList.size(); }

      /**
      * Retrieves the list of points.
      */
      const std::vector<dtCore::RefPtr<dtCore::Transformable> >& GetPointList(void) { return mPointList; }

      /**
      * Sets the list of points.
      */
      void SetPointList(const std::vector<dtCore::RefPtr<dtCore::Transformable> >& pointList);

      /**
      * Creates the drawable representation of a new link point.
      *
      * @param[in]  position  The position of the drawable.
      *
      * @return               The new drawable.
      */
      virtual dtCore::Transformable* CreatePointDrawable(osg::Vec3 position);

      /**
      * Re-creates the drawable node of a point.
      *
      * @param[in]  pointIndex  The point to re-create.
      */
      void RegeneratePointDrawable(int pointIndex);

      /**
      * This will find the nearest point on a line from the given test point.
      *
      * @param[in]  point1     The first point of the line.
      * @param[in]  point2     The second point of the line.
      * @param[in]  testPoint  The point to test with.
      *
      * @return     The nearest position.
      */
      osg::Vec3 FindNearestPointOnLine(osg::Vec3 point1, osg::Vec3 point2, osg::Vec3 testPoint);

   protected:
      virtual ~LinkedPointsActor();

      dtActors::LinkedPointsActorProxy* mProxy;

   private:

      bool mVisualize;
      std::vector<dtCore::RefPtr<dtCore::Transformable> >  mPointList;
   };

   /////////////////////////////////////////////////////////////////////////////
   // PROXY CODE
   /////////////////////////////////////////////////////////////////////////////
   class DT_PLUGIN_EXPORT LinkedPointsActorProxy : public dtDAL::TransformableActorProxy
   {
   public:
      typedef dtDAL::TransformableActorProxy BaseClass;

      static const dtUtil::RefString CLASS_NAME;

      /**
      * Constructor.
      */
      LinkedPointsActorProxy();

      /**
      * Creates the default actor associated with this proxy.
      */
      void CreateActor();

      /**
      * Builds the property map.
      */
      void BuildPropertyMap();
      
      /**
      * Retrieves whether we are in the middle of a map load.
      */
      bool IsLoading() {return mLoading;}

      /**
      * Called when this ActorProxy has been created during a map load
      * before any properties have been assigned.  Overwrite for custom
      * behavior.
      */
      void OnMapLoadBegin();

      /**
      *  Called when this ActorProxy has finished loading from a Map and
      *  all Property values have been assigned.  Overwrite for custom
      *  behavior.
      */
      void OnMapLoadEnd();

      /**
      * Functor to set the current point index.
      *
      * @param[in]  index  The index to set.
      */
      void SetPointIndex(int index);

      /**
      * Functor to retrieve the default value for a new point.
      */
      dtCore::RefPtr<dtCore::Transformable> GetDefaultPoint(void);

      /**
      * Functor to retrieve the current point array.
      */
      std::vector<dtCore::RefPtr<dtCore::Transformable> > GetPointArray(void);

      /**
      * Functor to set the current point array.
      *
      * @param[in]  value  The new array.
      */
      void SetPointArray(const std::vector<dtCore::RefPtr<dtCore::Transformable> >& value);

      /**
      * Functor to set the position of the current point.
      *
      * @param[in]  value  The new value.
      */
      void SetPointPosition(const osg::Vec3& value);

      /**
      * Functor to retrieve the position of the current point.
      */
      osg::Vec3 GetPointPosition(void);

      /**
      * Functor to set the rotation of the current point.
      *
      * @param[in]  value  The new value.
      */
      void SetPointRotation(const osg::Vec3& value);

      /**
      * Functor to retrieve the rotation of the current point.
      */
      osg::Vec3 GetPointRotation(void);

   protected:
      virtual ~LinkedPointsActorProxy();

   private:

      int      mPointIndex;

      bool     mLoading;
   };
}
#endif // linkedpointarrayactorproxy_h__

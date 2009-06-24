#ifndef linkedpointarrayactorproxy_h__
#define linkedpointarrayactorproxy_h__

#include <dtDAL/plugin_export.h>
#include <dtDAL/transformableactorproxy.h>
#include <dtABC/export.h>
#include <dtDAL/actorproperty.h>
#include <osgText/Text>
#include <dtCore/transformable.h>

namespace dtActors
{
   /////////////////////////////////////////////////////////////////////////////
   // ACTOR CODE
   /////////////////////////////////////////////////////////////////////////////
   class DT_PLUGIN_EXPORT LinkedPointsActor : public dtCore::Transformable
   {
   public:
      typedef dtCore::Transformable BaseClass;

      LinkedPointsActor(const std::string& name = "LinkedPointsActor");

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
      void AddPoint(osg::Vec3 location, int index = -1);

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
      * @param[in]  transformable  The transformable to find.
      *
      * @return     Returns the index of the found transformable or -1 if it doesn't exist.
      */
      int GetPointIndex(dtCore::Transformable* transformable);

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

   protected:
      virtual ~LinkedPointsActor();

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
   };
}
#endif // linkedpointarrayactorproxy_h__

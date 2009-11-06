#ifndef FenceActor_h__
#define FenceActor_h__

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
   class FencePostGeomData : public LinkedPointsGeomDataBase
   {
   public:
      FencePostGeomData(LinkedPointsGeomNodeBase* parent): 
         LinkedPointsGeomDataBase(parent) {}

      /**
      * Initializes the Geom Data.
      */
      bool Initialize();

      /**
      * Shuts down the Geom Data.
      */
      bool Shutdown();

      /**
      * Sets the validity of this segment.
      *
      * @param[in]  enabled  The new status.
      */
      void SetSegmentEnabled(bool enabled);

      // Post
      dtCore::RefPtr<dtCore::Object>         mPost;

      // Segment
      bool                                   mSegEnabled;
      dtCore::RefPtr<dtCore::Transformable>  mSegOrigin;
      osg::ref_ptr<osg::Geometry>            mSegGeom;
      osg::ref_ptr<osg::Geode>               mSegGeode;

      osg::ref_ptr<osg::Vec3Array>           mSegVertexList;
      osg::ref_ptr<osg::Vec2Array>           mSegTextureList;
      osg::ref_ptr<osg::Vec3Array>           mSegNormalList;

      osg::ref_ptr<osg::Texture2D>           mSegTexture;
   };

   ////////////////////////////////////////////////////////////////////////////////
   // FENCE POST GEOM NODE
   ////////////////////////////////////////////////////////////////////////////////
   class FencePostGeomNode : public LinkedPointsGeomNodeBase
   {
   public:

      /**
      * Default Constructor.
      */
      FencePostGeomNode();

      /**
      * Default Destructor.
      */
      ~FencePostGeomNode();

      /**
      * Creates a new geom data.  This should be overloaded
      * to create the Geom Data of the proper inheritance.
      */
      LinkedPointsGeomDataBase* CreateGeom();

      osg::ref_ptr<osg::Vec4Array>          mSegColorList;
   };

   /////////////////////////////////////////////////////////////////////////////
   // ACTOR CODE
   /////////////////////////////////////////////////////////////////////////////
   class DT_PLUGIN_EXPORT FenceActor : public dtActors::LinkedPointsActor
   {
   public:
      typedef dtActors::LinkedPointsActor BaseClass;

      enum DrawableType
      {
         DRAWABLE_TYPE_POST,
         DRAWABLE_TYPE_SEGMENT,
         DRAWABLE_TYPE_NONE,
      };

      struct ResourceIDData
      {
         ResourceIDData()
         {
            pointIndex = 0;
            segmentIndex = 0;

            postID = 0;
            segmentID = 0;
         }

         int      pointIndex;
         int      segmentIndex;

         int      postID;
         int      segmentID;
      };

      FenceActor(dtActors::LinkedPointsActorProxy* proxy, const std::string& name = "FenceActor");

      /**
      * This will visualize the current actor.
      */
      virtual void Visualize(void);

      /**
      * Retrieves the index of a point given a transformable.
      *
      * @param[in]  drawable  The drawable to find.
      *
      * @return     Returns the index of the found transformable or -1 if it doesn't exist.
      */
      int GetPointIndex(dtCore::DeltaDrawable* drawable);
      int GetPointIndex(dtCore::DeltaDrawable* drawable, osg::Vec3 pickPos);

      /**
      * Returns whether the drawable is a segment or post.
      *
      * @param[in]   drawable    The drawable.
      * @param[in]   position    The pick position.
      * @param[out]  pointIndex  The index of the drawable found.
      * @param[out]  subIndex    The sub index of the drawable found.
      *
      * @return     The type of the drawable.
      */
      DrawableType GetDrawableType(dtCore::DeltaDrawable* drawable, osg::Vec3 position, int& pointIndex, int& subIndex);

      /**
      * This will update the visuals on a single point.
      *
      * @param[in]  pointIndex  The point to update.
      */
      virtual void Visualize(int pointIndex);

      /**
      * Retrieves the list of fence post resources.
      */
      std::vector<std::string> GetPostResourceArray() const;

      /**
      * Functor to set the current post resource array.
      *
      * @param[in]  value  The new array.
      */
      void SetPostResourceArray(const std::vector<std::string>& value);

      /**
      * Retrieves the list of fence post resources.
      */
      std::vector<std::string> GetSegmentResourceArray() const;

      /**
      * Functor to set the current post resource array.
      *
      * @param[in]  value  The new array.
      */
      void SetSegmentResourceArray(const std::vector<std::string>& value);

      /**
      * Retrieves the list of resource IDs.
      */
      std::vector<FenceActor::ResourceIDData> GetResourceIDArray();

      /**
      * Sets the list of fence post mesh indexes.
      */
      void SetResourceIDArray(const std::vector<FenceActor::ResourceIDData>& value);

      /**
      * Retrieves the post mesh resource of a particular post.
      *
      * @param[in]  pointIndex  The index of the point.
      * @param[in]  subIndex    The index of the sub post between two points.
      *
      * @return     The name of the post mesh resource.  Empty if no meshes are available.
      */
      std::string GetPostMesh(int pointIndex, int subIndex);

      /**
      * Sets the post mesh resource for a particular post.
      *
      * @param[in]  pointIndex  The index of the point.
      * @param[in]  subIndex    The index of the sub post between two points.
      * @param[in]  meshIndex   The index of the mesh.
      */
      void SetPostMesh(int pointIndex, int subIndex, int meshIndex);

      /**
      * Increments the post mesh index.
      *
      * @param[in]  pointIndex  The index of the point.
      * @param[in]  subIndex    The index of the sub post between two points.
      */
      void IncrementPostMesh(int pointIndex, int subIndex);

      /**
      * Gets the minimum distance between posts.
      */
      float GetPostMinDistance() { return mPostMinDistance; }

      /**
      * Sets the minimum distance between posts.
      */
      void SetPostMinDistance(float value);

      /**
      * Gets the maximum distance between posts.
      */
      float GetPostMaxDistance() { return mPostMaxDistance; }

      /**
      * Sets the maximum distance between posts.
      */
      void SetPostMaxDistance(float value);

      /**
      * Gets the post scale.
      */
      float GetFenceScale() { return mFenceScale; }

      /**
      * Sets the post scale.
      */
      void SetFenceScale(float value);

      /**
      * Retrieves the segment texture resource for a particular segment.
      *
      * @param[in]  pointIndex  The index of the point.
      * @param[in]  subIndex    The index of the sub post between two points.
      *
      * @return     The name of the segment texture resource.  Empty if no textures are available.
      */
      std::string GetSegmentTexture(int pointIndex, int subIndex);

      /**
      * Sets the segment texture resource for a particular segment.
      *
      * @param[in]  pointIndex    The index of the point.
      * @param[in]  subIndex      The index of the sub post between two points.
      * @param[in]  textureIndex  The index of the texture.
      */
      void SetSegmentTexture(int pointIndex, int subIndex, int textureIndex);

      /**
      * Increments the post mesh index.
      *
      * @param[in]  pointIndex  The index of the point.
      * @param[in]  subIndex    The index of the sub post between two points.
      */
      void IncrementSegmentMesh(int pointIndex, int subIndex);

      /**
      * Gets the height of the segments.
      */
      float GetSegmentHeight() { return mSegmentHeight; }

      /**
      * Sets the height of the segments.
      */
      void SetSegmentHeight(float value);

      /**
      * Gets the width of the segments.
      */
      float GetSegmentWidth() { return mSegmentWidth; }

      /**
      * Sets the width of the segments.
      */
      void SetSegmentWidth(float value);

      /**
      * Gets the top texture ratio.
      */
      float GetTopTextureRatio() { return mTopTextureRatio; }

      /**
      * Sets the top texture ratio.
      */
      void SetTopTextureRatio(float value);


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
      virtual ~FenceActor();

   private:

      /**
      * Places a post.
      *
      * @param[in]  pointIndex  The parent point of the post.
      * @param[in]  subIndex    The post index.
      * @param[in]  position    The position to place the post.
      *
      * @return     The transform of the newly placed post.
      */
      dtCore::Transform PlacePost(int pointIndex, int subIndex, osg::Vec3 position);

      /**
      * Places a segment.
      *
      * @param[in]  pointIndex  The parent point of the segment.
      * @param[in]  subIndex    The segment index.
      * @param[in]  start       The start post transform.
      * @param[in]  end         The end post transform.
      * @param[in]  length      The percentage of the texture to show.
      */
      void PlaceSegment(int pointIndex, int subIndex, dtCore::Transform start, dtCore::Transform end, float length);

      std::vector<std::string>                  mPostResourceList;
      std::vector<std::string>                  mSegmentResourceList;
      std::vector<FenceActor::ResourceIDData>   mResourceIDList;

      float mPostMinDistance;
      float mPostMaxDistance;
      float mFenceScale;

      float mSegmentHeight;
      float mSegmentWidth;

      float mTopTextureRatio;
   };

   /////////////////////////////////////////////////////////////////////////////
   // PROXY CODE
   /////////////////////////////////////////////////////////////////////////////
   class DT_PLUGIN_EXPORT FenceActorProxy : public dtActors::LinkedPointsActorProxy
   {
   public:
      typedef dtActors::LinkedPointsActorProxy BaseClass;

      /**
      * Constructor.
      */
      FenceActorProxy();

      /**
      * Creates the default actor associated with this proxy.
      */
      void CreateActor();

      /**
      * Builds the property map.
      */
      void BuildPropertyMap();

      /**
      * Functor to set the current post index.
      *
      * @param[in]  index  The index to set.
      */
      void SetPostResourceIndex(int index);

      /**
      * Functor to set the current post index.
      *
      * @param[in]  index  The index to set.
      */
      void SetSegmentResourceIndex(int index);

      /**
      * Functor to retrieve the default value for a new post.
      */
      std::string GetDefaultPost(void);

      /**
      * Functor to retrieve the default value for a new post.
      */
      std::string GetDefaultSegment(void);

      /**
      * Gets a ResourceDescriptor of the requested property name.
      * @param name Name of the resource to retrieve.
      * @return A pointer to the resource descripter or NULL if it
      * is not found.
      */
      dtDAL::ResourceDescriptor* GetResource(const std::string& name);

      /**
      * Gets a ResourceDescriptor of the requested property name.
      * @param name Name of the resource to retrieve.
      * @return A pointer to the resource descripter or NULL if it
      * is not found.
      */
      const dtDAL::ResourceDescriptor* GetResource(const std::string& name) const;

      /**
      * Sets the mesh used as a fence post.
      */
      void SetPostMesh(const std::string& fileName);

      /**
      * Sets the texture used as a fence segment.
      */
      void SetSegmentTexture(const std::string& fileName);

      /**
      * Functor to set the current post ID index.
      *
      * @param[in]  index  The index to set.
      */
      void SetResourceIDIndex(int index);

      /**
      * Functor to retrieve the default value for a post ID list.
      */
      FenceActor::ResourceIDData GetDefaultResourceID();

      /**
      * Gets and Sets the point index.
      */
      int GetPointIndex();
      void SetPointIndex(int value);

      /**
      * Gets and Sets the segment index.
      */
      int GetSegmentIndex();
      void SetSegmentIndex(int value);

      /**
      * Gets and Sets the post IDs.
      */
      int GetPostID();
      void SetPostID(int value);

      /**
      * Gets and Sets the segment IDs.
      */
      int GetSegmentID();
      void SetSegmentID(int value);

   protected:
      virtual ~FenceActorProxy();

   private:

      int      mPostResourceIndex;
      int      mSegmentResourceIndex;

      int      mResourceIDIndex;
   };
}
#endif // FenceActor_h__

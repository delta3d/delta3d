#include <dtActors/linkedpointsactorproxy.h>

#include <dtCore/transform.h>
#include <dtCore/transformable.h>

#include <dtCore/arrayactorproperty.h>
#include <dtCore/containeractorproperty.h>
#include <dtCore/functor.h>
#include <dtCore/mapxml.h>
#include <dtCore/project.h>
#include <dtCore/vectoractorproperties.h>

#include <dtGame/gamemanager.h>
#include <dtGame/invokable.h>
#include <dtGame/messagetype.h>

#include <dtUtil/exception.h>

namespace dtActors
{
   bool LinkedPointsGeomData::Initialize()
   {
      // Point
      mPointGeode = new osg::Geode();
      if (!mPointGeode.valid()) return false;

      mPointSphere = new osg::Sphere(osg::Vec3(0.0f, 0.0f, 0.0f), 1.0f);
      if (!mPointSphere.valid()) return false;

      mPointDrawable = new osg::ShapeDrawable(mPointSphere.get());
      if (!mPointDrawable.valid()) return false;

      mPointDrawable->setColor(osg::Vec4(0.0f, 1.0f, 1.0f, 1.0f));

      mPointGeode->addDrawable(mPointDrawable.get());

      if (!mParent) return false;
      osg::Group* pointGroup = mParent->GetOSGNode()->asGroup();
      if (!pointGroup) return false;
      pointGroup->addChild(mPointGeode.get());

      // Segment
      mSegTransformable = new dtCore::Transformable();
      if (!mSegTransformable.valid()) return false;

      mSegGeode = new osg::Geode();
      if (!mSegGeode.valid()) return false;

      mSegCylinder = new osg::Cylinder(osg::Vec3(0.0f, 0.0f, 0.0f), 0.15f, 0.15f);
      if (!mSegCylinder.valid()) return false;

      mSegDrawable = new osg::ShapeDrawable(mSegCylinder.get());
      if (!mSegDrawable.valid()) return false;
      mSegDrawable->setColor(osg::Vec4(0.0f, 0.5f, 1.0f, 1.0f));

      pointGroup->addChild(mSegTransformable->GetOSGNode());
      mSegGeode->addDrawable(mSegDrawable.get());

      osg::Group* segGroup = mSegTransformable->GetOSGNode()->asGroup();
      if (!segGroup) return false;
      segGroup->addChild(mSegGeode.get());

      return true;
   }

   bool LinkedPointsGeomData::Shutdown()
   {
      if (!mParent) return false;
      osg::Group* pointGroup = mParent->GetOSGNode()->asGroup();
      if (!pointGroup) return false;
      pointGroup->removeChild(mPointGeode.get());
      pointGroup->removeChild(mSegTransformable->GetOSGNode());

      mPointGeode = NULL;
      mPointSphere = NULL;
      mPointDrawable = NULL;

      mSegTransformable = NULL;
      mSegGeode = NULL;
      mSegCylinder = NULL;
      mSegDrawable = NULL;

      return true;
   }


   ////////////////////////////////////////////////////////////////////////////////
   // LINKED POINTS GEOM NODE BASE
   ////////////////////////////////////////////////////////////////////////////////

   ////////////////////////////////////////////////////////////////////////////////
   bool LinkedPointsGeomNodeBase::SetIndex(int index)
   {
      if (index < 0) index = 0;

      // Expand the size.
      while (index >= (int)mGeomList.size())
      {
         LinkedPointsGeomDataBase* data = CreateGeom();
         if (!data) return false;
         if (!data->Initialize()) return false;
         mGeomList.push_back(data);
      }

      return true;
   }

   ////////////////////////////////////////////////////////////////////////////////
   bool LinkedPointsGeomNodeBase::SetSize(int size)
   {
      if (size < 0) { size = 0; }

      // Shrink the size.
      if (size <= int(mGeomList.size()))
      {
         while (int(mGeomList.size()) > size)
         {
            LinkedPointsGeomDataBase* data = mGeomList.back().get();
            if (!data) { return false; }
            data->Shutdown();
            mGeomList.pop_back();
         }
      }
      // Expand the size.
      else
      {
         return SetIndex(size - 1);
      }

      return true;
   }


   ////////////////////////////////////////////////////////////////////////////////
   // LINKED POINTS GEOM NODE
   ////////////////////////////////////////////////////////////////////////////////

   ////////////////////////////////////////////////////////////////////////////////
   LinkedPointsGeomNode::LinkedPointsGeomNode()
   {
   }

   ////////////////////////////////////////////////////////////////////////////////
   LinkedPointsGeomNode::~LinkedPointsGeomNode()
   {
      SetSize(0);
   }

   ////////////////////////////////////////////////////////////////////////////////
   LinkedPointsGeomDataBase* LinkedPointsGeomNode::CreateGeom()
   {
      return new LinkedPointsGeomData(this);
   }

   /////////////////////////////////////////////////////////////////////////////
   // ACTOR CODE
   /////////////////////////////////////////////////////////////////////////////

   /////////////////////////////////////////////////////////////////////////////
   LinkedPointsActor::LinkedPointsActor(dtActors::LinkedPointsActorProxy* proxy, const std::string& name)
      : BaseClass(name)
      , mProxy(proxy)
      , mVisualize(false)
   {
   }

   /////////////////////////////////////////////////////////////////////////////
   LinkedPointsActor::~LinkedPointsActor()
   {
   }

   ////////////////////////////////////////////////////////////////////////////////
   bool LinkedPointsActor::Initialize()
   {
      // Always initialize with a single Link Point.
      AddPoint(osg::Vec3());
      return true;
   }

   ////////////////////////////////////////////////////////////////////////////////
   void LinkedPointsActor::Visualize()
   {
      if (mProxy->IsLoading())
      {
         return;
      }

      // Iterate through each point.
      for (int pointIndex = 0; pointIndex < GetPointCount(); pointIndex++)
      {
         Visualize(pointIndex);
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void LinkedPointsActor::Visualize(int pointIndex)
   {
      // Don't visualize during a map load or if we're not in STAGE unless we're set to visualize.
      if (mProxy->IsLoading() || (!mProxy->IsInSTAGE() && !mVisualize))
      {
         return;
      }

      dtActors::LinkedPointsGeomNode* point =
         dynamic_cast<dtActors::LinkedPointsGeomNode*>(GetPointDrawable(pointIndex));
      // If the drawable is not of the proper class, regenerate it.
      if (!point)
      {
         RegeneratePointDrawable(pointIndex);

         point = dynamic_cast<dtActors::LinkedPointsGeomNode*>(GetPointDrawable(pointIndex));
      }

      if (point)
      {
         if (!point->SetSize(1)) return;

         dtActors::LinkedPointsGeomData* geomData =
            dynamic_cast<dtActors::LinkedPointsGeomData*>(point->mGeomList[0].get());
         if (!geomData) return;

         // Now create a cylinder to connect this point with the previous.
         if (pointIndex < GetPointCount() - 1)
         {
            int nextPoint = pointIndex + 1;

            // First calculate the cylinder size.
            osg::Vec3 start = GetPointPosition(pointIndex);
            osg::Vec3 end = GetPointPosition(nextPoint);

            osg::Vec3 dir = end - start;
            float height = dir.length();
            osg::Vec3 center = osg::Vec3(0.0f, 0.0f, height * 0.5f);
            dir.normalize();

            geomData->mSegCylinder->setCenter(center);
            geomData->mSegCylinder->setHeight(height);

            osg::Matrix matrix;
            matrix = matrix.rotate(osg::Vec3(0.0f, 0.0f, 1.0f), dir);
            dtCore::Transform transform;
            if (matrix.valid())
            {
               transform.SetRotation(matrix);
            }

            transform.SetTranslation(start);
            geomData->mSegTransformable->SetTransform(transform);
         }
         else
         {
            osg::Vec3 center = osg::Vec3(0.0f, 0.0f, 0.0f);
            geomData->mSegCylinder->setCenter(center);
            geomData->mSegCylinder->setHeight(0.15f);
         }

         geomData->mSegDrawable->dirtyDisplayList();
         geomData->mSegDrawable->dirtyBound();
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void LinkedPointsActor::AddPoint(osg::Vec3 location, int index)
   {
      dtCore::RefPtr<dtCore::Transformable> point = CreatePointDrawable(location);
      if (!point.valid())
      {
         return;
      }

      if (index <= -1 || index >= GetPointCount())
      {
         mPointList.push_back(point);

         // Update the new point and the previous one.
         Visualize(GetPointCount() - 2);
         Visualize(GetPointCount() - 1);
      }
      else
      {
         mPointList.insert(mPointList.begin() + index, point);

         // Update both the new point and the previous one.
         Visualize(index);
         Visualize(index - 1);
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   int LinkedPointsActor::AddPointOnSegment(osg::Vec3 location)
   {
      // Must have at least one segment (two connected points).
      if (GetPointCount() < 2)
      {
         return -1;
      }

      float nearestDistance = -1;
      int newIndex = -1;
      osg::Vec3 newPosition;

      for (int index = 0; index < GetPointCount() - 1; index++)
      {
         osg::Vec3 firstPoint = GetPointPosition(index);
         osg::Vec3 secondPoint = GetPointPosition(index + 1);

         osg::Vec3 pos = FindNearestPointOnLine(firstPoint, secondPoint, location);

         float distance = (pos - location).length2();
         if (nearestDistance == -1 || distance < nearestDistance)
         {
            nearestDistance = distance;
            newIndex = index + 1;
            newPosition = pos;
         }
      }

      if (newIndex > -1)
      {
         AddPoint(newPosition, newIndex);
      }

      return newIndex;
   }

   ////////////////////////////////////////////////////////////////////////////////
   void LinkedPointsActor::RemovePoint(int index)
   {
      // Can't remove a point that doesn't exist.
      if (index <= -1 || index >= GetPointCount())
      {
         return;
      }

      // First remove the drawable from the parent.
      dtCore::Transformable* point = mPointList[index].get();
      if (point)
      {
         RemoveChild(point);
      }

      mPointList.erase(mPointList.begin() + index);

      // Update the previous point that was connected to the removed point.
      Visualize(index - 1);
      Visualize(index);
      Visualize(index + 1);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void LinkedPointsActor::SetPointPosition(int index, osg::Vec3 location)
   {
      if (index <= -1 || index >= GetPointCount())
      {
         return;
      }

      dtCore::Transform transform;
      mPointList[index]->GetTransform(transform);
      transform.SetTranslation(location);
      mPointList[index]->SetTransform(transform);

      Visualize(index - 1);
      Visualize(index);
      Visualize(index + 1);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void LinkedPointsActor::SetPointRotation(int index, osg::Vec3 rotation)
   {
      if (index <= -1 || index >= GetPointCount())
      {
         return;
      }

      dtCore::Transform transform;
      mPointList[index]->GetTransform(transform);
      transform.SetRotation(rotation);
      mPointList[index]->SetTransform(transform);

      Visualize(index - 1);
      Visualize(index);
      Visualize(index + 1);
   }

   ////////////////////////////////////////////////////////////////////////////////
   osg::Vec3 LinkedPointsActor::GetPointPosition(int index)
   {
      if (index <= -1 || index >= GetPointCount())
      {
         return osg::Vec3();
      }

      dtCore::Transform transform;
      mPointList[index]->GetTransform(transform);
      return transform.GetTranslation();
   }

   ////////////////////////////////////////////////////////////////////////////////
   osg::Vec3 LinkedPointsActor::GetPointRotation(int index)
   {
      if (index <= -1 || index >= GetPointCount())
      {
         return osg::Vec3();
      }

      dtCore::Transform transform;
      mPointList[index]->GetTransform(transform);

      osg::Vec3 rotation;
      transform.GetRotation(rotation);
      return rotation;
   }

   ////////////////////////////////////////////////////////////////////////////////
   dtCore::Transformable* LinkedPointsActor::GetPointDrawable(int index)
   {
      if (index <= -1 || index >= GetPointCount())
      {
         return NULL;
      }

      return mPointList[index].get();
   }

   ////////////////////////////////////////////////////////////////////////////////
   int LinkedPointsActor::GetPointIndex(dtCore::DeltaDrawable* drawable)
   {
      int pointCount = GetPointCount();
      for (int pointIndex = 0; pointIndex < pointCount; pointIndex++)
      {
         // Make sure we check the transformable as well as all its parents.
         dtCore::DeltaDrawable* parent = drawable;
         while (parent)
         {
            if (mPointList[pointIndex].get() == parent)
            {
               return pointIndex;
            }

            parent = parent->GetParent();
         }
      }

      return -1;
   }

   ////////////////////////////////////////////////////////////////////////////////
   int LinkedPointsActor::GetPointIndex(dtCore::DeltaDrawable* drawable, osg::Vec3 pickPos)
   {
      return GetPointIndex(drawable);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void LinkedPointsActor::SetPointList(const std::vector< dtCore::RefPtr<dtCore::Transformable> >& pointList)
   {
      // First clear all children.
      int pointCount = GetPointCount();
      for (int pointIndex = 0; pointIndex < pointCount; pointIndex++)
      {
         RemoveChild(mPointList[pointIndex].get());
      }
      pointCount = (int)pointList.size();
      for (int pointIndex = 0; pointIndex < pointCount; pointIndex++)
      {
         RemoveChild(pointList[pointIndex].get());
      }

      mPointList.clear();

      // Now add all the new children.
      pointCount = (int)pointList.size();
      for (int pointIndex = 0; pointIndex < pointCount; pointIndex++)
      {
         AddChild(pointList[pointIndex].get());
      }

      mPointList = pointList;

      Visualize();
   }

   ////////////////////////////////////////////////////////////////////////////////
   dtCore::Transformable* LinkedPointsActor::CreatePointDrawable(osg::Vec3 position)
   {
      dtCore::Transformable* point = new dtActors::LinkedPointsGeomNode();

      // Attach this new point to the actor.
      AddChild(point);

      // Set the position of this point.
      dtCore::Transform transform;
      point->GetTransform(transform);
      transform.SetTranslation(position);
      point->SetTransform(transform);

      return point;
   }

   ////////////////////////////////////////////////////////////////////////////////
   void LinkedPointsActor::RegeneratePointDrawable(int pointIndex)
   {
      if (pointIndex < 0 || pointIndex >= (int)mPointList.size()) return;

      // Copy the transform of the current point.
      dtCore::Transform transform;
      mPointList[pointIndex]->GetTransform(transform);

      // Now re-create a new point drawable.
      dtCore::Transformable* point = CreatePointDrawable(transform.GetTranslation());

      // And set it to the same transform.
      point->SetTransform(transform);

      RemoveChild(mPointList[pointIndex].get());

      // Now make this the new point.
      mPointList[pointIndex] = point;
   }

   ////////////////////////////////////////////////////////////////////////////////
   osg::Vec3 LinkedPointsActor::FindNearestPointOnLine(osg::Vec3 point1, osg::Vec3 point2, osg::Vec3 testPoint)
   {
      osg::Vec3 normal = point2 - point1;
      float length = normal.length();
      if (length == 0)
      {
         return point1;
      }
      normal.normalize();

      float distance = (normal * (testPoint - point1));

      // Point falls beyond the first point.
      if (distance < 0)
      {
         return point1;
      }

      // Point falls beyond the second point.
      if (distance > length)
      {
         return point2;
      }

      return point1 + (normal * distance);
   }

   ///////////////////////////////////////////////////////////////////////////////
   void LinkedPointsActor::SetLineWidth(float width)
   {
      // Iterate through all of our points, finding the geometry, and setting the line width
      for (size_t pointIndex = 0; pointIndex < mPointList.size(); ++pointIndex)
      {
         dtActors::LinkedPointsGeomNode* point =
            dynamic_cast<dtActors::LinkedPointsGeomNode*>(GetPointDrawable(pointIndex));
         if (point != NULL)
         {
            for (size_t geomIndex = 0; geomIndex < point->mGeomList.size(); ++geomIndex)
            {
               dtActors::LinkedPointsGeomData* geomData =
                  dynamic_cast<dtActors::LinkedPointsGeomData*>(point->mGeomList[geomIndex].get());
               if (geomData != NULL)
               {
                  // The cylinder's radius determines the line width
                  geomData->mSegCylinder->setRadius(width);
               }
            }
         }
      }
   }

   ///////////////////////////////////////////////////////////////////////////////
   void LinkedPointsActor::SetPointRadius(float radius)
   {
      // Iterate through all of our points, finding the geometry, and setting the line width
      for (size_t pointIndex = 0; pointIndex < mPointList.size(); ++pointIndex)
      {
         dtActors::LinkedPointsGeomNode* point =
            dynamic_cast<dtActors::LinkedPointsGeomNode*>(GetPointDrawable(pointIndex));
         if (point != NULL)
         {
            for (size_t geomIndex = 0; geomIndex < point->mGeomList.size(); ++geomIndex)
            {
               dtActors::LinkedPointsGeomData* geomData =
                  dynamic_cast<dtActors::LinkedPointsGeomData*>(point->mGeomList[geomIndex].get());
               if (geomData != NULL)
               {
                  geomData->mPointSphere->setRadius(radius);
               }
            }
         }
      }
   }

   ///////////////////////////////////////////////////////////////////////////////
   void LinkedPointsActor::SetLineColor(const osg::Vec4& color)
   {
      // Iterate through all of our points, finding the geometry, and setting the line width
      for (size_t pointIndex = 0; pointIndex < mPointList.size(); ++pointIndex)
      {
         dtActors::LinkedPointsGeomNode* point =
            dynamic_cast<dtActors::LinkedPointsGeomNode*>(GetPointDrawable(pointIndex));
         if (point != NULL)
         {
            for (size_t geomIndex = 0; geomIndex < point->mGeomList.size(); ++geomIndex)
            {
               dtActors::LinkedPointsGeomData* geomData =
                  dynamic_cast<dtActors::LinkedPointsGeomData*>(point->mGeomList[geomIndex].get());
               if (geomData != NULL)
               {
                  geomData->mSegDrawable->setColor(color);
               }
            }
         }
      }
   }

   ///////////////////////////////////////////////////////////////////////////////
   void LinkedPointsActor::SetPointColor(const osg::Vec4& color)
   {
      // Iterate through all of our points, finding the geometry, and setting the line width
      for (size_t pointIndex = 0; pointIndex < mPointList.size(); ++pointIndex)
      {
         dtActors::LinkedPointsGeomNode* point =
            dynamic_cast<dtActors::LinkedPointsGeomNode*>(GetPointDrawable(pointIndex));
         if (point != NULL)
         {
            for (size_t geomIndex = 0; geomIndex < point->mGeomList.size(); ++geomIndex)
            {
               dtActors::LinkedPointsGeomData* geomData =
                  dynamic_cast<dtActors::LinkedPointsGeomData*>(point->mGeomList[geomIndex].get());
               if (geomData != NULL)
               {
                  geomData->mPointDrawable->setColor(color);
               }
            }
         }
      }
   }

   /////////////////////////////////////////////////////////////////////////////
   // PROXY CODE
   /////////////////////////////////////////////////////////////////////////////
   const dtUtil::RefString LinkedPointsActorProxy::CLASS_NAME("dtActors::LinkedPointsActor");

   /////////////////////////////////////////////////////////////////////////////
   LinkedPointsActorProxy::LinkedPointsActorProxy()
      : BaseClass()
      , mPointIndex(0)
      , mLoading(false)
   {
      SetClassName(CLASS_NAME.Get());
   }

   /////////////////////////////////////////////////////////////////////////////
   LinkedPointsActorProxy::~LinkedPointsActorProxy()
   {
   }

   /////////////////////////////////////////////////////////////////////////////
   void LinkedPointsActorProxy::CreateDrawable()
   {
      LinkedPointsActor* actor = new LinkedPointsActor(this);
      SetDrawable(*actor);
      actor->Initialize();
   }

   /////////////////////////////////////////////////////////////////////////////
   void LinkedPointsActorProxy::BuildPropertyMap()
   {
      BaseClass::BuildPropertyMap();

      LinkedPointsActor* actor = NULL;
      GetDrawable(actor);

      dtCore::ContainerActorProperty* pointProp = new dtCore::ContainerActorProperty(
         "Point", "Point", "The value of the point.", "Points");

      dtCore::Vec3ActorProperty* positionProp = new dtCore::Vec3ActorProperty(
         "Position", "Position",
         dtCore::Vec3ActorProperty::SetFuncType(this, &LinkedPointsActorProxy::SetPointPosition),
         dtCore::Vec3ActorProperty::GetFuncType(this, &LinkedPointsActorProxy::GetPointPosition),
         "The position of the point.", "Points");

      dtCore::Vec3ActorProperty* rotationProp = new dtCore::Vec3ActorProperty(
         "Rotation", "Rotation",
         dtCore::Vec3ActorProperty::SetFuncType(this, &LinkedPointsActorProxy::SetPointRotation),
         dtCore::Vec3ActorProperty::GetFuncType(this, &LinkedPointsActorProxy::GetPointRotation),
         "The rotation of the point.", "Points");

      pointProp->AddProperty(positionProp);
      pointProp->AddProperty(rotationProp);

      dtCore::ArrayActorPropertyBase* arrayProp = new dtCore::ArrayActorProperty< dtCore::RefPtr<dtCore::Transformable> >(
         "PointList", "Point List", "The list of points.",
         dtCore::ArrayActorProperty< dtCore::RefPtr<dtCore::Transformable> >::SetIndexFuncType(this, &LinkedPointsActorProxy::SetPointIndex),
         dtCore::ArrayActorProperty< dtCore::RefPtr<dtCore::Transformable> >::GetDefaultFuncType(this, &LinkedPointsActorProxy::GetDefaultPoint),
         dtCore::ArrayActorProperty< dtCore::RefPtr<dtCore::Transformable> >::GetArrayFuncType(this, &LinkedPointsActorProxy::GetPointArray),
         dtCore::ArrayActorProperty< dtCore::RefPtr<dtCore::Transformable> >::SetArrayFuncType(this, &LinkedPointsActorProxy::SetPointArray),
         pointProp, "Points");
      arrayProp->SetMinArraySize(1);

      AddProperty(arrayProp);
   }

   //////////////////////////////////////////////////////////////////////////
   void LinkedPointsActorProxy::OnMapLoadBegin()
   {
      mLoading = true;
   }

   //////////////////////////////////////////////////////////////////////////
   void LinkedPointsActorProxy::OnMapLoadEnd()
   {
      mLoading = false;

      // Once we have finished loading, we can visualize the actor.
      LinkedPointsActor* actor = NULL;
      GetDrawable(actor);

      return actor->Visualize();
   }

   ////////////////////////////////////////////////////////////////////////////////
   void LinkedPointsActorProxy::SetPointIndex(int index)
   {
      mPointIndex = index;
   }

   ////////////////////////////////////////////////////////////////////////////////
   dtCore::RefPtr<dtCore::Transformable> LinkedPointsActorProxy::GetDefaultPoint(void)
   {
      LinkedPointsActor* actor = NULL;
      GetDrawable(actor);

      return actor->CreatePointDrawable(osg::Vec3());
   }

   ////////////////////////////////////////////////////////////////////////////////
   std::vector<dtCore::RefPtr<dtCore::Transformable> > LinkedPointsActorProxy::GetPointArray(void)
   {
      LinkedPointsActor* actor = NULL;
      GetDrawable(actor);

      return actor->GetPointList();
   }

   ////////////////////////////////////////////////////////////////////////////////
   void LinkedPointsActorProxy::SetPointArray(const std::vector<dtCore::RefPtr<dtCore::Transformable> >& value)
   {
      LinkedPointsActor* actor = NULL;
      GetDrawable(actor);

      if (actor)
      {
         actor->SetPointList(value);
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void LinkedPointsActorProxy::SetPointPosition(const osg::Vec3& value)
   {
      LinkedPointsActor* actor = NULL;
      GetDrawable(actor);

      if (actor)
      {
         actor->SetPointPosition(mPointIndex, value);
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   osg::Vec3 LinkedPointsActorProxy::GetPointPosition(void)
   {
      LinkedPointsActor* actor = NULL;
      GetDrawable(actor);

      if (actor)
      {
         return actor->GetPointPosition(mPointIndex);
      }

      return osg::Vec3();
   }

   ////////////////////////////////////////////////////////////////////////////////
   void LinkedPointsActorProxy::SetPointRotation(const osg::Vec3& value)
   {
      LinkedPointsActor* actor = NULL;
      GetDrawable(actor);

      if (actor)
      {
         actor->SetPointRotation(mPointIndex, value);
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   osg::Vec3 LinkedPointsActorProxy::GetPointRotation(void)
   {
      LinkedPointsActor* actor = NULL;
      GetDrawable(actor);

      if (actor)
      {
         return actor->GetPointRotation(mPointIndex);
      }

      return osg::Vec3();
   }
}

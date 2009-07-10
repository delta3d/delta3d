#include <dtActors/linkedpointsactorproxy.h>
#include <dtCore/transformable.h>
#include <dtCore/transform.h>
#include <dtDAL/enginepropertytypes.h>
#include <dtDAL/arrayactorproperty.h>
#include <dtDAL/containeractorproperty.h>
#include <dtDAL/mapxml.h>
#include <dtDAL/project.h>
#include <dtDAL/functor.h>
#include <dtGame/invokable.h>
#include <dtGame/messagetype.h>
#include <dtGame/gamemanager.h>
#include <dtUtil/exception.h>
#include <osg/Geode>
#include <osg/ShapeDrawable>

namespace dtActors
{
   /////////////////////////////////////////////////////////////////////////////
   // ACTOR CODE
   /////////////////////////////////////////////////////////////////////////////

   /////////////////////////////////////////////////////////////////////////////
   LinkedPointsActor::LinkedPointsActor(dtDAL::ActorProxy* proxy, const std::string& name)
      : BaseClass(name)
      , mVisualize(false)
      , mProxy(proxy)
   {
      // Always initialize with a single Link Point.
      AddPoint(osg::Vec3());
   }

   /////////////////////////////////////////////////////////////////////////////
   LinkedPointsActor::~LinkedPointsActor()
   {
   }

   ////////////////////////////////////////////////////////////////////////////////
   bool LinkedPointsActor::IsMapLoaded()
   {
      //if (mProxy)
      //{
      //   dtGame::GameManager* gm = mProxy->GetGameManager();
      //   if (gm && !gm->GetCurrentMap().empty())
      //   {
      //      return true;
      //   }
      //}

      //return false;
      return true;
      //return !dtDAL::Project::GetInstance().IsMapBeingParsed();
   }

   ////////////////////////////////////////////////////////////////////////////////
   void LinkedPointsActor::Visualize()
   {
      if (!IsMapLoaded())
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
      if (!IsMapLoaded())
      {
         return;
      }

      dtCore::Transformable* point = GetPointDrawable(pointIndex);
      if (point)
      {
         // First clear the old visualization data from the point.
         while (point->GetNumChildren())
         {
            point->RemoveChild(point->GetChild(0));
         }

         osg::Group* pointGroup = point->GetOSGNode()->asGroup();
         pointGroup->removeChildren(0, pointGroup->getNumChildren());

         // Create a default sphere geode to represent this point.
         osg::Geode* geode = new osg::Geode();
         osg::Sphere* sphere = new osg::Sphere(osg::Vec3(0.0f, 0.0f, 0.0f), 1.0f);

         osg::ShapeDrawable* sphereDrawable = new osg::ShapeDrawable(sphere);
         sphereDrawable->setColor(osg::Vec4(0.0f, 1.0f, 1.0f, 1.0f));

         geode->addDrawable(sphereDrawable);
         pointGroup->addChild(geode);

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

            dtCore::Transformable* lineRot = new dtCore::Transformable();
            geode = new osg::Geode();
            osg::Cylinder* line = new osg::Cylinder(center, 0.15f, height);
            osg::ShapeDrawable* lineDrawable = new osg::ShapeDrawable(line);
            lineDrawable->setColor(osg::Vec4(0.0f, 0.5f, 1.0f, 1.0f));

            // Now add the line into the scene.
            point->AddChild(lineRot);
            geode->addDrawable(lineDrawable);

            osg::Group* rotGroup = lineRot->GetOSGNode()->asGroup();
            rotGroup->addChild(geode);

            osg::Matrix matrix;
            matrix = matrix.rotate(osg::Vec3(0.0f, 0.0f, 1.0f), dir);
            dtCore::Transform transform;
            transform.SetRotation(matrix);
            transform.SetTranslation(start);
            lineRot->SetTransform(transform);
         }
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
   void LinkedPointsActor::SetPointList(const std::vector<dtCore::RefPtr<dtCore::Transformable> >& pointList)
   {
      // First clear all children.
      int pointCount = GetPointCount();
      for (int pointIndex = 0; pointIndex < pointCount; pointIndex++)
      {
         RemoveChild(mPointList[pointIndex].get());
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
      dtCore::Transformable* point = new dtCore::Transformable();

      // Attach this new point to the actor.
      AddChild(point);

      // Set the position of this point.
      dtCore::Transform transform;
      point->GetTransform(transform);
      transform.SetTranslation(position);
      point->SetTransform(transform);

      return point;
   }

   /////////////////////////////////////////////////////////////////////////////
   // PROXY CODE
   /////////////////////////////////////////////////////////////////////////////
   const dtUtil::RefString LinkedPointsActorProxy::CLASS_NAME("dtActors::LinkedPointsActor");

   /////////////////////////////////////////////////////////////////////////////
   LinkedPointsActorProxy::LinkedPointsActorProxy()
      : BaseClass()
      , mPointIndex(0)
   {
      SetClassName(CLASS_NAME.Get());
   }

   /////////////////////////////////////////////////////////////////////////////
   LinkedPointsActorProxy::~LinkedPointsActorProxy()
   {
   }

   /////////////////////////////////////////////////////////////////////////////
   void LinkedPointsActorProxy::CreateActor()
   {
      SetActor(*new LinkedPointsActor(this));
   }

   /////////////////////////////////////////////////////////////////////////////
   void LinkedPointsActorProxy::BuildPropertyMap()
   {
      BaseClass::BuildPropertyMap();

      LinkedPointsActor* actor = NULL;
      GetActor(actor);

      dtDAL::ContainerActorProperty* pointProp = new dtDAL::ContainerActorProperty(
         "Point", "Point", "The value of the point.", "Points");

      dtDAL::Vec3ActorProperty* positionProp = new dtDAL::Vec3ActorProperty(
         "Position", "Position",
         dtDAL::MakeFunctor(*this, &LinkedPointsActorProxy::SetPointPosition),
         dtDAL::MakeFunctorRet(*this, &LinkedPointsActorProxy::GetPointPosition),
         "The position of the point.", "Points");

      dtDAL::Vec3ActorProperty* rotationProp = new dtDAL::Vec3ActorProperty(
         "Rotation", "Rotation",
         dtDAL::MakeFunctor(*this, &LinkedPointsActorProxy::SetPointRotation),
         dtDAL::MakeFunctorRet(*this, &LinkedPointsActorProxy::GetPointRotation),
         "The rotation of the point.", "Points");

      pointProp->AddProperty(positionProp);
      pointProp->AddProperty(rotationProp);

      dtDAL::ArrayActorPropertyBase* arrayProp = new dtDAL::ArrayActorProperty<dtCore::RefPtr<dtCore::Transformable> >(
         "PointList", "Point List", "The list of points.",
         dtDAL::MakeFunctor(*this, &LinkedPointsActorProxy::SetPointIndex),
         dtDAL::MakeFunctorRet(*this, &LinkedPointsActorProxy::GetDefaultPoint),
         dtDAL::MakeFunctorRet(*this, &LinkedPointsActorProxy::GetPointArray),
         dtDAL::MakeFunctor(*this, &LinkedPointsActorProxy::SetPointArray),
         pointProp, "Points");
      arrayProp->SetMinArraySize(1);

      AddProperty(arrayProp);
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
      GetActor(actor);

      return actor->CreatePointDrawable(osg::Vec3());
   }

   ////////////////////////////////////////////////////////////////////////////////
   std::vector<dtCore::RefPtr<dtCore::Transformable> > LinkedPointsActorProxy::GetPointArray(void)
   {
      LinkedPointsActor* actor = NULL;
      GetActor(actor);

      return actor->GetPointList();
   }

   ////////////////////////////////////////////////////////////////////////////////
   void LinkedPointsActorProxy::SetPointArray(const std::vector<dtCore::RefPtr<dtCore::Transformable> >& value)
   {
      LinkedPointsActor* actor = NULL;
      GetActor(actor);

      if (actor)
      {
         actor->SetPointList(value);
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void LinkedPointsActorProxy::SetPointPosition(const osg::Vec3& value)
   {
      LinkedPointsActor* actor = NULL;
      GetActor(actor);

      if (actor)
      {
         actor->SetPointPosition(mPointIndex, value);
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   osg::Vec3 LinkedPointsActorProxy::GetPointPosition(void)
   {
      LinkedPointsActor* actor = NULL;
      GetActor(actor);

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
      GetActor(actor);

      if (actor)
      {
         actor->SetPointRotation(mPointIndex, value);
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   osg::Vec3 LinkedPointsActorProxy::GetPointRotation(void)
   {
      LinkedPointsActor* actor = NULL;
      GetActor(actor);

      if (actor)
      {
         return actor->GetPointRotation(mPointIndex);
      }

      return osg::Vec3();
   }
}

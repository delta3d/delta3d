/*
* Delta3D Open Source Game and Simulation Engine
* Copyright (C) 2005, BMH Associates, Inc.
*
* This library is free software; you can redistribute it and/or modify it under
* the terms of the GNU Lesser General Public License as published by the Free
* Software Foundation; either version 2.1 of the License, or (at your option)
* any later version.
*
* This library is distributed in the hope that it will be useful, but WITHOUT
* ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
* FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more
* details.
*
* You should have received a copy of the GNU Lesser General Public License
* along with this library; if not, write to the Free Software Foundation, Inc.,
* 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
*
* William E. Johnson II
*/

#include <dtActors/deltaobjectactorproxy.h>
#include <dtCore/object.h>
#include <dtDAL/enginepropertytypes.h>
#include <dtUtil/matrixutil.h>

namespace dtActors 
{
   ///////////////////////////////////////////////////////////////////////////////
   void DeltaObjectActorProxy::CreateActor()
   {
     SetActor(*new dtCore::Object);
     //std::cout << "Creating actor proxy." << std::endl;
   }

   ///////////////////////////////////////////////////////////////////////////////
   void DeltaObjectActorProxy::SetRotation(const osg::Vec3& v3)
   {
      dtCore::Object* ourObject = dynamic_cast<dtCore::Object*>(GetActor());
      if(ourObject)
      {
         dtCore::Transform ourTransform;
         ourObject->GetTransform(ourTransform);
         ourTransform.SetRotation(v3);

         ourObject->SetTransform(ourTransform);
      }
   }

   ///////////////////////////////////////////////////////////////////////////////
   osg::Vec3 DeltaObjectActorProxy::GetRotation()
   {
      osg::Vec3 v3;
      const dtCore::Object* ourObject = dynamic_cast<const dtCore::Object*>(GetActor());
      if(ourObject)
      {
         dtCore::Transform ourTransform;
         ourObject->GetTransform(ourTransform);
         ourTransform.GetRotation(v3);
      }

      return v3;
   }

   ///////////////////////////////////////////////////////////////////////////////
   void DeltaObjectActorProxy::SetTranslation(const osg::Vec3& v3)
   {
      dtCore::Object* ourObject = dynamic_cast<dtCore::Object*>(GetActor());
      if(ourObject)
      {
         dtCore::Transform ourTransform;
         ourObject->GetTransform(ourTransform);
         ourTransform.SetTranslation(v3);

         ourObject->SetTransform(ourTransform);
      }
   }

   ///////////////////////////////////////////////////////////////////////////////
   osg::Vec3 DeltaObjectActorProxy::GetTranslation()
   {
      osg::Vec3 v3;
      const dtCore::Object* ourObject = dynamic_cast<const dtCore::Object*>(GetActor());
      if(ourObject)
      {
         dtCore::Transform ourTransform;
         ourObject->GetTransform(ourTransform);
         ourTransform.GetTranslation(v3);
      }
      return v3;
   }

   ///////////////////////////////////////////////////////////////////////////////
   void DeltaObjectActorProxy::BuildPropertyMap()
   {
      PhysicalActorProxy::BuildPropertyMap();

      dtCore::Object *actor = NULL;
      GetActor(actor);

      AddProperty(new dtDAL::Vec3ActorProperty("Scale", "Scale", 
        dtDAL::MakeFunctor(*actor, &dtCore::Object::SetScale), 
        dtDAL::MakeFunctorRet(*actor, &dtCore::Object::GetScale), 
        "Specifies the scale of the object", 
        "Transformable"));

      AddProperty(new dtDAL::Vec3ActorProperty("Rotation", "Rotation", 
        dtDAL::MakeFunctor(*this, &DeltaObjectActorProxy::SetRotation), 
        dtDAL::MakeFunctorRet(*this, &DeltaObjectActorProxy::GetRotation), 
        "Specifies the Rotation of the object", 
        "Transformable"));

      AddProperty(new dtDAL::Vec3ActorProperty("Translation", "Translation", 
        dtDAL::MakeFunctor(*this, &DeltaObjectActorProxy::SetTranslation), 
        dtDAL::MakeFunctorRet(*this, &DeltaObjectActorProxy::GetTranslation), 
        "Specifies the Translation of the object", 
        "Transformable"));
   }
}

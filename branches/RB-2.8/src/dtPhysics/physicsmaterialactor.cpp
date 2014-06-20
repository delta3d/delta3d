/* -*-c++-*-
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2006, Alion Science and Technology, BMH Operation
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
 * @author Allen Danklefsen
 * @author Brad Anderegg
 */

#include <dtPhysics/physicsmaterialactor.h>
#include <dtDAL/enginepropertytypes.h>
#include <dtUtil/funtraits.h>
#include <dtUtil/typetraits.h>

namespace dtPhysics
{
   //////////////////////////////////////////////////////////
   // Proxy code
   //////////////////////////////////////////////////////////

   const dtUtil::RefString MaterialActorProxy::PROPERTY_KINETIC_FRICTION("KineticFriction");
   const dtUtil::RefString MaterialActorProxy::PROPERTY_STATIC_FRICTION("StaticFriction");
   const dtUtil::RefString MaterialActorProxy::PROPERTY_RESTITUTION("Restitution");
   const dtUtil::RefString MaterialActorProxy::PROPERTY_KINETIC_ANISOTROPIC_FRICTION("KineticAnisotropicFriction");
   const dtUtil::RefString MaterialActorProxy::PROPERTY_STATIC_ANISOTROPIC_FRICTION("StaticAnisotropicFriction");
   const dtUtil::RefString MaterialActorProxy::PROPERTY_DIRECTION_OF_ANISOTROPY("DirOfAnisotropy");
   const dtUtil::RefString MaterialActorProxy::PROPERTY_ENABLE_ANISOTROPIC_FRICTION("EnableAnisotropicFriction");
   const dtUtil::RefString MaterialActorProxy::PROPERTY_DISABLE_STRONG_FRICTION("DisableStrongFriction");
   const dtUtil::RefString MaterialActorProxy::PROPERTY_MATERIAL_INDEX("MaterialIndex");

   //////////////////////////////////////////////////////////
   MaterialActorProxy::MaterialActorProxy()
   {
      SetClassName("dtPhysics::MaterialActor");
   }

   //////////////////////////////////////////////////////////
   MaterialActorProxy::~MaterialActorProxy()
   {

   }

   //////////////////////////////////////////////////////////
   void MaterialActorProxy::CreateActor()
   {
      SetDrawable(*new MaterialActor());
   }

   //////////////////////////////////////////////////////////
   void MaterialActorProxy::BuildPropertyMap()
   {
      static const dtUtil::RefString GROUP = "Material";

      MaterialActor* actor = dynamic_cast<MaterialActor*>(GetDrawable());

      typedef dtDAL::PropertyRegHelper<MaterialActorProxy&, MaterialDef> PropRegType;
      PropRegType propRegHelper(*this, &actor->GetMateralDef(), GROUP);

      DT_REGISTER_PROPERTY(KineticFriction, "Material Setting - coefficient of kinetic friction -- should be in [0, +inf]. If set to greater than staticFriction, "
               "the effective value of staticFriction will be increased to match.", PropRegType, propRegHelper);

      DT_REGISTER_PROPERTY(StaticFriction, "Material Setting - coefficient of static friction -- should be in [0, +inf]", PropRegType, propRegHelper);

      DT_REGISTER_PROPERTY(Restitution, "Material Setting - coefficient of restitution -- 0 makes the object bounce as little as possible, "
               "higher values up to 1.0 result in more bounce. Note that values close to or above 1 may cause stability "
               "problems and/or increasing energy. Range: [0,1]", PropRegType, propRegHelper);

      DT_REGISTER_PROPERTY(KineticAnisotropicFriction, "Material Setting - vector of values that are scales, usually 0 - 1, for each axis to scale the base kinetic friction "
               "when anisotropic friction is turned on", PropRegType, propRegHelper);

      DT_REGISTER_PROPERTY(StaticAnisotropicFriction, "Material Setting - vector of values that are scales, usually 0 - 1, for each axis to scale the base static friction "
               "when anisotropic friction is turned on", PropRegType, propRegHelper);

      DT_REGISTER_PROPERTY(DirOfAnisotropy, "When anisotropic friction is enabled, some physics engines allow one to re-orient the reference direction vector for the friction essentially"
               "allowing one to re-orient the static and kinetic anisotropic friction vec3s.", PropRegType, propRegHelper);

      DT_REGISTER_PROPERTY(EnableAnisotropicFriction,  "Enable anistropic friction, which allows one to set different friction values for a material depending on the direction of a collision.", PropRegType, propRegHelper);

      DT_REGISTER_PROPERTY(DisableStrongFriction, "When computing friction forces, some extra can accumulate between frames.  Some engines allow disable using the extra force.", PropRegType, propRegHelper);

      DT_REGISTER_PROPERTY(MaterialIndex, "The material index is a unique identifier for this material type.", PropRegType, propRegHelper);

   }

   //////////////////////////////////////////////////////////
   // Actor code
   //////////////////////////////////////////////////////////

   //////////////////////////////////////////////////////////
   MaterialActor::MaterialActor()
   {
      SetName("Material");
   }

   /////////////////////////////////////////////////////
   MaterialActor::~MaterialActor()
   {

   }

   /////////////////////////////////////////////////////
   osg::Node* MaterialActor::GetOSGNode()
   {
      return mNode.get();
   }

   /////////////////////////////////////////////////////
   const osg::Node* MaterialActor::GetOSGNode() const
   {
      return mNode.get();
   }

   //////////////////////////////////////////////////////////
   const MaterialDef& MaterialActor::GetMateralDef() const
   {
      return mMaterial;
   }

   //////////////////////////////////////////////////////////
   MaterialDef& MaterialActor::GetMateralDef()
   {
      return mMaterial;
   }

}

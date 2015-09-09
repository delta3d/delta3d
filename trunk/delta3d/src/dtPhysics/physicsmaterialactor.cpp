/* -*-c++-*-
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2006, Alion Science and Technology, BMH Operation
 * Copyright (C) 2014, Caper Holdings, LLC
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
 * Allen Danklefsen
 * Brad Anderegg
 * David Guthrie
 */

#include <dtPhysics/physicsmaterialactor.h>
#include <dtPhysics/physicscomponent.h>
#include <dtCore/propertymacros.h>
#include <dtCore/arrayactorpropertycomplex.h>
#include <dtGame/gamemanager.h>
#include <dtUtil/funtraits.h>
#include <dtUtil/typetraits.h>

namespace dtPhysics
{
   /**
   * This is a simple stub drawable that does nothing.
   */
   class MaterialDrawable : public dtCore::DeltaDrawable
   {
   public:

      // Constructor
      MaterialDrawable();

      ///required by DeltaDrawable
      osg::Node* GetOSGNode();
      const osg::Node* GetOSGNode() const;

   protected:

      // Destructor
      virtual ~MaterialDrawable();

   private:

      dtCore::RefPtr<osg::Node> mNode;
   };

   //////////////////////////////////////////////////////////
   // Actor code
   //////////////////////////////////////////////////////////

   const dtUtil::RefString MaterialActor::PROPERTY_ALIASES("Aliases");
   const dtUtil::RefString MaterialActor::PROPERTY_KINETIC_FRICTION("KineticFriction");
   const dtUtil::RefString MaterialActor::PROPERTY_STATIC_FRICTION("StaticFriction");
   const dtUtil::RefString MaterialActor::PROPERTY_RESTITUTION("Restitution");
   const dtUtil::RefString MaterialActor::PROPERTY_KINETIC_ANISOTROPIC_FRICTION("KineticAnisotropicFriction");
   const dtUtil::RefString MaterialActor::PROPERTY_STATIC_ANISOTROPIC_FRICTION("StaticAnisotropicFriction");
   const dtUtil::RefString MaterialActor::PROPERTY_DIRECTION_OF_ANISOTROPY("DirOfAnisotropy");
   const dtUtil::RefString MaterialActor::PROPERTY_ENABLE_ANISOTROPIC_FRICTION("EnableAnisotropicFriction");
   const dtUtil::RefString MaterialActor::PROPERTY_DISABLE_STRONG_FRICTION("DisableStrongFriction");
   const dtUtil::RefString MaterialActor::PROPERTY_MATERIAL_INDEX("MaterialIndex");

   //////////////////////////////////////////////////////////
   MaterialActor::MaterialActor()
   {
      SetName("Material");
      SetClassName("dtPhysics::MaterialActor");
      SetInitialOwnership(dtGame::GameActorProxy::Ownership::CLIENT_AND_SERVER_LOCAL);
   }

   //////////////////////////////////////////////////////////
   MaterialActor::~MaterialActor()
   {

   }

   //////////////////////////////////////////////////////////
   void MaterialActor::CreateDrawable()
   {
      SetDrawable(*new MaterialDrawable());
   }

   //////////////////////////////////////////////////////////
   void MaterialActor::OnEnteredWorld()
   {
      PhysicsComponent* physComp;
      GetGameManager()->GetComponentByName(PhysicsComponent::DEFAULT_NAME, physComp);
      if (physComp != NULL)
      {
         PhysicsMaterials& materials = physComp->GetPhysicsWorld().GetMaterials();

         materials.CreateOrUpdateMaterial(GetName(), GetMaterialDef());
         for (unsigned i = 0; i < mAliases.size(); ++i)
         {
            materials.SetMaterialAlias(GetName(), mAliases[i]);
         }
      }
   }

   //////////////////////////////////////////////////////////
   void MaterialActor::OnRemovedFromWorld()
   {
      PhysicsComponent* physComp;
      GetGameManager()->GetComponentByName(PhysicsComponent::DEFAULT_NAME, physComp);
      if (physComp != NULL)
      {
         PhysicsMaterials& materials = physComp->GetPhysicsWorld().GetMaterials();
         for (unsigned i = 0; i < mAliases.size(); ++i)
         {
            materials.RemoveAlias(mAliases[i]);
         }
      }
   }

   //////////////////////////////////////////////////////////
   void MaterialActor::BuildPropertyMap()
   {
      BaseClass::BuildPropertyMap();
      static const dtUtil::RefString GROUP = "Material";

      typedef dtCore::PropertyRegHelper<MaterialActor&, MaterialDef> PropRegType;
      PropRegType propRegHelper(*this, &GetMaterialDef(), GROUP);

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

      typedef dtCore::ArrayActorPropertyComplex<std::string> AliasArrayPropType;
      dtCore::RefPtr<AliasArrayPropType> arrayProp =
               new AliasArrayPropType
                  (PROPERTY_ALIASES, PROPERTY_ALIASES,
                   AliasArrayPropType::SetFuncType(this, &MaterialActor::SetAlias),
                   AliasArrayPropType::GetFuncType(this, &MaterialActor::GetAlias),
                   AliasArrayPropType::GetSizeFuncType(this, &MaterialActor::GetNumAliases),
                   AliasArrayPropType::InsertFuncType(this, &MaterialActor::InsertAlias),
                   AliasArrayPropType::RemoveFuncType(this, &MaterialActor::RemoveAlias),
                   "Aliases names for this material.",
                   GROUP
                  );

      dtCore::RefPtr<dtCore::StringActorProperty> nestedPropertyContainer =
               new dtCore::StringActorProperty("NestedAliasProperty",
                     "NestedAliasProperty",
                     dtCore::StringActorProperty::SetFuncType(arrayProp.get(), &AliasArrayPropType::SetCurrentValue),
                     dtCore::StringActorProperty::GetFuncType(arrayProp.get(), &AliasArrayPropType::GetCurrentValue),
                     "", GROUP);

      arrayProp->SetArrayProperty(*nestedPropertyContainer);
      AddProperty(arrayProp);

   }
   //////////////////////////////////////////////////////////
   Material* MaterialActor::GetMaterial() const
   {
      Material* uniqueMaterial = nullptr;
      PhysicsComponent* physComp = nullptr;
      GetGameManager()->GetComponentByName(PhysicsComponent::DEFAULT_NAME, physComp);
      if (physComp != nullptr)
      {
         PhysicsMaterials& materials = physComp->GetPhysicsWorld().GetMaterials();

         uniqueMaterial = materials.GetMaterial(GetName());
      }
      return uniqueMaterial;
   }

   //////////////////////////////////////////////////////////
   const MaterialDef& MaterialActor::GetMaterialDef() const
   {
      return mMaterial;
   }

   //////////////////////////////////////////////////////////
   MaterialDef& MaterialActor::GetMaterialDef()
   {
      return mMaterial;
   }

   DT_IMPLEMENT_ARRAY_ACCESSOR(MaterialActor, std::string, Alias, Aliases, std::string());


   //////////////////////////////////////////////////////////
   // Stub Drawable code
   //////////////////////////////////////////////////////////

   //////////////////////////////////////////////////////////
   MaterialDrawable::MaterialDrawable()
   : mNode(new osg::Node)
   {
      SetName("Material");
   }

   /////////////////////////////////////////////////////
   MaterialDrawable::~MaterialDrawable()
   {

   }

   /////////////////////////////////////////////////////
   osg::Node* MaterialDrawable::GetOSGNode()
   {
      return mNode.get();
   }

   /////////////////////////////////////////////////////
   const osg::Node* MaterialDrawable::GetOSGNode() const
   {
      return mNode.get();
   }

}

/* -*-c++-*-
 * dtPhysics
 * Copyright 2007-2008, Alion Science and Technology
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
 * This software was developed by Alion Science and Technology Corporation under
 * circumstances in which the U. S. Government may have rights in the software.
 * david
 */

#include <dtPhysics/physicsmaterials.h>
#include <dtPhysics/palutil.h>
#include <pal/pal.h>

namespace dtPhysics
{
   class PhysicsMaterialsImpl
   {
   public:
      PhysicsMaterialsImpl(palMaterials& materials)
      : mPalMaterials(materials)
      {
      }
      palMaterials& mPalMaterials;
   };


   //////////////////////////////////////
   const std::string PhysicsMaterials::DEFAULT_MATERIAL_NAME("__DEFAULT__");

   //////////////////////////////////////
   PhysicsMaterials::PhysicsMaterials(palMaterials& materials)
   {
      mImpl = new PhysicsMaterialsImpl(materials);
   }

   //////////////////////////////////////
   PhysicsMaterials::~PhysicsMaterials()
   {
      delete mImpl;
   }

   //////////////////////////////////////
   Material* PhysicsMaterials::NewMaterial(const std::string& name, const MaterialDef& def)
   {
      palMaterials& pm = mImpl->mPalMaterials;
      palMaterialDesc desc;
      MatDefToPalMatDesc(desc, def);
      return pm.NewMaterial(name, desc);
   }

   //////////////////////////////////////
   void PhysicsMaterials::SetMaterialInteraction(const std::string& name1, const std::string& name2, const MaterialDef& def)
   {
      palMaterials& pm = mImpl->mPalMaterials;
      palMaterialDesc desc;
      MatDefToPalMatDesc(desc, def);

      pm.SetMaterialInteraction(name1, name2, desc);
   }

   //////////////////////////////////////
   Material* PhysicsMaterials::GetMaterial(const std::string& name)
   {
      palMaterials& pm = mImpl->mPalMaterials;
      return pm.GetMaterial(name);
   }

   //////////////////////////////////////
   void PhysicsMaterials::SetMaterialDef(const std::string& name, MaterialDef& def)
   {
      SetMaterialInteraction(name, name, def);
   }

   //////////////////////////////////////
   void PhysicsMaterials::GetMaterialDef(const std::string& name, MaterialDef& def)
   {
      Material* mat = GetMaterial(name);
      if (mat != NULL)
      {
         PalMatDescToMatDef(def, *mat);
      }
   }

   //////////////////////////////////////
   bool PhysicsMaterials::GetMaterialInteraction(const std::string& name1, const std::string& name2, MaterialDef& defToFill)
   {
      palMaterials& pm = mImpl->mPalMaterials;
      palMaterialInteraction* pMI = pm.GetMaterialInteraction(name1, name2);
      if (pMI != NULL)
      {
         PalMatDescToMatDef(defToFill, *pMI);
         return true;
      }
      return false;
   }

   //////////////////////////////////////
   palMaterials& PhysicsMaterials::GetPalMaterials()
   {
      return mImpl->mPalMaterials;
   }
   //////////////////////////////////////////////////////////////////////////
   MaterialDef::MaterialDef()
   : mStaticFriction(0.5f)
   , mKineticFriction(0.5f)
   , mRestitution(0.2f)
   , mKineticAnisotropicFriction(1.0f, 1.0f, 1.0f)
   , mStaticAnisotropicFriction(1.0f, 1.0f, 1.0f)
   , mDirOfAnisotropy(1.0f, 0.0f, 0.0f)
   , mEnableAnisotropicFriction(false)
   , mDisableStrongFriction(false)
   {
   };
   //////////////////////////////////////////////////////////
   DT_IMPLEMENT_ACCESSOR(MaterialDef, float, Restitution);

   //////////////////////////////////////////////////////////
   DT_IMPLEMENT_ACCESSOR(MaterialDef, float, StaticFriction);

   //////////////////////////////////////////////////////////
   DT_IMPLEMENT_ACCESSOR(MaterialDef, float, KineticFriction);

   //////////////////////////////////////////////////////////
   DT_IMPLEMENT_ACCESSOR(MaterialDef, VectorType, StaticAnisotropicFriction);

   //////////////////////////////////////////////////////////
   DT_IMPLEMENT_ACCESSOR(MaterialDef, VectorType, KineticAnisotropicFriction);

   //////////////////////////////////////////////////////////
   DT_IMPLEMENT_ACCESSOR(MaterialDef, VectorType, DirOfAnisotropy);

   //////////////////////////////////////////////////////////
   DT_IMPLEMENT_ACCESSOR(MaterialDef, bool, EnableAnisotropicFriction);

   //////////////////////////////////////////////////////////
   DT_IMPLEMENT_ACCESSOR(MaterialDef, bool, DisableStrongFriction);

   //////////////////////////////////////////////////////////
   DT_IMPLEMENT_ACCESSOR(MaterialDef, MaterialIndex, MaterialIndex);

}

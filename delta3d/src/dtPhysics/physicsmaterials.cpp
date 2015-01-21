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
   typedef std::map<std::string, Material*> NameToMaterialMap;

   class PhysicsMaterialsImpl
   {
   public:
      PhysicsMaterialsImpl(palMaterials& materials)
      : mPalMaterials(materials)
      {
      }
      palMaterials& mPalMaterials;
      NameToMaterialMap mMaterialMap;
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
   Material* PhysicsMaterials::NewMaterial(const std::string& name, MaterialDef& def)
   {
      palMaterials& pm = mImpl->mPalMaterials;
      palMaterialDesc desc;
      MatDefToPalMatDesc(desc, def);

      Material* mat = pm.NewMaterial(name, desc);
      if (mat != NULL)
      {
         def.SetMaterialIndex(mat->GetId());

         mImpl->mMaterialMap.insert(std::make_pair(name, mat));
      }


      return mat;
   }

   //////////////////////////////////////
   Material* PhysicsMaterials::CreateOrUpdateMaterial(const std::string& name, MaterialDef& def)
   {
      Material* m = GetMaterial(name);
      if (m == NULL)
      {
         m = NewMaterial(name, def);
      }
      else
      {
         palMaterialDesc desc;
         MatDefToPalMatDesc(desc, def);
         m->SetParameters(desc);
      }
      return m;
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
   void PhysicsMaterials::SetMaterialInteraction(Material& mat1, Material& mat2, MaterialDef& def, MaterialInteractionCollisionCallback* callback)
   {
      palMaterials& pm = mImpl->mPalMaterials;
      palMaterialDesc desc;
      MatDefToPalMatDesc(desc, def);

      // Set this to make sure it exists.
      pm.SetMaterialInteraction(&mat1, &mat2, desc);
      palMaterialInteraction* pmi = pm.GetMaterialInteraction(&mat1, &mat2);
      if (pmi != NULL)
      {
         pmi->SetCollisionCallback(callback);
      }
   }

   //////////////////////////////////////
   Material* PhysicsMaterials::GetMaterial(const std::string& name)
   {
      palMaterials& pm = mImpl->mPalMaterials;
      return pm.GetMaterial(name);
   }

   //////////////////////////////////////
   const Material* PhysicsMaterials::GetMaterial(const std::string& name) const
   {
      palMaterials& pm = mImpl->mPalMaterials;
      return pm.GetMaterial(name);
   }

   //////////////////////////////////////
   bool PhysicsMaterials::SetMaterialDef(const std::string& name, const MaterialDef& def)
   {
      Material* m = GetMaterial(name);
      if (m != NULL)
      {
         SetMaterialDef(*m, def);
      }
      return m != NULL;
   }

   //////////////////////////////////////
   void PhysicsMaterials::SetMaterialDef(Material& mat, const MaterialDef& def)
   {
      MatDefToPalMatDesc(mat, def);
   }

   //////////////////////////////////////
   bool PhysicsMaterials::GetMaterialDef(const std::string& name, MaterialDef& outDef) const
   {
      const Material* mat = GetMaterial(name);
      if (mat != NULL)
      {
         GetMaterialDef(*mat, outDef);
      }
      return mat != NULL;
   }

   //////////////////////////////////////
   void PhysicsMaterials::GetMaterialDef(const Material& mat, MaterialDef& outDef) const
   {
      PalMatDescToMatDef(outDef, mat);

      outDef.SetMaterialIndex(mat.GetId());
   }

   //////////////////////////////////////
   bool PhysicsMaterials::GetMaterialInteraction(const std::string& name1, const std::string& name2, MaterialDef& defToFill)
   {
      palMaterials& pm = mImpl->mPalMaterials;
      palMaterialInteraction* pMI = pm.GetMaterialInteraction(name1, name2);
      if (pMI != NULL)
      {
         PalMatDescToMatDef(defToFill, *pMI);
      }
      return pMI != NULL;
   }

   //////////////////////////////////////
   bool PhysicsMaterials::GetMaterialInteraction(Material& mat1, Material& mat2, MaterialDef& defToFill)
   {
      palMaterials& pm = mImpl->mPalMaterials;
      palMaterialInteraction* pMI = pm.GetMaterialInteraction(&mat1, &mat2);
      if (pMI != NULL)
      {
         PalMatDescToMatDef(defToFill, *pMI);
      }
      return pMI != NULL;
   }

   //////////////////////////////////////
   palMaterials& PhysicsMaterials::GetPalMaterials()
   {
      return mImpl->mPalMaterials;
   }

   //////////////////////////////////////
   int PhysicsMaterials::GetMaterialCount() const
   {
      return mImpl->mMaterialMap.size();
   }

   //////////////////////////////////////
   Material* PhysicsMaterials::GetMaterialByIndex(MaterialIndex index) const
   {
      Material* curMat = NULL;
      NameToMaterialMap::const_iterator curIter = mImpl->mMaterialMap.begin();
      NameToMaterialMap::const_iterator endIter = mImpl->mMaterialMap.end();
      for (; curIter != endIter; ++curIter)
      {
         curMat = curIter->second;
         if (curMat->GetId() == index)
         {
            break;
         }

         // Material is not a match.
         // Clear the reference so that it is not returned.
         curMat = NULL;
      }

      return curMat;
   }

   //////////////////////////////////////
   bool PhysicsMaterials::GetMaterialDefByIndex(MaterialIndex index, MaterialDef& outDef) const
   {
      Material* mat = GetMaterialByIndex(index);
      if (mat != NULL)
      {
         GetMaterialDef(*mat, outDef);
      }
      return mat != NULL;
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

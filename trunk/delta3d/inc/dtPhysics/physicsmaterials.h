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

#ifndef PHYSICS_MATERIALS_H_
#define PHYSICS_MATERIALS_H_

#include <dtPhysics/physicsexport.h>
#include <dtPhysics/physicstypes.h>
#include <osg/Referenced>
#include <dtUtil/getsetmacros.h>

class palMaterials;

namespace dtPhysics
{
   /**
    * This is a little helper class for defining a material.  It allows one to add more
    * material parameters laters without changing method signatures.
    */
   class DT_PHYSICS_EXPORT MaterialDef
   {
   public:

      MaterialDef();

      /**
      * Setter. Static Friction is a material property for objects.  It defines
      * how easy or hard it is to get something moving when it is stopped. 0 means
      * a frictionless environment (space).
      * @param value The value of static friction (0.5 default)
      */
      DT_DECLARE_ACCESSOR(float, StaticFriction);

      /**
      * Setter. Kinetic Friction is an material property for objects.  It defines
      * how easy or hard it is to keep something sliding. 0 means a frictionless
      * environment (space).
      * @param value The value of dynamic friction (0.5 default)
      */
      DT_DECLARE_ACCESSOR(float, KineticFriction);

      /**
      * Setter. Restitution is a material property for objects. It defines the 'bouncy-ness' of
      * objects. It should be [0,1).  Values close to 0 mean almost NO bounce. Values close to 1
      * will retain all energy (possibly increase).
      * @param value The value of restituion [0, 1) (0.2 default)
      */
      DT_DECLARE_ACCESSOR(float, Restitution);

      DT_DECLARE_ACCESSOR(VectorType, KineticAnisotropicFriction);
      DT_DECLARE_ACCESSOR(VectorType, StaticAnisotropicFriction);
      DT_DECLARE_ACCESSOR(VectorType, DirOfAnisotropy);

      DT_DECLARE_ACCESSOR(bool, EnableAnisotropicFriction);
      DT_DECLARE_ACCESSOR(bool, DisableStrongFriction);

      /**
      *	This is a unique index assigned to a material, it can be used as a vertex attribute to support
      *  per triangle materials.
      */
      DT_DECLARE_ACCESSOR(MaterialIndex, MaterialIndex);

   };

   class PhysicsMaterialsImpl;

   /**
    * Materials for dtPhysics are created and managed by this class.
    * The physics world holds onto the instance of this, so there is no need to create one.
    */
   class DT_PHYSICS_EXPORT  PhysicsMaterials : public osg::Referenced
   {
   public:
      /// The name given to the default material.  Physics objects will use this as the default.
      static const std::string DEFAULT_MATERIAL_NAME;

      PhysicsMaterials(palMaterials& materials);

      /**
       * Creates a new material with the given name and the definition.
       * If the named material already exists, nothing happens.
       * If there is an alias with this name, it deletes the alias in favor of the material.
       * TODO I may revise this to throw an exception.
       * @param name The name for the new material.
       * @param def the definintion of the material.
       */
      Material* NewMaterial(const std::string& name, MaterialDef& def);
      /**
       * Creates a new material with the given name and the definition.
       * if it exists, it updates the definition to the given one.
       * @param name The name for the material.
       * @param def the definintion of the material.
       * @param ignoreAliases defaults to true because the code normally calling this would want to create a new material and drop the alias.
       */
      Material* CreateOrUpdateMaterial(const std::string& name, MaterialDef& def, bool ignoreAliases = true);

      ///@return a material by name (or aliases if desired) or NULL if it doesn't exist.
      Material* GetMaterial(const std::string& name, bool ignoreAliases = false);
      ///@return a material by name or NULL if it doesn't exist.
      const Material* GetMaterial(const std::string& name) const;

      /**
       * Changes the definition of a material.
       * @param name Name of the material to modify.
       * @param def Material defition containing the parameters to update.
       * @return TRUE if a material was found and modified.
       */
      bool SetMaterialDef(const std::string& name, const MaterialDef& def);

      /**
       * Loaded physics data will often have material data with names that haven't been configured in a
       * material object, or one may temporarily want to use the same definition for multiple materials.
       * This allows one to do that without the data duplication.
       */
      void SetMaterialAlias(const std::string& name, const std::string& aliasName);
      void RemoveAlias(const std::string& aliasName);
      void ClearAliases();

      /// Changes the definition of a material directly.
      void SetMaterialDef(Material& mat, const MaterialDef& def);

      /**
       * Retrieves the definition of a material.
       * @param name Name of the material for which to retrieve the definition.
       * @param outDef The definition to capture information about the specified material.
       * @return TRUE if a material was found by the specified name.
       */
      bool GetMaterialDef(const std::string& name, MaterialDef& outDef) const;
   
      /**
       * Retrieves the definition of a material.
       * @param mat Material for which to retrieve the definition.
       * @param outDef The definition to capture information about the specified material.
       */
      void GetMaterialDef(const Material& mat, MaterialDef& outDef) const;

      /**
       * Defines the interaction between two materials by name.
       * If the names match, this is a no-op.
       * @param name1 the name of the first material
       * @param name2 the name of the second material
       * @param def the definintion of the material.
       */
      void SetMaterialInteraction(const std::string& name1, const std::string& name2, const MaterialDef& def);

      /**
       * Defines the interaction between two materials by name.
       * @param mat1 First material
       * @param mat2 Second material
       * @param def Definintion of the material.
       * @param callback Method to be called when interaction between the two specified materials occurs.
       */
      void SetMaterialInteraction(Material& mat1, Material& mat2, MaterialDef& def, MaterialInteractionCollisionCallback* callback = NULL);

      /**
       * Retrieves the interaction between two materials.  The order of the material names does not matter.
       * If you have the materials handy, it is much faster to call the version that takes the two pointers.
       * @param name1 The name of the first material
       * @param name2 The name of the second material
       * @param defToFill the material def that will be assigned the values of the interaction.
       *    If the method returns false, the values in the def object are not changed.
       * @return true if the material interaction exists.
       */
      bool GetMaterialInteraction(const std::string& name1, const std::string& name2, MaterialDef& defToFill);

      /**
       * Retrieves a material interaction.  This is the preferred function to call.
       * @param mat1 The first material
       * @param mat2 The second material
       * @param defToFill the material def that will be assigned the values of the interaction.
       *    If the method returns false, the values in the def object are not changed.
       * @return true if the material interaction exists.
       */
      bool GetMaterialInteraction(Material& mat1, Material& mat2, MaterialDef& defToFill);

      palMaterials& GetPalMaterials();

      /// Returns the number of materials referenced in this collection.
      int GetMaterialCount() const;

      /**
       * Convenience method for acquiring a material by its index.
       * @param index Id of the material to be find.
       * @return Material matching the index; NULL if not found.
       */
      Material* GetMaterialByIndex(MaterialIndex index) const;
      
      /**
       * Convenience method for acquiring a material by its index.
       * @param index Id of the material to be find.
       * @param outDef The definition to capture information about the found material.
       * @return TRUE if a material was found and the definition set.
       */
      bool GetMaterialDefByIndex(MaterialIndex index, MaterialDef& outDef) const;

   protected:
      virtual ~PhysicsMaterials();
   private:
      PhysicsMaterialsImpl* mImpl;
   };

}

#endif /* PHYSICS_MATERIALS_H_ */

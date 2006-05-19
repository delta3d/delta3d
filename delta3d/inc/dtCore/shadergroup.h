/*
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2006, Alion Science and Technology
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
 * @author Matthew W. Campbell
 */
#ifndef DELTA_SHADERGROUP
#define DELTA_SHADERGROUP

#include "dtCore/shader.h"
#include "dtCore/export.h"

#include <dtCore/refptr.h>
#include <osg/Referenced>

#include <map>
#include <vector>

namespace dtCore
{
   /**
    * This class is a container for shaders.  It allows shaders to be logically grouped
    * together providing a simple way to assign shaders to actors.  If a shader group
    * contains more than a default shader (all defined in the Shader Definitions XML file)
    * then it is the actor's responsibility to find the appropriate shader from the group
    * and ask the shader manager to apply it.
    */
   class DT_CORE_EXPORT ShaderGroup : public osg::Referenced
   {
      public:

         /**
          * Constructs an empty shader group.
          * @param groupName Name of this group.  Group names must be unique.
          */
         ShaderGroup(const std::string &groupName);

         /**
          * Gets the name currently assigned to this group.
          * @return The groups name.
          */
         const std::string &GetName() const { return mName; }

         /**
          * Adds a shader to the group's list of shaders.
          * @param shader The new shader to add to the group.  Note, if a shader
          *    with the same name already exists in the group, an exception is thrown.
          * @param isDefault If true, this shader is set as the default shader.
          * @see GetDefaultShader()
          */
         void AddShader(Shader &shader, bool isDefault = false);

         /**
          * Removes the specified shader from this shader group.
          * @param shader The shader to remove.
          */
         void RemoveShader(Shader &shader);

         /**
          * Removes the shader with the specified name from this shader group.
          * @param shaderName The name of the shader to remove.  If a shader
          *    is not found with that name a warning is logged and this method
          *    becomes a no-op.
          */
         void RemoveShader(const std::string &shaderName);

         /**
          * Clears the group's list of shaders.
          */
         void RemoveAllShaders() { mShaders.clear(); mDefaultShader = NULL; }

         /**
          * Searches the group for a shader with the specified name.
          * @return The shader if found, or NULL if the shader could not be located.
          */
         Shader *FindShader(const std::string &name);

         /**
          * Searches the group for a shader with the specified name. (const-version).
          * @return A const pointer to the shader or NULL if the shader could not be found.
          */
         const Shader *FindShader(const std::string &name) const;

         /**
          * Gets the number of shaders currently owned by this shader group.
          * @return The shader count.
          */
         unsigned int GetNumShaders() const { return mShaders.size(); }

         /**
          * Gets a list of all the shaders owned by this shader group.
          * @param toFill A list to be filled with the shaders in this group.
          * @note The specified vector parameter is cleared before filling.
          */
         void GetAllShaders(std::vector<dtCore::RefPtr<Shader> > &toFill);

         /**
          * Gets the default shader assigned to this group.
          * @return The default shader.
          * @note The default shader is either the first shader in the group's
          *    list, or the shader specified as default when it was added,
          *    or NULL if there are no shaders in this group.
          */
         const Shader *GetDefaultShader() const { return mDefaultShader.get(); }

         /**
          * Non const verstion of GetDefaultShader().
          */
         Shader *GetDefaultShader() { return mDefaultShader.get(); }

         /**
          * Updates this shader group's state by calling update on each shader it is
          * currently maintaining.
          * @note As with the Shader::Update method, this method does not rebind shader
          *   sources.  Must call ShaderManager::AssignShader for shader sources to get
          *   reloaded and rebound.
          */
         virtual void Update();

         /**
          * Sets the dirty flag of this shader group.
          * @param flag True if dirty, false otherwise.
          */
         void SetDirty(bool flag) { mIsDirty = flag; }

         /**
          * Gets the dirty bit of this shader group.
          * @return True if dirty, false otherwise.
          */
         bool IsDirty() const { return mIsDirty; }

      protected:

         /**
          * Destroys the shader group.
          */
         virtual ~ShaderGroup();

      private:
         std::string mName;
         bool mIsDirty;
         std::map<std::string,dtCore::RefPtr<Shader> > mShaders;
         dtCore::RefPtr<Shader> mDefaultShader;
   };
}

#endif

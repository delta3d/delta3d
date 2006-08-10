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
#ifndef DELTA_SHADER
#define DELTA_SHADER

#include <dtCore/refptr.h>
#include <dtUtil/enumeration.h>
#include "dtCore/export.h"

#include <osg/Referenced>
#include <osg/Program>
#include <osg/Shader>

#include <string>
#include <map>

namespace dtCore
{
   class ShaderManager;
   class ShaderParameter;
   class ShaderGroup;

   /**
    * This class represents a single shader or rendering effect.  It contains a vertex
    * shader, fragment shader, and any sort of library shader support code that may
    * exist.
    * @note Although it contains methods for setting vertex and fragment programs,
    *    the shader itself could support fixed function effects as well; however it is
    *    tailored towards the programmable pipeline.
    * @note
    *    Each shader has a list of parameters associated with it.  These may be used
    *    by the shader and or the objects using the shader in any way they see fit.
    */
   class DT_CORE_EXPORT Shader : public osg::Referenced
   {
      public:

         /**
          * Construct an empty shader.
          * @param name Name to assign the new shader.  Shader names must be unique.
          */
         Shader(const std::string &name);

         /**
          * Gets the name currently assigned to this shader.
          * @return The shader's name.
          */
         const std::string &GetName() const { return mName; }

         /**
          * This method loads the shader source meant for vertex processing from the
          * specified file name.
          * @param fileName The path of the shader file source.  This must be either the
          *    full path or a path relative to the Delta3d path list.
          */
         void SetVertexShaderSource(const std::string &fileName);

         /**
          * Gets the vertex shader file name.
          * @return The vertex shader file name and path currently in use by this shader.
          */
         const std::string &GetVertexShaderSource() const { return mVertexShaderFileName; }

         /**
          * This method loads the shader source meant for fragment processing from the
          * specified file name.
          * @param fileName The path of the shader file source.  This must be either the
          *    full path or a path relative to the Delta3d path list.
          */
         void SetFragmentShaderSource(const std::string &fileName);

         /**
          * Gets the fragment shader file name.
          * @return The fragment shader file name and path currently in use by this shader.
          */
         const std::string &GetFragmentShaderSource() const { return mFragmentShaderFileName; }

         /**
          * Binds a shader parameter to this shader.
          * @param newParam The new shader parameter to bind or attach to the shader.
          * @note An exception is thrown if a parameter with the same name already
          *    exists on the shader.
          */
         void AddParameter(ShaderParameter &newParam);

         /**
          * Removes an existing parameter from the shader.
          * @param param The parameter to remove.  This results in a no-op if the parameter
          *    to remove is not owned by this shader.
          */
         void RemoveParameter(ShaderParameter &param);

         /**
          * Removes an existing parameter from this shader.
          * @param name The name of the shader parameter to remove.  If this parameter is not
          *    currently owned by this shader, this method is a no-op.
          */
         void RemoveParameter(const std::string &name);

         /**
          * Clears this shaders list of parameters.
          * @note This does not mean that any shaders and its parameters bound to a render state
          *   are cleared, just this shader instance.
          */
         void RemoveAllParameters();

         /**
          * Looks up a shader parameter with the given name. (Const version).
          * @return A const pointer to the shader parameter or NULL if it could not be found.
          */
         const ShaderParameter *FindParameter(const std::string &name) const;

         /**
          * Looks up a shader parameter with the given name. (Non-const version).
          * @return A pointer to the shader parameter or NULL if it could not be found.
          */
         ShaderParameter *FindParameter(const std::string &name);

         /**
          * Gets the number of parameters currently attached to this shader.
          * @return The parameter count.
          */
         unsigned int GetNumParameters() const { return mParameters.size(); }

         /**
          * Fills the specified vector with all the shader parameters currently
          * attached to this shader.
          * @param toFill The vector to fill.  Note, this is cleared first.
          */
         void GetParameterList(std::vector<dtCore::RefPtr<ShaderParameter> > &toFill) const;

         /**
          * Updates any parameter state which may have changed on this shader since the
          * last update.
          * @note This does not rebind any changed shader sources.  In order to rebind,
          *   the ShaderManager's AssignShader method must be called.
          * @see ShaderManager::AssignShader
          */
         virtual void Update();

         /**
          * Gets a pointer to the actual shader object used for vertex processing.
          * @return A reference counted pointer to the shader object or NULL if one is currently
          *    not assigned.
          */
         const osg::Shader *GetVertexShader() const { return mVertexShader.get(); }

         /**
          * Gets a pointer to the actual shader object used for fragment processing.
          * @return A reference counted pointer to the shader object or NULL in one is
          *    currently not assigned.
          */
         const osg::Shader *GetFragmentShader() const { return mFragmentShader.get(); }

         /**
          * Gets a pointer to the actual shader program constructed by the shader source code
          * compiled and linked.
          * @return A referenced counted pointer to the shader program.  This may be shared
          *    amoungst many renderable objects.
          */
         const osg::Program *GetShaderProgram() const { return mGLSLProgram.get(); }

         /**
          * Removes all shader source and compiler programs from this shader object.
          */
         void Reset();

         /**
          * Marks this shader as dirty and informs its parent shader group that one of
          * the shaders it contains has been marked as dirty.
          * @param flag True to set dirty, false to clear the dirty bit.
          */
         void SetDirty(bool flag);

         /**
          * Gets whether ot not this shader is dirty.
          * @return True if dirty, false otherwise.
          */
         bool IsDirty() const { return mIsDirty; }

      protected:

         /**
          * Destroys this shader object.
          */
         virtual ~Shader();

         ///Hidden copy constructor and assignment operator.
         Shader &operator=(const Shader &rhs);
         Shader(const Shader &rhs);

         void SetParentGroup(ShaderGroup *parent) { mParentGroup = parent; }
         ShaderGroup *GetParentGroup() const { return mParentGroup; }

      private:
         void SetVertexShader(osg::Shader &shader) { mVertexShader = &shader; }
         void SetFragmentShader(osg::Shader &shader) { mFragmentShader = &shader; }
         void SetGLSLProgram(osg::Program &program) { mGLSLProgram = &program; }

         std::string mName;
         std::string mVertexShaderFileName;
         std::string mFragmentShaderFileName;

         //List of parameters attached to this shader.  Parameters could be texture handles,
         //floats, ints, or special parameters like simulation time, or simulation delta time.
         std::map<std::string,dtCore::RefPtr<ShaderParameter> > mParameters;

         //Parent group containing this shader
         ShaderGroup *mParentGroup;
         bool mIsDirty;

         //These are set by the shader manager when this shader is added since these
         //the actual shader programs could be shared amounst different logical shaders.
         dtCore::RefPtr<osg::Program> mGLSLProgram;
         dtCore::RefPtr<osg::Shader> mVertexShader;
         dtCore::RefPtr<osg::Shader> mFragmentShader;

         friend class ShaderManager;
         friend class ShaderGroup;
   };
}

#endif

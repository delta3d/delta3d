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
* Matthew W. Campbell, Curtiss Murphy
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
#include <dtUtil/assocvector.h>

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
   * @note If you are using this class, you should also be using the ShaderManager. 
   *    If so, be sure you are modifying an instance of a Shader as opposed to a template. 
   * @note
   *    Each shader has a list of parameters associated with it.  These may be used
   *    by the shader and or the objects using the shader in any way they see fit.
   */
   class DT_CORE_EXPORT ShaderProgram : public osg::Referenced
   {
   public:

      /**
      * Construct an empty shader.
      * @param name Name to assign the new shader.  Shader names must be unique.
      */
      ShaderProgram(const std::string& name);

      /**
      * Gets the name currently assigned to this shader.
      * @return The shader's name.
      */
      const std::string& GetName() const { return mName; }

      /**
      * This method adds a shader source meant for geometry processing from the
      * specified file name.
      * @param fileName The path of the shader file source.  This must be either the
      *    full path or a path relative to the Delta3d path list.
      * @throw ShaderSourceException
      */
      void AddGeometryShader(const std::string& fileName);

      /**
      * This method sets the number of vertices that the geometry shader is expected to output.
      * This value must be correctly set in order for the program to work.
      * @param The number of vertices that the geometry shader should output.
      */
      void SetGeometryShaderVerticesOut(unsigned int verticesOut) { mGeometryShaderVerticesOut = verticesOut; }

      /**
      * Gets the number of vertices that the geometry shader is expected to emit.
      * @return The number of vertices the geometry shader should emit.
      */
      unsigned int GetGeometryShaderVerticesOut() { return mGeometryShaderVerticesOut; }

      /**
      * Gets the vertex shaders file names.
      * @return A Vector that contains a set of strings that represent the different vertex file names 
      */
      const std::vector<std::string>& GetGeometryShaders() const { return mGeometryShaderFileName; }


      /**
      * This method adds a shader source meant for vertex processing from the
      * specified file name.
      * @param fileName The path of the shader file source.  This must be either the
      *    full path or a path relative to the Delta3d path list.
      * @throw ShaderSourceException
      */
      void AddVertexShader(const std::string& fileName);

      /**
      * Gets the vertex shaders file names.
      * @return A Vector that contains a set of strings that represent the different vertex file names 
      */
      const std::vector<std::string>& GetVertexShaders() const { return mVertexShaderFileName; }


      /**
      * Gets the vertex cache key
      * @return The vertex cache key used to identify a certain shader group
      */
      const std::string& GetGeometryCacheKey();

      /**
      * Gets the vertex cache key
      * @return The vertex cache key used to identify a certain shader group
      */
      const std::string& GetVertexCacheKey();

      /**
      * Gets the fragment cache key
      * @return The fragment cache key used to identify a certain shader group
      */
      const std::string& GetFragmentCacheKey();

      /**
      * This method adds a shader source meant for fragment processing from the
      * specified file name.
      * @param fileName The path of the shader file source.  This must be either the
      *    full path or a path relative to the Delta3d path list.
      * @throw ShaderSourceException
      */
      void AddFragmentShader(const std::string& fileName);

      /**
      * Gets the fragment shaders file names.
      * @return A Vector that contains a set of strings that represent the different fragment file names 
      */
      const std::vector<std::string>& GetFragmentShaders() const { return mFragmentShaderFileName; }

      /**
      * Binds a shader parameter to this shader.
      * @param newParam The new shader parameter to bind or attach to the shader.
      * @note An exception is thrown if a parameter with the same name already
      *    exists on the shader.
      * @throw DuplicateShaderGroupException
      */
      void AddParameter(ShaderParameter& newParam);

      /**
      * Removes an existing parameter from the shader.
      * @param param The parameter to remove.  This results in a no-op if the parameter
      *    to remove is not owned by this shader.
      */
      void RemoveParameter(ShaderParameter& param);

      /**
      * Removes an existing parameter from this shader.
      * @param name The name of the shader parameter to remove.  If this parameter is not
      *    currently owned by this shader, this method is a no-op.
      */
      void RemoveParameter(const std::string& name);

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
      const ShaderParameter* FindParameter(const std::string& name) const;

      /**
      * Looks up a shader parameter with the given name. (Non-const version).
      * @return A pointer to the shader parameter or NULL if it could not be found.
      */
      ShaderParameter* FindParameter(const std::string &name);

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
      void GetParameterList(std::vector<dtCore::RefPtr<ShaderParameter> >& toFill) const;

      /** Add an attribute location binding. In Open GL Shader Language, each vertex
      * can have a number of attributes (minimum supported is 16 = 0 to 15).  You can use these 
      * attributes to put a single value for each vertex.
      */
      void AddBindAttributeLocation(const std::string& name, unsigned int index);

      /**
      * Updates any parameter state which may have changed on this shader since the
      * last update.
      * @note This does not rebind any changed shader sources.  In order to rebind,
      *   the ShaderManager's AssignShaderFromTemplate method must be called.
      * @see ShaderManager::AssignShaderFromTemplate
      */
      virtual void Update();

      /**
      * Gets a pointer to the actual shader program constructed by the shader source code
      * @return A pointer to the shader program.  This may be shared
      *    between many renderable objects.
      */
      const osg::Program* GetShaderProgram() const { return mGLSLProgram.get(); }

      /**
      * Gets a pointer to the actual shader program constructed by the shader source code.
      * @return A pointer to the shader program.  This may be shared
      *        between many renderable objects. Either put this in a RefPtr or don't hold onto it.
      */
      osg::Program* GetShaderProgram() { return mGLSLProgram.get(); }

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

      /**
      * Makes a deep copy of the shader and all its parameters. Used when a user assigns
      * a shader to a node using ShaderManager::AssignShaderFromTempalte.
      * @return the cloned shader instance with cloned parameters.
      */
      dtCore::ShaderProgram* Clone() const;

   protected:

      /**
      * Destroys this shader object.
      */
      virtual ~ShaderProgram();

      ///Hidden copy constructor and assignment operator.
      ShaderProgram &operator=(const ShaderProgram& rhs);
      ShaderProgram(const ShaderProgram& rhs);

   private:
      void SetGLSLProgram(osg::Program& program) { mGLSLProgram = &program; }

      std::string mName;

      //Cache Keys which are used as unique identifiers for identifying vertex and fragment shader groups
      //which are stored in map
      std::string mGeometryCacheKey;
      std::string mVertexCacheKey;
      std::string mFragmentCacheKey;

      std::vector<std::string> mGeometryShaderFileName;
      std::vector<std::string> mVertexShaderFileName;
      std::vector<std::string> mFragmentShaderFileName;

      unsigned int mGeometryShaderVerticesOut;

      //List of parameters attached to this shader.  Parameters could be texture handles,
      //floats, ints, or special parameters like simulation time, or simulation delta time.
      typedef dtUtil::AssocVector<std::string,dtCore::RefPtr<ShaderParameter> > ParameterListType;
      ParameterListType mParameters;

      // Note - removed mParentGroup since it was only used when you mark a shader dirty.  Since 
      // a template shader can never be marked dirty and an instanced Shader doesn't need a parent group, 
      // this is no longer needed.
      //Parent group containing this shader
      //ShaderGroup *mParentGroup;
      bool mIsDirty;

      //These are set by the shader manager when this shader is added since these
      //the actual shader programs could be shared amount different logical shaders.
      dtCore::RefPtr<osg::Program> mGLSLProgram;

      friend class ShaderManager;
      friend class ShaderGroup;
   };
}

#endif

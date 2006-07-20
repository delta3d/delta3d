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
#ifndef _SHADERMANAGER_H
#define _SHADERMANAGER_H

#include <dtCore/refptr.h>
#include <dtCore/base.h>
#include <dtUtil/exception.h>
#include "dtCore/shadergroup.h"
#include "dtCore/export.h"

#include <map>
#include <string>

/// @cond DOXYGEN_SHOULD_SKIP_THIS
namespace osg
{
   class Node;
}
/// @endcond

namespace dtCore
{

   /**
    * Defines the various exceptions that may be thrown when working with the shaders in the
    * Shader Manager.
    */
   class DT_CORE_EXPORT ShaderException : public dtUtil::Enumeration
   {
      DECLARE_ENUM(ShaderException);
      public:

         ///Thrown when an error occurs setting a shader source.
         static ShaderException SHADER_SOURCE_ERROR;

         ///Thrown when a shader with the same name is added to the manager.
         static ShaderException DUPLICATE_SHADERGROUP_FOUND;

         ///Thrown when an error is encounted while parsing an XML shader file.
         static ShaderException XML_PARSER_ERROR;

         ///Thrown when there is a duplicate shader parameter added to a shader.
         static ShaderException DUPLICATE_SHADER_PARAMETER_FOUND;

      private:
         ShaderException(const std::string &name) : dtUtil::Enumeration(name)
         {
            AddInstance(this);
         }
   };

   /**
    * This class manages the currently loaded and/or active shaders in the
    * current game.  It provides a central location for storing the shaders
    * thus allowing them to be shared amoungst many different actors or entities.
    * The shader manager can also parse shader definition files which are xml
    * files consisting of a list of shader definitions which can then be referenced
    * through the manager.
    * @note
    *    This class is a singleton class.
    * @note The ShaderManager actually stores ShaderGroups which are collections of
    *    related shaders.  When shader groups are added to the ShaderManager, the
    *    manager the specified shader source code, links it, and generates shader programs.
    *    If two or more shaders are added and their resulting compiled shader program
    *    is the same, then that program is shared amougst each shader containing the
    *    same code.  This means that users of this class need not worry about managing
    *    duplicate shaders as this is automatically resolved by the ShaderManager.
    */
   class DT_CORE_EXPORT ShaderManager : public dtCore::Base
   {
      protected:

         /**
          * This is a simple structure hold a shader program cache entry.
          * It contains names of shader sources and their resulting shader
          * programs.
          */
         struct ShaderCacheEntry
         {
            std::string vertexShaderSource;
            std::string fragmentShaderSource;
            dtCore::RefPtr<osg::Shader> vertexShader;
            dtCore::RefPtr<osg::Shader> fragmentShader;
            dtCore::RefPtr<osg::Program> shaderProgram;
         };

      public:

         /**
          * Gets the single global instance of this class.
          * @return The singleton instance.
          */
         static ShaderManager &GetInstance()
         {
            if (mInstance == NULL)
               mInstance = new ShaderManager();
            return *mInstance;
         }

         /**
          * Removes all shaders and shader groups from the manager.
          * @note This does not remove the shaders from the states with which they were bound.
          *    This method is useful to call when changing maps or something of that nature.
          */
         void Clear();

         /**
          * Adds a shader group to the manager's list of known shader groups.  Each shader
          * in the shader group is assigned the resulting compiled and linked shader program.
          * @param shaderGroup The new shader group to add.
          * @note An exception is thrown if the specified shader group does not have a unique
          *    name.
          */
         void AddShaderGroup(ShaderGroup &shaderGroup);

         /**
          * Removes the shader group with the specified name from the manager.
          * @param name The name of the shader group to remove.  If the shader group cannot be found
          *    in the manager's list of shaders this method is a no-op.
          */
         void RemoveShaderGroup(const std::string &name);

         /**
          * Removes the specified shader group from the manager.
          * @param shaderGroup The shader group to remove.  If this shader group is not currently being
          *    managed, this method is a no-op.
          */
         void RemoveShaderGroup(const ShaderGroup &shaderGroup);

         /**
          * Searches the shader manager for the specified shader group.
          * @param name Name of the shader group to find.
          * @return A const pointer to the shader group or NULL if it could not be found.
          */
         const ShaderGroup *FindShaderGroup(const std::string &name) const;

         /**
          * Searches the shader manager for the specified shader group.
          * @param name Name of the shader group to find.
          * @return A pointer to the shader group or NULL if it could not be found.
          */
         ShaderGroup *FindShaderGroup(const std::string &name);

         /**
          * Fills the specified vector with all the shader groups currently in the
          * shader manager.
          * @param toFill The vector with which to fill with shader groups.  Note, the
          *   vector is cleared before it is filled.
          */
         void GetAllShaderGroups(std::vector<dtCore::RefPtr<ShaderGroup> > &toFill);

         /**
          * Gets the specified shader from the manager.
          * @param name The name of the shader to find.
          * @param groupName The group containing the shader.  If this name is empty,
          *    the shader manager will traverse all the shader groups and return the
          *    first shader matching the specified shader name.
          * @return A const pointer to the shader or NULL if the shader could not be found.
          * @note For performance reasons, this method should be called with a shader
          *    group specified.
          */
         const Shader *FindShader(const std::string &name, const std::string &groupName="") const;

         /**
          * Gets the specified shader from the manager.
          * @param name The name of the shader to find.
          * @param groupName The group containing the shader.  If this name is empty,
          *    the shader manager will traverse all the shader groups and return the
          *    first shader matching the specified shader name.
          * @return A const pointer to the shader or NULL if the shader could not be found.
          * @note For performance reasons, this method should be called with a shader
          *    group specified.
          */
         Shader *FindShader(const std::string &name, const std::string &groupName="");

         /**
          * Assigns the shader including the program itself and its associated
          * parameters to the specified node.
          * @param name The shader to apply.
          * @param node The node with which to apply the shader.
          * @throws An exception is thrown if the shader does not have a valid
          *    vertex or fragment program bound to it.
          */
         void AssignShader(const Shader &shader, osg::Node &node);

         /**
          * Gets the number of shaders current managed by the shader manager.  This number reflects
          * the total number of shaders contained in all the shader groups currently in the shader
          * manager.
          * @return The shader count.
          */
         unsigned int GetNumShaders() const { return mTotalShaderCount; }

         /**
          * Gets the number of shader groups currently owned by the shader manager.
          * @return The number of shader groups contained in the shader manager.
          */
         unsigned int GetNumShaderGroups() const { return mShaderGroups.size(); }

         /**
          * Gets the size of the shader program cache.  This cache contains compiled shaders
          * that are shared within the shader manager.
          * @return The number of shader programs currently cached by the manager.
          */
         unsigned int GetShaderCacheSize() const { return mShaderProgramCache.size(); }

         /**
          * Loads a list of shaders defined in an external XML file.
          * @param fileName The XML file containing the shader definitions.
          * @param merge If false, this method will first clear the current list of shaders
          *    in the manager.  If true, the shaders loaded from the shader file will
          *    be added to the list of shaders already in the manager. (true by default).
          */
         void LoadShaderDefinitions(const std::string &fileName, bool merge=true);

         /**
          * Called when a message is sent from one of this classes message signalers.
          * @param msgData Data pertaining to the particular message sent.
          */
         void OnMessage(dtCore::Base::MessageData *msgData);

      protected:

         /**
          * Called before each frame gets rendered.  This method will check for changes to shader state
          * and update them if necessary.
          * @param deltaRealTime The delta time in real time since the last frame was rendered.
          * @param deltaSimTime The delta simulation time since the last frame was rendered.
          */
         void OnPreFrame(double deltaRealTime, double deltaSimTime);

         /**
          * Checks the shader program cache for the compiled programs for the specified
          * shader.  If none are found, a new cache entry is created.
          * @param shader The shader to resolve.
          */
         void ResolveShaderPrograms(Shader &shader);

         /**
          * Internal helper method which searches for a shader in the shader manager.
          * This is used by the const and non-const versions of FindShader in the
          * public interface.
          * @param shaderName Name of the shader to search for.
          * @param groupName Name of the group containing the shader.  If this is empty
          *    the first shader with the specified name is returned.
          */
         const Shader *InternalFindShader(const std::string &shaderName, const std::string &groupName) const;

      private:

         ///Count of the total number of shaders in the shader manager.
         unsigned int mTotalShaderCount;

         ///List of the shader groups currently loaded by the manager.
         std::map<std::string,dtCore::RefPtr<ShaderGroup> > mShaderGroups;

         ///Shader program cache which stores compiled shader programs that may
         ///be shared amoungst the loaded shaders.
         std::map<std::string,ShaderCacheEntry> mShaderProgramCache;

         /**
          * Constructs the shader manager.  Since this is a singleton class, this is private.
          */
         ShaderManager();

         /**
          * Destroys the shader manager.
          */
         virtual ~ShaderManager();

         ShaderManager(const ShaderManager &rhs) { }
         ShaderManager &operator=(const ShaderManager &rhs) { return *this; }

         ///Single instance of this class.
         static dtCore::RefPtr<ShaderManager> mInstance;
   };
}

#endif

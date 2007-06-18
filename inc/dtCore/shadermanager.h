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
#ifndef _SHADERMANAGER_H
#define _SHADERMANAGER_H

#include <dtCore/refptr.h>
#include <dtCore/base.h>
#include <dtUtil/exception.h>
#include <dtCore/shadergroup.h>
#include <dtCore/export.h>

#include <dtCore/observerptr.h>
#include <osg/Node>

#include <map>
#include <vector>
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

         /**
          * This is a simple structure that holds a connection between an actively 
          * shaded node and it's shader instance.  These are created when you assign a 
          * shader to a node.  It has a weak reference to the node.
          */
         struct ActiveNodeEntry
         {
            dtCore::ObserverPtr<osg::Node> nodeWeakReference;
            dtCore::RefPtr<dtCore::Shader> shaderInstance;
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
          * Adds a shader group template to the manager's list of known shader group templates.  
          * Each template shader in the shader group is assigned the resulting compiled and 
          * linked shader program.  Templates are cloned when you call AssignShaderFromTemplate()
          * @param shaderGroup The new shader group template to add.
          * @note An exception is thrown if the specified shader group does not have a unique
          *    name.
          */
         void AddShaderGroupTemplate(ShaderGroup &shaderGroup);

         /**
          * Removes the shader group template with the specified name from the manager.
          * @param name The name of the shader group template to remove.  If the shader group 
          *    template cannot be found in the manager's list this method is a no-op.
          */
         void RemoveShaderGroupTemplate(const std::string &name);

         /**
          * Removes the specified shader group template from the manager.
          * @param shaderGroup The shader group template to remove.  If this shader group is not 
          *    currently being managed, this method is a no-op.
          */
         void RemoveShaderGroupTemplate(const ShaderGroup &shaderGroup);

         /**
          * Searches the shader manager for the specified shader group template. The returned value
          * is a template for a shader group. You can look for a specific shader template and then 
          * assign it to your node using AssignShaderFromTemplate().
          * @param name Name of the shader group template to find.
          * @return A const pointer to the shader group template or NULL if it could not be found.
          */
         const ShaderGroup *FindShaderGroupTemplate(const std::string &name) const;

         /**
          * Searches the shader manager for the specified shader group template. The returned value
          * is a template for a shader group. You can look for a specific shader template and then 
          * assign it to your node using AssignShaderFromTemplate().  
          * @Note This version returns a non-const pointer which will let you modify the group. 
          *    Changes will only affect the template itself, not any cloned instances. 
          * @param name Name of the shader group template to find.
          * @return A pointer to the shader group template or NULL if it could not be found.
          */
         ShaderGroup *FindShaderGroupTemplate(const std::string &name);

         /**
          * Fills the specified vector with all the shader group templates currently in the
          * shader manager. 
          * @param toFill The vector with which to fill with shader groups.  Note, the
          *   vector is cleared before it is filled.
          */
         void GetAllShaderGroupTemplates(std::vector<dtCore::RefPtr<ShaderGroup> > &toFill);

         /**
          * Gets the specified shader template from the manager.
          * @param name The name of the shader template to find.
          * @param groupName The group containing the shader.  If this name is empty,
          *    the shader manager will traverse all the shader groups and return the
          *    first shader matching the specified shader name.
          * @return A const pointer to the shader or NULL if the shader could not be found.
          * @note For performance reasons, this method should be called with a shader
          *    group specified.
          */
         const Shader *FindShaderTemplate(const std::string &name, const std::string &groupName="") const;

         /**
          * Gets the specified shader template from the manager. Changing the returned template will 
          * have no effect on nodes that were assigned to this shader with AssignShaderFromTemplate(). 
          * To modify the parameters of a node's shader instance, call GetShaderInstanceForNode().
          * @param name The name of the shader template to find.
          * @param groupName The group containing the shader.  If this name is empty,
          *    the shader manager will traverse all the shader groups and return the
          *    first shader matching the specified shader name.
          * @return A const pointer to the shader or NULL if the shader could not be found.
          * @note For performance reasons, this method should be called with a shader
          *    group specified.
          */
         Shader *FindShaderTemplate(const std::string &name, const std::string &groupName="");

         /**
          * Clones a new instance of the passed in shader template and its parameters. It then assigns 
          * the shader's associated parameters to the specified node. It returns the new shader instance.
          * If you plan to modify parameter values on the shader, you should hold onto it. You can find 
          * this shader later by calling GetShaderInstanceForNode().
          * @param name The shader template to clone.
          * @param node The node that will get the new shader instance.
          * @throws An exception is thrown if the shader does not have a valid
          *    vertex or fragment program bound to it.
          * @return the unique instance of this shader for this node.  If you plan
          * to modify any parameters of the shader, then you should hold onto this.
          */
         dtCore::Shader *AssignShaderFromTemplate(const dtCore::Shader &shader, osg::Node &node);

         /**
          * Use this if you no longer want the shader assigned to the node. It will attempt to
          * put the stateset back to the way it was before.  Note, this method does not guarantee
          * that the stateset will be completely returned to its original state. This will also 
          * remove the node from the active node list. You can only use this method if you have
          * previously called AssignShaderFromTemplate(). If the node is unassigned, it is a NO-OP.
          * @param node The node you previously called AssignShaderFromTemplate() on.
          */
         void UnassignShaderFromNode(osg::Node &node); 

         /**
          * Gets the number of shader templates currently managed by the shader manager.  This 
          * number reflects the total number of shader templates contained in all the shader 
          * group templates currently in the shader manager.
          * @return The shader count.
          */
         unsigned int GetNumShaderTemplates() const { return mTotalShaderCount; }

         /**
          * Gets the number of shader group templates currently owned by the shader manager.
          * @return The number of shader group templates contained in the shader manager.
          */
         unsigned int GetNumShaderGroupTemplates() const { return mShaderGroups.size(); }

         /**
          * Gets the size of the shader program cache.  This cache contains compiled shaders
          * that are shared within the shader manager.
          * @return The number of shader programs currently cached by the manager.
          */
         unsigned int GetShaderCacheSize() const { return mShaderProgramCache.size(); }

         /**
          * Loads a list of shader templates defined in an external XML file.
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

         /**
          * Attempts to find an active shader instance for this node. Active shaders are 
          * created by calling AssignShaderFromTemplate() for a node. The returned shader
          * is a unique instance for this node. To change the behavior of your shader, simply access
          * the parameters of your instance and set their value directly.
          * @param node The node that was previously used with AssignShaderFromTemplate()
          * @return The unique shader instance for this node. NULL if none found for this node.
          */
         dtCore::Shader *GetShaderInstanceForNode(osg::Node *node);

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
          * Internal helper method which searches for a shader template in the shader manager.
          * This is used by the const and non-const versions of FindShaderTeemplate in the
          * public interface.
          * @param shaderName Name of the shader to search for.
          * @param groupName Name of the group containing the shader.  If this is empty
          *    the first shader with the specified name is returned.
          */
         const Shader *InternalFindShader(const std::string &shaderName, const std::string &groupName) const;

         /**
          * Finds and removes any entries in the active shader list that are assigned to this node. 
          * @param node The node we are looking for.
          */
         void RemoveShaderFromActiveNodeList(osg::Node *node);

      private:

         ///Count of the total number of shaders in the shader manager.
         unsigned int mTotalShaderCount;

         ///List of the shader groups currently loaded by the manager.
         std::map<std::string,dtCore::RefPtr<ShaderGroup> > mShaderGroups;

         ///Shader program cache which stores compiled shader programs that may
         ///be shared amoungst the loaded shaders.
         std::map<std::string,ShaderCacheEntry> mShaderProgramCache;

         // list of all the actively assigned nodes.  Each active entry has a ref to its instance 
         // of the shader as well as a weak reference to the node itself.  
         std::vector<ActiveNodeEntry> mActiveNodeList;

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

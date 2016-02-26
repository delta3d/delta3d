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

#include <dtUtil/hashmap.h>
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
   class DeltaDrawable;

   class ShaderSourceException : public dtUtil::Exception
   {
   public:
   	ShaderSourceException(const std::string& message, const std::string& filename, unsigned int linenum);
      virtual ~ShaderSourceException() {};
   };

   class DuplicateShaderGroupException : public dtUtil::Exception
   {
   public:
      DuplicateShaderGroupException(const std::string& message, const std::string& filename, unsigned int linenum);
      virtual ~DuplicateShaderGroupException() {};
   };

   class ShaderXmlParserException : public dtUtil::Exception
   {
   public:
      ShaderXmlParserException(const std::string& message, const std::string& filename, unsigned int linenum);
      virtual ~ShaderXmlParserException() {};
   };

   class DuplicateShaderParameterException : public dtUtil::Exception
   {
   public:
      DuplicateShaderParameterException(const std::string& message, const std::string& filename, unsigned int linenum);
      virtual ~DuplicateShaderParameterException() {};
   };

   /**
    * This class manages the currently loaded and/or active shaders in the
    * current game.  It provides a central location for storing the shaders
    * thus allowing them to be shared amongst many different actors or entities.
    * The shader manager can also parse shader definition files which are xml
    * files consisting of a list of shader definitions which can then be referenced
    * through the manager.
    * @note
    *    This class is a singleton class.
    * @note The ShaderManager actually stores ShaderGroups which are collections of
    *    related shaders.  When shader groups are added to the ShaderManager, the
    *    manager the specified shader source code, links it, and generates shader programs.
    *    If two or more shaders are added and their resulting compiled shader program
    *    is the same, then that program is shared amongst each shader containing the
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
            dtCore::RefPtr<dtCore::ShaderProgram> shaderInstance;
         };

      public:

         /**
          * Gets the single global instance of this class.
          * @return The singleton instance.
          */
         static ShaderManager& GetInstance();

         /** 
          * Destroy the singleton instance of ShaderManager, if one was created.
          * Should only be called when the application is finished with it.
          */
         static void Destroy();

         /**
          * Removes all shaders and shader groups from the manager.
          * @note This does not remove the shaders from the states with which they were bound.
          *    This method is useful to call when changing maps or something of that nature.
          */
         void Clear();

         /**
          * Adds a shader group prototype to the manager's list of known shader group prototypes.  
          * Each prototype shader in the shader group is assigned the resulting compiled and 
          * linked shader program.  prototypes are cloned when you call AssignShaderFromPrototype()
          * @param shaderGroup The new shader group prototype to add.
          * @note An exception is thrown if the specified shader group does not have a unique
          *    name.
          * @throw DuplicateShaderGroupException
          */
         void AddShaderGroupPrototype(ShaderGroup& shaderGroup);

         /**
          * Removes the shader group prototype with the specified name from the manager.
          * @param name The name of the shader group prototype to remove.  If the shader group 
          *    prototype cannot be found in the manager's list this method is a no-op.
          */
         void RemoveShaderGroupPrototype(const std::string& name);

         /**
          * Removes the specified shader group prototype from the manager.
          * @param shaderGroup The shader group prototype to remove.  If this shader group is not 
          *    currently being managed, this method is a no-op.
          */
         void RemoveShaderGroupPrototype(const ShaderGroup& shaderGroup);

         /**
          * Searches the shader manager for the specified shader group prototype. The returned value
          * is a prototype for a shader group. You can look for a specific shader prototype and then 
          * assign it to your node using AssignShaderFromPrototype().
          * @param name Name of the shader group prototype to find.
          * @return A const pointer to the shader group prototype or NULL if it could not be found.
          */
         const ShaderGroup* FindShaderGroupPrototype(const std::string& name) const;

         /**
          * Searches the shader manager for the specified shader group prototype. The returned value
          * is a prototype for a shader group. You can look for a specific shader prototype and then 
          * assign it to your node using AssignShaderFromPrototype().  
          * @note This version returns a non-const pointer which will let you modify the group. 
          *    Changes will only affect the prototype itself, not any cloned instances. 
          * @param name Name of the shader group prototype to find.
          * @return A pointer to the shader group prototype or NULL if it could not be found.
          */
         ShaderGroup* FindShaderGroupPrototype(const std::string& name);

         /**
          * Fills the specified vector with all the shader group prototypes currently in the
          * shader manager. 
          * @param toFill The vector with which to fill with shader groups.  Note, the
          *   vector is cleared before it is filled.
          */
         void GetAllShaderGroupPrototypes(std::vector<dtCore::RefPtr<ShaderGroup> >& toFill);

         /**
          * Gets the specified shader prototype from the manager.
          * @param name The name of the shader prototype to find.
          * @param groupName The group containing the shader.  If this name is empty,
          *    the shader manager will traverse all the shader groups and return the
          *    first shader matching the specified shader name.
          * @return A const pointer to the shader or NULL if the shader could not be found.
          * @note For performance reasons, this method should be called with a shader
          *    group specified.
          */
         const ShaderProgram* FindShaderPrototype(const std::string& name, const std::string& groupName="") const;

         /**
          * Gets the specified shader prototype from the manager. Changing the returned prototype will 
          * have no effect on nodes that were assigned to this shader with AssignShaderFromPrototype(). 
          * To modify the parameters of a node's shader instance, call GetShaderInstanceForNode().
          * @param name The name of the shader prototype to find.
          * @param groupName The group containing the shader.  If this name is empty,
          *    the shader manager will traverse all the shader groups and return the
          *    first shader matching the specified shader name.
          * @return A const pointer to the shader or NULL if the shader could not be found.
          * @note For performance reasons, this method should be called with a shader
          *    group specified.
          */
         ShaderProgram* FindShaderPrototype(const std::string& name, const std::string& groupName="");

         /**
          * Clones a new instance of the passed in shader prototype and its parameters. It then assigns 
          * the shader's associated parameters to the specified node. It returns the new shader instance.
          * If you plan to modify parameter values on the shader, you should hold onto it. You can find 
          * this shader later by calling GetShaderInstanceForNode().
          * @param name The shader prototype to clone.
          * @param node The node that will get the new shader instance.
          * @throws An exception is thrown if the shader does not have a valid
          *    vertex or fragment program bound to it.
          * @return the unique instance of this shader for this node.  If you plan
          * to modify any parameters of the shader, then you should hold onto this.
          */
         dtCore::ShaderProgram* AssignShaderFromPrototype(const dtCore::ShaderProgram& shader, osg::Node& node);

         /**
          * Use this if you no longer want the shader assigned to the node. It will attempt to
          * put the stateset back to the way it was before.  Note, this method does not guarantee
          * that the stateset will be completely returned to its original state. This will also 
          * remove the node from the active node list. You can only use this method if you have
          * previously called AssignShaderFromPrototype(). If the node is unassigned, it is a NO-OP.
          * @param node The node you previously called AssignShaderFromPrototype() on.
          */
         void UnassignShaderFromNode(osg::Node& node);

         /** 
          * Remove a previously assigned ShaderProgram from the supplied DeltaDrawable.
          * @param drawable A DeltaDrawable that has an existing ShaderProgram applied
          */
         void UnassignShader(dtCore::DeltaDrawable& drawable);

         /**
          * Gets the number of shader prototypes currently managed by the shader manager.  This 
          * number reflects the total number of shader prototypes contained in all the shader 
          * group prototypes currently in the shader manager.
          * @return The shader count.
          */
         unsigned int GetNumShaderPrototypes() const { return mTotalShaderCount; }

         /**
          * Gets the number of shader group prototypes currently owned by the shader manager.
          * @return The number of shader group prototypes contained in the shader manager.
          */
         unsigned int GetNumShaderGroupPrototypes() const { return mShaderGroups.size(); }

         /**
          * Gets the size of the shader program cache.  This cache contains compiled shaders
          * that are shared within the shader manager.
          * @return The number of shader programs currently cached by the manager.
          */
         unsigned int GetShaderCacheSize() const { return mShaderProgramCache.size(); }

         /**
          * Loads a list of shader prototypes defined in an external XML file.
          * @param fileName The XML file containing the shader definitions.
          * @param merge If false, this method will first clear the current list of shaders
          *    in the manager.  If true, the shaders loaded from the shader file will
          *    be added to the list of shaders already in the manager. (true by default).
          * @throw ShaderXmlParserException if the file is not parsed correctly.
          */
         void LoadShaderDefinitions(const std::string& fileName, bool merge = true);

         /** 
          * Loads the supplied shader file and applies the first ShaderProgram in 
          * the first ShaderGroup to the supplied DeltaDrawable.
          * @param drawable The DeltaDrawable to apply the ShaderProgram to
          * @param shaderResource The shader definition file to load. Uses similar 
          * format as the file supplied to LoadShaderDefinitions(), but will only
          * use the first ShaderProgram of the first ShaderGroup found.
          */
         void LoadAndAssignShader(dtCore::DeltaDrawable& drawable,
                                  const std::string& shaderResource);

         /**
          * This method is kind of weird and should be used ONLY for testing. It 
          * unassigns ALL currently assigned nodes, clears out all prototypes, 
          * and then reloads the passed in XML definition file. It then, tries to 
          * remap all of the previously assigned nodes using the new definitions. 
          * The primary use of this method is to allow an artist to reload all the 
          * shader definitions in the system to test out different effects.
          * THIS IS INTENDED FOR TESTING PURPOSES. AN END USER SHOULD NEVER ACCESS 
          * THIS METHOD DURING REAL GAME PLAY. 
          * @param fileName (NO LONGER USED) The XML file containing the shader definitions. Merge is not supported
          */
         DEPRECATE_FUNC void ReloadAndReassignShaderDefinitions(const std::string& fileName);

         /**
         * This method should be used ONLY for testing and tweaking shaders interatively.
         * It unassigns ALL currently assigned nodes, clears out all prototypes,
         * and then reloads all previously loaded shader definition files. It then, tries
         * to remap all of the previously assigned nodes using the newly loaded definitions.
         *
         * THIS IS INTENDED FOR TESTING PURPOSES. AN END USER SHOULD NEVER ACCESS
         * THIS METHOD DURING REAL GAME PLAY.
         */
         void ReloadAndReassignShaderDefinitions();

         /**
          * Called when system sends out update data.
          */
         void OnSystem(const dtUtil::RefString& str, double deltaSim, double deltaReal)
;

         /**
          * Attempts to find an active shader instance for this node. Active shaders are 
          * created by calling AssignShaderFromPrototype() for a node. The returned shader
          * is a unique instance for this node. To change the behavior of your shader, simply access
          * the parameters of your instance and set their value directly.
          * @param node The node that was previously used with AssignShaderFromPrototype()
          * @return The unique shader instance for this node. NULL if none found for this node.
          */
         dtCore::ShaderProgram* GetShaderInstanceForNode(const osg::Node* node);

         /**
          * Attempts to find an active shader instance for the supplied DeltaDrawable. Active shaders are 
          * created by calling AssignShaderFromPrototype(). The returned shader
          * is a unique instance for this DeltaDrawable. To change the behavior of your shader, simply access
          * the parameters of your instance and set their value directly.
          * @param drawable The DeltaDrawable that was previously used with AssignShaderFromPrototype()
          * @return The unique shader instance for this DeltaDrawable. NULL if none found.
          */
         dtCore::ShaderProgram* GetShaderInstanceForDrawable(const dtCore::DeltaDrawable& drawable);

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
          * @throw ShaderSourceException
          */
         void ResolveShaderPrograms(ShaderProgram& shader, const std::string& groupName);

         /**
          * Internal helper method which searches for a shader prototype in the shader manager.
          * This is used by the const and non-const versions of FindShaderTeemplate in the
          * public interface.
          * @param shaderName Name of the shader to search for.
          * @param groupName Name of the group containing the shader.  If this is empty
          *    the first shader with the specified name is returned.
          */
         const ShaderProgram* InternalFindShader(const std::string& shaderName, const std::string& groupName) const;

         /**
          * Finds and removes any entries in the active shader list that are assigned to this node. 
          * @param node The node we are looking for.
          */
         void RemoveShaderFromActiveNodeList(osg::Node* node);

      private:

         ///Count of the total number of shaders in the shader manager.
         unsigned int mTotalShaderCount;

         typedef dtUtil::HashMap<std::string, dtCore::RefPtr<ShaderGroup> > ShaderGroupListType;
         ///List of the shader groups currently loaded by the manager.
         ShaderGroupListType mShaderGroups;

         typedef dtUtil::HashMap<std::string, ShaderCacheEntry> ShaderProgramListType;
         ///Shader program cache which stores compiled shader programs that may
         ///be shared amongst the loaded shaders.
         ShaderProgramListType mShaderProgramCache;

         typedef std::set<std::string> ShaderResourceSet;
         /// Keep track of all loaded shader resources.
         ShaderResourceSet mShaderResources;

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

         ShaderManager(const ShaderManager&);
         ShaderManager &operator=(const ShaderManager&);

         ///Single instance of this class.
         static dtCore::RefPtr<ShaderManager> mInstance;
   };
}

#endif

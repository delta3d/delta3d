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
#include <prefix/dtcoreprefix-src.h>
#include <dtCore/shadermanager.h>
#include <dtCore/shaderxml.h>
#include <dtCore/shaderparameter.h>

#include <osg/Texture2D>
#include <osg/Node>

#include <dtCore/globals.h>
#include <dtCore/system.h>

namespace dtCore
{
   ///////////////////////////////////////////////////////////////////////////////
   dtCore::RefPtr<ShaderManager> ShaderManager::mInstance(NULL);

   ///////////////////////////////////////////////////////////////////////////////
   IMPLEMENT_ENUM(ShaderException)
   ShaderException ShaderException::SHADER_SOURCE_ERROR("SHADER_SOURCE_ERROR");
   ShaderException ShaderException::DUPLICATE_SHADERGROUP_FOUND("DUPLICATE_SHADERGROUP_FOUND");
   ShaderException ShaderException::XML_PARSER_ERROR("XML_PARSER_ERROR");
   ShaderException ShaderException::DUPLICATE_SHADER_PARAMETER_FOUND("DUPLICATE_SHADER_PARAMETER_FOUND");


   ///////////////////////////////////////////////////////////////////////////////
   ShaderManager::ShaderManager() : dtCore::Base("ShaderManager")
   {
      Clear();
      AddSender(&dtCore::System::GetInstance());
   }

   ///////////////////////////////////////////////////////////////////////////////
   ShaderManager::~ShaderManager()
   {
   }

   ///////////////////////////////////////////////////////////////////////////////
   void ShaderManager::Clear()
   {
      std::vector<dtCore::RefPtr<ShaderParameter> > params;
      std::vector<dtCore::RefPtr<ShaderParameter> >::iterator currParam;

      // Loop through our active nodes and clear currently preassigned shaders. 
      for (int i = mActiveNodeList.size() - 1; i >= 0; i--)
      {
         if (mActiveNodeList[i].nodeWeakReference.valid())
         {
            osg::Node *node = mActiveNodeList[i].nodeWeakReference.get();
            dtCore::RefPtr<osg::StateSet> stateSet = node->getOrCreateStateSet();

            // clean up the parameters effects to the stateset
            mActiveNodeList[i].shaderInstance->GetParameterList(params);
            for (currParam=params.begin(); currParam!=params.end(); ++currParam)
               (*currParam)->DetachFromRenderState(*stateSet);

            // remove the program
            stateSet->setAttributeAndModes(new osg::Program(), osg::StateAttribute::ON);
         }
      }

      mShaderGroups.clear();
      mShaderProgramCache.clear();
      mTotalShaderCount = 0;
      mActiveNodeList.clear();
   }

   ///////////////////////////////////////////////////////////////////////////////
   void ShaderManager::OnMessage(dtCore::Base::MessageData *msgData)
   {
      if (msgData->message == "preframe")
      {
         double *timeData = (double *)msgData->userData;
         OnPreFrame(timeData[0],timeData[1]);
      }
   }

   ///////////////////////////////////////////////////////////////////////////////
   void ShaderManager::OnPreFrame(double deltaRealTime, double deltaSimTime)
   {
      for (int i = mActiveNodeList.size() - 1; i >= 0; i--)
      {
         // if weak reference observer is still valid, then update our shader instance.
         if (mActiveNodeList[i].nodeWeakReference.valid())
         {
            if (mActiveNodeList[i].shaderInstance->IsDirty())
               mActiveNodeList[i].shaderInstance->Update();
         }
         else 
         {
            // If the weak reference is NULL, then remove the item from our active list.
            mActiveNodeList.erase(mActiveNodeList.begin() + i);
         }
      }
   }

   ///////////////////////////////////////////////////////////////////////////////
   void ShaderManager::AddShaderGroupPrototype(ShaderGroup &shaderGroup)
   {
      std::map<std::string,dtCore::RefPtr<ShaderGroup> >::iterator itor =
         mShaderGroups.find(shaderGroup.GetName());

      //Do not allow shader groups with the same name...
      if (itor != mShaderGroups.end())
         throw dtUtil::Exception(ShaderException::DUPLICATE_SHADERGROUP_FOUND, 
         "Shader groups must have unique names.  The conflicting name is \"" + 
         shaderGroup.GetName() + "\".", __FILE__, __LINE__);

      //Before we insert the group, we need to check our program cache and update it
      //if necessary for each shader in the group.  Also update our total shader count.
      std::vector<dtCore::RefPtr<ShaderProgram> > shaderList;
      std::vector<dtCore::RefPtr<ShaderProgram> >::iterator shaderItor;

      shaderGroup.GetAllShaders(shaderList);
      for (shaderItor=shaderList.begin(); shaderItor!=shaderList.end(); ++shaderItor)
      {
         ResolveShaderPrograms(*(shaderItor->get()));
         mTotalShaderCount++;
      }

      mShaderGroups.insert(std::make_pair(shaderGroup.GetName(),&shaderGroup));
   }

   ///////////////////////////////////////////////////////////////////////////////
   void ShaderManager::RemoveShaderGroupPrototype(const std::string &name)
   {
      std::map<std::string,dtCore::RefPtr<ShaderGroup> >::iterator itor =
         mShaderGroups.find(name);

      mTotalShaderCount -= itor->second->GetNumShaders();
      mShaderGroups.erase(itor);
   }

   ///////////////////////////////////////////////////////////////////////////////
   void ShaderManager::RemoveShaderGroupPrototype(const ShaderGroup &shaderGroup)
   {
      RemoveShaderGroupPrototype(shaderGroup.GetName());
   }

   ///////////////////////////////////////////////////////////////////////////////
   const ShaderGroup *ShaderManager::FindShaderGroupPrototype(const std::string &name) const
   {
      std::map<std::string,dtCore::RefPtr<ShaderGroup> >::const_iterator itor =
            mShaderGroups.find(name);

      if (itor != mShaderGroups.end())
         return itor->second.get();
      else
         return NULL;
   }

   ///////////////////////////////////////////////////////////////////////////////
   ShaderGroup *ShaderManager::FindShaderGroupPrototype(const std::string &name)
   {
      std::map<std::string,dtCore::RefPtr<ShaderGroup> >::iterator itor =
            mShaderGroups.find(name);

      if (itor != mShaderGroups.end())
         return itor->second.get();
      else
         return NULL;
   }

   ///////////////////////////////////////////////////////////////////////////////
   void ShaderManager::GetAllShaderGroupPrototypes(std::vector<dtCore::RefPtr<ShaderGroup> > &toFill)
   {
      std::map<std::string,dtCore::RefPtr<ShaderGroup> >::iterator itor;

      toFill.clear();
      toFill.reserve(mShaderGroups.size());
      for (itor=mShaderGroups.begin(); itor!=mShaderGroups.end(); ++itor)
         toFill.push_back(itor->second);
   }

   ///////////////////////////////////////////////////////////////////////////////
   const ShaderProgram *ShaderManager::FindShaderPrototype(const std::string &name, const std::string &groupName) const
   {
      return InternalFindShader(name,groupName);
   }

   ///////////////////////////////////////////////////////////////////////////////
   ShaderProgram *ShaderManager::FindShaderPrototype(const std::string &name, const std::string &groupName)
   {
      return const_cast<ShaderProgram *>(InternalFindShader(name,groupName));
   }

   ///////////////////////////////////////////////////////////////////////////////
   const ShaderProgram *ShaderManager::InternalFindShader(const std::string &shaderName,
         const std::string &groupName) const
   {
      std::map<std::string,dtCore::RefPtr<ShaderGroup> >::const_iterator itor;

      if (!groupName.empty())
      {
         itor = mShaderGroups.find(groupName);
         if (itor == mShaderGroups.end())
         {
            LOG_WARNING("Shader group: " + groupName + " is not currently loaded in the shader manager.");
            return NULL;
         }
         else
         {
            return itor->second->FindShader(shaderName);
         }
      }
      else
      {
         for (itor=mShaderGroups.begin(); itor!=mShaderGroups.end(); ++itor)
         {
            const ShaderProgram *shader = itor->second->FindShader(shaderName);
            if (shader != NULL)
               return shader;
         }
      }

      return NULL;
   }

   ///////////////////////////////////////////////////////////////////////////////
   // Curt - Add a method to get an active shader for a given node
   dtCore::ShaderProgram *ShaderManager::GetShaderInstanceForNode(osg::Node *node)
   {
      // Try to find the node in the active node list.
      for (int i = mActiveNodeList.size() - 1; i >= 0 && node != NULL; i--)
      {
         if (mActiveNodeList[i].nodeWeakReference.valid() && 
            mActiveNodeList[i].nodeWeakReference.get() == node)
         {
            return mActiveNodeList[i].shaderInstance.get();
         }
      }

      return NULL;
   }


   ///////////////////////////////////////////////////////////////////////////////
   void ShaderManager::RemoveShaderFromActiveNodeList(osg::Node *node)
   {
      // find any instances of weak references to this node and remove it from the active list.
      for (int i = mActiveNodeList.size() - 1; i >= 0 && node != NULL; i--)
      {
         if (mActiveNodeList[i].nodeWeakReference.valid() && 
            mActiveNodeList[i].nodeWeakReference.get() == node)
         {
            mActiveNodeList.erase(mActiveNodeList.begin() + i);
         }
      }
   }


   ///////////////////////////////////////////////////////////////////////////////
   void ShaderManager::UnassignShaderFromNode(osg::Node &node)
   {
      std::vector<dtCore::RefPtr<ShaderParameter> > params;
      std::vector<dtCore::RefPtr<ShaderParameter> >::iterator currParam;
      dtCore::RefPtr<osg::StateSet> stateSet = node.getOrCreateStateSet();

      // find any instances of weak references to this node and remove it from the active list.
      for (int i = mActiveNodeList.size() - 1; i >= 0; i--)
      {
         if (mActiveNodeList[i].nodeWeakReference.valid() && 
            mActiveNodeList[i].nodeWeakReference.get() == &node)
         {
            // clean up the parameters effects to the stateset
            mActiveNodeList[i].shaderInstance->GetParameterList(params);
            for (currParam=params.begin(); currParam!=params.end(); ++currParam)
               (*currParam)->DetachFromRenderState(*stateSet);

            // remove the program
            stateSet->setAttributeAndModes(new osg::Program(), osg::StateAttribute::ON);
         }
      }

      // Remove all references to this node.  Call the method for safety to eliminate any possible 
      // chance that there is more than one (not sure how that could happen). .
      RemoveShaderFromActiveNodeList(&node);
   }


   ///////////////////////////////////////////////////////////////////////////////
   dtCore::ShaderProgram *ShaderManager::AssignShaderFromPrototype(const dtCore::ShaderProgram &templateShader, osg::Node &node)
   {
      // If this node is already assigned to a shader, remove it from our active list. 
      RemoveShaderFromActiveNodeList(&node);

      // create a duplicate of the shader prototype.  The group and shaders that you use to find
      // are simply prototypes that we use to create unique instances for each node. 
      dtCore::RefPtr<dtCore::ShaderProgram> newShader = templateShader.Clone();

      std::vector<dtCore::RefPtr<ShaderParameter> > params;
      std::vector<dtCore::RefPtr<ShaderParameter> >::iterator currParam;
      dtCore::RefPtr<osg::StateSet> stateSet = node.getOrCreateStateSet();

      //First assign the vertex,fragment,and shader program to the render state.
      //if (newShader->GetVertexShader() == NULL)
      //   LOG_WARNING("Error assigning shader: " + newShader->GetName() + "  Vertex shader was invalid.");
      //if (newShader->GetFragmentShader() == NULL)
      //   LOG_WARNING("Error assigning shader: " + newShader->GetName() + "  Fragment shader was invalid.");
      if (newShader->GetShaderProgram() == NULL)
         LOG_WARNING("Error assigning shader: " + newShader->GetName() + "  Shader program was invalid.");

      //I realize const-cast is not a great idea here, but I did not want the have a non-const version
      //of the GetShaderProgram() method on the shader class.
      stateSet->setAttributeAndModes(const_cast<osg::Program*>(newShader->GetShaderProgram()),
         osg::StateAttribute::ON);

      //Now add all the shader's parameters to the render state.  Each class of shader parameter
      //is responcible for knowning how to attach itself to the render state.
      newShader->GetParameterList(params);
      for (currParam=params.begin(); currParam!=params.end(); ++currParam)
         (*currParam)->AttachToRenderState(*stateSet);

      // add the new shader and node to the active node list.
      ActiveNodeEntry activeNode;
      activeNode.shaderInstance = newShader.get();
      activeNode.nodeWeakReference = &node;
      mActiveNodeList.push_back(activeNode);

      return newShader.get();
   }

   ///////////////////////////////////////////////////////////////////////////////
   void ShaderManager::ResolveShaderPrograms(ShaderProgram &shader)
   {
      //Shader cache entries are keyed by a combination of the source to the
      //vertex shader and the source to the fragment shader.
      
      std::string cacheKey = shader.GetVertexCacheKey() + ":" + shader.GetFragmentCacheKey();

      std::map<std::string,ShaderCacheEntry>::iterator itor =
         mShaderProgramCache.find(cacheKey);

      //If we found the cache entry, the the shader uses the same programs,
      //but may have different parameter bindings.  Therefore, share the
      //compiled programs.  If not, we need to load the programs and insert
      //them in the cache.
      if (itor != mShaderProgramCache.end())
      {
         shader.SetGLSLProgram(*(itor->second.shaderProgram)); // program is required
         return;
      }

      //Load, compile, and link the shaders...
      std::string path;
      dtCore::RefPtr<osg::Shader> vertexShader;
      dtCore::RefPtr<osg::Shader> fragmentShader;
      dtCore::RefPtr<osg::Program> program = new osg::Program();

      std::vector<std::string>::const_iterator vertexShaderIterator = shader.GetVertexShaders().begin();

      while(vertexShaderIterator != shader.GetVertexShaders().end())
      {
         // Load and set the vertex shader - note, this is not required
         //path = dtCore::FindFileInPathList(shader.GetVertexShaders());
         path = dtCore::FindFileInPathList(*vertexShaderIterator);
         if (!path.empty()) 
         {
            vertexShader = new osg::Shader(osg::Shader::VERTEX);
            if (!vertexShader->loadShaderSourceFromFile(path))
               throw dtUtil::Exception(ShaderException::SHADER_SOURCE_ERROR,"Error loading vertex shader file: " +
                  *vertexShaderIterator + " from shader: " + shader.GetName(), __FILE__, __LINE__);
            program->addShader(vertexShader.get());
         }
         vertexShaderIterator++;
      }

      std::vector<std::string>::const_iterator fragmentShaderIterator = shader.GetFragmentShaders().begin();

      while(fragmentShaderIterator != shader.GetFragmentShaders().end())
      {
         // Load and set the fragment shader - note, this is not required
         //path = dtCore::FindFileInPathList(shader.GetFragmentShaders());
         path = dtCore::FindFileInPathList(*fragmentShaderIterator);
         if (!path.empty()) 
         {
            fragmentShader = new osg::Shader(osg::Shader::FRAGMENT);
            if (path.empty() || !fragmentShader->loadShaderSourceFromFile(path))
               throw dtUtil::Exception(ShaderException::SHADER_SOURCE_ERROR,"Error loading fragment shader file: " +
                  *fragmentShaderIterator + " from shader: " + shader.GetName(), __FILE__, __LINE__);
            program->addShader(fragmentShader.get());
         }
         fragmentShaderIterator++;
      }

      shader.SetGLSLProgram(*program);

      //Put a new entry in the cache...
      ShaderCacheEntry newCacheEntry;
      newCacheEntry.vertexShader = vertexShader; // may be null
      newCacheEntry.fragmentShader = fragmentShader; // may be null
      newCacheEntry.shaderProgram = program;
      mShaderProgramCache.insert(std::make_pair(cacheKey,newCacheEntry));
   }

   ///////////////////////////////////////////////////////////////////////////////
   void ShaderManager::ReloadAndReassignShaderDefinitions(const std::string &fileName)
   {
      LOG_WARNING("Attempting to reload ALL Shaders using file[" + fileName + "]. This is a test behavior and may result in artifacts or changes in the scene.");

      std::vector<ActiveNodeEntry> mCopiedNodeList;
      std::map<std::string,dtCore::RefPtr<ShaderGroup> >::const_iterator groupItor;

      // Loop through our active nodes and make a copy of each one. 
      for (int i = mActiveNodeList.size() - 1; i >= 0; i--)
      {
         ActiveNodeEntry activeNode;
         activeNode.shaderInstance = mActiveNodeList[i].shaderInstance;
         activeNode.nodeWeakReference = mActiveNodeList[i].nodeWeakReference;
         mCopiedNodeList.push_back(activeNode);
      }

      // Now, clear everything - all prototypes, all assignments, everything!!!
      Clear();

      // Reload our file
      LoadShaderDefinitions(fileName, false);

      // Now, the tricky part. Loop through all our previous nodes and try to 
      // find a match to the new shader. If we find one, reassign it. 
      for (int i = mCopiedNodeList.size() - 1; i >= 0; i--)
      {
         bool bFoundMatch = false;
         std::string oldCacheKey = mCopiedNodeList[i].shaderInstance->GetVertexCacheKey() + ":" +
            mCopiedNodeList[i].shaderInstance->GetFragmentCacheKey();

         ShaderProgram *matchingShader = NULL;

         // Loop to find a match
         for (groupItor=mShaderGroups.begin(); !bFoundMatch && groupItor!=mShaderGroups.end(); ++groupItor)
         {
            matchingShader = groupItor->second->FindShader(mCopiedNodeList[i].shaderInstance->GetName());
            if (matchingShader != NULL)
            {
               // build cache keys for loop one and pre-existing shader
               std::string foundCacheKey = matchingShader->GetVertexCacheKey() + ":" +
                  matchingShader->GetFragmentCacheKey();

               // Is it a perfect match??
               if (foundCacheKey == oldCacheKey)
                  bFoundMatch = true;
            }
         }

         // if we got a match, we rock!!!
         if (bFoundMatch)
         {
            //LOG_ERROR("FOUND A MATCH FOR shader[" + mCopiedNodeList[i].shaderInstance->GetName() +  "]!!!");
            AssignShaderFromPrototype(*matchingShader, *mCopiedNodeList[i].nodeWeakReference.get());
         }
         else
         {
            LOG_ERROR("Error reloading shader[" + mCopiedNodeList[i].shaderInstance->GetName() + 
               "]. No exact match was found in the cache for the full key[" + oldCacheKey + 
               "]. Cannot reapply shader so this node will have no shader!");
         }
      }

      // Clear our our copy list since we're done. Not required, but safe.
      mCopiedNodeList.clear();
   }


   ///////////////////////////////////////////////////////////////////////////////
   void ShaderManager::LoadShaderDefinitions(const std::string &fileName, bool merge)
   {
      ShaderXML parser;
      std::string path = dtCore::FindFileInPathList(fileName);
      if (path.empty())
      {
         LOG_WARNING("Could not find shader definitions file: " + fileName);
         return;
      }

      if (!merge)
         Clear();
      
      try
      {
         parser.ParseXML(path);
      }
      catch (const dtUtil::Exception &e)
      {
         throw dtUtil::Exception(ShaderException::XML_PARSER_ERROR,e.ToString(), __FILE__, __LINE__);
      }
   }
}

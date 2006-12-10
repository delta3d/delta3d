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
   void ShaderManager::AddShaderGroupTemplate(ShaderGroup &shaderGroup)
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
      std::vector<dtCore::RefPtr<Shader> > shaderList;
      std::vector<dtCore::RefPtr<Shader> >::iterator shaderItor;

      shaderGroup.GetAllShaders(shaderList);
      for (shaderItor=shaderList.begin(); shaderItor!=shaderList.end(); ++shaderItor)
      {
         ResolveShaderPrograms(*(shaderItor->get()));
         mTotalShaderCount++;
      }

      mShaderGroups.insert(std::make_pair(shaderGroup.GetName(),&shaderGroup));
   }

   ///////////////////////////////////////////////////////////////////////////////
   void ShaderManager::RemoveShaderGroupTemplate(const std::string &name)
   {
      std::map<std::string,dtCore::RefPtr<ShaderGroup> >::iterator itor =
         mShaderGroups.find(name);

      mTotalShaderCount -= itor->second->GetNumShaders();
      mShaderGroups.erase(itor);
   }

   ///////////////////////////////////////////////////////////////////////////////
   void ShaderManager::RemoveShaderGroupTemplate(const ShaderGroup &shaderGroup)
   {
      RemoveShaderGroupTemplate(shaderGroup.GetName());
   }

   ///////////////////////////////////////////////////////////////////////////////
   const ShaderGroup *ShaderManager::FindShaderGroupTemplate(const std::string &name) const
   {
      std::map<std::string,dtCore::RefPtr<ShaderGroup> >::const_iterator itor =
            mShaderGroups.find(name);

      if (itor != mShaderGroups.end())
         return itor->second.get();
      else
         return NULL;
   }

   ///////////////////////////////////////////////////////////////////////////////
   ShaderGroup *ShaderManager::FindShaderGroupTemplate(const std::string &name)
   {
      std::map<std::string,dtCore::RefPtr<ShaderGroup> >::iterator itor =
            mShaderGroups.find(name);

      if (itor != mShaderGroups.end())
         return itor->second.get();
      else
         return NULL;
   }

   ///////////////////////////////////////////////////////////////////////////////
   void ShaderManager::GetAllShaderGroupTemplates(std::vector<dtCore::RefPtr<ShaderGroup> > &toFill)
   {
      std::map<std::string,dtCore::RefPtr<ShaderGroup> >::iterator itor;

      toFill.clear();
      toFill.reserve(mShaderGroups.size());
      for (itor=mShaderGroups.begin(); itor!=mShaderGroups.end(); ++itor)
         toFill.push_back(itor->second);
   }

   ///////////////////////////////////////////////////////////////////////////////
   const Shader *ShaderManager::FindShaderTemplate(const std::string &name, const std::string &groupName) const
   {
      return InternalFindShader(name,groupName);
   }

   ///////////////////////////////////////////////////////////////////////////////
   Shader *ShaderManager::FindShaderTemplate(const std::string &name, const std::string &groupName)
   {
      return const_cast<Shader *>(InternalFindShader(name,groupName));
   }

   ///////////////////////////////////////////////////////////////////////////////
   const Shader *ShaderManager::InternalFindShader(const std::string &shaderName,
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
            const Shader *shader = itor->second->FindShader(shaderName);
            if (shader != NULL)
               return shader;
         }
      }

      return NULL;
   }

   ///////////////////////////////////////////////////////////////////////////////
   // Curt - Add a method to get an active shader for a given node
   dtCore::Shader *ShaderManager::GetShaderInstanceForNode(osg::Node *node)
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
   dtCore::Shader *ShaderManager::AssignShaderFromTemplate(const dtCore::Shader &templateShader, osg::Node &node)
   {
      // If this node is already assigned to a shader, remove it from our active list. 
      RemoveShaderFromActiveNodeList(&node);

      // create a duplicate of the shader template.  The group and shaders that you use to find
      // are simply templates that we use to create unique instances for each node. 
      dtCore::RefPtr<dtCore::Shader> newShader = templateShader.Clone();

      std::vector<dtCore::RefPtr<ShaderParameter> > params;
      std::vector<dtCore::RefPtr<ShaderParameter> >::iterator currParam;
      dtCore::RefPtr<osg::StateSet> stateSet = node.getOrCreateStateSet();

      //First assign the vertex,fragment,and shader program to the render state.
      if (newShader->GetVertexShader() == NULL)
         LOG_WARNING("Error assigning shader: " + newShader->GetName() + "  Vertex shader was invalid.");
      if (newShader->GetFragmentShader() == NULL)
         LOG_WARNING("Error assigning shader: " + newShader->GetName() + "  Fragment shader was invalid.");
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
   void ShaderManager::ResolveShaderPrograms(Shader &shader)
   {
      //Shader cache entries are keyed by a combination of the source to the
      //vertex shader and the source to the fragment shader.
      std::string cacheKey = shader.GetVertexShaderSource() + ":" +
         shader.GetFragmentShaderSource();

      std::map<std::string,ShaderCacheEntry>::iterator itor =
         mShaderProgramCache.find(cacheKey);

      //If we found the cache entry, the the shader uses the same programs,
      //but may have different parameter bindings.  Therefore, share the
      //compiled programs.  If not, we need to load the programs and insert
      //them in the cache.
      if (itor != mShaderProgramCache.end())
      {
         shader.SetVertexShader(*(itor->second.vertexShader));
         shader.SetFragmentShader(*(itor->second.fragmentShader));
         shader.SetGLSLProgram(*(itor->second.shaderProgram));
         return;
      }

      //Load, compile, and link the shaders...
      std::string path;
      dtCore::RefPtr<osg::Shader> vertexShader = new osg::Shader(osg::Shader::VERTEX);

      path = dtCore::FindFileInPathList(shader.GetVertexShaderSource());
      if (path.empty() || !vertexShader->loadShaderSourceFromFile(path))
         throw dtUtil::Exception(ShaderException::SHADER_SOURCE_ERROR,"Error loading vertex shader file: " +
            shader.GetVertexShaderSource() + " from shader: " + shader.GetName(), __FILE__, __LINE__);

      dtCore::RefPtr<osg::Shader> fragmentShader = new osg::Shader(osg::Shader::FRAGMENT);
      path = dtCore::FindFileInPathList(shader.GetFragmentShaderSource());
      if (path.empty() || !fragmentShader->loadShaderSourceFromFile(path))
         throw dtUtil::Exception(ShaderException::SHADER_SOURCE_ERROR,"Error loading fragment shader file: " +
            shader.GetFragmentShaderSource() + " from shader: " + shader.GetName(), __FILE__, __LINE__);

      dtCore::RefPtr<osg::Program> program = new osg::Program();
      program->addShader(vertexShader.get());
      program->addShader(fragmentShader.get());

      shader.SetVertexShader(*vertexShader);
      shader.SetFragmentShader(*fragmentShader);
      shader.SetGLSLProgram(*program);

      //Put a new entry in the cache...
      ShaderCacheEntry newCacheEntry;
      newCacheEntry.vertexShader = vertexShader;
      newCacheEntry.fragmentShader = fragmentShader;
      newCacheEntry.shaderProgram = program;
      mShaderProgramCache.insert(std::make_pair(cacheKey,newCacheEntry));
   }

   ///////////////////////////////////////////////////////////////////////////////
   void ShaderManager::LoadShaderDefinitions(const std::string &fileName, bool merge)
   {
      try
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

         parser.ParseXML(path);
      }
      catch (const dtUtil::Exception &e)
      {
         throw dtUtil::Exception(ShaderException::XML_PARSER_ERROR,e.ToString(), __FILE__, __LINE__);
      }
   }
}

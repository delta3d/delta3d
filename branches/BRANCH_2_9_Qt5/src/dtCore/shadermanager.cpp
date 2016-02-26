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
#include <prefix/dtcoreprefix.h>
#include <dtCore/shadermanager.h>
#include <dtCore/deltadrawable.h>
#include <dtCore/shaderxml.h>
#include <dtCore/shaderparameter.h>

#include <osg/Texture2D>
#include <osg/Node>

#include <dtCore/system.h>
#include <dtUtil/datapathutils.h>

namespace dtCore
{
   /////////////////////////////////////////////////////////////////////////////
   dtCore::RefPtr<ShaderManager> ShaderManager::mInstance(NULL);

   /////////////////////////////////////////////////////////////////////////////
   ShaderManager::ShaderManager()
      : dtCore::Base("ShaderManager")
   {
      Clear();
      dtCore::System::GetInstance().TickSignal.connect_slot(this, &ShaderManager::OnSystem);
   }

   /////////////////////////////////////////////////////////////////////////////
   ShaderManager::~ShaderManager()
   {
      Clear();
   }

   //////////////////////////////////////////////////////////////////////////////
   ShaderManager& ShaderManager::GetInstance()
   {
      if (mInstance == NULL)
      {
         mInstance = new ShaderManager();
      }

      return *mInstance;
   }

   //////////////////////////////////////////////////////////////////////////////
   void ShaderManager::Destroy()
   {
      mInstance = NULL;
   }

   /////////////////////////////////////////////////////////////////////////////
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
            dtCore::RefPtr<osg::StateSet> stateSet = node->getStateSet();

            if (stateSet.valid())
            {
               // clean up the parameters effects to the stateset
               mActiveNodeList[i].shaderInstance->GetParameterList(params);
               for (currParam=params.begin(); currParam!=params.end(); ++currParam)
                  (*currParam)->DetachFromRenderState(*stateSet);

               // remove the program
               stateSet->removeAttribute(osg::StateAttribute::PROGRAM);
            }
         }
      }

      mShaderGroups.clear();
      mShaderProgramCache.clear();
      mShaderResources.clear();
      mTotalShaderCount = 0;
      mActiveNodeList.clear();
   }

   /////////////////////////////////////////////////////////////////////////////
   void ShaderManager::OnSystem(const dtUtil::RefString& str, double deltaSim, double deltaReal)

   {
      if (str == dtCore::System::MESSAGE_PRE_FRAME)
      {
         OnPreFrame(deltaSim, deltaReal);
      }
   }

   /////////////////////////////////////////////////////////////////////////////
   void ShaderManager::OnPreFrame(double /*deltaRealTime*/, double /*deltaSimTime*/)
   {
      for (int i = mActiveNodeList.size() - 1; i >= 0; i--)
      {
         // if weak reference observer is still valid, then update our shader instance.
         if (mActiveNodeList[i].nodeWeakReference.valid())
         {
            if (mActiveNodeList[i].shaderInstance->IsDirty())
            {
               mActiveNodeList[i].shaderInstance->Update();
            }
         }
         else
         {
            // If the weak reference is NULL, then remove the item from our active list.
            mActiveNodeList.erase(mActiveNodeList.begin() + i);
         }
      }
   }

   /////////////////////////////////////////////////////////////////////////////
   void ShaderManager::AddShaderGroupPrototype(ShaderGroup& shaderGroup)
   {
      ShaderGroupListType::iterator itor =
         mShaderGroups.find(shaderGroup.GetName());

      //Do not allow shader groups with the same name...
      if (itor != mShaderGroups.end())
      {
         throw DuplicateShaderGroupException("Shader groups must have unique names.  The conflicting name is \"" +
         shaderGroup.GetName() + "\".", __FILE__, __LINE__);
      }

      //Before we insert the group, we need to check our program cache and update it
      //if necessary for each shader in the group.  Also update our total shader count.
      std::vector<dtCore::RefPtr<ShaderProgram> > shaderList;
      std::vector<dtCore::RefPtr<ShaderProgram> >::iterator shaderItor;

      shaderGroup.GetAllShaders(shaderList);
      for (shaderItor=shaderList.begin(); shaderItor!=shaderList.end(); ++shaderItor)
      {
         ResolveShaderPrograms(*(shaderItor->get()), shaderGroup.GetName());
         mTotalShaderCount++;
      }

      mShaderGroups.insert(std::make_pair(shaderGroup.GetName(), &shaderGroup));
   }

   ///////////////////////////////////////////////////////////////////////////////
   void ShaderManager::RemoveShaderGroupPrototype(const std::string& name)
   {
      ShaderGroupListType::iterator itor =
         mShaderGroups.find(name);

      mTotalShaderCount -= itor->second->GetNumShaders();
      mShaderGroups.erase(itor);
   }

   /////////////////////////////////////////////////////////////////////////////
   void ShaderManager::RemoveShaderGroupPrototype(const ShaderGroup& shaderGroup)
   {
      RemoveShaderGroupPrototype(shaderGroup.GetName());
   }

   /////////////////////////////////////////////////////////////////////////////
   const ShaderGroup *ShaderManager::FindShaderGroupPrototype(const std::string& name) const
   {
      ShaderGroupListType::const_iterator itor =
            mShaderGroups.find(name);

      if (itor != mShaderGroups.end())
      {
         return itor->second.get();
      }
      else
      {
         return NULL;
      }
   }

   /////////////////////////////////////////////////////////////////////////////
   ShaderGroup *ShaderManager::FindShaderGroupPrototype(const std::string& name)
   {
      ShaderGroupListType::iterator itor =
            mShaderGroups.find(name);

      if (itor != mShaderGroups.end())
      {
         return itor->second.get();
      }
      else
      {
         return NULL;
      }
   }

   /////////////////////////////////////////////////////////////////////////////
   void ShaderManager::GetAllShaderGroupPrototypes(std::vector<dtCore::RefPtr<ShaderGroup> >& toFill)
   {
      ShaderGroupListType::iterator itor;

      toFill.clear();
      toFill.reserve(mShaderGroups.size());

      for (itor=mShaderGroups.begin(); itor!=mShaderGroups.end(); ++itor)
      {
         toFill.push_back(itor->second);
      }
   }

   /////////////////////////////////////////////////////////////////////////////
   const ShaderProgram* ShaderManager::FindShaderPrototype(const std::string& name, const std::string& groupName) const
   {
      return InternalFindShader(name,groupName);
   }

   /////////////////////////////////////////////////////////////////////////////
   ShaderProgram* ShaderManager::FindShaderPrototype(const std::string& name, const std::string& groupName)
   {
      return const_cast<ShaderProgram*>(InternalFindShader(name,groupName));
   }

   /////////////////////////////////////////////////////////////////////////////
   const ShaderProgram* ShaderManager::InternalFindShader(const std::string& shaderName,
         const std::string& groupName) const
   {
      ShaderGroupListType::const_iterator itor;

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

   /////////////////////////////////////////////////////////////////////////////
   dtCore::ShaderProgram *ShaderManager::GetShaderInstanceForNode(const osg::Node* node)
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


   ////////////////////////////////////////////////////////////////////////////////
   dtCore::ShaderProgram* ShaderManager::GetShaderInstanceForDrawable(const dtCore::DeltaDrawable& drawable)
   {
      return GetShaderInstanceForNode(drawable.GetOSGNode());
   }

   /////////////////////////////////////////////////////////////////////////////
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

   /////////////////////////////////////////////////////////////////////////////
   void ShaderManager::UnassignShaderFromNode(osg::Node& node)
   {
      dtCore::RefPtr<osg::StateSet> stateSet = node.getStateSet(); //node.getOrCreateStateSet();
      if (!stateSet.valid())
      {
         // Exit early if no stateset exists yet
         return;
      }

      std::vector<dtCore::RefPtr<ShaderParameter> > params;
      std::vector<dtCore::RefPtr<ShaderParameter> >::iterator currParam;

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

            // remove the program - which causes it to inherit.
            //stateSet->setAttributeAndModes(new osg::Program(), osg::StateAttribute::ON); // or INHERIT
            stateSet->removeAttribute(osg::StateAttribute::PROGRAM);
         }
      }

      // Remove all references to this node.  Call the method for safety to eliminate any possible
      // chance that there is more than one (not sure how that could happen).
      RemoveShaderFromActiveNodeList(&node);
   }

   /////////////////////////////////////////////////////////////////////////////
   dtCore::ShaderProgram* ShaderManager::AssignShaderFromPrototype(const dtCore::ShaderProgram& templateShader, osg::Node& node)
   {
      // If this node is already assigned to a shader, remove it from our active list.
      RemoveShaderFromActiveNodeList(&node);

      // create a duplicate of the shader prototype.  The group and shaders that you use to find
      // are simply prototypes that we use to create unique instances for each node.
      dtCore::RefPtr<dtCore::ShaderProgram> newShader = templateShader.Clone();

      std::vector<dtCore::RefPtr<ShaderParameter> > params;
      std::vector<dtCore::RefPtr<ShaderParameter> >::iterator currParam;
      dtCore::RefPtr<osg::StateSet> stateSet = node.getOrCreateStateSet();
      stateSet->setDataVariance(osg::Object::DYNAMIC);

      if (newShader->GetShaderProgram() == NULL)
      {
         LOG_WARNING("Error assigning shader: " + newShader->GetName() + "  Shader program was invalid.");
      }

      // If this contains a geometry shader, the vertex output number needs to be set
      if (newShader->GetGeometryShaders().size() > 0)
      {
         unsigned int verticesOut = newShader->GetGeometryShaderVerticesOut();
         newShader->GetShaderProgram()->setParameter(GL_GEOMETRY_VERTICES_OUT_EXT, verticesOut);
      }

      //I realize const-cast is not a great idea here, but I did not want the have a non-const version
      //of the GetShaderProgram() method on the shader class.
      stateSet->setAttributeAndModes(const_cast<osg::Program*>(newShader->GetShaderProgram()),
         osg::StateAttribute::ON | osg::StateAttribute::PROTECTED);

      //Now add all the shader's parameters to the render state.  Each class of shader parameter
      //is responcible for knowning how to attach itself to the render state.
      newShader->GetParameterList(params);
      for (currParam=params.begin(); currParam!=params.end(); ++currParam)
      {
         (*currParam)->AttachToRenderState(*stateSet);
      }

      // add the new shader and node to the active node list.
      ActiveNodeEntry activeNode;
      activeNode.shaderInstance = newShader.get();
      activeNode.nodeWeakReference = &node;
      mActiveNodeList.push_back(activeNode);

      return newShader.get();
   }

   /////////////////////////////////////////////////////////////////////////////
   void ShaderManager::ResolveShaderPrograms(ShaderProgram& shader, const std::string& groupName)
   {
      //Shader cache entries are keyed by a combination of the source to the
      //geometry shader, the source to the vertex shader and the source to the fragment shader.

      std::string cacheKey = shader.GetGeometryCacheKey() + ":" + shader.GetVertexCacheKey() + ":" + shader.GetFragmentCacheKey();

      ShaderProgramListType::iterator itor =
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
      dtCore::RefPtr<osg::Shader> geometryShader;
      dtCore::RefPtr<osg::Shader> vertexShader;
      dtCore::RefPtr<osg::Shader> fragmentShader;
      dtCore::RefPtr<osg::Program> program = new osg::Program();
      program->setName(std::string('('+groupName+')'+ shader.GetName()).c_str());

      std::vector<std::string>::const_iterator geometryShaderIterator = shader.GetGeometryShaders().begin();

      while(geometryShaderIterator != shader.GetGeometryShaders().end())
      {
          // Load and set the geometry shader - note, this is not required
          path = dtUtil::FindFileInPathList(*geometryShaderIterator);

          if (!path.empty())
          {
              geometryShader = new osg::Shader(osg::Shader::GEOMETRY);
              if (!geometryShader->loadShaderSourceFromFile(path))
              {
                  throw ShaderSourceException("Error loading geometry shader file: " +
                  *geometryShaderIterator + " from shader: " + shader.GetName(), __FILE__, __LINE__);
              }

              geometryShader->setName(*geometryShaderIterator);
              program->addShader(geometryShader.get());
          }
          geometryShaderIterator++;
      }

      std::vector<std::string>::const_iterator vertexShaderIterator = shader.GetVertexShaders().begin();

      while(vertexShaderIterator != shader.GetVertexShaders().end())
      {
         // Load and set the vertex shader - note, this is not required
         path = dtUtil::FindFileInPathList(*vertexShaderIterator);
         if (!path.empty())
         {
            vertexShader = new osg::Shader(osg::Shader::VERTEX);
            if (!vertexShader->loadShaderSourceFromFile(path))
               throw ShaderSourceException("Error loading vertex shader file: " +
                  *vertexShaderIterator + " from shader: " + shader.GetName(), __FILE__, __LINE__);
            vertexShader->setName(*vertexShaderIterator);
            program->addShader(vertexShader.get());
         }
         vertexShaderIterator++;
      }

      std::vector<std::string>::const_iterator fragmentShaderIterator = shader.GetFragmentShaders().begin();

      while(fragmentShaderIterator != shader.GetFragmentShaders().end())
      {
         // Load and set the fragment shader - note, this is not required
         path = dtUtil::FindFileInPathList(*fragmentShaderIterator);
         if (!path.empty())
         {
            fragmentShader = new osg::Shader(osg::Shader::FRAGMENT);
            if (!fragmentShader->loadShaderSourceFromFile(path))
               throw ShaderSourceException("Error loading fragment shader file: " +
                  *fragmentShaderIterator + " from shader: " + shader.GetName(), __FILE__, __LINE__);
            fragmentShader->setName(*fragmentShaderIterator);
            program->addShader(fragmentShader.get());
         }
         fragmentShaderIterator++;
      }

      program->setName(shader.GetName());
      shader.SetGLSLProgram(*program);

      //Put a new entry in the cache...
      ShaderCacheEntry newCacheEntry;
      newCacheEntry.vertexShader = vertexShader; // may be null
      newCacheEntry.fragmentShader = fragmentShader; // may be null
      newCacheEntry.shaderProgram = program;
      mShaderProgramCache.insert(std::make_pair(cacheKey,newCacheEntry));
   }

   /////////////////////////////////////////////////////////////////////////////
   void ShaderManager::ReloadAndReassignShaderDefinitions(const std::string& /*fileName*/)
   {
      // NOTE: This method that takes a file parameter is deprecated.
      // Call the parameterless method.
      ReloadAndReassignShaderDefinitions();
   }

   /////////////////////////////////////////////////////////////////////////////
   void ShaderManager::ReloadAndReassignShaderDefinitions()
   {
      LOG_WARNING("Attempting to reload ALL previously loaded shaders. This is a test behavior and may result in artifacts or changes in the scene.");

      std::vector<ActiveNodeEntry> mCopiedNodeList;
      ShaderGroupListType::const_iterator groupItor;

      // Loop through our active nodes and make a copy of each one.
      for (int i = mActiveNodeList.size() - 1; i >= 0; i--)
      {
         ActiveNodeEntry activeNode;
         activeNode.shaderInstance = mActiveNodeList[i].shaderInstance;
         activeNode.nodeWeakReference = mActiveNodeList[i].nodeWeakReference;
         mCopiedNodeList.push_back(activeNode);
      }

      // Copy resource set.
      ShaderResourceSet shaderResourceSet;
      shaderResourceSet.insert(mShaderResources.begin(), mShaderResources.end());

      // Now, clear everything - all prototypes, all assignments, everything!!!
      Clear();

      // Reload our file
      std::for_each(shaderResourceSet.begin(), shaderResourceSet.end(),
         [&](const std::string& fileName)
         {
            LoadShaderDefinitions(fileName, true);
         }
      );

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
               {
                  bFoundMatch = true;
               }
            }
         }

         // if we got a match, we rock!!!
         if (bFoundMatch)
         {
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

   /////////////////////////////////////////////////////////////////////////////
   void ShaderManager::LoadShaderDefinitions(const std::string& fileName, bool merge)
   {
      ShaderXML parser;
      std::string path = dtUtil::FindFileInPathList(fileName);
      if (path.empty())
      {
         LOG_WARNING("Could not find shader definitions file: " + fileName);
         return;
      }

      LOG_INFO("Loading Shader Definitions file: " + fileName);

      if (!merge)
      {
         Clear();
      }

      try
      {
         parser.ParseXML(path);
      }
      catch (const dtUtil::Exception& e)
      {
         throw ShaderXmlParserException(e.ToString(), __FILE__, __LINE__);
      }

      //store the loaded ShaderGroups
      const ShaderXML::ShaderContainer &shaders = parser.GetLoadedShaders();
      ShaderXML::ShaderContainer::const_iterator itr = shaders.begin();
      while (itr != shaders.end())
      {
         AddShaderGroupPrototype(*(*itr));
         ++itr;
      }

      if (mShaderResources.find(fileName) == mShaderResources.end())
      {
         mShaderResources.insert(fileName);
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void ShaderManager::LoadAndAssignShader(DeltaDrawable& drawable,
                                           const std::string& shaderResource)
   {
      //find it
      const std::string path = dtUtil::FindFileInPathList(shaderResource);
      if (path.empty())
      {
         LOG_WARNING("Could not find shader definitions file: " + shaderResource);
         return;
      }

      //parse it
      ShaderXML parser;
      parser.ParseXML(path);

      const ShaderXML::ShaderContainer &shaders = parser.GetLoadedShaders();
      if (shaders.empty())
      {
         //nothing loaded?
         return;
      }

      //we'll just use the first shader defined
      const std::string newlyLoadedShaderName = shaders[0]->GetName();
      ShaderGroup* shaderGroupToAssign = FindShaderGroupPrototype(newlyLoadedShaderName);

      //if it doesn't already exist in our list, add it
      if (shaderGroupToAssign == NULL)
      {
         AddShaderGroupPrototype(*shaders[0]);

         shaderGroupToAssign = FindShaderGroupPrototype(newlyLoadedShaderName);
      }

      //Apply the first ShaderProgram in the ShaderGroup
      if (shaderGroupToAssign->GetNumShaders() > 0 && drawable.GetOSGNode())
      {
         std::vector<dtCore::RefPtr<ShaderProgram> > shadersInGroup;
         shaderGroupToAssign->GetAllShaders(shadersInGroup);

         //apply it to the supplied drawable
         AssignShaderFromPrototype(*shadersInGroup[0],
                                   *drawable.GetOSGNode());

         if (mShaderResources.find(shaderResource) == mShaderResources.end())
         {
            mShaderResources.insert(shaderResource);
         }
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void ShaderManager::UnassignShader(dtCore::DeltaDrawable& drawable)
   {
      if (drawable.GetOSGNode())
      {
         UnassignShaderFromNode(*drawable.GetOSGNode());
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   ShaderSourceException::ShaderSourceException(const std::string& message, const std::string& filename, unsigned int linenum)
      :dtUtil::Exception(message, filename, linenum)
   {
   }

   ////////////////////////////////////////////////////////////////////////////////
   DuplicateShaderGroupException::DuplicateShaderGroupException(const std::string& message, const std::string& filename, unsigned int linenum)
      :dtUtil::Exception(message, filename, linenum)
   {
   }

   ////////////////////////////////////////////////////////////////////////////////
   ShaderXmlParserException::ShaderXmlParserException(const std::string& message, const std::string& filename, unsigned int linenum)
      :dtUtil::Exception(message, filename, linenum)
   {
   }

   ////////////////////////////////////////////////////////////////////////////////
   DuplicateShaderParameterException::DuplicateShaderParameterException(const std::string& message, const std::string& filename, unsigned int linenum)
      :dtUtil::Exception(message, filename, linenum)
   {
   }

   ////////// Hiding copy and assign ////////////////////
   ShaderManager::ShaderManager(const ShaderManager&) : mTotalShaderCount(0U) { }
   ShaderManager& ShaderManager::operator=(const ShaderManager&) { return *this; }

}

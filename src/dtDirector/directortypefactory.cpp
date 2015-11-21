/*
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2008 MOVES Institute
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
 * Author: Jeff P. Houde
 */

#include <dtDirector/directortypefactory.h>
#include <dtDirector/directorbinary.h>

#include <dtCore/project.h>
#include <dtCore/map.h>

#include <dtUtil/exception.h>
#include <dtUtil/datapathutils.h>

#include <osgDB/FileNameUtils>


namespace dtDirector
{
   const std::string DirectorTypeFactory::DIRECTOR_EXTENSION_TEXT("dtdir");
   const std::string DirectorTypeFactory::DIRECTOR_EXTENSION_BINARY("dtdirb");

   DirectorTypeFactory* DirectorTypeFactory::mInstance(NULL);

   ////////////////////////////////////////////////////////////////////////////////
   DirectorTypeFactory* DirectorTypeFactory::GetInstance()
   {
      if (!mInstance)
      {
         mInstance = new DirectorTypeFactory();
      }
      return mInstance;
   }

   ////////////////////////////////////////////////////////////////////////////////
   void DirectorTypeFactory::Destroy()
   {
      delete mInstance;
      mInstance = NULL;
   }

   ////////////////////////////////////////////////////////////////////////////////
   DirectorTypeFactory::DirectorTypeFactory()
      : mFactory(new dtUtil::ObjectFactory<std::string, Director>)
      , mCurrentXMLParser(-1)
   {
      // Register our default Director script type.
      RegisterScriptType<Director>("Scenario");
   }

   ////////////////////////////////////////////////////////////////////////////////
   DirectorTypeFactory::~DirectorTypeFactory()
   {
   }

   ////////////////////////////////////////////////////////////////////////////////
   dtCore::RefPtr<Director> DirectorTypeFactory::CreateDirector(const std::string& scriptType)
   {
      return mFactory->CreateObject(scriptType);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void DirectorTypeFactory::UnRegisterScriptType(const std::string& scriptType)
   {
      mFactory->RemoveType(scriptType);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void DirectorTypeFactory::GetScriptTypes(std::vector<std::string>& scriptTypes)
   {
      mFactory->GetSupportedTypes(scriptTypes);
   }

   ////////////////////////////////////////////////////////////////////////////////
   dtCore::RefPtr<Director> DirectorTypeFactory::LoadScript(const std::string& scriptFile, dtGame::GameManager* gm, dtCore::Map* map, bool cacheScript, Director* parent)
   {
      dtCore::RefPtr<Director> newDirector = NULL;

      // Parse the file to find out what script type to create.
      std::string scriptType;
      if (scriptFile.empty())
      {
         return NULL;
      }

      std::string ext = osgDB::getLowerCaseFileExtension(scriptFile);
      if (ext.empty())
      {
         ext = DIRECTOR_EXTENSION_TEXT;
      }

      bool binaryFormat = false;
      if (ext.compare(DIRECTOR_EXTENSION_BINARY) == 0)
      {
         binaryFormat = true;
      }

      std::string fileName = osgDB::getNameLessExtension(scriptFile) + "." + ext;
      fileName = osgDB::getRealPath(fileName);

      // First attempt to load a cached script.
      Director* cache = GetCachedScript(fileName);
      if (cache)
      {
         newDirector = cache->Clone();
         newDirector->SetParent(parent);
      }
      else
      {
         try
         {
            if (binaryFormat)
            {
               dtCore::RefPtr<BinaryParser> parser = new BinaryParser();
               if (parser.valid())
               {
                  scriptType = parser->ParseScriptType(fileName);
                  newDirector = CreateDirector(scriptType);
                  if (!newDirector.valid())
                  {
                     throw dtUtil::Exception("Failed to load script, \'" + scriptType + "\' is an unknown script type.  Please ensure that any plugin libraries for this script type are properly loaded.", __FILE__, __LINE__);
                  }

                  newDirector->mLoading = true;
                  newDirector->Init(gm, map);
                  newDirector->SetParent(parent);
                  if (parent != NULL)
                  {
                     newDirector->SetScriptOwner(parent->GetScriptOwner());
                  }
                  parser->Parse(newDirector, map, fileName);
                  newDirector->mMissingNodeTypes = parser->GetMissingNodeTypes();
                  newDirector->mMissingLibraries = parser->GetMissingLibraries();
                  newDirector->mMissingImportedScripts = parser->GetMissingImportedScripts();
                  newDirector->mHasDeprecatedProperty = parser->HasDeprecatedProperty();
               }
            }
            else
            {
               mCurrentXMLParser++;
               if (mCurrentXMLParser >= (int)mXMLParserList.size())
               {
                  mXMLParserList.push_back(new DirectorParser());
               }
               dtCore::RefPtr<DirectorParser> parser = mXMLParserList[mCurrentXMLParser];

               if (parser.valid())
               {
                  scriptType = parser->ParseDirectorHeaderData(fileName)->GetScriptType();
                  newDirector = CreateDirector(scriptType);
                  if (!newDirector.valid())
                  {
                     throw dtUtil::Exception("Failed to load script, \'" + scriptType + "\' is an unknown script type.  Please ensure that any plugin libraries for this script type are properly loaded.", __FILE__, __LINE__);
                  }

                  newDirector->mLoading = true;
                  newDirector->Init(gm, map);
                  newDirector->SetParent(parent);
                  if (parent != NULL)
                  {
                     newDirector->SetScriptOwner(parent->GetScriptOwner());
                  }
                  parser->Parse(newDirector, map, fileName);
                  newDirector->mMissingNodeTypes = parser->GetMissingNodeTypes();
                  newDirector->mMissingLibraries = parser->GetMissingLibraries();
                  newDirector->mMissingImportedScripts = parser->GetMissingImportedScripts();
                  newDirector->mHasDeprecatedProperty = parser->HasDeprecatedProperty();
               }

               mCurrentXMLParser--;

               // Unload any extra parsers created for referenced scripts.
               if (mCurrentXMLParser == -1)
               {
                  mXMLParserList.clear();
               }
            }
         }
         catch (const dtUtil::Exception& e)
         {
            if (!binaryFormat)
            {
               mCurrentXMLParser--;

               // Unload any extra parsers created for referenced scripts.
               if (mCurrentXMLParser == -1)
               {
                  mXMLParserList.clear();
               }
            }

            std::string error = "Unable to parse " + scriptFile + " with error " + e.What();
            dtUtil::Log::GetInstance().LogMessage(dtUtil::Log::LOG_INFO, __FUNCTION__, __LINE__, error.c_str());
            throw e;
         }
      }

      if (newDirector->mMissingNodeTypes.size() > 0 ||
         newDirector->mMissingLibraries.size() > 0 ||
         newDirector->mMissingImportedScripts.size() > 0 ||
         newDirector->mHasDeprecatedProperty)
      {
         newDirector->mModified = true;
      }

      newDirector->mLoading = false;
      newDirector->mScriptName = fileName;

      std::vector<Node*> nodes;
      newDirector->GetAllNodes(nodes);
      int count = (int)nodes.size();
      for (int index = 0; index < count; ++index)
      {
         nodes[index]->OnFinishedLoading();
      }

      // If we are caching this script, and it is not already cached,
      // then we should create a clone of this script to be stored in cache.
      if (cacheScript && !cache)
      {
         AddCacheScript(newDirector->Clone());
      }

      return newDirector;
   }

   ////////////////////////////////////////////////////////////////////////////////
   void DirectorTypeFactory::LoadScript(Director* director, const std::string& scriptFile, bool cacheScript)
   {
      if (!director)
      {
         throw dtUtil::Exception("Attempted to load a Director Script when no Director Script class was provided.", __FILE__, __LINE__);
      }

      director->Clear();

      if (scriptFile.empty())
      {
         return;
      }

      std::string ext = osgDB::getLowerCaseFileExtension(scriptFile);
      if (ext.empty())
      {
         ext = "dtdir";
      }

      bool binaryFormat = false;
      if (ext.compare("dtdirb") == 0)
      {
         binaryFormat = true;
      }

      std::string fileName = osgDB::getNameLessExtension(scriptFile) + "." + ext;
      fileName = osgDB::getRealPath(fileName);

      // First attempt to load a cached script.
      Director* cache = GetCachedScript(fileName);
      if (cache)
      {
         director->CopyPropertiesFrom(*cache);
         director->SetGraphRoot(cache->GetGraphRoot()->Clone(director));
      }
      else
      {
         try
         {
            director->mLoading = true;
            director->mScriptName = fileName;

            if (binaryFormat)
            {
               dtCore::RefPtr<BinaryParser> parser = new BinaryParser();
               if (parser.valid())
               {
                  parser->Parse(director, director->GetMap(), fileName);
                  director->mMissingNodeTypes = parser->GetMissingNodeTypes();
                  director->mMissingLibraries = parser->GetMissingLibraries();
                  director->mMissingImportedScripts = parser->GetMissingImportedScripts();
                  director->mHasDeprecatedProperty = parser->HasDeprecatedProperty();
               }
            }
            else
            {
               dtCore::RefPtr<DirectorParser> parser = new DirectorParser();
               if (parser.valid())
               {
                  parser->Parse(director, director->GetMap(), fileName);
                  director->mMissingNodeTypes = parser->GetMissingNodeTypes();
                  director->mMissingLibraries = parser->GetMissingLibraries();
                  director->mMissingImportedScripts = parser->GetMissingImportedScripts();
                  director->mHasDeprecatedProperty = parser->HasDeprecatedProperty();
               }
            }
         }
         catch (const dtUtil::Exception& e)
         {
            std::string error = "Unable to parse " + scriptFile + " with error " + e.What();
            dtUtil::Log::GetInstance().LogMessage(dtUtil::Log::LOG_INFO, __FUNCTION__, __LINE__, error.c_str());
            director->mLoading = false;
            throw e;
         }
      }

      if (director->mMissingNodeTypes.size() > 0 ||
         director->mMissingLibraries.size() > 0 ||
         director->mMissingImportedScripts.size() > 0 ||
         director->mHasDeprecatedProperty)
      {
         director->mModified = true;
      }

      director->mLoading = false;


      // If we are caching this script, and it is not already cached,
      // then we should create a clone of this script to be stored in cache.
      if (cacheScript && !cache)
      {
         AddCacheScript(director->Clone());
      }

      std::vector<Node*> nodes;
      director->GetAllNodes(nodes);
      int count = (int)nodes.size();
      for (int index = 0; index < count; ++index)
      {
         nodes[index]->OnFinishedLoading();
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void DirectorTypeFactory::SaveScript(Director* director, const std::string& scriptFile)
   {
      if (!director)
      {
         throw dtUtil::Exception("Attempted to save a Director Script when no Director Script class was provided.", __FILE__, __LINE__);
      }

      std::string ext = osgDB::getLowerCaseFileExtension(scriptFile);
      if (ext.empty())
      {
         ext = "dtdir";
      }

      bool binaryFormat = false;
      if (ext.compare("dtdirb") == 0)
      {
         binaryFormat = true;
      }

      std::string fileName = osgDB::getNameLessExtension(scriptFile) + "." + ext;
      fileName = osgDB::getRealPath(fileName);

      try
      {
         if (binaryFormat)
         {
            dtCore::RefPtr<BinaryWriter> writer = new BinaryWriter();
            if (writer.valid())
            {
               writer->Save(director, fileName);
            }
         }
         else
         {
            dtCore::RefPtr<DirectorWriter> writer = new DirectorWriter();
            if (writer.valid())
            {
               writer->Save(director, fileName);
            }
         }
      }
      catch (const dtUtil::Exception& e)
      {
         std::string error = "Unable to parse " + scriptFile + " with error " + e.What();
         dtUtil::Log::GetInstance().LogMessage(dtUtil::Log::LOG_INFO, __FUNCTION__, __LINE__, error.c_str());
         throw e;
      }

      director->mScriptName = fileName;

      // Re-cache if we need to.
      Director* cache = GetCachedScript(fileName);
      if (cache)
      {
         AddCacheScript(director->Clone());
      }

      director->mModified = false;
      director->mMissingNodeTypes.clear();
      director->mMissingLibraries.clear();
      director->mMissingImportedScripts.clear();
      director->mHasDeprecatedProperty = false;
   }

   ////////////////////////////////////////////////////////////////////////////////
   void DirectorTypeFactory::RegisterScriptInstance(Director* director)
   {
      if (!director)
      {
         return;
      }

      UnRegisterScriptInstance(director);

      mScriptInstances.push_back(director);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void DirectorTypeFactory::UnRegisterScriptInstance(Director* director)
   {
      if (!director)
      {
         return;
      }

      int count = (int)mScriptInstances.size();
      for (int index = 0; index < count; ++index)
      {
         Director* instance = mScriptInstances[index];
         if (instance == director)
         {
            mScriptInstances.erase(mScriptInstances.begin() + index);
            break;
         }
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   const std::vector<Director*>& DirectorTypeFactory::GetScriptInstances() const
   {
      return mScriptInstances;
   }

   ////////////////////////////////////////////////////////////////////////////////
   bool DirectorTypeFactory::AddCacheScript(Director* director)
   {
      if (!director || director->GetScriptName().empty())
      {
         return false;
      }

      mCachedScripts[director->GetScriptName()] = director;
      director->SetVisibleInInspector(false);

      return true;
   }

   ////////////////////////////////////////////////////////////////////////////////
   bool DirectorTypeFactory::RemoveCacheScript(const std::string& script)
   {
      if (script.empty())
      {
         return false;
      }

      std::map<std::string, dtCore::RefPtr<Director> >::iterator iter =
         mCachedScripts.find(script);

      if (iter == mCachedScripts.end())
      {
         return false;
      }

      mCachedScripts.erase(iter);
      return true;
   }

   ////////////////////////////////////////////////////////////////////////////////
   Director* DirectorTypeFactory::GetCachedScript(const std::string& script)
   {
      if (script.empty())
      {
         return NULL;
      }

      std::map<std::string, dtCore::RefPtr<Director> >::iterator iter =
         mCachedScripts.find(script);

      if (iter == mCachedScripts.end())
      {
         return NULL;
      }

      return iter->second.get();
   }

   ////////////////////////////////////////////////////////////////////////////////
   const Director* DirectorTypeFactory::GetCachedScript(const std::string& script) const
   {
      if (script.empty())
      {
         return NULL;
      }

      std::map<std::string, dtCore::RefPtr<Director> >::const_iterator iter =
         mCachedScripts.find(script);

      if (iter == mCachedScripts.end())
      {
         return NULL;
      }

      return iter->second.get();
   }
} // namespace dtDirector

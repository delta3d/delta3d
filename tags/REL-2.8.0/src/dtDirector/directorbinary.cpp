/*
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2005, BMH Associates, Inc.
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
 * Jeff P. Houde
 */

#include <osgDB/FileNameUtils>

#include <dtDirector/director.h>
#include <dtDirector/directorbinary.h>
#include <dtDirector/node.h>
#include <dtDirector/nodetype.h>
#include <dtDirector/nodemanager.h>
#include <dtDirector/directortypefactory.h>

#include <dtCore/map.h>
#include <dtCore/exceptionenum.h>

#include <dtUtil/datapathutils.h>
#include <dtUtil/fileutils.h>
#include <dtUtil/datetime.h>
#include <dtUtil/log.h>

#define BINARY_SCRIPT_VERSION 5.0f

namespace dtDirector
{
   /////////////////////////////////////////////////////////////////
   BinaryParser::BinaryParser()
   {
   }

   /////////////////////////////////////////////////////////////////
   BinaryParser::~BinaryParser()
   {
   }

   /////////////////////////////////////////////////////////////////
   void BinaryParser::Parse(Director* director, dtCore::Map* map, const std::string& filePath)
   {
      if (!director)
      {
         throw dtUtil::Exception("Invalid data found.", __FILE__, __LINE__);
      }

      FILE* file = fopen(filePath.c_str(), "rb");
      if (!file)
      {
         throw dtUtil::Exception(std::string("Unable to open Director Script file \"") + filePath + "\" for reading.", __FILE__, __LINE__);
      }

      try
      {
         float version = ParseHeader(director, file);

         ParseGraph(version, director, NULL, file);

         if (version >= 3.0f)
         {
            ParseLinkConnections(version, director, file);
         }

         LinkNodes(director);
         fclose(file);
      }
      catch (dtUtil::Exception& ex)
      {
         dtUtil::Log::GetInstance().LogMessage(dtUtil::Log::LOG_ERROR, __FUNCTION__, __LINE__,
            "Caught Exception \"%s\" while attempting to load Director script \"%s\".",
            ex.What().c_str(), filePath.c_str());
         fclose(file);
         throw;
      }
      catch (...)
      {
         dtUtil::Log::GetInstance().LogMessage(dtUtil::Log::LOG_ERROR, __FUNCTION__, __LINE__,
            "Unknown exception while attempting to load Director script \"%s\".",
            filePath.c_str());
         fclose(file);
         throw dtUtil::Exception( std::string("Unknown exception loading Director script \"") + filePath + ("\"."), __FILE__, __LINE__);
      }
   }

   /////////////////////////////////////////////////////////////////
   const std::string& BinaryParser::ParseScriptType(const std::string& filePath)
   {
      FILE* file = fopen(filePath.c_str(), "rb");
      if (!file)
      {
         throw dtUtil::Exception(std::string("Unable to open Director Script file \"") + filePath + "\" for reading.", __FILE__, __LINE__);
      }

      mScriptType = "Unknown";

      try
      {
         //float version =
         ParseHeader(NULL, file);
         fclose(file);
      }
      catch (dtUtil::Exception& ex)
      {
         dtUtil::Log::GetInstance().LogMessage(dtUtil::Log::LOG_ERROR, __FUNCTION__, __LINE__,
            "Caught Exception \"%s\" while attempting to load Director script \"%s\".",
            ex.What().c_str(), filePath.c_str());
         fclose(file);
         throw;
      }
      catch (...)
      {
         dtUtil::Log::GetInstance().LogMessage(dtUtil::Log::LOG_ERROR, __FUNCTION__, __LINE__,
            "Unknown exception while attempting to load Director script \"%s\".",
            filePath.c_str());
         fclose(file);
         throw dtUtil::Exception( std::string("Unknown exception loading Director script \"") + filePath + ("\"."), __FILE__, __LINE__);
      }

      return mScriptType;
   }

   /////////////////////////////////////////////////////////////////
   const std::set<std::string>& BinaryParser::GetMissingNodeTypes()
   {
      return mMissingNodeTypes;
   }

   /////////////////////////////////////////////////////////////////
   const std::vector<std::string>& BinaryParser::GetMissingLibraries()
   {
      return mMissingLibraries;
   }

   ////////////////////////////////////////////////////////////////////////////////
   const std::vector<std::string>& BinaryParser::GetMissingImportedScripts()
   {
      return mMissingImportedScripts;
   }

   ////////////////////////////////////////////////////////////////////////////////
   bool BinaryParser::HasDeprecatedProperty() const
   {
      return mHasDeprecatedProperty;
   }

   ////////////////////////////////////////////////////////////////////////////////
   float BinaryParser::ParseHeader(Director* director, FILE* file)
   {
      if (!file)
      {
         throw dtUtil::Exception("Invalid data found.", __FILE__, __LINE__);
      }

      // File version.
      float version = 0;
      fread(&version, sizeof(float), 1, file);

      // If the files version is higher than the version of our,
      // application, then we can't load this file.
      if (version > BINARY_SCRIPT_VERSION)
      {
         throw dtUtil::Exception("Unable to load script, version not supported!", __FILE__, __LINE__);
      }

      // Creation timestamp.
      std::string utcTime = ParseString(file);
      if (director)
      {
         director->SetCreateDateTime(utcTime);
      }

      // Current timestamp.
      utcTime = ParseString(file);

      // Script type.
      mScriptType = ParseString(file);
      if (!director)
      {
         return version;
      }

      if (director->GetScriptType() != mScriptType)
      {
         throw dtUtil::Exception("Attempted to load an invalid script type.", __FILE__, __LINE__);
      }

      int count = 0;

      // Imported Scripts.
      if (version >= 2.0f)
      {
         fread(&count, sizeof(int), 1, file);
         for (int index = 0; index < count; ++index)
         {
            std::string scriptId = ParseString(file);
            if (!scriptId.empty())
            {
               if (!director->ImportScript(scriptId))
               {
                  mMissingImportedScripts.push_back(scriptId);
               }
            }
         }
      }

      // Node Libraries.
      fread(&count, sizeof(int), 1, file);
      for (int index = 0; index < count; ++index)
      {
         std::string lib = ParseString(file);
         if (!lib.empty())
         {
            try
            {
               director->AddLibrary(lib);
            }
            catch (const dtCore::ProjectResourceErrorException& e)
            {
               mMissingLibraries.push_back(lib);

               dtUtil::Log::GetInstance().LogMessage(dtUtil::Log::LOG_ERROR, __FUNCTION__, __LINE__,
                  "Error loading node library %s in the director node manager.  Exception message to follow.",
                  lib.c_str());

               e.LogException(dtUtil::Log::LOG_ERROR, dtUtil::Log::GetInstance());
            }
            catch (const dtUtil::Exception& e)
            {
               mMissingLibraries.push_back(lib);

               dtUtil::Log::GetInstance().LogMessage(dtUtil::Log::LOG_ERROR, __FUNCTION__, __LINE__,
                  "Unknown exception loading node library %s in the director node manager.  Exception message to follow.",
                  lib.c_str());

               e.LogException(dtUtil::Log::LOG_ERROR, dtUtil::Log::GetInstance());
            }
         }
      }

      // Properties.
      ParsePropertyContainer(director, file);

      return version;
   }

   ////////////////////////////////////////////////////////////////////////////////
   void BinaryParser::ParseGraph(float version, Director* director, DirectorGraph* parent, FILE* file)
   {
      if (!director || !file)
      {
         throw dtUtil::Exception("Invalid data found.", __FILE__, __LINE__);
      }

      bool isImported = false;
      bool isReadOnly = false;
      ID id;

      // Imported.
      if (version >= 4.0f)
      {
         fread(&isImported, sizeof(bool), 1, file);
      }

      // Read Only.
      if (version >= 5.0f)
      {
         fread(&isReadOnly, sizeof(bool), 1, file);
      }

      // Graph ID.
      if (version >= 3.5f)
      {
         int index = -1;
         fread(&index, sizeof(int), 1, file);
         id.index = index;
      }

      id.id = dtCore::UniqueId(ParseString(file));

      // If this graph is meant to be imported, then attempt to find the graph
      // as it should already exist.
      dtCore::RefPtr<DirectorGraph> graph = NULL;
      if (isImported)
      {
         ID tempID = id;
         tempID.index = -1;
         graph = director->GetGraph(tempID);

         // If the graph was not found, it may have been removed from the
         // imported script in which it came from.  In this case, we still
         // need to create a graph for the loading process to complete
         // properly, but this graph will not be added to the script.
         if (!graph)
         {
            graph = new DirectorGraph(director);
            graph->BuildPropertyMap();
         }

         graph->SetID(id);
         graph->SetReadOnly(isReadOnly);
      }
      // Otherwise, create a new graph.
      else
      {
         if (!parent)
         {
            graph = director->GetGraphRoot();
         }
         else
         {
            graph = new DirectorGraph(director);
            graph->SetParent(parent);
            graph->BuildPropertyMap();
            parent->GetSubGraphs().push_back(graph);
         }

         if (id.index > -1)
         {
            graph->SetIDIndex(id.index);
         }
         graph->SetID(id.id);
         graph->SetReadOnly(isReadOnly);
      }

      // Properties.
      ParsePropertyContainer(graph, file);

      // Event Nodes.
      int count = 0;
      fread(&count, sizeof(int), 1, file);
      for (int index = 0; index < count; index++)
      {
         ParseNode(version, graph, file);
      }

      // Action Nodes.
      count = 0;
      fread(&count, sizeof(int), 1, file);
      for (int index = 0; index < count; index++)
      {
         ParseNode(version, graph, file);
      }

      // Value nodes.
      count = 0;
      fread(&count, sizeof(int), 1, file);
      for (int index = 0; index < count; index++)
      {
         ParseNode(version, graph, file);
      }

      // Sub Graphs.
      count = 0;
      fread(&count, sizeof(int), 1, file);
      for (int index = 0; index < count; index++)
      {
         ParseGraph(version, director, graph, file);
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void BinaryParser::ParseNode(float version, DirectorGraph* graph, FILE* file)
   {
      if (!graph || !file)
      {
         throw dtUtil::Exception("Invalid data found.", __FILE__, __LINE__);
      }

      // Node Type.
      std::string typeName     = ParseString(file);
      std::string typeCategory = ParseString(file);

      std::string nodeName = typeName;
      std::string nodeCategory = typeCategory;

      dtCore::RefPtr<Node> newNode = NULL;

      dtDirector::NodeManager& nodeManager = dtDirector::NodeManager::GetInstance();
      while (!newNode.valid() && !nodeName.empty() && !nodeCategory.empty())
      {
         newNode = nodeManager.CreateNode(nodeName, nodeCategory, graph).get();
         if (!newNode.valid())
         {
            // Attempt to find a replacement for this node.
            dtDirector::NodePluginRegistry::NodeReplacementData repData;
            repData = nodeManager.FindNodeTypeReplacement(nodeCategory + "." + nodeName);

            // Attempt to load the library that contains the node.
            if (!repData.library.empty())
            {
               if (!graph->GetDirector()->HasLibrary(repData.library))
               {
                  graph->GetDirector()->AddLibrary(repData.library);
                  mHasDeprecatedProperty = true;
               }
            }

            nodeName = repData.newName;
            nodeCategory = repData.newCategory;
         }
      }

      if (newNode.valid())
      {
         // Now make sure the new node is part of a valid library.
         NodePluginRegistry* reg = NodeManager::GetInstance().GetRegistryForType(newNode->GetType());
         if (reg && !graph->GetDirector()->HasLibrary(reg->GetName()))
         {
            if (graph->GetDirector()->AddLibrary(reg->GetName()))
            {
               mHasDeprecatedProperty = true;
            }
            else
            {
               newNode = NULL;
            }
         }
      }

      if (!newNode.valid())
      {
         std::string nodeType = typeCategory + "." + typeName;
         if (mMissingNodeTypes.find(nodeType) == mMissingNodeTypes.end())
         {
            mMissingNodeTypes.insert(mMissingNodeTypes.end(), nodeType);
         }
      }

      // Node ID.
      if (version >= 3.5f)
      {
         int index = -1;
         fread(&index, sizeof(int), 1, file);

         if (newNode.valid())
         {
            newNode->SetIDIndex(index);
         }
      }

      std::string nodeID = ParseString(file);
      if (newNode.valid())
      {
         newNode->SetID(dtCore::UniqueId(nodeID));
      }

      // Properties.
      ParsePropertyContainer(newNode, file);

      // Input Links.
      int count = 0;
      fread(&count, sizeof(int), 1, file);
      for (int index = 0; index < count; index++)
      {
         // Input Link Name.
         std::string linkName = ParseString(file);

         if (newNode.valid())
         {
            InputLink* link = newNode->GetInputLink(linkName);
            if (link)
            {
               link->SetVisible(false);
            }
         }
      }

      // Output Links.
      count = 0;
      fread(&count, sizeof(int), 1, file);
      for (int index = 0; index < count; index++)
      {
         // Output Link Name.
         std::string linkName = ParseString(file);
         OutputLink* link = NULL;
         if (newNode.valid())
         {
            link = newNode->GetOutputLink(linkName);
         }

         // Output Visibility.
         bool visible = true;
         fread(&visible, sizeof(bool), 1, file);
         if (link)
         {
            link->SetVisible(visible);
         }

         // Link connection data has been moved since version 3.0,
         // see ParseChainConnections().
         if (version < 3.0f)
         {
            // Links.
            int linkCount = 0;
            fread(&linkCount, sizeof(int), 1, file);
            for (int linkIndex = 0; linkIndex < linkCount; linkIndex++)
            {
               // Linked Node ID.
               ID linkedID;
               linkedID.id = dtCore::UniqueId(ParseString(file));

               if (!linkedID.id.ToString().empty())
               {
                  // Linked Input Name.
                  std::string linkedName = ParseString(file);

                  if (link)
                  {
                     ToLinkData data;
                     data.outputNodeID   = link->GetOwner()->GetID();
                     data.inputNodeID    = linkedID;
                     data.outputLinkName = link->GetName();
                     data.inputLinkName  = linkedName;
                     data.isValue        = false;
                     mLinkList.push_back(data);
                  }
               }
            }
         }
      }

      // Value Links.
      count = 0;
      fread(&count, sizeof(int), 1, file);
      for (int index = 0; index < count; index++)
      {
         // Value Link Name.
         std::string linkName = ParseString(file);
         ValueLink* link = NULL;
         if (newNode.valid())
         {
            link = newNode->GetValueLink(linkName);
         }

         // Value Link Visibility.
         bool visible = true;
         fread(&visible, sizeof(bool), 1, file);
         if (link)
         {
            link->SetVisible(visible);
         }

         // Value Link Exposed.
         bool exposed = true;
         fread(&exposed, sizeof(bool), 1, file);
         if (link)
         {
            link->SetExposed(exposed);
         }

         // Link connection data has been moved since version 3.0,
         // see ParseValueConnections().
         if (version < 3.0f)
         {
            // Links.
            int linkCount = 0;
            fread(&linkCount, sizeof(int), 1, file);
            for (int linkIndex = 0; linkIndex < linkCount; linkIndex++)
            {
               // Linked Node ID.
               ID linkedID;
               linkedID.id = dtCore::UniqueId(ParseString(file));

               if (!linkedID.id.ToString().empty() && link)
               {
                  ToLinkData data;
                  data.outputNodeID   = link->GetOwner()->GetID();
                  data.inputNodeID    = linkedID;
                  data.outputLinkName = link->GetName();
                  data.isValue        = true;
                  mLinkList.push_back(data);
               }
            }
         }
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void BinaryParser::ParseLinkConnections(float version, Director* script, FILE* file)
   {
      int linkCount = 0;
      fread(&linkCount, sizeof(int), 1, file);
      for (int linkIndex = 0; linkIndex < linkCount; linkIndex++)
      {
         ToLinkData data;
         if (version >= 3.5f)
         {
            fread(&data.outputNodeID.index, sizeof(int), 1, file);
         }
         data.outputNodeID.id = dtCore::UniqueId(ParseString(file));
         if (version >= 3.5f)
         {
            fread(&data.inputNodeID.index, sizeof(int), 1, file);
         }
         data.inputNodeID.id  = dtCore::UniqueId(ParseString(file));
         data.outputLinkName  = ParseString(file);
         data.inputLinkName   = ParseString(file);
         data.isValue = false;
         mLinkList.push_back(data);
      }

      fread(&linkCount, sizeof(int), 1, file);
      for (int linkIndex = 0; linkIndex < linkCount; linkIndex++)
      {
         ToLinkData data;
         if (version >= 3.5f)
         {
            fread(&data.outputNodeID.index, sizeof(int), 1, file);
         }
         data.outputNodeID.id = dtCore::UniqueId(ParseString(file));
         if (version >= 3.5f)
         {
            fread(&data.inputNodeID.index, sizeof(int), 1, file);
         }
         data.inputNodeID.id  = dtCore::UniqueId(ParseString(file));
         data.outputLinkName  = ParseString(file);
         data.isValue = true;
         mLinkList.push_back(data);
      }

      // Parse link removals.
      if (version >= 3.5f)
      {
         fread(&linkCount, sizeof(int), 1, file);
         for (int linkIndex = 0; linkIndex < linkCount; linkIndex++)
         {
            ToLinkData data;
            if (version >= 3.5f)
            {
               fread(&data.outputNodeID.index, sizeof(int), 1, file);
            }
            data.outputNodeID.id = dtCore::UniqueId(ParseString(file));
            if (version >= 3.5f)
            {
               fread(&data.inputNodeID.index, sizeof(int), 1, file);
            }
            data.inputNodeID.id  = dtCore::UniqueId(ParseString(file));
            data.outputLinkName  = ParseString(file);
            data.inputLinkName   = ParseString(file);
            data.isValue = false;
            mRemovedLinkList.push_back(data);
         }

         fread(&linkCount, sizeof(int), 1, file);
         for (int linkIndex = 0; linkIndex < linkCount; linkIndex++)
         {
            ToLinkData data;
            if (version >= 3.5f)
            {
               fread(&data.outputNodeID.index, sizeof(int), 1, file);
            }
            data.outputNodeID.id = dtCore::UniqueId(ParseString(file));
            if (version >= 3.5f)
            {
               fread(&data.inputNodeID.index, sizeof(int), 1, file);
            }
            data.inputNodeID.id  = dtCore::UniqueId(ParseString(file));
            data.outputLinkName  = ParseString(file);
            data.isValue = true;
            mRemovedLinkList.push_back(data);
         }
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void BinaryParser::ParsePropertyContainer(dtCore::PropertyContainer* container, FILE* file)
   {
      if (!file)
      {
         throw dtUtil::Exception("Invalid data found.", __FILE__, __LINE__);
      }

      int count = 0;
      fread(&count, sizeof(int), 1, file);
      for (int index = 0; index < count; ++index)
      {
         std::string propName = ParseString(file);
         std::string propValue = ParseString(file);

         if (container)
         {
            dtCore::RefPtr<dtCore::ActorProperty> prop = container->GetProperty(propName);
            if (!prop.valid())
            {
               prop = container->GetDeprecatedProperty(propName);
               if (prop.valid())
               {
                  mHasDeprecatedProperty = true;
               }
            }

            if (prop.valid())
            {
               prop->FromString(propValue);
            }
         }
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   std::string BinaryParser::ParseString(FILE* file)
   {
      if (!file)
      {
         throw dtUtil::Exception("Invalid data found.", __FILE__, __LINE__);
      }

      int count = 0;
      fread(&count, sizeof(int), 1, file);

      std::string str;
      if (count > 0)
      {
         str.resize(count);
         fread(&str[0], sizeof(char), count, file);
      }

      return str;
   }

   //////////////////////////////////////////////////////////////////////////
   void BinaryParser::LinkNodes(Director* director)
   {
      std::vector<ToLinkData> failedLinks;

      int count = (int)mLinkList.size();
      for (int index = 0; index < count; index++)
      {
         dtCore::RefPtr<Node> outputNode = director->GetNode(mLinkList[index].outputNodeID, true);
         dtCore::RefPtr<Node> inputNode = director->GetNode(mLinkList[index].inputNodeID, true);

         if (outputNode.valid() && inputNode.valid())
         {
            // Connect a value link to a value node.
            if (mLinkList[index].isValue)
            {
               ValueLink* link = outputNode->GetValueLink(mLinkList[index].outputLinkName);
               ValueNode* valueNode = inputNode->AsValueNode();
               if (link && valueNode)
               {
                  link->SetExposed(true);
                  link->SetVisible(true);
                  if (!link->Connect(valueNode))
                  {
                     // If the connection failed, it may require another link
                     // connection before it can be made.  Add to the failed
                     // list and try this again later.
                     failedLinks.push_back(mLinkList[index]);
                  }
               }
            }
            // Connect chain links.
            else
            {
               OutputLink* outputLink = outputNode->GetOutputLink(mLinkList[index].outputLinkName);
               InputLink* inputLink = inputNode->GetInputLink(mLinkList[index].inputLinkName);
               if (outputLink && inputLink)
               {
                  outputLink->Connect(inputLink);
               }
            }
         }
      }

      // If we have any failed links, but we also had some successful ones,
      // then we should try to link them again.
      if (!failedLinks.empty() &&
         (int)failedLinks.size() != count)
      {
         mLinkList = failedLinks;
         LinkNodes(director);
      }

      mLinkList.clear();
   }

   //////////////////////////////////////////////////////////////////////////
   void BinaryParser::RemoveLinkNodes(Director* director)
   {
      int count = (int)mRemovedLinkList.size();
      for (int index = 0; index < count; index++)
      {
         dtCore::RefPtr<Node> outputNode = director->GetNode(mRemovedLinkList[index].outputNodeID, true);
         dtCore::RefPtr<Node> inputNode = director->GetNode(mRemovedLinkList[index].inputNodeID, true);

         if (outputNode.valid() && inputNode.valid())
         {
            // Disconnect a value link from a value node.
            if (mRemovedLinkList[index].isValue)
            {
               ValueLink* link = outputNode->GetValueLink(mRemovedLinkList[index].outputLinkName);
               ValueNode* valueNode = inputNode->AsValueNode();
               if (link && valueNode)
               {
                  link->Disconnect(valueNode);
               }
            }
            // Disconnect chain links.
            else
            {
               OutputLink* outputLink = outputNode->GetOutputLink(mRemovedLinkList[index].outputLinkName);
               InputLink* inputLink = inputNode->GetInputLink(mRemovedLinkList[index].inputLinkName);
               if (outputLink && inputLink)
               {
                  outputLink->Disconnect(inputLink);
               }
            }
         }
      }

      mRemovedLinkList.clear();
   }

   //////////////////////////////////////////////////////////////////////////
   //////////////////////////////////////////////////////////////////////////

   //////////////////////////////////////////////////////////////////////////
   BinaryWriter::BinaryWriter()
   {
   }

   //////////////////////////////////////////////////////////////////////////
   BinaryWriter::~BinaryWriter()
   {
   }

   /////////////////////////////////////////////////////////////////
   void BinaryWriter::Save(Director* director, const std::string& filePath)
   {
      if (!director)
      {
         throw dtUtil::Exception("Invalid data found.", __FILE__, __LINE__);
      }

      FILE* file = fopen(filePath.c_str(), "wb");
      if (!file)
      {
         throw dtCore::MapSaveException( std::string("Unable to open Director Script file \"") + filePath + "\" for writing.", __FILE__, __LINE__);
      }

      try
      {
         director->CleanIDs();

         SaveHeader(director, file);

         SaveGraph(director->GetGraphRoot(), file);

         SaveLinkConnections(director, file);

         fclose(file);
      }
      catch (dtUtil::Exception& ex)
      {
         dtUtil::Log::GetInstance().LogMessage(dtUtil::Log::LOG_ERROR, __FUNCTION__, __LINE__,
                             "Caught Exception \"%s\" while attempting to save Director script \"%s\".",
                             ex.What().c_str(), filePath.c_str());

         fclose(file);

         throw;
      }
      catch (...)
      {
         dtUtil::Log::GetInstance().LogMessage(dtUtil::Log::LOG_ERROR, __FUNCTION__, __LINE__,
                             "Unknown exception while attempting to save Director script \"%s\".",
                             filePath.c_str());

         fclose(file);

         throw dtCore::MapSaveException( std::string("Unknown exception saving Director script \"") + filePath + ("\"."), __FILE__, __LINE__);
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void BinaryWriter::SaveHeader(Director* director, FILE* file)
   {
      if (!director || !file)
      {
         throw dtUtil::Exception("Invalid data found.", __FILE__, __LINE__);
      }

      const std::string& utcTime = dtUtil::DateTime::ToString(dtUtil::DateTime(dtUtil::DateTime::TimeOrigin::LOCAL_TIME),
         dtUtil::DateTime::TimeFormat::CALENDAR_DATE_AND_TIME_FORMAT);

      // Version Info.
      float version = BINARY_SCRIPT_VERSION;
      fwrite(&version, sizeof(float), 1, file);

      // Creation timestamp.
      if (director->GetCreateDateTime().length() == 0)
      {
         director->SetCreateDateTime(utcTime);
      }
      SaveString(director->GetCreateDateTime(), file);

      // Current timestamp.
      SaveString(utcTime, file);

      // Script type.
      SaveString(director->GetScriptType(), file);

      // Imported Scripts.
      int count = (int)director->GetImportedScriptList().size();
      fwrite(&count, sizeof(int), 1, file);

      for (int index = 0; index < count; ++index)
      {
         std::string scriptId = "";
         dtDirector::Director* script = director->GetImportedScriptList()[index];
         if (script)
         {
            scriptId = script->GetResource().GetResourceIdentifier();
         }

         SaveString(scriptId, file);
      }

      // Node Libraries.
      const std::vector<std::string>& libs = director->GetAllLibraries();
      count = (int)libs.size();
      fwrite(&count, sizeof(int), 1, file);

      for (int index = 0; index < count; ++index)
      {
         std::string lib = libs[index];
         SaveString(lib, file);
      }

      // Properties.
      SavePropertyContainer(director, file);
   }

   //////////////////////////////////////////////////////////////////////////
   void BinaryWriter::SaveGraph(DirectorGraph* graph, FILE* file)
   {
      if (!graph || !file)
      {
         throw dtUtil::Exception("Invalid data found.", __FILE__, __LINE__);
      }

      // Imported.
      bool isImported = graph->IsImported();
      fwrite(&isImported, sizeof(bool), 1, file);

      // Read Only.
      bool isReadOnly = graph->IsReadOnly();
      fwrite(&isReadOnly, sizeof(bool), 1, file);

      // Graph ID.
      fwrite(&graph->GetID().index, sizeof(int), 1, file);
      SaveString(graph->GetID().id.ToString(), file);

      // Properties.
      SavePropertyContainer(graph, file);

      // Event Nodes.
      const std::vector<dtCore::RefPtr<EventNode> >& EventNodes = graph->GetEventNodes();
      int count = (int)EventNodes.size();
      fwrite(&count, sizeof(int), 1, file);
      for (int index = 0; index < count; index++)
      {
         SaveNode(EventNodes[index].get(), file);
      }

      // Action Nodes.
      const std::vector<dtCore::RefPtr<ActionNode> >& ActionNodes = graph->GetActionNodes();
      count = (int)ActionNodes.size();
      fwrite(&count, sizeof(int), 1, file);
      for (int index = 0; index < count; index++)
      {
         SaveNode(ActionNodes[index].get(), file);
      }

      // Value Nodes.
      const std::vector<dtCore::RefPtr<ValueNode> >& ValueNodes = graph->GetValueNodes();
      count = (int)ValueNodes.size();
      fwrite(&count, sizeof(int), 1, file);
      for (int index = 0; index < count; index++)
      {
         SaveNode(ValueNodes[index].get(), file);
      }

      // Sub Graphs.
      count = (int)graph->GetSubGraphs().size();
      fwrite(&count, sizeof(int), 1, file);
      for (int index = 0; index < count; index++)
      {
         SaveGraph(graph->GetSubGraphs()[index], file);
      }
   }

   //////////////////////////////////////////////////////////////////////////
   void BinaryWriter::SaveNode(Node* node, FILE* file)
   {
      // Node Type Name.
      SaveString(node->GetType().GetName(), file);

      // Node Category Name.
      SaveString(node->GetType().GetCategory(), file);

      // Node ID.
      fwrite(&node->GetID().index, sizeof(int), 1, file);
      SaveString(node->GetID().id.ToString(), file);

      // Properties.
      SavePropertyContainer(node, file);

      // Input Links.
      std::vector<InputLink>& inputs = node->GetInputLinks();
      int count = (int)inputs.size();
      int saveCount = 0;
      for (int index = 0; index < count; index++)
      {
         InputLink& input = inputs[index];
         if (!input.GetVisible())
         {
            saveCount++;
         }
      }

      fwrite(&saveCount, sizeof(int), 1, file);
      for (int index = 0; index < count; index++)
      {
         InputLink& input = inputs[index];
         if (!input.GetVisible())
         {
            // Input Name.
            SaveString(input.GetName(), file);
         }
      }

      // Output Links.
      std::vector<OutputLink>& outputs = node->GetOutputLinks();
      count = (int)outputs.size();
      saveCount = 0;
      for (int index = 0; index < count; index++)
      {
         OutputLink& output = outputs[index];
         if (!output.GetVisible() || !output.GetLinks().empty())
         {
            saveCount++;
         }
      }

      fwrite(&saveCount, sizeof(int), 1, file);
      for (int index = 0; index < count; index++)
      {
         OutputLink& output = outputs[index];
         bool visible = output.GetVisible();
         if (!visible || !output.GetLinks().empty())
         {
            // Output Name.
            SaveString(output.GetName(), file);

            // Output Visibility.
            fwrite(&visible, sizeof(bool), 1, file);

            //// Links.
            //std::vector<InputLink*>& links = output.GetLinks();
            //int linkCount = (int)links.size();
            //fwrite(&linkCount, sizeof(int), 1, file);
            //for (int linkIndex = 0; linkIndex < linkCount; linkIndex++)
            //{
            //   InputLink* link = links[linkIndex];
            //   if (link)
            //   {
            //      // Linked Node ID.
            //      SaveString(link->GetOwner()->GetID().ToString(), file);

            //      // Linked input name.
            //      SaveString(link->GetName(), file);
            //   }
            //   else
            //   {
            //      SaveString("", file);
            //   }
            //}
         }
      }

      // Value Links.
      std::vector<ValueLink>& values = node->GetValueLinks();
      count = (int)values.size();
      saveCount = 0;
      for (int index = 0; index < count; index++)
      {
         ValueLink& value = values[index];

         if (!value.GetVisible() || !value.GetExposed() || !value.GetLinks().empty())
         {
            saveCount++;
         }
      }

      fwrite(&saveCount, sizeof(int), 1, file);
      for (int index = 0; index < count; index++)
      {
         ValueLink& value = values[index];

         if (!value.GetVisible() || !value.GetExposed() || !value.GetLinks().empty())
         {
            // Link Name.
            SaveString(value.GetName(), file);

            // Link Visibility.
            bool visible = value.GetVisible();
            fwrite(&visible, sizeof(bool), 1, file);

            // Link Exposed.
            bool exposed = value.GetExposed();
            fwrite(&exposed, sizeof(bool), 1, file);

            //// Links.
            //const std::vector<ValueNode*>& links = value.GetLinks();
            //int linkCount = (int)links.size();
            //fwrite(&linkCount, sizeof(int), 1, file);
            //for (int linkIndex = 0; linkIndex < linkCount; linkIndex++)
            //{
            //   ValueNode* link = links[linkIndex];
            //   if (link)
            //   {
            //      // Linked Node ID.
            //      SaveString(link->GetID().ToString(), file);
            //   }
            //   else
            //   {
            //      SaveString("", file);
            //   }
            //}
         }
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void BinaryWriter::SaveLinkConnections(Director* script, FILE* file)
   {
      std::vector<ToLinkData> addedChainLinks;
      std::vector<ToLinkData> removedChainLinks;
      std::vector<ToLinkData> addedValueLinks;
      std::vector<ToLinkData> removedValueLinks;

      std::vector<Node*> nodes;
      script->GetAllNodes(nodes, true);

      int count = (int)nodes.size();
      for (int index = 0; index < count; ++index)
      {
         Node* myNode = nodes[index];
         if (myNode)
         {
            // Now iterate through all output links on this node.
            std::vector<OutputLink>& outputs = myNode->GetOutputLinks();
            for (int outputIndex = 0; outputIndex < (int)outputs.size(); outputIndex++)
            {
               OutputLink& output = outputs[outputIndex];

               // Find all links that were removed from the original imported node.
               std::vector<InputLink> removedLinks = myNode->GetRemovedImportedOutputLinkConnections(output.GetName());
               int removedCount = (int)removedLinks.size();
               for (int removedIndex = 0; removedIndex < removedCount; ++removedIndex)
               {
                  InputLink& link = removedLinks[removedIndex];

                  // If we get here, it means the link connection was removed and should be saved.
                  ToLinkData data;
                  data.outputNodeID   = myNode->GetID();
                  data.inputNodeID    = link.GetOwner()->GetID();
                  data.outputLinkName = output.GetName();
                  data.inputLinkName  = link.GetName();
                  removedChainLinks.push_back(data);
               }

               // Iterate through all connected links and check if they are new.
               std::vector<InputLink*>& links = output.GetLinks();
               for (int linkIndex = 0; linkIndex < (int)links.size(); linkIndex++)
               {
                  InputLink* input = links[linkIndex];
                  if (!input) continue;;

                  if (!myNode->IsOutputLinkImported(output.GetName(), input->GetOwner()->GetID(), input->GetName()))
                  {
                     ToLinkData data;
                     data.outputNodeID   = myNode->GetID();
                     data.inputNodeID    = input->GetOwner()->GetID();
                     data.outputLinkName = output.GetName();
                     data.inputLinkName  = input->GetName();
                     addedChainLinks.push_back(data);
                  }
               }
            }

            // Now iterate through all value links on this node.
            std::vector<ValueLink>& values = myNode->GetValueLinks();
            for (int valueIndex = 0; valueIndex < (int)values.size(); valueIndex++)
            {
               ValueLink& valueLink = values[valueIndex];

               // Find all links that were removed from the original imported node.
               std::vector<ValueNode*> removedLinks = myNode->GetRemovedImportedValueLinkConnections(valueLink.GetName());
               int removedCount = (int)removedLinks.size();
               for (int removedIndex = 0; removedIndex < removedCount; ++removedIndex)
               {
                  ValueNode* link = removedLinks[removedIndex];

                  // If we get here, it means the link connection was removed and should be saved.
                  ToLinkData data;
                  data.outputNodeID   = myNode->GetID();
                  data.inputNodeID    = link->GetID();
                  data.outputLinkName = valueLink.GetName();
                  removedValueLinks.push_back(data);
               }

               // Iterate through all connected links and check if they are new.
               std::vector<ValueNode*>& links = valueLink.GetLinks();
               for (int linkIndex = 0; linkIndex < (int)links.size(); linkIndex++)
               {
                  ValueNode* link = links[linkIndex];
                  if (!link) continue;;

                  if (!myNode->IsValueLinkImported(valueLink.GetName(), link->GetID()))
                  {
                     ToLinkData data;
                     data.outputNodeID   = myNode->GetID();
                     data.inputNodeID    = link->GetID();
                     data.outputLinkName = valueLink.GetName();
                     addedValueLinks.push_back(data);
                  }
               }
            }
         }
      }

      // Output all added chain link connections.
      count = (int)addedChainLinks.size();
      fwrite(&count, sizeof(int), 1, file);
      for (int index = 0; index < count; ++index)
      {
         ToLinkData& data = addedChainLinks[index];

         // Output link owner.
         fwrite(&data.outputNodeID.index, sizeof(int), 1, file);
         SaveString(data.outputNodeID.id.ToString(), file);

         // Input link owner.
         fwrite(&data.inputNodeID.index, sizeof(int), 1, file);
         SaveString(data.inputNodeID.id.ToString(), file);

         // Output link name.
         SaveString(data.outputLinkName, file);

         // Input link name.
         SaveString(data.inputLinkName, file);
      }

      // Output all added value link connections.
      count = (int)addedValueLinks.size();
      fwrite(&count, sizeof(int), 1, file);
      for (int index = 0; index < count; ++index)
      {
         ToLinkData& data = addedValueLinks[index];

         // Output link owner.
         fwrite(&data.outputNodeID.index, sizeof(int), 1, file);
         SaveString(data.outputNodeID.id.ToString(), file);

         // Input link owner.
         fwrite(&data.inputNodeID.index, sizeof(int), 1, file);
         SaveString(data.inputNodeID.id.ToString(), file);

         // Output link name.
         SaveString(data.outputLinkName, file);
      }

      // Output all removed chain link connections.
      count = (int)removedChainLinks.size();
      fwrite(&count, sizeof(int), 1, file);
      for (int index = 0; index < count; ++index)
      {
         ToLinkData& data = removedChainLinks[index];

         // Output link owner.
         fwrite(&data.outputNodeID.index, sizeof(int), 1, file);
         SaveString(data.outputNodeID.id.ToString(), file);

         // Input link owner.
         fwrite(&data.inputNodeID.index, sizeof(int), 1, file);
         SaveString(data.inputNodeID.id.ToString(), file);

         // Output link name.
         SaveString(data.outputLinkName, file);

         // Input link name.
         SaveString(data.inputLinkName, file);
      }

      // Output all removed value link connections.
      count = (int)removedValueLinks.size();
      fwrite(&count, sizeof(int), 1, file);
      for (int index = 0; index < count; ++index)
      {
         ToLinkData& data = removedValueLinks[index];

         // Output link owner.
         fwrite(&data.outputNodeID.index, sizeof(int), 1, file);
         SaveString(data.outputNodeID.id.ToString(), file);

         // Input link owner.
         fwrite(&data.inputNodeID.index, sizeof(int), 1, file);
         SaveString(data.inputNodeID.id.ToString(), file);

         // Output link name.
         SaveString(data.outputLinkName, file);
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void BinaryWriter::SavePropertyContainer(dtCore::PropertyContainer* container, FILE* file)
   {
      if (!container || !file)
      {
         throw dtUtil::Exception("Invalid data found.", __FILE__, __LINE__);
      }

      std::vector<dtCore::ActorProperty*> propList;
      container->GetPropertyList(propList);

      std::vector<dtCore::ActorProperty*> saveList;

      int saveCount = 0;
      int count = (int)propList.size();
      for (int index = 0; index < count; ++index)
      {
         dtCore::ActorProperty* prop = propList[index];
         if (prop && container->ShouldPropertySave(*prop))
         {
            saveList.push_back(prop);
            saveCount++;
         }
      }
      fwrite(&saveCount, sizeof(int), 1, file);

      for (int index = 0; index < saveCount; ++index)
      {
         dtCore::ActorProperty* prop = saveList[index];
         SaveString(prop->GetName(), file);
         SaveString(prop->ToString(), file);
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void BinaryWriter::SaveString(const std::string& str, FILE* file)
   {
      if (!file)
      {
         throw dtUtil::Exception("Invalid data found.", __FILE__, __LINE__);
      }

      int count = (int)str.size();
      fwrite(&count, sizeof(int), 1, file);

      if (count > 0)
      {
         fwrite(str.c_str(), sizeof(char), count, file);
      }
   }
}

//////////////////////////////////////////////////////////////////////////

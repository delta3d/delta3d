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

#include <prefix/dtdalprefix.h>

#include <osgDB/FileNameUtils>

#include <dtDirector/director.h>
#include <dtDirector/directorbinary.h>
#include <dtDirector/node.h>
#include <dtDirector/nodetype.h>
#include <dtDirector/nodemanager.h>

#include <dtDAL/map.h>
#include <dtDAL/exceptionenum.h>

#include <dtUtil/datapathutils.h>
#include <dtUtil/fileutils.h>
#include <dtUtil/datetime.h>
#include <dtUtil/log.h>

#define BINARY_SCRIPT_VERSION 1.0f

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
   void BinaryParser::Parse(Director* director, dtDAL::Map* map, const std::string& filePath)
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

         ParseGraph(version, director->GetGraphRoot(), file);

         LinkNodes(director);
         fclose(file);
      }
      catch (dtUtil::Exception& ex)
      {
         dtUtil::Log::GetInstance().LogMessage(dtUtil::Log::LOG_ERROR, __FUNCTION__, __LINE__,
            "Caught Exception \"%s\" while attempting to load Director script \"%s\".",
            ex.What().c_str(), filePath.c_str());
         fclose(file);
         throw ex;
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
         float version = ParseHeader(NULL, file);
         fclose(file);
      }
      catch (dtUtil::Exception& ex)
      {
         dtUtil::Log::GetInstance().LogMessage(dtUtil::Log::LOG_ERROR, __FUNCTION__, __LINE__,
            "Caught Exception \"%s\" while attempting to load Director script \"%s\".",
            ex.What().c_str(), filePath.c_str());
         fclose(file);
         throw ex;
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

      // Node Libraries.
      int count = 0;
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
            catch (const dtDAL::ProjectResourceErrorException& e)
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
   void BinaryParser::ParseGraph(float version, DirectorGraph* graph, FILE* file)
   {
      if (!graph || !file)
      {
         throw dtUtil::Exception("Invalid data found.", __FILE__, __LINE__);
      }

      // Graph ID.
      graph->SetID(dtCore::UniqueId(ParseString(file)));

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
         dtCore::RefPtr<DirectorGraph> newGraph = new DirectorGraph(graph->GetDirector());
         if (!newGraph.valid())
         {
            throw dtUtil::Exception("Unable to allocate the necessary memory to create a macro.", __FILE__, __LINE__);
         }

         newGraph->SetParent(graph);
         newGraph->BuildPropertyMap();
         graph->GetSubGraphs().push_back(newGraph);

         ParseGraph(version, newGraph, file);
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

      dtDirector::NodeManager& nodeManager = dtDirector::NodeManager::GetInstance();
      dtCore::RefPtr<Node> newNode = nodeManager.CreateNode(typeName, typeCategory, graph);
      if (!newNode.valid())
      {
         std::string nodeType = typeCategory + "." + typeName;
         if (mMissingNodeTypes.find(nodeType) == mMissingNodeTypes.end())
         {
            mMissingNodeTypes.insert(mMissingNodeTypes.end(), nodeType);
         }
      }

      // Node ID.
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

         // Links.
         int linkCount = 0;
         fread(&linkCount, sizeof(int), 1, file);
         for (int linkIndex = 0; linkIndex < linkCount; linkIndex++)
         {
            // Linked Node ID.
            std::string linkedID = ParseString(file);

            if (!linkedID.empty())
            {
               // Linked Input Name.
               std::string linkedName = ParseString(file);

               if (link)
               {
                  ToLinkData data;
                  data.outputLink = link;
                  data.linkNodeID = linkedID;
                  data.linkToName = linkedName;
                  mLinkList.push_back(data);
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

         // Links.
         int linkCount = 0;
         fread(&linkCount, sizeof(int), 1, file);
         for (int linkIndex = 0; linkIndex < linkCount; linkIndex++)
         {
            // Linked Node ID.
            std::string linkedID = ParseString(file);

            if (!linkedID.empty() && link)
            {
               ToLinkData data;
               data.valueLink  = link;
               data.linkNodeID = linkedID;
               mLinkList.push_back(data);
            }
         }
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void BinaryParser::ParsePropertyContainer(dtDAL::PropertyContainer* container, FILE* file)
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
            dtCore::RefPtr<dtDAL::ActorProperty> prop = container->GetProperty(propName);
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
         dtCore::RefPtr<Node> linkNode = director->GetNode(dtCore::UniqueId(mLinkList[index].linkNodeID));

         // If we have both an ID and a name, we can link them.
         if (linkNode.valid())
         {
            // Connect a value link to a value node.
            if (mLinkList[index].valueLink)
            {
               ValueNode* valueNode = linkNode->AsValueNode();
               if (!mLinkList[index].valueLink->Connect(valueNode))
               {
                  // If the connection failed, it may require another link
                  // connection before it can be made.  Add to the failed
                  // list and try this again later.
                  failedLinks.push_back(mLinkList[index]);
               }
            }
            else if (!mLinkList[index].linkToName.empty())
            {
               // Connect an output link to an input link.
               if (mLinkList[index].outputLink)
               {
                  InputLink* link = linkNode->GetInputLink(mLinkList[index].linkToName);
                  if (link) link->Connect(mLinkList[index].outputLink);
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
         throw dtDAL::MapSaveException( std::string("Unable to open Director Script file \"") + filePath + "\" for writing.", __FILE__, __LINE__);
      }

      try
      {
         SaveHeader(director, file);

         SaveGraph(director->GetGraphRoot(), file);

         fclose(file);
      }
      catch (dtUtil::Exception& ex)
      {
         dtUtil::Log::GetInstance().LogMessage(dtUtil::Log::LOG_ERROR, __FUNCTION__, __LINE__,
                             "Caught Exception \"%s\" while attempting to save Director script \"%s\".",
                             ex.What().c_str(), filePath.c_str());

         fclose(file);

         throw ex;
      }
      catch (...)
      {
         dtUtil::Log::GetInstance().LogMessage(dtUtil::Log::LOG_ERROR, __FUNCTION__, __LINE__,
                             "Unknown exception while attempting to save Director script \"%s\".",
                             filePath.c_str());

         fclose(file);

         throw dtDAL::MapSaveException( std::string("Unknown exception saving Director script \"") + filePath + ("\"."), __FILE__, __LINE__);
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

      // Node Libraries.
      const std::vector<std::string>& libs = director->GetAllLibraries();
      int count = (int)libs.size();
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

      // Graph ID.
      SaveString(graph->GetID().ToString(), file);

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
      SaveString(node->GetID().ToString(), file);

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

            // Links.
            std::vector<InputLink*>& links = output.GetLinks();
            int linkCount = (int)links.size();
            fwrite(&linkCount, sizeof(int), 1, file);
            for (int linkIndex = 0; linkIndex < linkCount; linkIndex++)
            {
               InputLink* link = links[linkIndex];
               if (link)
               {
                  // Linked Node ID.
                  SaveString(link->GetOwner()->GetID().ToString(), file);

                  // Linked input name.
                  SaveString(link->GetName(), file);
               }
               else
               {
                  SaveString("", file);
               }
            }
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

            // Links.
            std::vector<ValueNode*>& links = value.GetLinks();
            int linkCount = (int)links.size();
            fwrite(&linkCount, sizeof(int), 1, file);
            for (int linkIndex = 0; linkIndex < linkCount; linkIndex++)
            {
               ValueNode* link = links[linkIndex];
               if (link)
               {
                  // Linked Node ID.
                  SaveString(link->GetID().ToString(), file);
               }
               else
               {
                  SaveString("", file);
               }
            }
         }
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void BinaryWriter::SavePropertyContainer(dtDAL::PropertyContainer* container, FILE* file)
   {
      if (!container || !file)
      {
         throw dtUtil::Exception("Invalid data found.", __FILE__, __LINE__);
      }

      std::vector<dtDAL::ActorProperty*> propList;
      container->GetPropertyList(propList);

      std::vector<dtDAL::ActorProperty*> saveList;

      int saveCount = 0;
      int count = (int)propList.size();
      for (int index = 0; index < count; ++index)
      {
         dtDAL::ActorProperty* prop = propList[index];
         if (prop && container->ShouldPropertySave(*prop))
         {
            saveList.push_back(prop);
            saveCount++;
         }
      }
      fwrite(&saveCount, sizeof(int), 1, file);

      for (int index = 0; index < saveCount; ++index)
      {
         dtDAL::ActorProperty* prop = saveList[index];
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

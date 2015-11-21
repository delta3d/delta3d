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

#include <cstdio>
#include <cstdlib>
#include <string>
#include <cmath>

#include <dtUtil/deprecationmgr.h>
DT_DISABLE_WARNING_ALL_START

#include <xercesc/util/XMLString.hpp>
#include <xercesc/util/OutOfMemoryException.hpp>
#include <xercesc/sax2/XMLReaderFactory.hpp>
#include <xercesc/framework/LocalFileInputSource.hpp>
#include <xercesc/sax/SAXParseException.hpp>

#include <osgDB/FileNameUtils>
DT_DISABLE_WARNING_END

#include <dtCore/exceptionenum.h>
#include <dtCore/map.h>
#include <dtCore/mapxmlconstants.h>
#include <dtCore/transformable.h>
#include <dtCore/transform.h>

#include <dtDirector/director.h>
#include <dtDirector/directorxml.h>
#include <dtDirector/node.h>
#include <dtDirector/nodetype.h>
#include <dtDirector/directortypefactory.h>

#include <dtUtil/datapathutils.h>
#include <dtUtil/fileutils.h>
#include <dtUtil/datetime.h>
#include <dtUtil/xercesutils.h>
#include <dtUtil/log.h>

#include <osgDB/FileNameUtils>

#include <iostream>
#include <fstream>

XERCES_CPP_NAMESPACE_USE

namespace dtDirector
{
   static const std::string logName("directorxml.cpp");

   /////////////////////////////////////////////////////////////////
   DirectorParser::DirectorParser()
      : dtCore::BaseXMLParser()
      , mDirectorHandler(new DirectorXMLHandler())
   {
      SetHandler(mDirectorHandler);

      mXercesParser->setFeature(XMLUni::fgSAX2CoreValidation, true);
      mXercesParser->setFeature(XMLUni::fgXercesDynamic, false);

      mXercesParser->setFeature(XMLUni::fgSAX2CoreNameSpaces, true);
      mXercesParser->setFeature(XMLUni::fgXercesSchema, true);
      mXercesParser->setFeature(XMLUni::fgXercesSchemaFullChecking, true);
      mXercesParser->setFeature(XMLUni::fgSAX2CoreNameSpacePrefixes, true);
      mXercesParser->setFeature(XMLUni::fgXercesUseCachedGrammarInParse, true);
      mXercesParser->setFeature(XMLUni::fgXercesCacheGrammarFromParse, true);

      std::string schemaFileName = dtUtil::FindFileInPathList("director.xsd");

      if (!dtUtil::FileUtils::GetInstance().FileExists(schemaFileName))
      {
         throw dtCore::ProjectException( "Unable to load required file \"director.xsd\", can not load director script.", __FILE__, __LINE__);
      }

      XMLCh* value = XMLString::transcode(schemaFileName.c_str());
      LocalFileInputSource inputSource(value);
      //cache the schema
      mXercesParser->loadGrammar(inputSource, Grammar::SchemaGrammarType, true);
      XMLString::release(&value);
   }

   /////////////////////////////////////////////////////////////////
   DirectorParser::~DirectorParser()
   {
   }

   /////////////////////////////////////////////////////////////////
   bool DirectorParser::Parse(Director* director, dtCore::Map* map, const std::string& filePath)
   {
      mDirectorHandler->SetDirector(director);
      mDirectorHandler->SetMap(map);
      if (dtCore::BaseXMLParser::Parse(filePath))
      {
         return true;
      }

      return false;
   }

   ////////////////////////////////////////////////////////////////////////////////
   DirectorPtr DirectorParser::ParseDirectorHeaderData(const std::string& directorFilename) const
   {
      DirectorPtr result = new Director();
      mDirectorHandler->SetDirector(result);
      mDirectorHandler->SetMap(NULL);

      bool parserNeedsReset = false;
      XMLPScanToken token;

      mXercesParser->setContentHandler(mDirectorHandler);
      mXercesParser->setErrorHandler(mDirectorHandler);

      try
      {
         std::ifstream fileStream;
         fileStream.open(directorFilename.c_str());
         if(fileStream.fail())
         {
            throw dtUtil::Exception("Failed to find Director script file \'" + directorFilename + "\'.", __FILE__, __LINE__);
         }

         dtCore::InputSourcefStream xerStream(fileStream);

         if (mXercesParser->parseFirst(xerStream, token))
         {
            parserNeedsReset = true;

            bool cont = mXercesParser->parseNext(token);
            while (cont && !mDirectorHandler->HasParsedHeader())
            {
               cont = mXercesParser->parseNext(token);
            }

            parserNeedsReset = false;

            //reset the parser and close the file handles.
            mXercesParser->parseReset(token);

            if (mDirectorHandler->HasParsedHeader())
            {
               mLogger->LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__,  __LINE__, "Parsing complete.");
            }
            else
            {
               throw dtUtil::Exception( "Parser stopped without finding the script type.", __FILE__, __LINE__);
            }
         }
         else
         {
            throw dtUtil::Exception( "Parsing to find the script type did not begin.", __FILE__, __LINE__);
         }
      }
      catch (const OutOfMemoryException&)
      {
         if (parserNeedsReset)
         {
            mXercesParser->parseReset(token);
         }

         mLogger->LogMessage(dtUtil::Log::LOG_ERROR, __FUNCTION__,  __LINE__, "Ran out of memory parsing!");
         throw dtUtil::Exception( "Ran out of memory parsing save file.", __FILE__, __LINE__);
      }
      catch (const XMLException& toCatch)
      {
         if (parserNeedsReset)
            mXercesParser->parseReset(token);

         mLogger->LogMessage(dtUtil::Log::LOG_ERROR, __FUNCTION__,  __LINE__, "Error during parsing! %ls :\n",
            toCatch.getMessage());
         throw dtUtil::Exception( "Error while parsing Director script file. See log for more information.", __FILE__, __LINE__);
      }
      catch (const SAXParseException&)
      {
         if (parserNeedsReset)
         {
            mXercesParser->parseReset(token);
         }

         //this will already by logged by the content handler
         throw dtUtil::Exception( "Error while parsing Director script file. See log for more information.", __FILE__, __LINE__);
      }
      catch (const dtUtil::Exception& e)
      {
         if (parserNeedsReset)
         {
            mXercesParser->parseReset(token);
         }

         throw e;
      }

      return result;
   }

   /////////////////////////////////////////////////////////////////
   const std::set<std::string>& DirectorParser::GetMissingNodeTypes()
   {
      return mDirectorHandler->GetMissingNodeTypes();
   }

   /////////////////////////////////////////////////////////////////
   const std::vector<std::string>& DirectorParser::GetMissingLibraries()
   {
      return mDirectorHandler->GetMissingLibraries();
   }

   ////////////////////////////////////////////////////////////////////////////////
   const std::vector<std::string>& DirectorParser::GetMissingImportedScripts()
   {
      return mDirectorHandler->GetMissingImportedScripts();
   }

   ////////////////////////////////////////////////////////////////////////////////
   bool DirectorParser::HasDeprecatedProperty() const
   {
      return mDirectorHandler->HasDeprecatedProperty();
   }

   //////////////////////////////////////////////////////////////////////////
   //////////////////////////////////////////////////////////////////////////

   //////////////////////////////////////////////////////////////////////////
   DirectorWriter::DirectorWriter()
      : BaseXMLWriter()
   {
      mPropSerializer = new dtCore::ActorPropertySerializer(this);
   }

   //////////////////////////////////////////////////////////////////////////
   DirectorWriter::~DirectorWriter()
   {
      delete mPropSerializer; mPropSerializer = NULL;
   }

   /////////////////////////////////////////////////////////////////
   void DirectorWriter::Save(Director* director, const std::string& filePath)
   {
      if (!director) return;
      mPropSerializer->Reset();
      mPropSerializer->SetMap(director->GetMap());
      mPropSerializer->SetCurrentPropertyContainer(director);

      std::ofstream stream(filePath.c_str(), std::ios_base::trunc|std::ios_base::binary);
      if (!stream.is_open())
      {
         throw dtCore::MapSaveException( std::string("Unable to open Director Script file \"") + filePath + "\" for writing.", __FILE__, __LINE__);
      }

      mFormatTarget.SetOutputStream(&stream);

      try
      {
         director->CleanIDs();
         WriteHeader();

         const std::string& utcTime = dtUtil::DateTime::ToString(dtUtil::DateTime(dtUtil::DateTime::TimeOrigin::LOCAL_TIME),
            dtUtil::DateTime::TimeFormat::CALENDAR_DATE_AND_TIME_FORMAT);

         // Director.
         BeginElement(dtCore::MapXMLConstants::DIRECTOR_ELEMENT, dtCore::MapXMLConstants::DIRECTOR_NAMESPACE);
         {
            // Header.
            BeginElement(dtCore::MapXMLConstants::HEADER_ELEMENT);
            {
               // Creation timestamp.
               BeginElement(dtCore::MapXMLConstants::CREATE_TIMESTAMP_ELEMENT);
               {
                  if (director->GetCreateDateTime().length() == 0)
                  {
                     director->SetCreateDateTime(utcTime);
                  }
                  AddCharacters(director->GetCreateDateTime());
               }
               EndElement(); // End Create Timestamp Element.

               // Last update timestamp.
               BeginElement(dtCore::MapXMLConstants::LAST_UPDATE_TIMESTAMP_ELEMENT);
               {
                  AddCharacters(utcTime);
               }
               EndElement(); // End Last Update Timestamp Element.

               // Editor version.
               BeginElement(dtCore::MapXMLConstants::EDITOR_VERSION_ELEMENT);
               {
                  AddCharacters(std::string(dtCore::MapXMLConstants::EDITOR_VERSION));
               }
               EndElement(); // End Editor Version Element.

               // Schema Version.
               BeginElement(dtCore::MapXMLConstants::SCHEMA_VERSION_ELEMENT);
               {
                  AddCharacters(std::string(dtCore::MapXMLConstants::DIRECTOR_SCHEMA_VERSION));
               }
               EndElement(); // End Schema Version Element.

               // Script Type.
               BeginElement(dtCore::MapXMLConstants::DIRECTOR_SCRIPT_TYPE);
               {
                  AddCharacters(director->GetScriptType());
               }
               EndElement();

               // Inheritances
               int count = (int)director->GetImportedScriptList().size();
               for (int index = 0; index < count; ++index)
               {
                  Director* imported = director->GetImportedScriptList()[index];
                  if (imported)
                  {
                     BeginElement(dtCore::MapXMLConstants::DIRECTOR_IMPORTED_SCRIPT);
                     {
                        AddCharacters(imported->GetResource().GetResourceIdentifier());
                     }
                     EndElement();
                  }
               }

               // Properties.
               std::vector<const dtCore::ActorProperty*> propList;
               director->GetPropertyList(propList);
               for (std::vector<const dtCore::ActorProperty*>::const_iterator i = propList.begin();
                  i != propList.end(); ++i)
               {
                  const dtCore::ActorProperty& property = *(*i);

                  // If the property is read only, skip it
                  if (property.IsReadOnly()) continue;

                  mPropSerializer->WriteProperty(property);
               }
            }
            EndElement(); // End Header Element.

            // Node Libraries.
            BeginElement(dtCore::MapXMLConstants::LIBRARIES_ELEMENT);
            {
               const std::vector<std::string>& libs = director->GetAllLibraries();
               for (std::vector<std::string>::const_iterator i = libs.begin(); i != libs.end(); ++i)
               {
                  // Library.
                  BeginElement(dtCore::MapXMLConstants::LIBRARY_ELEMENT);
                  {
                     // Library name.
                     BeginElement(dtCore::MapXMLConstants::NAME_ELEMENT);
                     {
                        AddCharacters(*i);
                     }
                     EndElement(); // End Library Name Element.

                     // Library version.
                     BeginElement(dtCore::MapXMLConstants::LIBRARY_VERSION_ELEMENT);
                     {
                        AddCharacters(director->GetLibraryVersion(*i));
                     }
                     EndElement(); // End Library Version Element.
                  }
                  EndElement(); // End Library Element.
               }
            }
            EndElement(); // End Libraries Element.

            SaveGraphs(director->GetGraphRoot());

            SaveLinkConnections(director);
         }
         EndElement(); // End Director Element.

         //closes the file.
         mFormatTarget.SetOutputStream(NULL);
         mPropSerializer->SetMap(NULL);
      }
      catch (dtUtil::Exception& ex)
      {
         dtUtil::Log::GetInstance().LogMessage(dtUtil::Log::LOG_ERROR, __FUNCTION__, __LINE__,
                             "Caught Exception \"%s\" while attempting to save Director script \"%s\".",
                             ex.What().c_str(), director->GetName().c_str());
         mFormatTarget.SetOutputStream(NULL);
         throw;
      }
      catch (...)
      {
         dtUtil::Log::GetInstance().LogMessage(dtUtil::Log::LOG_ERROR, __FUNCTION__, __LINE__,
                             "Unknown exception while attempting to save Director script \"%s\".",
                             director->GetName().c_str());
         mFormatTarget.SetOutputStream(NULL);
         throw dtCore::MapSaveException( std::string("Unknown exception saving Director script \"") + director->GetName() + ("\"."), __FILE__, __LINE__);
      }
   }

   //////////////////////////////////////////////////////////////////////////
   void DirectorWriter::SaveGraphs(DirectorGraph* graph)
   {
      mPropSerializer->SetCurrentPropertyContainer(graph);

      // Graph.
      BeginElement(dtCore::MapXMLConstants::DIRECTOR_GRAPH_ELEMENT);
      {
         // Imported.
         if (graph->IsImported())
         {
            BeginElement(dtCore::MapXMLConstants::DIRECTOR_IMPORTED_SCRIPT);
            {
               AddCharacters("true");
            }
            EndElement(); // End Imported.
         }

         // Read Only.
         if (graph->IsReadOnly())
         {
            BeginElement(dtCore::MapXMLConstants::DIRECTOR_READ_ONLY);
            {
               AddCharacters("true");
            }
            EndElement(); // End Read Only.
         }

         // Graph ID.
         BeginElement(dtCore::MapXMLConstants::ID_INDEX_ELEMENT);
         {
            AddCharacters(dtUtil::ToString(graph->GetID().index));
         }
         EndElement(); // End Node ID Elements.

         BeginElement(dtCore::MapXMLConstants::ID_ELEMENT);
         {
            AddCharacters(graph->GetID().id.ToString());
         }
         EndElement(); // End Node ID Elements.

         // Properties.
         std::vector<const dtCore::ActorProperty*> propList;
         graph->GetPropertyList(propList);
         for (std::vector<const dtCore::ActorProperty*>::const_iterator i = propList.begin();
            i != propList.end(); ++i)
         {
            const dtCore::ActorProperty& property = *(*i);

            // If the property is read only, skip it
            if (property.IsReadOnly()) continue;

            mPropSerializer->WriteProperty(property);
         }

         // Event Nodes.
         BeginElement(dtCore::MapXMLConstants::DIRECTOR_EVENT_NODES_ELEMENT);
         {
            const std::vector<dtCore::RefPtr<EventNode> >& EventNodes = graph->GetEventNodes();
            for (int nodeIndex = 0; nodeIndex < (int)EventNodes.size(); nodeIndex++)
            {
               SaveNode(EventNodes[nodeIndex].get());
            }
         }
         EndElement(); // End Event Nodes Element.

         // Action Nodes.
         BeginElement(dtCore::MapXMLConstants::DIRECTOR_ACTION_NODES_ELEMENT);
         {
            const std::vector<dtCore::RefPtr<ActionNode> >& ActionNodes = graph->GetActionNodes();
            for (int nodeIndex = 0; nodeIndex < (int)ActionNodes.size(); nodeIndex++)
            {
               SaveNode(ActionNodes[nodeIndex].get());
            }
         }
         EndElement(); // End Action Nodes Element.

         // Value Nodes.
         BeginElement(dtCore::MapXMLConstants::DIRECTOR_VALUE_NODES_ELEMENT);
         {
            const std::vector<dtCore::RefPtr<ValueNode> >& ValueNodes = graph->GetValueNodes();
            for (int nodeIndex = 0; nodeIndex < (int)ValueNodes.size(); nodeIndex++)
            {
               SaveNode(ValueNodes[nodeIndex].get());
            }
         }
         EndElement(); // End Value Nodes Element.

         // Now save sub graphs.
         for (int graphIndex = 0; graphIndex < (int)graph->GetSubGraphs().size(); graphIndex++)
         {
            SaveGraphs(graph->GetSubGraphs()[graphIndex]);
         }
      }
      EndElement(); // End Graph.
   }

   //////////////////////////////////////////////////////////////////////////
   void DirectorWriter::SaveNode(Node* node)
   {
      mPropSerializer->SetCurrentPropertyContainer(node);

      BeginElement(dtCore::MapXMLConstants::DIRECTOR_NODE_ELEMENT);
      {
         // Node Name.
         BeginElement(dtCore::MapXMLConstants::NAME_ELEMENT);
         {
            AddCharacters(node->GetType().GetName());
         }
         EndElement(); // End Node Name Element.

         // Node Category.
         BeginElement(dtCore::MapXMLConstants::CATEGORY_ELEMENT);
         {
            AddCharacters(node->GetType().GetCategory());
         }
         EndElement(); // End Node Category Element.

         // Node ID.
         BeginElement(dtCore::MapXMLConstants::ID_INDEX_ELEMENT);
         {
            AddCharacters(dtUtil::ToString(node->GetID().index));
         }
         EndElement(); // End Node ID Elements.

         BeginElement(dtCore::MapXMLConstants::ID_ELEMENT);
         {
            AddCharacters(node->GetID().id.ToString());
         }
         EndElement(); // End Node ID Elements.

         // Properties.
         std::vector<const dtCore::ActorProperty*> propList;
         node->GetPropertyList(propList);
         for (std::vector<const dtCore::ActorProperty*>::const_iterator i = propList.begin();
              i != propList.end(); ++i)
         {
            const dtCore::ActorProperty& property = *(*i);

            // If the property is read only, skip it
            if (property.IsReadOnly()) continue;

            mPropSerializer->WriteProperty(property);
         }

         // Input Links.
         std::vector<InputLink>& inputs = node->GetInputLinks();
         for (int inputIndex = 0; inputIndex < (int)inputs.size(); inputIndex++)
         {
            BeginElement(dtCore::MapXMLConstants::DIRECTOR_LINKS_INPUT_ELEMENT);
            {
               InputLink& input = inputs[inputIndex];

               // Link Name.
               BeginElement(dtCore::MapXMLConstants::NAME_ELEMENT);
               {
                  AddCharacters(input.GetName());
               }
               EndElement(); // End Link Name.

               // Visibility
               if (!input.GetVisible())
               {
                  BeginElement(dtCore::MapXMLConstants::DIRECTOR_LINK_VISIBLE_ELEMENT);
                  {
                     AddCharacters("false");
                  }
                  EndElement(); // End Visibility.
               }
            }
            EndElement(); // End Input Links Element.
         }

         // Output Links.
         std::vector<OutputLink>& outputs = node->GetOutputLinks();
         for (int outputIndex = 0; outputIndex < (int)outputs.size(); outputIndex++)
         {
            BeginElement(dtCore::MapXMLConstants::DIRECTOR_LINKS_OUTPUT_ELEMENT);
            {
               OutputLink& output = outputs[outputIndex];

               // Link Name.
               BeginElement(dtCore::MapXMLConstants::NAME_ELEMENT);
               {
                  AddCharacters(output.GetName());
               }
               EndElement(); // End Link Name.

               // Visibility
               if (!output.GetVisible())
               {
                  BeginElement(dtCore::MapXMLConstants::DIRECTOR_LINK_VISIBLE_ELEMENT);
                  {
                     AddCharacters("false");
                  }
                  EndElement(); // End Visibility.
               }
            }
            EndElement(); // End Output Links Element.
         }

         // Value Links.
         std::vector<ValueLink>& values = node->GetValueLinks();
         for (int valueIndex = 0; valueIndex < (int)values.size(); valueIndex++)
         {
            BeginElement(dtCore::MapXMLConstants::DIRECTOR_LINKS_VALUE_ELEMENT);
            {
               ValueLink& value = values[valueIndex];

               // Link Name.
               BeginElement(dtCore::MapXMLConstants::NAME_ELEMENT);
               {
                  AddCharacters(value.GetName());
               }
               EndElement(); // End Link Name.

               // Visibility
               if (value.GetVisible() == false)
               {
                  BeginElement(dtCore::MapXMLConstants::DIRECTOR_LINK_VISIBLE_ELEMENT);
                  {
                     AddCharacters("false");
                  }
                  EndElement(); // End Visibility.
               }

               // Exposed.
               BeginElement(dtCore::MapXMLConstants::DIRECTOR_LINK_EXPOSED_ELEMENT);
               {
                  AddCharacters(value.GetExposed()? "true": "false");
               }
               EndElement(); // End Exposed.

               // Is Out Value.
               if (value.IsOutLink())
               {
                  BeginElement(dtCore::MapXMLConstants::DIRECTOR_LINK_VALUE_IS_OUT_ELEMENT);
                  {
                     AddCharacters("true");
                  }
                  EndElement(); // End Is Out Value.
               }

               // Allow Multiple values.
               if (value.AllowMultiple())
               {
                  BeginElement(dtCore::MapXMLConstants::DIRECTOR_LINK_VALUE_ALLOW_MULTIPLE_ELEMENT);
                  {
                     AddCharacters("true");
                  }
                  EndElement(); // End Allow Multiple values.
               }

               // Type Checking.
               if (!value.IsTypeChecking())
               {
                  BeginElement(dtCore::MapXMLConstants::DIRECTOR_LINK_VALUE_TYPE_CHECK_ELEMENT);
                  {
                     AddCharacters("false");
                  }
                  EndElement(); // End Type Checking.
               }
            }
            EndElement(); // End Value Links Element.
         }
      }
      EndElement(); // End Director Node Element.
   }

   ////////////////////////////////////////////////////////////////////////////////
   void DirectorWriter::SaveLinkConnections(Director* script)
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
      for (int index = 0; index < count; ++index)
      {
         ToLinkData& data = addedChainLinks[index];

         BeginElement(dtCore::MapXMLConstants::DIRECTOR_LINK_CHAIN_CONNECTION);
         {
            // Output link owner.
            BeginElement(dtCore::MapXMLConstants::DIRECTOR_LINK_OUTPUT_OWNER_INDEX_ELEMENT);
            {
               AddCharacters(dtUtil::ToString(data.outputNodeID.index));
            }
            EndElement(); // End output link owner.

            BeginElement(dtCore::MapXMLConstants::DIRECTOR_LINK_OUTPUT_OWNER_ELEMENT);
            {
               AddCharacters(data.outputNodeID.id.ToString());
            }
            EndElement(); // End output link owner.

            // Input link owner.
            BeginElement(dtCore::MapXMLConstants::DIRECTOR_LINK_INPUT_OWNER_INDEX_ELEMENT);
            {
               AddCharacters(dtUtil::ToString(data.inputNodeID.index));
            }
            EndElement(); // End input link owner.

            BeginElement(dtCore::MapXMLConstants::DIRECTOR_LINK_INPUT_OWNER_ELEMENT);
            {
               AddCharacters(data.inputNodeID.id.ToString());
            }
            EndElement(); // End input link owner.

            // Output link name.
            BeginElement(dtCore::MapXMLConstants::DIRECTOR_LINK_OUTPUT_NAME_ELEMENT);
            {
               AddCharacters(data.outputLinkName);
            }
            EndElement(); // End output link name.

            // Input link name.
            BeginElement(dtCore::MapXMLConstants::DIRECTOR_LINK_INPUT_NAME_ELEMENT);
            {
               AddCharacters(data.inputLinkName);
            }
            EndElement(); // End input link name.
         }
         EndElement(); // End Link Element.
      }

      // Output all added value link connections.
      count = (int)addedValueLinks.size();
      for (int index = 0; index < count; ++index)
      {
         ToLinkData& data = addedValueLinks[index];

         BeginElement(dtCore::MapXMLConstants::DIRECTOR_LINK_VALUE_CONNECTION);
         {
            // Output link owner.
            BeginElement(dtCore::MapXMLConstants::DIRECTOR_LINK_OUTPUT_OWNER_INDEX_ELEMENT);
            {
               AddCharacters(dtUtil::ToString(data.outputNodeID.index));
            }
            EndElement(); // End output link owner.

            BeginElement(dtCore::MapXMLConstants::DIRECTOR_LINK_OUTPUT_OWNER_ELEMENT);
            {
               AddCharacters(data.outputNodeID.id.ToString());
            }
            EndElement(); // End output link owner.

            // Input link owner.
            BeginElement(dtCore::MapXMLConstants::DIRECTOR_LINK_INPUT_OWNER_INDEX_ELEMENT);
            {
               AddCharacters(dtUtil::ToString(data.inputNodeID.index));
            }
            EndElement(); // End input link owner.

            BeginElement(dtCore::MapXMLConstants::DIRECTOR_LINK_INPUT_OWNER_ELEMENT);
            {
               AddCharacters(data.inputNodeID.id.ToString());
            }
            EndElement(); // End input link owner.

            // Output link name.
            BeginElement(dtCore::MapXMLConstants::DIRECTOR_LINK_OUTPUT_NAME_ELEMENT);
            {
               AddCharacters(data.outputLinkName);
            }
            EndElement(); // End output link name.
         }
         EndElement(); // End Link Element.
      }

      // Output all removed chain link connections.
      count = (int)removedChainLinks.size();
      for (int index = 0; index < count; ++index)
      {
         ToLinkData& data = removedChainLinks[index];

         BeginElement(dtCore::MapXMLConstants::DIRECTOR_REMOVED_LINK_CHAIN_CONNECTION);
         {
            // Output link owner.
            BeginElement(dtCore::MapXMLConstants::DIRECTOR_LINK_OUTPUT_OWNER_INDEX_ELEMENT);
            {
               AddCharacters(dtUtil::ToString(data.outputNodeID.index));
            }
            EndElement(); // End output link owner.

            BeginElement(dtCore::MapXMLConstants::DIRECTOR_LINK_OUTPUT_OWNER_ELEMENT);
            {
               AddCharacters(data.outputNodeID.id.ToString());
            }
            EndElement(); // End output link owner.

            // Input link owner.
            BeginElement(dtCore::MapXMLConstants::DIRECTOR_LINK_INPUT_OWNER_INDEX_ELEMENT);
            {
               AddCharacters(dtUtil::ToString(data.inputNodeID.index));
            }
            EndElement(); // End input link owner.

            BeginElement(dtCore::MapXMLConstants::DIRECTOR_LINK_INPUT_OWNER_ELEMENT);
            {
               AddCharacters(data.inputNodeID.id.ToString());
            }
            EndElement(); // End input link owner.

            // Output link name.
            BeginElement(dtCore::MapXMLConstants::DIRECTOR_LINK_OUTPUT_NAME_ELEMENT);
            {
               AddCharacters(data.outputLinkName);
            }
            EndElement(); // End output link name.

            // Input link name.
            BeginElement(dtCore::MapXMLConstants::DIRECTOR_LINK_INPUT_NAME_ELEMENT);
            {
               AddCharacters(data.inputLinkName);
            }
            EndElement(); // End input link name.
         }
         EndElement(); // End Link Element.
      }

      // Output all removed value link connections.
      count = (int)removedValueLinks.size();
      for (int index = 0; index < count; ++index)
      {
         ToLinkData& data = removedValueLinks[index];

         BeginElement(dtCore::MapXMLConstants::DIRECTOR_REMOVED_LINK_VALUE_CONNECTION);
         {
            // Output link owner.
            BeginElement(dtCore::MapXMLConstants::DIRECTOR_LINK_OUTPUT_OWNER_INDEX_ELEMENT);
            {
               AddCharacters(dtUtil::ToString(data.outputNodeID.index));
            }
            EndElement(); // End output link owner.

            BeginElement(dtCore::MapXMLConstants::DIRECTOR_LINK_OUTPUT_OWNER_ELEMENT);
            {
               AddCharacters(data.outputNodeID.id.ToString());
            }
            EndElement(); // End output link owner.

            // Input link owner.
            BeginElement(dtCore::MapXMLConstants::DIRECTOR_LINK_INPUT_OWNER_INDEX_ELEMENT);
            {
               AddCharacters(dtUtil::ToString(data.inputNodeID.index));
            }
            EndElement(); // End input link owner.

            BeginElement(dtCore::MapXMLConstants::DIRECTOR_LINK_INPUT_OWNER_ELEMENT);
            {
               AddCharacters(data.inputNodeID.id.ToString());
            }
            EndElement(); // End input link owner.

            // Output link name.
            BeginElement(dtCore::MapXMLConstants::DIRECTOR_LINK_OUTPUT_NAME_ELEMENT);
            {
               AddCharacters(data.outputLinkName);
            }
            EndElement(); // End output link name.
         }
         EndElement(); // End Link Element.
      }
   }
}

//////////////////////////////////////////////////////////////////////////

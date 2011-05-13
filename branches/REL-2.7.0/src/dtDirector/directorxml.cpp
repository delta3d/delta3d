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
#include <cstdio>
#include <cstdlib>
#include <string>
#include <cmath>

#ifdef _MSC_VER
#   pragma warning(push)
#   pragma warning(disable : 4267) // for warning C4267: 'argument' : conversion from 'size_t' to 'const unsigned int', possible loss of data
#endif

#include <xercesc/util/XMLString.hpp>
#include <xercesc/util/OutOfMemoryException.hpp>
#include <xercesc/sax2/XMLReaderFactory.hpp>
#include <xercesc/framework/LocalFileInputSource.hpp>
#if XERCES_VERSION_MAJOR < 3
#include <xercesc/internal/XMLGrammarPoolImpl.hpp>
#endif
#include <xercesc/sax/SAXParseException.hpp>

#ifdef _MSC_VER
#   pragma warning(pop)
#endif

#include <osgDB/FileNameUtils>

#include <dtCore/transformable.h>
#include <dtCore/transform.h>

#include <dtDirector/directorxml.h>
#include <dtDirector/node.h>
#include <dtDirector/nodetype.h>

#include <dtDAL/map.h>
#include <dtDAL/exceptionenum.h>
#include <dtDAL/mapxmlconstants.h>

#include <dtUtil/datapathutils.h>
#include <dtUtil/fileutils.h>
#include <dtUtil/datetime.h>
#include <dtUtil/xercesutils.h>
#include <dtUtil/log.h>

#include <iostream>
#include <fstream>

XERCES_CPP_NAMESPACE_USE

namespace dtDirector
{
   static const std::string logName("directorxml.cpp");

   /////////////////////////////////////////////////////////////////
   DirectorParser::DirectorParser()
      : dtDAL::BaseXMLParser()
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
         throw dtDAL::ProjectException( "Unable to load required file \"director.xsd\", can not load director script.", __FILE__, __LINE__);
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
   bool DirectorParser::Parse(Director* director, dtDAL::Map* map, const std::string& filePath)
   {
      mDirectorHandler->SetDirector(director);
      mDirectorHandler->SetMap(map);
      if (dtDAL::BaseXMLParser::Parse(filePath))
      {
         return true;
      }

      return false;
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
      mPropSerializer = new dtDAL::ActorPropertySerializer(this);
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
         throw dtDAL::MapSaveException( std::string("Unable to open map file \"") + filePath + "\" for writing.", __FILE__, __LINE__);
      }

      mFormatTarget.SetOutputStream(&stream);

      try
      {
         WriteHeader();

         const std::string& utcTime = dtUtil::DateTime::ToString(dtUtil::DateTime(dtUtil::DateTime::TimeOrigin::LOCAL_TIME),
            dtUtil::DateTime::TimeFormat::CALENDAR_DATE_AND_TIME_FORMAT);

         // Director.
         BeginElement(dtDAL::MapXMLConstants::DIRECTOR_ELEMENT, dtDAL::MapXMLConstants::DIRECTOR_NAMESPACE);
         {
            // Header.
            BeginElement(dtDAL::MapXMLConstants::HEADER_ELEMENT);
            {
               // Creation timestamp.
               BeginElement(dtDAL::MapXMLConstants::CREATE_TIMESTAMP_ELEMENT);
               {
                  if (director->GetCreateDateTime().length() == 0)
                  {
                     director->SetCreateDateTime(utcTime);
                  }
                  AddCharacters(director->GetCreateDateTime());
               }
               EndElement(); // End Create Timestamp Element.

               // Last update timestamp.
               BeginElement(dtDAL::MapXMLConstants::LAST_UPDATE_TIMESTAMP_ELEMENT);
               {
                  AddCharacters(utcTime);
               }
               EndElement(); // End Last Update Timestamp Element.
               
               // Editor version.
               BeginElement(dtDAL::MapXMLConstants::EDITOR_VERSION_ELEMENT);
               {
                  AddCharacters(std::string(dtDAL::MapXMLConstants::EDITOR_VERSION));
               }
               EndElement(); // End Editor Version Element.
               
               // Schema Version.
               BeginElement(dtDAL::MapXMLConstants::SCHEMA_VERSION_ELEMENT);
               {
                  AddCharacters(std::string(dtDAL::MapXMLConstants::SCHEMA_VERSION));
               }
               EndElement(); // End Schema Version Element.

               // Properties.
               std::vector<const dtDAL::ActorProperty*> propList;
               director->GetPropertyList(propList);
               for (std::vector<const dtDAL::ActorProperty*>::const_iterator i = propList.begin();
                  i != propList.end(); ++i)
               {
                  const dtDAL::ActorProperty& property = *(*i);

                  // If the property is read only, skip it
                  if (property.IsReadOnly()) continue;

                  mPropSerializer->WriteProperty(property);
               }
            }
            EndElement(); // End Header Element.

            // Node Libraries.
            BeginElement(dtDAL::MapXMLConstants::LIBRARIES_ELEMENT);
            {
               const std::vector<std::string>& libs = director->GetAllLibraries();
               for (std::vector<std::string>::const_iterator i = libs.begin(); i != libs.end(); ++i)
               {
                  // Library.
                  BeginElement(dtDAL::MapXMLConstants::LIBRARY_ELEMENT);
                  {
                     // Library name.
                     BeginElement(dtDAL::MapXMLConstants::NAME_ELEMENT);
                     {
                        AddCharacters(*i);
                     }
                     EndElement(); // End Library Name Element.

                     // Library version.
                     BeginElement(dtDAL::MapXMLConstants::LIBRARY_VERSION_ELEMENT);
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
         }
         EndElement(); // End Director Element.

         //closes the file.
         mFormatTarget.SetOutputStream(NULL);
         mPropSerializer->SetMap(NULL);
      }
      catch (dtUtil::Exception& ex)
      {
         mLogger->LogMessage(dtUtil::Log::LOG_ERROR, __FUNCTION__, __LINE__,
                             "Caught Exception \"%s\" while attempting to save Director script \"%s\".",
                             ex.What().c_str(), director->GetName().c_str());
         mFormatTarget.SetOutputStream(NULL);
         throw ex;
      }
      catch (...)
      {
         mLogger->LogMessage(dtUtil::Log::LOG_ERROR, __FUNCTION__, __LINE__,
                             "Unknown exception while attempting to save Director script \"%s\".",
                             director->GetName().c_str());
         mFormatTarget.SetOutputStream(NULL);
         throw dtDAL::MapSaveException( std::string("Unknown exception saving Director script \"") + director->GetName() + ("\"."), __FILE__, __LINE__);
      }
   }

   //////////////////////////////////////////////////////////////////////////
   void DirectorWriter::SaveGraphs(DirectorGraph* graph)
   {
      mPropSerializer->SetCurrentPropertyContainer(graph);

      // Graph.
      BeginElement(dtDAL::MapXMLConstants::DIRECTOR_GRAPH_ELEMENT);
      {
         // Properties.
         std::vector<const dtDAL::ActorProperty*> propList;
         graph->GetPropertyList(propList);
         for (std::vector<const dtDAL::ActorProperty*>::const_iterator i = propList.begin();
            i != propList.end(); ++i)
         {
            const dtDAL::ActorProperty& property = *(*i);

            // If the property is read only, skip it
            if (property.IsReadOnly()) continue;

            mPropSerializer->WriteProperty(property);
         }

         //// Name Element.
         //BeginElement(dtDAL::MapXMLConstants::NAME_ELEMENT);
         //{
         //   AddCharacters(graph->GetName());
         //}
         //EndElement(); // End Name Element.

         // Event Nodes.
         BeginElement(dtDAL::MapXMLConstants::DIRECTOR_EVENT_NODES_ELEMENT);
         {
            const std::vector<dtCore::RefPtr<EventNode> >& EventNodes = graph->GetEventNodes();
            for (int nodeIndex = 0; nodeIndex < (int)EventNodes.size(); nodeIndex++)
            {
               SaveNode(EventNodes[nodeIndex].get());
            }
         }
         EndElement(); // End Event Nodes Element.

         // Action Nodes.
         BeginElement(dtDAL::MapXMLConstants::DIRECTOR_ACTION_NODES_ELEMENT);
         {
            const std::vector<dtCore::RefPtr<ActionNode> >& ActionNodes = graph->GetActionNodes();
            for (int nodeIndex = 0; nodeIndex < (int)ActionNodes.size(); nodeIndex++)
            {
               SaveNode(ActionNodes[nodeIndex].get());
            }
         }
         EndElement(); // End Action Nodes Element.

         // Value Nodes.
         BeginElement(dtDAL::MapXMLConstants::DIRECTOR_VALUE_NODES_ELEMENT);
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

      BeginElement(dtDAL::MapXMLConstants::DIRECTOR_NODE_ELEMENT);
      {
         // Node Name.
         BeginElement(dtDAL::MapXMLConstants::NAME_ELEMENT);
         {
            AddCharacters(node->GetType().GetName());
         }
         EndElement(); // End Node Name Element.

         // Node Category.
         BeginElement(dtDAL::MapXMLConstants::CATEGORY_ELEMENT);
         {
            AddCharacters(node->GetType().GetCategory());
         }
         EndElement(); // End Node Category Element.

         // Node ID.
         BeginElement(dtDAL::MapXMLConstants::ID_ELEMENT);
         {
            AddCharacters(node->GetID().ToString());
         }
         EndElement(); // End Node ID Elements.

         // Properties.
         std::vector<const dtDAL::ActorProperty*> propList;
         node->GetPropertyList(propList);
         for (std::vector<const dtDAL::ActorProperty*>::const_iterator i = propList.begin();
              i != propList.end(); ++i)
         {
            const dtDAL::ActorProperty& property = *(*i);

            // If the property is read only, skip it
            if (property.IsReadOnly()) continue;

            mPropSerializer->WriteProperty(property);
         }

         // Save Input Links.
         std::vector<InputLink>& inputs = node->GetInputLinks();
         for (int inputIndex = 0; inputIndex < (int)inputs.size(); inputIndex++)
         {
            BeginElement(dtDAL::MapXMLConstants::DIRECTOR_LINKS_INPUT_ELEMENT);
            {
               InputLink& input = inputs[inputIndex];

               // Link Name.
               BeginElement(dtDAL::MapXMLConstants::NAME_ELEMENT);
               {
                  AddCharacters(input.GetName());
               }
               EndElement(); // End Link Name.

               // Visibility
               BeginElement(dtDAL::MapXMLConstants::DIRECTOR_LINK_VISIBLE_ELEMENT);
               {
                  AddCharacters(input.GetVisible()? "true": "false");
               }
               EndElement(); // End Visibility.

               // Links.
               std::vector<OutputLink*>& links = input.GetLinks();
               for (int linkIndex = 0; linkIndex < (int)links.size(); linkIndex++)
               {
                  OutputLink* link = links[linkIndex];
                  if (!link) continue;

                  BeginElement(dtDAL::MapXMLConstants::DIRECTOR_LINK_ELEMENT);
                  {
                     // Linked Nodes ID.
                     BeginElement(dtDAL::MapXMLConstants::ID_ELEMENT);
                     {
                        AddCharacters(link->GetOwner()->GetID().ToString());
                     }
                     EndElement(); // End ID Element.

                     // Linked Link Name.
                     BeginElement(dtDAL::MapXMLConstants::NAME_ELEMENT);
                     {
                        AddCharacters(link->GetName());
                     }
                     EndElement(); // End Linked Link Name.
                  }
                  EndElement(); // End Link Element.
               }
            }
            EndElement(); // End Input Links Element.
         }

         // Save Output Links.
         std::vector<OutputLink>& outputs = node->GetOutputLinks();
         for (int outputIndex = 0; outputIndex < (int)outputs.size(); outputIndex++)
         {
            BeginElement(dtDAL::MapXMLConstants::DIRECTOR_LINKS_OUTPUT_ELEMENT);
            {
               OutputLink& output = outputs[outputIndex];

               // Link Name.
               BeginElement(dtDAL::MapXMLConstants::NAME_ELEMENT);
               {
                  AddCharacters(output.GetName());
               }
               EndElement(); // End Link Name.

               // Visibility
               BeginElement(dtDAL::MapXMLConstants::DIRECTOR_LINK_VISIBLE_ELEMENT);
               {
                  AddCharacters(output.GetVisible()? "true": "false");
               }
               EndElement(); // End Visibility.

               // Links.
               std::vector<InputLink*>& links = output.GetLinks();
               for (int linkIndex = 0; linkIndex < (int)links.size(); linkIndex++)
               {
                  InputLink* link = links[linkIndex];
                  if (!link) continue;

                  BeginElement(dtDAL::MapXMLConstants::DIRECTOR_LINK_ELEMENT);
                  {
                     // Linked Nodes ID.
                     BeginElement(dtDAL::MapXMLConstants::ID_ELEMENT);
                     {
                        AddCharacters(link->GetOwner()->GetID().ToString());
                     }
                     EndElement(); // End ID Element.

                     // Linked Link Name.
                     BeginElement(dtDAL::MapXMLConstants::NAME_ELEMENT);
                     {
                        AddCharacters(link->GetName());
                     }
                     EndElement(); // End Linked Link Name.
                  }
                  EndElement(); // End Link Element.
               }
            }
            EndElement(); // End Output Links Element.
         }

         // Save Value Links.
         std::vector<ValueLink>& values = node->GetValueLinks();
         for (int valueIndex = 0; valueIndex < (int)values.size(); valueIndex++)
         {
            BeginElement(dtDAL::MapXMLConstants::DIRECTOR_LINKS_VALUE_ELEMENT);
            {
               ValueLink& value = values[valueIndex];

               // Link Name.
               BeginElement(dtDAL::MapXMLConstants::NAME_ELEMENT);
               {
                  AddCharacters(value.GetName());
               }
               EndElement(); // End Link Name.

               // Visibility
               BeginElement(dtDAL::MapXMLConstants::DIRECTOR_LINK_VISIBLE_ELEMENT);
               {
                  AddCharacters(value.GetVisible()? "true": "false");
               }
               EndElement(); // End Visibility.

               // Exposed.
               BeginElement(dtDAL::MapXMLConstants::DIRECTOR_LINK_EXPOSED_ELEMENT);
               {
                  AddCharacters(value.GetExposed()? "true": "false");
               }
               EndElement(); // End Exposed.

               // Is Out Value.
               BeginElement(dtDAL::MapXMLConstants::DIRECTOR_LINK_VALUE_IS_OUT_ELEMENT);
               {
                  AddCharacters(value.IsOutLink()? "true": "false");
               }
               EndElement(); // End Is Out Value.

               // Allow Multiple values.
               BeginElement(dtDAL::MapXMLConstants::DIRECTOR_LINK_VALUE_ALLOW_MULTIPLE_ELEMENT);
               {
                  AddCharacters(value.AllowMultiple()? "true": "false");
               }
               EndElement(); // End Allow Multiple values.

               // Type Checking.
               BeginElement(dtDAL::MapXMLConstants::DIRECTOR_LINK_VALUE_TYPE_CHECK_ELEMENT);
               {
                  AddCharacters(value.IsTypeChecking()? "true": "false");
               }
               EndElement(); // End Type Checking.

               // Links.
               std::vector<ValueNode*>& links = value.GetLinks();
               for (int linkIndex = 0; linkIndex < (int)links.size(); linkIndex++)
               {
                  dtCore::RefPtr<ValueNode> link = links[linkIndex];
                  if (!link.valid()) continue;

                  BeginElement(dtDAL::MapXMLConstants::DIRECTOR_LINK_ELEMENT);
                  {
                     // Linked Nodes ID.
                     BeginElement(dtDAL::MapXMLConstants::ID_ELEMENT);
                     {
                        AddCharacters(link->GetID().ToString());
                     }
                     EndElement(); // End ID Element.
                  }
                  EndElement(); // End Link Element.
               }
            }
            EndElement(); // End Value Links Element.
         }
      }
      EndElement(); // End Director Node Element.
   }
}

//////////////////////////////////////////////////////////////////////////

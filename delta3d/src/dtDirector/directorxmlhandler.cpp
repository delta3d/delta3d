/* -*-c++-*-
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2006, Alion Science and Technology, BMH Operation.
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

#include <typeinfo>

#include <dtDirector/directorxmlhandler.h>
#include <dtDirector/nodemanager.h>
#include <dtDirector/director.h>

#include <dtDirector/valuenode.h>

#include <dtDirector/inputlink.h>
#include <dtDirector/outputlink.h>
#include <dtDirector/valuelink.h>

#include <dtCore/exceptionenum.h>
#include <dtCore/mapxmlconstants.h>

#include <dtUtil/xercesutils.h>

#include <dtUtil/deprecationmgr.h>
DT_DISABLE_WARNING_ALL_START
#include <xercesc/util/XMLString.hpp>
#include <xercesc/util/OutOfMemoryException.hpp>
#include <xercesc/sax/SAXParseException.hpp>
DT_DISABLE_WARNING_END

#include <osg/Vec2f>
#include <osg/Vec2d>
#include <osg/Vec3f>
#include <osg/Vec3d>

XERCES_CPP_NAMESPACE_USE


// Default iimplementation of char_traits<XMLCh>, needed for gcc3.3
#if (__GNUC__ == 3 && __GNUC_MINOR__ <= 3)
/// @cond DOXYGEN_SHOULD_SKIP_THIS
namespace std
{
   template<>
   struct char_traits<unsigned short>
   {
      typedef unsigned short char_type;

      static void
         assign(char_type& __c1, const char_type& __c2)
      { __c1 = __c2; }

      static int
         compare(const char_type* __s1, const char_type* __s2, size_t __n)
      {
         for (;__n > 0; ++__s1, ++__s2, --__n) {
            if (*__s1 < *__s2) return -1;
            if (*__s1 > *__s2) return +1;
         }
         return 0;
      }

      static size_t
         length(const char_type* __s)
      { size_t __n = 0; while (*__s++) ++__n; return __n; }

      static char_type*
         copy(char_type* __s1, const char_type* __s2, size_t __n)
      {  return static_cast<char_type*>(memcpy(__s1, __s2, __n * sizeof(char_type))); }

   };
}
/// @endcond
#endif

namespace dtDirector
{
   /////////////////////////////////////////////////////////////////
   DirectorXMLHandler::DirectorXMLHandler()
      : dtCore::BaseXMLHandler()
      , mDirector(NULL)
      , mMap(NULL)
      , mHasParsedHeader(false)
      , mHasImportedScripts(false)
      , mSchemaVersion(1.0f)
      , mNode(NULL)
   {
      mPropSerializer = new dtCore::ActorPropertySerializer(this);
      //dtUtil::Log::GetInstance().SetLogLevel(dtUtil::Log::LOG_DEBUG);
   }

   /////////////////////////////////////////////////////////////////
   DirectorXMLHandler::~DirectorXMLHandler()
   {
      delete mPropSerializer;
   }

   //////////////////////////////////////////////////////////////////////////
   DirectorXMLHandler::DirectorXMLHandler(const DirectorXMLHandler&) {}

   //////////////////////////////////////////////////////////////////////////
   DirectorXMLHandler& DirectorXMLHandler::operator=(const DirectorXMLHandler&) { return *this;}

   /////////////////////////////////////////////////////////////////
   void DirectorXMLHandler::startDocument()
   {
      dtCore::BaseXMLHandler::startDocument();

      mPropSerializer->SetMap(mMap.get());
      mPropSerializer->SetCurrentPropertyContainer(mDirector);
   }

   /////////////////////////////////////////////////////////////////
   void DirectorXMLHandler::endDocument()
   {
      dtCore::BaseXMLHandler::endDocument();

      mPropSerializer->LinkActors();
      mPropSerializer->AssignGroupProperties();

      LinkNodes();
      RemoveLinkNodes();
   }

   /////////////////////////////////////////////////////////////////
   void DirectorXMLHandler::ElementStarted(  const XMLCh* const uri,
                                             const XMLCh* const localname,
                                             const XMLCh* const qname,
                                             const xercesc::Attributes& attrs)
   {
      dtCore::BaseXMLHandler::ElementStarted(uri, localname, qname, attrs);

      // Library element started.
      if (mInHeaders)
      {
         if (!mPropSerializer->ElementStarted(localname))
         {
         }
      }
      else if (mInLibraries)
      {
         if (!mPropSerializer->ElementStarted(localname))
         {
            // Library
            if (XMLString::compareString(localname, dtCore::MapXMLConstants::LIBRARY_ELEMENT) == 0)
            {
               ClearLibraryValues();
            }
         }
      }
      // Graph element started.
      else if (mInGraph)
      {
         // Node element started.
         if (mInNodes)
         {
            if (mInNode)
            {
               // Check if we are starting a property element.
               if (!mPropSerializer->ElementStarted(localname))
               {
                  if (XMLString::compareString(localname, dtCore::MapXMLConstants::DIRECTOR_LINKS_INPUT_ELEMENT) == 0)
                  {
                     if (dtUtil::Log::GetInstance().IsLevelEnabled(dtUtil::Log::LOG_DEBUG))
                        dtUtil::Log::GetInstance().LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__,  __LINE__, "Found Input Link");

                     mInLink = true;
                     mInInputLink = true;
                  }
                  else if (XMLString::compareString(localname, dtCore::MapXMLConstants::DIRECTOR_LINKS_OUTPUT_ELEMENT) == 0)
                  {
                     if (dtUtil::Log::GetInstance().IsLevelEnabled(dtUtil::Log::LOG_DEBUG))
                        dtUtil::Log::GetInstance().LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__,  __LINE__, "Found Output Link");

                     mInLink = true;
                     mInOutputLink = true;
                  }
                  else if (XMLString::compareString(localname, dtCore::MapXMLConstants::DIRECTOR_LINKS_VALUE_ELEMENT) == 0)
                  {
                     if (dtUtil::Log::GetInstance().IsLevelEnabled(dtUtil::Log::LOG_DEBUG))
                        dtUtil::Log::GetInstance().LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__,  __LINE__, "Found Value Link");

                     mInLink = true;
                     mInValueLink = true;
                  }
                  else if (mInLink && XMLString::compareString(localname,dtCore::MapXMLConstants::DIRECTOR_LINK_ELEMENT) == 0)
                  {
                     mInLinkTo = true;
                  }
               }
            }
            // Check if we are starting a node.
            else if (XMLString::compareString(localname, dtCore::MapXMLConstants::DIRECTOR_NODE_ELEMENT) == 0)
            {
               ClearNodeValues();
               mInNode = true;

               // Make sure we have a graph.
               if (mGraphID.id.ToString().empty() && mHasImportedScripts)
               {
                  // In older versions, graphs did not save their ID values.
                  // For this case, we need to create a new graph and add it
                  // to the stack.
                  dtCore::RefPtr<DirectorGraph> graph = NULL;
                  if (mGraphs.empty())
                  {
                     if (mDirector)
                     {
                        graph = mDirector->GetGraphRoot();
                     }
                  }
                  else
                  {
                     graph = new DirectorGraph(mDirector);
                     DirectorGraph* parent = mGraphs.top();
                     graph->SetParent(parent);
                     graph->BuildPropertyMap();
                     parent->GetSubGraphs().push_back(graph);
                  }

                  if (graph)
                  {
                     mGraphID = graph->GetID();

                     mGraphs.push(graph);
                     mPropSerializer->SetCurrentPropertyContainer(graph);
                  }
               }
            }
         }
         else
         {
            if (!mPropSerializer->ElementStarted(localname))
            {
               // Value node.
               if (XMLString::compareString(localname, dtCore::MapXMLConstants::DIRECTOR_VALUE_NODES_ELEMENT) == 0)
               {
                  if (dtUtil::Log::GetInstance().IsLevelEnabled(dtUtil::Log::LOG_DEBUG))
                     dtUtil::Log::GetInstance().LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__,  __LINE__, "Found Value Nodes");
                  mInValueNodes = true;
                  mInNodes = true;
               }
               // Event node.
               else if (XMLString::compareString(localname, dtCore::MapXMLConstants::DIRECTOR_EVENT_NODES_ELEMENT) == 0)
               {
                  if (dtUtil::Log::GetInstance().IsLevelEnabled(dtUtil::Log::LOG_DEBUG))
                     dtUtil::Log::GetInstance().LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__,  __LINE__, "Found Event Nodes");
                  mInEventNodes = true;
                  mInNodes = true;
               }
               // Action node.
               else if (XMLString::compareString(localname, dtCore::MapXMLConstants::DIRECTOR_ACTION_NODES_ELEMENT) == 0)
               {
                  if (dtUtil::Log::GetInstance().IsLevelEnabled(dtUtil::Log::LOG_DEBUG))
                     dtUtil::Log::GetInstance().LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__,  __LINE__, "Found Action Nodes");
                  mInActionNodes = true;
                  mInNodes = true;
               }
               // Graph.
               else if (XMLString::compareString(localname, dtCore::MapXMLConstants::DIRECTOR_GRAPH_ELEMENT) == 0)
               {
                  if (dtUtil::Log::GetInstance().IsLevelEnabled(dtUtil::Log::LOG_DEBUG))
                  {
                     dtUtil::Log::GetInstance().LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__,  __LINE__, "Found a Graph");
                  }

                  mInGraph++;

                  mGraphID.clear();
                  mIsGraphImported = false;
                  mIsGraphReadOnly = false;

                  mPropSerializer->SetCurrentPropertyContainer(NULL);

                  // If we know we aren't importing, create the graph immediately.
                  if (!mHasImportedScripts)
                  {
                     dtCore::RefPtr<DirectorGraph> graph = NULL;

                     if (mGraphs.empty())
                     {
                        graph = mDirector->GetGraphRoot();
                     }
                     else
                     {
                        DirectorGraph* parent = mGraphs.top();
                        graph = new DirectorGraph(mDirector);
                        graph->SetParent(parent);
                        parent->GetSubGraphs().push_back(graph);
                        graph->BuildPropertyMap();
                     }

                     mGraphs.push(graph);
                     mPropSerializer->SetCurrentPropertyContainer(graph);
                  }
               }
            }
         }
      }
      // Header.
      else if (XMLString::compareString(localname, dtCore::MapXMLConstants::HEADER_ELEMENT) == 0)
      {
         if (dtUtil::Log::GetInstance().IsLevelEnabled(dtUtil::Log::LOG_DEBUG))
         {
            dtUtil::Log::GetInstance().LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__,  __LINE__, "Found Header");
         }
         mInHeaders = true;
      }
      // Libraries.
      else if (XMLString::compareString(localname, dtCore::MapXMLConstants::LIBRARIES_ELEMENT) == 0)
      {
         if (dtUtil::Log::GetInstance().IsLevelEnabled(dtUtil::Log::LOG_DEBUG))
            dtUtil::Log::GetInstance().LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__,  __LINE__, "Found Libraries");
         mInLibraries = true;
      }
      // Graph.
      else if (XMLString::compareString(localname, dtCore::MapXMLConstants::DIRECTOR_GRAPH_ELEMENT) == 0)
      {
         if (dtUtil::Log::GetInstance().IsLevelEnabled(dtUtil::Log::LOG_DEBUG))
            dtUtil::Log::GetInstance().LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__,  __LINE__, "Found a Graph");
         mInGraph++;

         mGraphID.clear();
         mIsGraphImported = false;
         mIsGraphReadOnly = false;

         mPropSerializer->SetCurrentPropertyContainer(NULL);

         // If we know we aren't importing, create the graph immediately.
         if (!mHasImportedScripts)
         {
            dtCore::RefPtr<DirectorGraph> graph = NULL;

            if (mGraphs.empty())
            {
               graph = mDirector->GetGraphRoot();
            }
            else
            {
               DirectorGraph* parent = mGraphs.top();
               graph = new DirectorGraph(mDirector);
               graph->SetParent(parent);
               parent->GetSubGraphs().push_back(graph);
               graph->BuildPropertyMap();
            }

            mGraphs.push(graph);
            mPropSerializer->SetCurrentPropertyContainer(graph);
         }
      }
      // Chain Link Connections.
      else if (XMLString::compareString(localname, dtCore::MapXMLConstants::DIRECTOR_LINK_CHAIN_CONNECTION) == 0)
      {
         if (dtUtil::Log::GetInstance().IsLevelEnabled(dtUtil::Log::LOG_DEBUG))
            dtUtil::Log::GetInstance().LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__,  __LINE__, "Found Chain Link Connection");

         mInLink = true;
         mInOutputLink = true;
      }
      // Value Link Connections.
      else if (XMLString::compareString(localname, dtCore::MapXMLConstants::DIRECTOR_LINK_VALUE_CONNECTION) == 0)
      {
         if (dtUtil::Log::GetInstance().IsLevelEnabled(dtUtil::Log::LOG_DEBUG))
            dtUtil::Log::GetInstance().LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__,  __LINE__, "Found Value Link Connection");

         mInLink = true;
         mInValueLink = true;
      }
      // Removed Chain Link Connections.
      else if (XMLString::compareString(localname, dtCore::MapXMLConstants::DIRECTOR_REMOVED_LINK_CHAIN_CONNECTION) == 0)
      {
         if (dtUtil::Log::GetInstance().IsLevelEnabled(dtUtil::Log::LOG_DEBUG))
            dtUtil::Log::GetInstance().LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__,  __LINE__, "Found Chain Link Connection");

         mInRemoveLink = true;
         mInOutputLink = true;
      }
      // Removed Value Link Connections.
      else if (XMLString::compareString(localname, dtCore::MapXMLConstants::DIRECTOR_REMOVED_LINK_VALUE_CONNECTION) == 0)
      {
         if (dtUtil::Log::GetInstance().IsLevelEnabled(dtUtil::Log::LOG_DEBUG))
            dtUtil::Log::GetInstance().LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__,  __LINE__, "Found Value Link Connection");

         mInRemoveLink = true;
         mInValueLink = true;
      }
   }

   /////////////////////////////////////////////////////////////////
   void DirectorXMLHandler::ElementEnded( const XMLCh* const uri,
                                          const XMLCh* const localname,
                                          const XMLCh* const qname)
   {
      dtCore::BaseXMLHandler::ElementEnded(uri, localname, qname);

      if (mInHeaders)
      {
         EndHeaderElement(localname);
      }
      else if (mInLibraries)
      {
         EndLibrarySection(localname);
      }
      else if (mInGraph)
      {
         if (mInNodes)
         {
            EndNodeSection(localname);
            if (!mInNodes && !mGraphs.empty())
            {
               mPropSerializer->SetCurrentPropertyContainer(mGraphs.top());
            }
         }
         else
         {
            EndGraphSection(localname);
         }
      }
      else if (mInLink)
      {
         if (XMLString::compareString(localname, dtCore::MapXMLConstants::DIRECTOR_LINK_CHAIN_CONNECTION) == 0 ||
            XMLString::compareString(localname, dtCore::MapXMLConstants::DIRECTOR_LINK_VALUE_CONNECTION) == 0)
         {
            ClearLinkValues();
         }
      }
      else if (mInRemoveLink)
      {
         if (XMLString::compareString(localname, dtCore::MapXMLConstants::DIRECTOR_REMOVED_LINK_CHAIN_CONNECTION) == 0 ||
            XMLString::compareString(localname, dtCore::MapXMLConstants::DIRECTOR_REMOVED_LINK_VALUE_CONNECTION) == 0)
         {
            ClearLinkValues();
         }
      }
   }

   /////////////////////////////////////////////////////////////////
   void DirectorXMLHandler::CombinedCharacters(const XMLCh* const chars, size_t length)
   {
      dtCore::BaseXMLHandler::CombinedCharacters(chars, length);

      xmlCharString& topEl = mElements.top();
      if (mInHeaders)
      {
         // If we find any header actor properties, we have gone passed the point
         // where we should have found a script type.  If we are searching for a
         // script type, we can stop now and assume it is the default type.
         if (!mHasParsedHeader && topEl == dtCore::MapXMLConstants::ACTOR_PROPERTY_ELEMENT)
         {
            mHasParsedHeader = true;
            mScriptType = "Scenario";
         }

         if (!mPropSerializer->Characters(topEl, chars))
         {
            if (topEl == dtCore::MapXMLConstants::CREATE_TIMESTAMP_ELEMENT)
            {
               if (mDirector)
               {
                  mDirector->SetCreateDateTime(dtUtil::XMLStringConverter(chars).ToString());
               }
            }
            else if (topEl == dtCore::MapXMLConstants::SCHEMA_VERSION_ELEMENT)
            {
               mSchemaVersion = dtUtil::ToType<float>(dtUtil::XMLStringConverter(chars).ToString());
            }
            else if (topEl == dtCore::MapXMLConstants::DIRECTOR_SCRIPT_TYPE)
            {
               mScriptType = dtUtil::XMLStringConverter(chars).ToString();

               // Check if the loaded script matches our current script type
               if (mDirector && mDirector->GetScriptType() != mScriptType)
               {
                  throw dtUtil::Exception("Attempted to load an invalid script type.", __FILE__, __LINE__);
               }
            }
            else if (topEl == dtCore::MapXMLConstants::DIRECTOR_IMPORTED_SCRIPT)
            {
               mHasImportedScripts = true;
               std::string importedScript = dtUtil::XMLStringConverter(chars).ToString();
               if (!importedScript.empty())
               {
                  Director* script = mDirector->ImportScript(importedScript);
                  if (!script)
                  {
                     mMissingImportedScripts.push_back(importedScript);
                  }
                  else
                  {
                     if (!script->GetMissingNodeTypes().empty())
                     {
                        std::set<std::string>::const_iterator iter;
                        for (iter = script->GetMissingNodeTypes().begin(); iter != script->GetMissingNodeTypes().end(); ++iter)
                        {
                           std::string nodeType = *iter;
                           if (mMissingNodeTypes.find(nodeType) == mMissingNodeTypes.end())
                           {
                              mMissingNodeTypes.insert(mMissingNodeTypes.end(), nodeType);
                           }
                        }
                     }
                     if (!script->GetMissingLibraries().empty())
                     {
                        mMissingLibraries.insert(mMissingLibraries.end(),
                           script->GetMissingLibraries().begin(),
                           script->GetMissingLibraries().end());
                     }
                     if (!script->GetMissingImportedScripts().empty())
                     {
                        mMissingImportedScripts.insert(mMissingImportedScripts.end(),
                           script->GetMissingImportedScripts().begin(),
                           script->GetMissingImportedScripts().end());
                     }
                  }
               }
            }
         }
      }
      else if (mInLibraries)
      {
         if (topEl == dtCore::MapXMLConstants::LIBRARY_NAME_ELEMENT)
         {
            mLibName = dtUtil::XMLStringConverter(chars).ToString();
         }
         else if (topEl == dtCore::MapXMLConstants::LIBRARY_VERSION_ELEMENT)
         {
            mLibVersion = dtUtil::XMLStringConverter(chars).ToString();
         }
      }
      else if (mInGraph)
      {
         if (mInNode)
         {
            DirectorGraph* graph = mGraphs.top();

            // If we don't have a node yet.
            if (!mNode.valid())
            {
               // Get the name of the node.
               if (topEl == dtCore::MapXMLConstants::NAME_ELEMENT)
               {
                  mNodeName = dtUtil::XMLStringConverter(chars).ToString();
               }
               else if (topEl == dtCore::MapXMLConstants::CATEGORY_ELEMENT)
               {
                  mNodeCategory = dtUtil::XMLStringConverter(chars).ToString();
               }

               // If we have both a name and category, create a node of the desired type.
               if (!mNodeName.empty() && !mNodeCategory.empty())
               {
                  dtDirector::NodeManager& nodeManager = dtDirector::NodeManager::GetInstance();

                  std::string nodeName = mNodeName;
                  std::string nodeCategory = mNodeCategory;

                  while (!mNode.valid() && !nodeName.empty() && !nodeCategory.empty())
                  {
                     mNode = nodeManager.CreateNode(nodeName, nodeCategory, graph).get();
                     if (!mNode.valid())
                     {
                        // Attempt to find a replacement for this node.
                        dtDirector::NodePluginRegistry::NodeReplacementData repData;
                        repData = nodeManager.FindNodeTypeReplacement(nodeCategory + "." + nodeName);

                        // Attempt to load the library that contains the node.
                        if (!repData.library.empty())
                        {
                           if (!mDirector->HasLibrary(repData.library))
                           {
                              mDirector->AddLibrary(repData.library);
                              mPropSerializer->SetDeprecatedProperty();
                           }
                        }

                        nodeName = repData.newName;
                        nodeCategory = repData.newCategory;
                     }
                  }

                  if (mNode.valid())
                  {
                     // Now make sure the new node is part of a valid library.
                     NodePluginRegistry* reg = NodeManager::GetInstance().GetRegistryForType(mNode->GetType());
                     if (reg && !mDirector->HasLibrary(reg->GetName()))
                     {
                        if (mDirector->AddLibrary(reg->GetName()))
                        {
                           mPropSerializer->SetDeprecatedProperty();
                        }
                        else
                        {
                           mNode = NULL;
                        }
                     }
                  }

                  if (!mNode.valid())
                  {
                     std::string nodeType = mNodeCategory + "." + mNodeName;
                     if (mMissingNodeTypes.find(nodeType) == mMissingNodeTypes.end())
                     {
                        mMissingNodeTypes.insert(mMissingNodeTypes.end(), nodeType);
                     }
                  }

                  mNodeName = "";
                  mNodeCategory = "";
                  mPropSerializer->SetCurrentPropertyContainer(mNode);
               }
            }
            else if (mInLink)
            {
               // Linking a link to another link.
               if (mInLinkTo)
               {
                  // The other links node ID.
                  if (topEl == dtCore::MapXMLConstants::ID_INDEX_ELEMENT)
                  {
                     mLinkNodeID.index = dtUtil::ToType<int>(dtUtil::XMLStringConverter(chars).ToString());
                  }
                  else if (topEl == dtCore::MapXMLConstants::ID_ELEMENT)
                  {
                     mLinkNodeID.id = dtCore::UniqueId(dtUtil::XMLStringConverter(chars).ToString());
                  }
                  // The other links name.
                  else if (topEl == dtCore::MapXMLConstants::NAME_ELEMENT)
                  {
                     mLinkToName = dtUtil::XMLStringConverter(chars).ToString();
                  }

                  // If we have both an ID and a name, we can link them.
                  if (!mLinkNodeID.id.ToString().empty())
                  {
                     // Connect a value link to a value node.
                     if (mValueLink)
                     {
                        mLinkList.push_back(ToLinkData());
                        int index = (int)mLinkList.size() - 1;
                        mLinkList[index].outputNodeID   = mValueLink->GetOwner()->GetID();
                        mLinkList[index].inputNodeID    = mLinkNodeID;
                        mLinkList[index].outputLinkName = mValueLink->GetName();
                        mLinkList[index].isValue        = true;
                        mLinkNodeID.clear();
                     }
                     else if (!mLinkToName.empty())
                     {
                        mLinkList.push_back(ToLinkData());
                        int index = (int)mLinkList.size() - 1;

                        // Connect an input link to an output link.
                        if (mInputLink)
                        {
                           mLinkList[index].outputNodeID = mLinkNodeID;
                           mLinkList[index].inputNodeID = mInputLink->GetOwner()->GetID();
                           mLinkList[index].outputLinkName = mLinkToName;
                           mLinkList[index].inputLinkName = mInputLink->GetName();
                        }
                        // Connect an output link to an input link.
                        else if (mOutputLink)
                        {
                           mLinkList[index].outputNodeID = mOutputLink->GetOwner()->GetID();
                           mLinkList[index].inputNodeID = mLinkNodeID;
                           mLinkList[index].outputLinkName = mOutputLink->GetName();
                           mLinkList[index].inputLinkName = mLinkToName;
                        }
                        mLinkNodeID.clear();
                     }
                  }
               }
               else if (topEl == dtCore::MapXMLConstants::NAME_ELEMENT)
               {
                  if (mInInputLink)
                  {
                     std::string linkName = dtUtil::XMLStringConverter(chars).ToString();
                     mInputLink = mNode->GetInputLink(linkName);

                     if (!mInputLink)
                     {
                        if (dtUtil::Log::GetInstance().IsLevelEnabled(dtUtil::Log::LOG_ERROR))
                        {
                           std::string error = "Input link \"" + linkName + "\" was not found on node type \"" + mNode->GetType().GetFullName() + "\".";

                           dtUtil::Log::GetInstance().LogMessage(dtUtil::Log::LOG_ERROR, __FUNCTION__,  __LINE__, error);
                        }
                     }
                  }
                  else if (mInOutputLink)
                  {
                     std::string linkName = dtUtil::XMLStringConverter(chars).ToString();
                     mOutputLink = mNode->GetOutputLink(linkName);

                     if (!mOutputLink)
                     {
                        if (dtUtil::Log::GetInstance().IsLevelEnabled(dtUtil::Log::LOG_ERROR))
                        {
                           std::string error = "Output link \"" + linkName + "\" was not found on node type \"" + mNode->GetType().GetFullName() + "\".";

                           dtUtil::Log::GetInstance().LogMessage(dtUtil::Log::LOG_ERROR, __FUNCTION__,  __LINE__, error);
                        }
                     }
                  }
                  else if (mInValueLink)
                  {
                     std::string linkName = dtUtil::XMLStringConverter(chars).ToString();
                     mValueLink = mNode->GetValueLink(linkName);

                     // If the value link wasn't already in the node, create one instead.
                     if (!mValueLink)
                     {
                        if (dtUtil::Log::GetInstance().IsLevelEnabled(dtUtil::Log::LOG_ERROR))
                        {
                           std::string error = "Value link \"" + linkName + "\" was not found on node type \"" + mNode->GetType().GetFullName() + "\".";

                           dtUtil::Log::GetInstance().LogMessage(dtUtil::Log::LOG_ERROR, __FUNCTION__,  __LINE__, error);
                        }
                     }
                  }
               }
               else if (topEl == dtCore::MapXMLConstants::DIRECTOR_LINK_VISIBLE_ELEMENT)
               {
                  if (mInInputLink)
                  {
                     if (mInputLink)
                     {
                        mInputLink->SetVisible(dtUtil::XMLStringConverter(chars).ToString() == "true");
                     }
                  }
                  else if (mInOutputLink)
                  {
                     if (mOutputLink)
                     {
                        mOutputLink->SetVisible(dtUtil::XMLStringConverter(chars).ToString() == "true");
                     }
                  }
                  else if (mInValueLink)
                  {
                     if (mValueLink)
                     {
                        mValueLink->SetVisible(dtUtil::XMLStringConverter(chars).ToString() == "true");
                     }
                  }
               }
               else if (topEl == dtCore::MapXMLConstants::DIRECTOR_LINK_EXPOSED_ELEMENT)
               {
                  if (mInValueLink)
                  {
                     if (mValueLink)
                     {
                        mValueLink->SetExposed(dtUtil::XMLStringConverter(chars).ToString() == "true");
                     }
                  }
               }
               else if (mValueLink && topEl == dtCore::MapXMLConstants::DIRECTOR_LINK_VALUE_IS_OUT_ELEMENT)
               {
                  mValueLink->SetOutLink(dtUtil::XMLStringConverter(chars).ToString() == "true");
               }
               else if (mValueLink && topEl == dtCore::MapXMLConstants::DIRECTOR_LINK_VALUE_ALLOW_MULTIPLE_ELEMENT)
               {
                  mValueLink->SetAllowMultiple(dtUtil::XMLStringConverter(chars).ToString() == "true");
               }
               else if (mValueLink && topEl == dtCore::MapXMLConstants::DIRECTOR_LINK_VALUE_TYPE_CHECK_ELEMENT)
               {
                  mValueLink->SetTypeChecking(dtUtil::XMLStringConverter(chars).ToString() == "true");
               }
            }
            else if (!mPropSerializer->Characters(topEl, chars))
            {
               if (topEl == dtCore::MapXMLConstants::ID_INDEX_ELEMENT)
               {
                  mNode->SetIDIndex(dtUtil::ToType<int>(dtUtil::XMLStringConverter(chars).ToString()));
               }
               else if (topEl == dtCore::MapXMLConstants::ID_ELEMENT)
               {
                  mNode->SetID(dtCore::UniqueId(dtUtil::XMLStringConverter(chars).ToString()));
               }
            }
         }
         else if (!mPropSerializer->Characters(topEl, chars))
         {
            if (mHasImportedScripts)
            {
               if (topEl == dtCore::MapXMLConstants::DIRECTOR_IMPORTED_SCRIPT)
               {
                  mIsGraphImported = dtUtil::XMLStringConverter(chars).ToString() == "true"? true: false;
               }
               else if (topEl == dtCore::MapXMLConstants::DIRECTOR_READ_ONLY)
               {
                  mIsGraphReadOnly = dtUtil::XMLStringConverter(chars).ToString() == "true"? true: false;
               }
               else if (topEl == dtCore::MapXMLConstants::ID_INDEX_ELEMENT)
               {
                  mGraphID.index = dtUtil::ToType<int>(dtUtil::XMLStringConverter(chars).ToString());
               }
               else if (topEl == dtCore::MapXMLConstants::ID_ELEMENT)
               {
                  mGraphID.id = dtCore::UniqueId(dtUtil::XMLStringConverter(chars).ToString());

                  // If this graph is meant to be imported, then attempt to find the graph
                  // as it should already exist.
                  dtCore::RefPtr<DirectorGraph> graph = NULL;
                  if (mIsGraphImported && !mGraphs.empty() && mHasImportedScripts)
                  {
                     ID tempID = mGraphID;
                     tempID.index = -1;
                     graph = mDirector->GetGraph(tempID);

                     // If the graph was not found, it may have been removed from the
                     // imported script in which it came from.  In this case, we still
                     // need to create a graph for the loading process to complete
                     // properly, but this graph will not be added to the script.
                     if (!graph)
                     {
                        graph = new DirectorGraph(mDirector);
                        graph->BuildPropertyMap();
                     }

                     graph->SetID(mGraphID);
                     graph->SetReadOnly(mIsGraphReadOnly);
                     mGraphs.push(graph);
                     mPropSerializer->SetCurrentPropertyContainer(graph);
                  }
                  // Otherwise, create a new graph.
                  else
                  {
                     DirectorGraph* parent = NULL;
                     if (mGraphs.empty())
                     {
                        graph = mDirector->GetGraphRoot();
                     }
                     else
                     {
                        graph = new DirectorGraph(mDirector);
                        parent = mGraphs.top();
                        graph->SetParent(parent);
                        graph->BuildPropertyMap();
                        parent->GetSubGraphs().push_back(graph);
                     }

                     if (mGraphID.index > -1)
                     {
                        graph->SetIDIndex(mGraphID.index);
                     }
                     graph->SetID(mGraphID.id);
                     graph->SetReadOnly(mIsGraphReadOnly);
                     mGraphs.push(graph);
                     mPropSerializer->SetCurrentPropertyContainer(graph);
                  }
               }
            }
         }
      }
      // Link Connections.
      else if (mInLink)
      {
         if (topEl == dtCore::MapXMLConstants::DIRECTOR_LINK_OUTPUT_OWNER_INDEX_ELEMENT)
         {
            mLinkOutputOwnerID.index = dtUtil::ToType<int>(dtUtil::XMLStringConverter(chars).ToString());
         }
         else if (topEl == dtCore::MapXMLConstants::DIRECTOR_LINK_OUTPUT_OWNER_ELEMENT)
         {
            mLinkOutputOwnerID.id = dtCore::UniqueId(dtUtil::XMLStringConverter(chars).ToString());
         }
         else if (topEl == dtCore::MapXMLConstants::DIRECTOR_LINK_OUTPUT_NAME_ELEMENT)
         {
            mLinkOutputName = dtUtil::XMLStringConverter(chars).ToString();
         }
         else if (topEl == dtCore::MapXMLConstants::DIRECTOR_LINK_INPUT_OWNER_INDEX_ELEMENT)
         {
            mLinkNodeID.index = dtUtil::ToType<int>(dtUtil::XMLStringConverter(chars).ToString());
         }
         else if (topEl == dtCore::MapXMLConstants::DIRECTOR_LINK_INPUT_OWNER_ELEMENT)
         {
            mLinkNodeID.id = dtCore::UniqueId(dtUtil::XMLStringConverter(chars).ToString());
         }
         else if (topEl == dtCore::MapXMLConstants::DIRECTOR_LINK_INPUT_NAME_ELEMENT)
         {
            mLinkToName = dtUtil::XMLStringConverter(chars).ToString();
         }

         if (!mLinkOutputOwnerID.id.ToString().empty() &&
            !mLinkOutputName.empty() &&
            !mLinkNodeID.id.ToString().empty() &&
            (mInValueLink || !mLinkToName.empty()))
         {
            mLinkList.push_back(ToLinkData());
            int index = (int)mLinkList.size() - 1;
            mLinkList[index].outputNodeID   = mLinkOutputOwnerID;
            mLinkList[index].inputNodeID    = mLinkNodeID;
            mLinkList[index].outputLinkName = mLinkOutputName;
            mLinkList[index].inputLinkName  = mLinkToName;
            mLinkList[index].isValue        = mInValueLink;
         }
      }
      // Removed Link Connections.
      else if (mInRemoveLink)
      {
         if (topEl == dtCore::MapXMLConstants::DIRECTOR_LINK_OUTPUT_OWNER_INDEX_ELEMENT)
         {
            mLinkOutputOwnerID.index = dtUtil::ToType<int>(dtUtil::XMLStringConverter(chars).ToString());
         }
         else if (topEl == dtCore::MapXMLConstants::DIRECTOR_LINK_OUTPUT_OWNER_ELEMENT)
         {
            mLinkOutputOwnerID.id = dtCore::UniqueId(dtUtil::XMLStringConverter(chars).ToString());
         }
         else if (topEl == dtCore::MapXMLConstants::DIRECTOR_LINK_OUTPUT_NAME_ELEMENT)
         {
            mLinkOutputName = dtUtil::XMLStringConverter(chars).ToString();
         }
         else if (topEl == dtCore::MapXMLConstants::DIRECTOR_LINK_INPUT_OWNER_INDEX_ELEMENT)
         {
            mLinkNodeID.index = dtUtil::ToType<int>(dtUtil::XMLStringConverter(chars).ToString());
         }
         else if (topEl == dtCore::MapXMLConstants::DIRECTOR_LINK_INPUT_OWNER_ELEMENT)
         {
            mLinkNodeID.id = dtCore::UniqueId(dtUtil::XMLStringConverter(chars).ToString());
         }
         else if (topEl == dtCore::MapXMLConstants::DIRECTOR_LINK_INPUT_NAME_ELEMENT)
         {
            mLinkToName = dtUtil::XMLStringConverter(chars).ToString();
         }

         if (!mLinkOutputOwnerID.id.ToString().empty() &&
            !mLinkOutputName.empty() &&
            !mLinkNodeID.id.ToString().empty() &&
            (mInValueLink || !mLinkToName.empty()))
         {
            mRemovedLinkList.push_back(ToLinkData());
            int index = (int)mRemovedLinkList.size() - 1;
            mRemovedLinkList[index].outputNodeID   = mLinkOutputOwnerID;
            mRemovedLinkList[index].inputNodeID    = mLinkNodeID;
            mRemovedLinkList[index].outputLinkName = mLinkOutputName;
            mRemovedLinkList[index].inputLinkName  = mLinkToName;
            mRemovedLinkList[index].isValue        = mInValueLink;
         }
      }
   }

   /////////////////////////////////////////////////////////////////
   void DirectorXMLHandler::Reset()
   {
      dtCore::BaseXMLHandler::Reset();

      mScriptType = "";
      mHasParsedHeader = false;
      mHasImportedScripts = false;

      mSchemaVersion = 1.0f;

      mInHeaders = false;
      mInLibraries = false;
      mInValueNodes = false;
      mInEventNodes = false;
      mInActionNodes = false;
      mInNodes = false;

      mInLink = false;
      mInRemoveLink = false;
      mInInputLink = false;
      mInOutputLink = false;
      mInValueLink = false;
      mInLinkTo = false;

      mInGraph = 0;
      while(mGraphs.size()) mGraphs.pop();

      mInputLink = NULL;
      mOutputLink = NULL;
      mValueLink = NULL;

      mPropSerializer->Reset();
      mLinkList.clear();
      mRemovedLinkList.clear();

      ClearLibraryValues();
      ClearNodeValues();
      ClearLinkValues();
      mMissingLibraries.clear();
      mMissingNodeTypes.clear();
   }

   /////////////////////////////////////////////////////////////////
   void DirectorXMLHandler::ClearLibraryValues()
   {
      mLibName.clear();
      mLibVersion.clear();
   }

   /////////////////////////////////////////////////////////////////
   void DirectorXMLHandler::ClearNodeValues()
   {
      mNode = NULL;
      mNodeName.clear();
      mNodeCategory.clear();

      mInNode = false;

      ClearLinkValues();
      mPropSerializer->SetCurrentPropertyContainer(NULL);
   }

   //////////////////////////////////////////////////////////////////////////
   void DirectorXMLHandler::ClearLinkValues()
   {
      mInLink = false;
      mInRemoveLink = false;
      mInInputLink = false;
      mInOutputLink = false;
      mInValueLink = false;

      mInputLink = NULL;
      mOutputLink = NULL;
      mValueLink = NULL;

      ClearLinkToValues();
   }

   //////////////////////////////////////////////////////////////////////////
   void DirectorXMLHandler::ClearLinkToValues()
   {
      mInLinkTo = false;

      mLinkOutputOwnerID.clear();
      mLinkOutputName.clear();
      mLinkNodeID.clear();
      mLinkToName.clear();
   }

   //////////////////////////////////////////////////////////////////////////
   void DirectorXMLHandler::EndHeaderElement(const XMLCh* const localname)
   {
      if (!mPropSerializer->ElementEnded(localname))
      {
         if (XMLString::compareString(localname, dtCore::MapXMLConstants::HEADER_ELEMENT) == 0)
         {
            mInHeaders = false;
            mHasParsedHeader = true;
         }
      }
   }

   //////////////////////////////////////////////////////////////////////////
   void DirectorXMLHandler::EndNodeElement()
   {
      // Initialize messages on all nodes.
      if (mInEventNodes && mNode.valid())
      {
         mNode->RegisterMessages();
      }

      ClearNodeValues();
   }

   //////////////////////////////////////////////////////////////////////////
   void DirectorXMLHandler::EndGraphSection(const XMLCh* const localname)
   {
      if (mInGraph)
      {
         if (XMLString::compareString(localname, dtCore::MapXMLConstants::DIRECTOR_GRAPH_ELEMENT) == 0)
         {
            mGraphs.pop();
            mInGraph--;
            if (mGraphs.empty())
            {
               mPropSerializer->SetCurrentPropertyContainer(NULL);
            }
            else
            {
               mPropSerializer->SetCurrentPropertyContainer(mGraphs.top());
            }
         }
         else
         {
            mPropSerializer->ElementEnded(localname);
         }
      }
   }

   //////////////////////////////////////////////////////////////////////////
   void DirectorXMLHandler::EndNodeSection(const XMLCh* const localname)
   {
      if (mInLink)
      {
         if (XMLString::compareString(localname, dtCore::MapXMLConstants::DIRECTOR_LINKS_INPUT_ELEMENT) == 0 ||
            XMLString::compareString(localname, dtCore::MapXMLConstants::DIRECTOR_LINKS_OUTPUT_ELEMENT) == 0 ||
            XMLString::compareString(localname, dtCore::MapXMLConstants::DIRECTOR_LINKS_VALUE_ELEMENT) == 0)
         {
            ClearLinkValues();
         }
         else if (XMLString::compareString(localname, dtCore::MapXMLConstants::DIRECTOR_LINK_ELEMENT) == 0)
         {
            ClearLinkToValues();
         }
      }
      else if (XMLString::compareString(localname, dtCore::MapXMLConstants::DIRECTOR_ACTION_NODES_ELEMENT) == 0 ||
               XMLString::compareString(localname, dtCore::MapXMLConstants::DIRECTOR_EVENT_NODES_ELEMENT) == 0  ||
               XMLString::compareString(localname, dtCore::MapXMLConstants::DIRECTOR_VALUE_NODES_ELEMENT) == 0)
      {
         EndNodesElement();
      }
      else if (XMLString::compareString(localname, dtCore::MapXMLConstants::DIRECTOR_NODE_ELEMENT) == 0)
      {
         EndNodeElement();
      }
      else
      {
         mPropSerializer->ElementEnded(localname);
      }
   }

   //////////////////////////////////////////////////////////////////////////
   void DirectorXMLHandler::EndNodesElement()
   {
      EndNodeElement();

      mInNodes = false;
      mInValueNodes = false;
      mInEventNodes = false;
      mInActionNodes = false;

      if (mInNode)
      {
         dtUtil::Log::GetInstance().LogMessage(dtUtil::Log::LOG_ERROR, __FUNCTION__, __LINE__,
                             "Found the closing nodes section tag, but the content handler thinks it's still parsing a node.");
         mInNode = false;
      }
   }

   //////////////////////////////////////////////////////////////////////////
   void DirectorXMLHandler::EndLibrarySection(const XMLCh* const localname)
   {
      if (XMLString::compareString(localname, dtCore::MapXMLConstants::LIBRARIES_ELEMENT) == 0)
      {
         mInLibraries = false;
      }
      else if (XMLString::compareString(localname, dtCore::MapXMLConstants::LIBRARY_ELEMENT) == 0)
      {
         EndLibraryElement();
      }
   }

   //////////////////////////////////////////////////////////////////////////
   void DirectorXMLHandler::EndLibraryElement()
   {
      if (dtUtil::Log::GetInstance().IsLevelEnabled(dtUtil::Log::LOG_DEBUG))
      {
         dtUtil::Log::GetInstance().LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__,
            "Attempting to add library %s version %s to the library manager.",
            mLibName.c_str(),
            mLibVersion.c_str());
      }

      try
      {
         if (mDirector)
         {
            mDirector->AddLibrary(mLibName, mLibVersion);
         }
         ClearLibraryValues();
      }
      catch (const dtCore::ProjectResourceErrorException& e)
      {
         mMissingLibraries.push_back(mLibName);

         dtUtil::Log::GetInstance().LogMessage(dtUtil::Log::LOG_ERROR, __FUNCTION__, __LINE__,
            "Error loading node library %s version %s in the director node manager.  Exception message to follow.",
            mLibName.c_str(), mLibVersion.c_str());

         e.LogException(dtUtil::Log::LOG_ERROR, dtUtil::Log::GetInstance());
      }
      catch (const dtUtil::Exception& e)
      {
         mMissingLibraries.push_back(mLibName);

         dtUtil::Log::GetInstance().LogMessage(dtUtil::Log::LOG_ERROR, __FUNCTION__, __LINE__,
            "Unknown exception loading node library %s version %s in the director node manager.  Exception message to follow.",
            mLibName.c_str(), mLibVersion.c_str());

         e.LogException(dtUtil::Log::LOG_ERROR, dtUtil::Log::GetInstance());
      }
   }

   //////////////////////////////////////////////////////////////////////////
   void DirectorXMLHandler::LinkNodes()
   {
      std::vector<ToLinkData> failedLinks;

      int count = (int)mLinkList.size();
      for (int index = 0; index < count; index++)
      {
         dtCore::RefPtr<Node> outputNode = mDirector->GetNode(mLinkList[index].outputNodeID, true);
         dtCore::RefPtr<Node> inputNode = mDirector->GetNode(mLinkList[index].inputNodeID, true);

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
                  outputLink->SetVisible(true);
                  inputLink->SetVisible(true);

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
         LinkNodes();
      }

      mLinkList.clear();
   }

   //////////////////////////////////////////////////////////////////////////
   void DirectorXMLHandler::RemoveLinkNodes()
   {
      int count = (int)mRemovedLinkList.size();
      for (int index = 0; index < count; index++)
      {
         dtCore::RefPtr<Node> outputNode = mDirector->GetNode(mRemovedLinkList[index].outputNodeID, true);
         dtCore::RefPtr<Node> inputNode = mDirector->GetNode(mRemovedLinkList[index].inputNodeID, true);

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
}

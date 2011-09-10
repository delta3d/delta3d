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

#ifdef _MSC_VER
#   pragma warning(push)
#   pragma warning(disable : 4267) // for warning C4267: 'argument' : conversion from 'size_t' to 'const unsigned int', possible loss of data
#endif

#include <xercesc/util/XMLString.hpp>
#include <xercesc/util/OutOfMemoryException.hpp>
#include <xercesc/sax/SAXParseException.hpp>

#ifdef _MSC_VER
#   pragma warning(pop)
#endif

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
      , mFoundScriptType(false)
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

                  DirectorGraph* parent = mGraphs.top();
                  DirectorGraph* newGraph = new DirectorGraph(mDirector);
                  newGraph->SetParent(parent);
                  newGraph->BuildPropertyMap();
                  parent->GetSubGraphs().push_back(newGraph);
                  mGraphs.push(newGraph);
                  mPropSerializer->SetCurrentPropertyContainer(newGraph);
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

         mGraphs.push(mDirector->GetGraphRoot());
         mPropSerializer->SetCurrentPropertyContainer(mGraphs.top());
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
            if (!mInNodes)
            {
               mPropSerializer->SetCurrentPropertyContainer(mGraphs.top());
            }
         }
         else
         {
            EndGraphSection(localname);
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
         if (!mPropSerializer->Characters(topEl, chars))
         {
            if (topEl == dtCore::MapXMLConstants::CREATE_TIMESTAMP_ELEMENT)
            {
               if (mDirector)
               {
                  mDirector->SetCreateDateTime(dtUtil::XMLStringConverter(chars).ToString());
               }
            }
            else if (topEl == dtCore::MapXMLConstants::DIRECTOR_SCRIPT_TYPE)
            {
               mScriptType = dtUtil::XMLStringConverter(chars).ToString();
               mFoundScriptType = true;

               // Check if the loaded script matches our current script type
               if (mDirector && mDirector->GetScriptType() != mScriptType)
               {
                  throw dtUtil::Exception("Attempted to load an invalid script type.", __FILE__, __LINE__);
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
         DirectorGraph* graph = mGraphs.top();

         if (mInNode)
         {
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
                  mNode = nodeManager.CreateNode(mNodeName, mNodeCategory, graph).get();
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
                  if (topEl == dtCore::MapXMLConstants::ID_ELEMENT)
                  {
                     mLinkNodeID = dtUtil::XMLStringConverter(chars).ToString();
                  }
                  // The other links name.
                  else if (topEl == dtCore::MapXMLConstants::NAME_ELEMENT)
                  {
                     mLinkToName = dtUtil::XMLStringConverter(chars).ToString();
                  }

                  // If we have both an ID and a name, we can link them.
                  if (!mLinkNodeID.empty())
                  {
                     // Connect a value link to a value node.
                     if (mValueLink)
                     {
                        mLinkList.push_back(ToLinkData());
                        int index = (int)mLinkList.size() - 1;
                        mLinkList[index].linkNodeID = mLinkNodeID;
                        mLinkList[index].valueLink = mValueLink;
                        mLinkNodeID.clear();
                     }
                     else if (!mLinkToName.empty())
                     {
                        mLinkList.push_back(ToLinkData());
                        int index = (int)mLinkList.size() - 1;
                        mLinkList[index].linkNodeID = mLinkNodeID;
                        mLinkList[index].linkToName = mLinkToName;
                        mLinkNodeID.clear();

                        // Connect an input link to an output link.
                        if (mInputLink)
                        {
                           mLinkList[index].inputLink = mInputLink;
                        }
                        // Connect an output link to an input link.
                        else if (mOutputLink)
                        {
                           mLinkList[index].outputLink = mOutputLink;
                        }
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
               if (topEl == dtCore::MapXMLConstants::ID_ELEMENT)
               {
                  mNode->SetID(dtCore::UniqueId(dtUtil::XMLStringConverter(chars).ToString()));
               }
            }
         }
         else if (!mPropSerializer->Characters(topEl, chars))
         {
            if (topEl == dtCore::MapXMLConstants::ID_ELEMENT)
            {
               graph->SetID(dtCore::UniqueId(dtUtil::XMLStringConverter(chars).ToString()));
            }
         }
      }
   }

   /////////////////////////////////////////////////////////////////
   void DirectorXMLHandler::Reset()
   {
      dtCore::BaseXMLHandler::Reset();

      mInHeaders = false;
      mInLibraries = false;
      mInValueNodes = false;
      mInEventNodes = false;
      mInActionNodes = false;
      mInNodes = false;

      mInLink = false;
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
         mDirector->AddLibrary(mLibName, mLibVersion);
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
         dtCore::RefPtr<Node> linkNode = mDirector->GetNode(dtCore::UniqueId(mLinkList[index].linkNodeID));

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
               // Connect an input link to an output link.
               if (mLinkList[index].inputLink)
               {
                  OutputLink* link = linkNode->GetOutputLink(mLinkList[index].linkToName);
                  if (link) link->Connect(mLinkList[index].inputLink);
               }
               // Connect an output link to an input link.
               else if (mLinkList[index].outputLink)
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
         LinkNodes();
      }
   }
}

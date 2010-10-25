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

#include <dtDAL/exceptionenum.h>
#include <dtDAL/mapxmlconstants.h>

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
      : dtDAL::BaseXMLHandler()
      , mMap(NULL)
      , mNode(NULL)
   {
      mPropSerializer = new dtDAL::ActorPropertySerializer(this);
      //mLogger->SetLogLevel(dtUtil::Log::LOG_DEBUG);
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
      dtDAL::BaseXMLHandler::startDocument();

      mPropSerializer->SetMap(mMap.get());
      mPropSerializer->SetCurrentPropertyContainer(mDirector);
   }

   /////////////////////////////////////////////////////////////////
   void DirectorXMLHandler::endDocument()
   {
      dtDAL::BaseXMLHandler::endDocument();

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
      dtDAL::BaseXMLHandler::ElementStarted(uri, localname, qname, attrs);

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
            if (XMLString::compareString(localname, dtDAL::MapXMLConstants::LIBRARY_ELEMENT) == 0)
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
                  if (XMLString::compareString(localname, dtDAL::MapXMLConstants::DIRECTOR_LINKS_INPUT_ELEMENT) == 0)
                  {
                     if (mLogger->IsLevelEnabled(dtUtil::Log::LOG_DEBUG))
                        mLogger->LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__,  __LINE__, "Found Input Link");

                     mInLink = true;
                     mInInputLink = true;
                  }
                  else if (XMLString::compareString(localname, dtDAL::MapXMLConstants::DIRECTOR_LINKS_OUTPUT_ELEMENT) == 0)
                  {
                     if (mLogger->IsLevelEnabled(dtUtil::Log::LOG_DEBUG))
                        mLogger->LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__,  __LINE__, "Found Output Link");

                     mInLink = true;
                     mInOutputLink = true;
                  }
                  else if (XMLString::compareString(localname, dtDAL::MapXMLConstants::DIRECTOR_LINKS_VALUE_ELEMENT) == 0)
                  {
                     if (mLogger->IsLevelEnabled(dtUtil::Log::LOG_DEBUG))
                        mLogger->LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__,  __LINE__, "Found Value Link");

                     mInLink = true;
                     mInValueLink = true;
                  }
                  else if (mInLink && XMLString::compareString(localname,dtDAL::MapXMLConstants::DIRECTOR_LINK_ELEMENT) == 0)
                  {
                     mInLinkTo = true;
                  }
               }
            }
            // Check if we are starting a node.
            else if (XMLString::compareString(localname, dtDAL::MapXMLConstants::DIRECTOR_NODE_ELEMENT) == 0)
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
               if (XMLString::compareString(localname, dtDAL::MapXMLConstants::DIRECTOR_VALUE_NODES_ELEMENT) == 0)
               {
                  if (mLogger->IsLevelEnabled(dtUtil::Log::LOG_DEBUG))
                     mLogger->LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__,  __LINE__, "Found Value Nodes");
                  mInValueNodes = true;
                  mInNodes = true;
               }
               // Event node.
               else if (XMLString::compareString(localname, dtDAL::MapXMLConstants::DIRECTOR_EVENT_NODES_ELEMENT) == 0)
               {
                  if (mLogger->IsLevelEnabled(dtUtil::Log::LOG_DEBUG))
                     mLogger->LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__,  __LINE__, "Found Event Nodes");
                  mInEventNodes = true;
                  mInNodes = true;
               }
               // Action node.
               else if (XMLString::compareString(localname, dtDAL::MapXMLConstants::DIRECTOR_ACTION_NODES_ELEMENT) == 0)
               {
                  if (mLogger->IsLevelEnabled(dtUtil::Log::LOG_DEBUG))
                     mLogger->LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__,  __LINE__, "Found Action Nodes");
                  mInActionNodes = true;
                  mInNodes = true;
               }
               // Graph.
               else if (XMLString::compareString(localname, dtDAL::MapXMLConstants::DIRECTOR_GRAPH_ELEMENT) == 0)
               {
                  if (mLogger->IsLevelEnabled(dtUtil::Log::LOG_DEBUG))
                  {
                     mLogger->LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__,  __LINE__, "Found a Graph");
                  }

                  mInGraph++;

                  DirectorGraph* parent = mGraphs.top();
                  DirectorGraph* newGraph = new DirectorGraph(mDirector);
                  newGraph->mParent = parent;
                  newGraph->BuildPropertyMap();
                  parent->mSubGraphs.push_back(newGraph);
                  mGraphs.push(newGraph);
                  mPropSerializer->SetCurrentPropertyContainer(newGraph);
               }
            }
         }
      }
      // Header.
      else if (XMLString::compareString(localname, dtDAL::MapXMLConstants::HEADER_ELEMENT) == 0)
      {
         if (mLogger->IsLevelEnabled(dtUtil::Log::LOG_DEBUG))
         {
            mLogger->LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__,  __LINE__, "Found Header");
         }
         mInHeaders = true;
      }
      // Libraries.
      else if (XMLString::compareString(localname, dtDAL::MapXMLConstants::LIBRARIES_ELEMENT) == 0)
      {
         if (mLogger->IsLevelEnabled(dtUtil::Log::LOG_DEBUG))
            mLogger->LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__,  __LINE__, "Found Libraries");
         mInLibraries = true;
      }
      // Graph.
      else if (XMLString::compareString(localname, dtDAL::MapXMLConstants::DIRECTOR_GRAPH_ELEMENT) == 0)
      {
         if (mLogger->IsLevelEnabled(dtUtil::Log::LOG_DEBUG))
            mLogger->LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__,  __LINE__, "Found a Graph");
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
      dtDAL::BaseXMLHandler::ElementEnded(uri, localname, qname);

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
#if XERCES_VERSION_MAJOR < 3
   void DirectorXMLHandler::characters(const XMLCh* const chars, const unsigned int length)
#else
   void DirectorXMLHandler::characters(const XMLCh* const chars, const XMLSize_t length)
#endif
   {
      dtDAL::BaseXMLHandler::characters(chars, length);

      xmlCharString& topEl = mElements.top();
      if (mInHeaders)
      {
         if (!mPropSerializer->Characters(topEl, chars))
         {
            if (topEl == dtDAL::MapXMLConstants::CREATE_TIMESTAMP_ELEMENT)
            {
               mDirector->SetCreateDateTime(dtUtil::XMLStringConverter(chars).ToString());
            }
         }
      }
      else if (mInLibraries)
      {
         if (topEl == dtDAL::MapXMLConstants::LIBRARY_NAME_ELEMENT)
         {
            mLibName = dtUtil::XMLStringConverter(chars).ToString();
         }
         else if (topEl == dtDAL::MapXMLConstants::LIBRARY_VERSION_ELEMENT)
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
               if (topEl == dtDAL::MapXMLConstants::NAME_ELEMENT)
               {
                  mNodeName = dtUtil::XMLStringConverter(chars).ToString();
               }
               else if (topEl == dtDAL::MapXMLConstants::CATEGORY_ELEMENT)
               {
                  mNodeCategory = dtUtil::XMLStringConverter(chars).ToString();
               }

               // If we have both a name and category, create a node of the desired type.
               if (!mNodeName.empty() && !mNodeCategory.empty())
               {
                  dtDirector::NodeManager& nodeManager = dtDirector::NodeManager::GetInstance();
                  mNode = nodeManager.CreateNode(mNodeName, mNodeCategory, graph).get();
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
                  if (topEl == dtDAL::MapXMLConstants::ID_ELEMENT)
                  {
                     mLinkNodeID = dtUtil::XMLStringConverter(chars).ToString();
                  }
                  // The other links name.
                  else if (topEl == dtDAL::MapXMLConstants::NAME_ELEMENT)
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

                        //ValueNode* valueNode = dynamic_cast<ValueNode*>(mLinkNode.get());
                        //mValueLink->Connect(valueNode);
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
                           //OutputLink* link = mLinkNode->GetOutputLink(mLinkToName);
                           //if (link) link->Connect(mInputLink);
                        }
                        // Connect an output link to an input link.
                        else if (mOutputLink)
                        {
                           mLinkList[index].outputLink = mOutputLink;
                           //InputLink* link = mLinkNode->GetInputLink(mLinkToName);
                           //if (link) link->Connect(mOutputLink);
                        }
                     }
                  }
               }
               else if (topEl == dtDAL::MapXMLConstants::NAME_ELEMENT)
               {
                  if (mInInputLink)
                  {
                     mInputLink = mNode->GetInputLink(dtUtil::XMLStringConverter(chars).ToString());
                  }
                  else if (mInOutputLink)
                  {
                     mOutputLink = mNode->GetOutputLink(dtUtil::XMLStringConverter(chars).ToString());
                  }
                  else if (mInValueLink)
                  {
                     mValueLink = mNode->GetValueLink(dtUtil::XMLStringConverter(chars).ToString());

                     // If the value link wasn't already in the node, create one instead.
                     if (!mValueLink)
                     {
                        dtDAL::ActorProperty* prop = mNode->GetProperty(dtUtil::XMLStringConverter(chars).ToString());
                        if (prop)
                        {
                           mNode->GetValueLinks().push_back(ValueLink(mNode.get(), prop));
                           mValueLink = mNode->GetValueLink(dtUtil::XMLStringConverter(chars).ToString());
                        }
                     }
                  }
               }
               else if (topEl == dtDAL::MapXMLConstants::DIRECTOR_LINK_VISIBLE_ELEMENT)
               {
                  if (mInInputLink)
                  {
                     if (!mInputLink)
                     {
                        if (mLogger->IsLevelEnabled(dtUtil::Log::LOG_ERROR))
                           mLogger->LogMessage(dtUtil::Log::LOG_ERROR, __FUNCTION__,  __LINE__, "Attempted to set value on input link when the link was not loaded.");
                     }
                     else mInputLink->SetVisible(dtUtil::XMLStringConverter(chars).ToString() == "true");
                  }
                  else if (mInOutputLink)
                  {
                     if (!mOutputLink)
                     {
                        if (mLogger->IsLevelEnabled(dtUtil::Log::LOG_ERROR))
                           mLogger->LogMessage(dtUtil::Log::LOG_ERROR, __FUNCTION__,  __LINE__, "Attempted to set value on output link when the link was not loaded.");
                     }
                     else mOutputLink->SetVisible(dtUtil::XMLStringConverter(chars).ToString() == "true");
                  }
                  else if (mInValueLink)
                  {
                     if (!mValueLink)
                     {
                        if (mLogger->IsLevelEnabled(dtUtil::Log::LOG_ERROR))
                           mLogger->LogMessage(dtUtil::Log::LOG_ERROR, __FUNCTION__,  __LINE__, "Attempted to set value on value link when the link was not loaded.");
                     }
                     else mValueLink->SetVisible(dtUtil::XMLStringConverter(chars).ToString() == "true");
                  }
               }
               else if (topEl == dtDAL::MapXMLConstants::DIRECTOR_LINK_EXPOSED_ELEMENT)
               {
                  if (mInValueLink)
                  {
                     if (!mValueLink)
                     {
                        if (mLogger->IsLevelEnabled(dtUtil::Log::LOG_ERROR))
                           mLogger->LogMessage(dtUtil::Log::LOG_ERROR, __FUNCTION__, __LINE__, "Attempted to set value on value link when the link was not loaded.");
                     }
                     else mValueLink->SetExposed(dtUtil::XMLStringConverter(chars).ToString() == "true");
                  }
               }
               else if (mValueLink && topEl == dtDAL::MapXMLConstants::DIRECTOR_LINK_VALUE_IS_OUT_ELEMENT)
               {
                  mValueLink->SetOutLink(dtUtil::XMLStringConverter(chars).ToString() == "true");
               }
               else if (mValueLink && topEl == dtDAL::MapXMLConstants::DIRECTOR_LINK_VALUE_ALLOW_MULTIPLE_ELEMENT)
               {
                  mValueLink->SetAllowMultiple(dtUtil::XMLStringConverter(chars).ToString() == "true");
               }
               else if (mValueLink && topEl == dtDAL::MapXMLConstants::DIRECTOR_LINK_VALUE_TYPE_CHECK_ELEMENT)
               {
                  mValueLink->SetTypeChecking(dtUtil::XMLStringConverter(chars).ToString() == "true");
               }

            }
            else if (!mPropSerializer->Characters(topEl, chars))
            {
               if (topEl == dtDAL::MapXMLConstants::ID_ELEMENT)
               {
                  mNode->SetID(dtCore::UniqueId(dtUtil::XMLStringConverter(chars).ToString()));
               }

            }
         }
         else if (!mPropSerializer->Characters(topEl, chars))
         {
            //else if (topEl == dtDAL::MapXMLConstants::NAME_ELEMENT)
            //{
            //   graph->mName = dtUtil::XMLStringConverter(chars).ToString();
            //}
         }
      }
   }

   /////////////////////////////////////////////////////////////////
   void DirectorXMLHandler::Reset()
   {
      dtDAL::BaseXMLHandler::Reset();

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
         if (XMLString::compareString(localname, dtDAL::MapXMLConstants::HEADER_ELEMENT) == 0)
         {
            mInHeaders = false;
         }
      }
   }

   //////////////////////////////////////////////////////////////////////////
   void DirectorXMLHandler::EndNodeElement()
   {
      // Initialize messages on event nodes.
      if (mInEventNodes && mNode.valid())
      {
         dtDirector::EventNode* node = dynamic_cast<dtDirector::EventNode*>(mNode.get());
         if (node)
         {
            node->RegisterMessages();
         }
      }

      ClearNodeValues();
   }

   //////////////////////////////////////////////////////////////////////////
   void DirectorXMLHandler::EndGraphSection(const XMLCh* const localname)
   {
      if (mInGraph)
      {
         if (XMLString::compareString(localname, dtDAL::MapXMLConstants::DIRECTOR_GRAPH_ELEMENT) == 0)
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
         if (XMLString::compareString(localname, dtDAL::MapXMLConstants::DIRECTOR_LINKS_INPUT_ELEMENT) == 0 ||
            XMLString::compareString(localname, dtDAL::MapXMLConstants::DIRECTOR_LINKS_OUTPUT_ELEMENT) == 0 ||
            XMLString::compareString(localname, dtDAL::MapXMLConstants::DIRECTOR_LINKS_VALUE_ELEMENT) == 0)
         {
            ClearLinkValues();
         }
         else if (XMLString::compareString(localname, dtDAL::MapXMLConstants::DIRECTOR_LINK_ELEMENT) == 0)
         {
            ClearLinkToValues();
         }
      }
      else if (XMLString::compareString(localname, dtDAL::MapXMLConstants::DIRECTOR_ACTION_NODES_ELEMENT) == 0 ||
               XMLString::compareString(localname, dtDAL::MapXMLConstants::DIRECTOR_EVENT_NODES_ELEMENT) == 0  ||
               XMLString::compareString(localname, dtDAL::MapXMLConstants::DIRECTOR_VALUE_NODES_ELEMENT) == 0)
      {
         EndNodesElement();
      }
      else if (XMLString::compareString(localname, dtDAL::MapXMLConstants::DIRECTOR_NODE_ELEMENT) == 0)
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
         mLogger->LogMessage(dtUtil::Log::LOG_ERROR, __FUNCTION__, __LINE__,
                             "Found the closing nodes section tag, but the content handler thinks it's still parsing a node.");
         mInNode = false;
      }
   }

   //////////////////////////////////////////////////////////////////////////
   void DirectorXMLHandler::EndLibrarySection(const XMLCh* const localname)
   {
      if (XMLString::compareString(localname, dtDAL::MapXMLConstants::LIBRARIES_ELEMENT) == 0)
      {
         mInLibraries = false;
      }
      else if (XMLString::compareString(localname, dtDAL::MapXMLConstants::LIBRARY_ELEMENT) == 0)
      {
         EndLibraryElement();
      }
   }

   //////////////////////////////////////////////////////////////////////////
   void DirectorXMLHandler::EndLibraryElement()
   {
      if (mLogger->IsLevelEnabled(dtUtil::Log::LOG_DEBUG))
      {
         mLogger->LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__,
            "Attempting to add library %s version %s to the library manager.",
            mLibName.c_str(),
            mLibVersion.c_str());
      }

      try
      {
         if (NodeManager::GetInstance().GetRegistry(mLibName) == NULL)
         {
            NodeManager::GetInstance().LoadNodeRegistry(mLibName);
         }
         mDirector->AddLibrary(mLibName, mLibVersion);
         ClearLibraryValues();
      }
      catch (const dtDAL::ProjectResourceErrorException& e)
      {
         mMissingLibraries.push_back(mLibName);

         mLogger->LogMessage(dtUtil::Log::LOG_ERROR, __FUNCTION__, __LINE__,
            "Error loading node library %s version %s in the director node manager.  Exception message to follow.",
            mLibName.c_str(), mLibVersion.c_str());

         e.LogException(dtUtil::Log::LOG_ERROR, *mLogger);
      }
      catch (const dtUtil::Exception& e)
      {
         mMissingLibraries.push_back(mLibName);

         mLogger->LogMessage(dtUtil::Log::LOG_ERROR, __FUNCTION__, __LINE__,
            "Unknown exception loading node library %s version %s in the director node manager.  Exception message to follow.",
            mLibName.c_str(), mLibVersion.c_str());

         e.LogException(dtUtil::Log::LOG_ERROR, *mLogger);
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
               ValueNode* valueNode = dynamic_cast<ValueNode*>(linkNode.get());
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

/*
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2007 MOVES Institute
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
 */

#include <dtAnim/posemeshxml.h>
#include <dtAnim/posemesh.h>
#include <dtCore/exceptionenum.h>
#include <dtUtil/xercesutils.h>
#include <dtUtil/exception.h>

#include <cstddef>
#include <cassert>
#include <sstream>

using namespace dtAnim;

const char PoseMeshFileHandler::POSE_NODE[]      = { "PoseMesh\0"  };
const char PoseMeshFileHandler::TRIANGLE_NODE[]  = { "Triangle\0"  };
const char PoseMeshFileHandler::ANIMATION_NODE[] = { "Animation\0" };

const char PoseMeshFileHandler::NAME_ATTRIBUTE[]             = { "name\0"                };
const char PoseMeshFileHandler::ROOT_ATTRIBUTE[]             = { "root\0"                };
const char PoseMeshFileHandler::ROOT_FORWARD_ATTRIBUTE[]     = { "rootForwardAxis\0"     };
const char PoseMeshFileHandler::EFFECTOR_ATTRIBUTE[]         = { "effector\0"            };
const char PoseMeshFileHandler::EFFECTOR_FORWARD_ATTRIBUTE[] = { "effectorForwardAxis\0" };

const char PoseMeshFileHandler::DEFAULT_VALUE[] = { "default\0" };


////////////////////////////////////////////////////////////////////////////////
PoseMeshFileHandler::PoseMeshFileHandler()
   : mMeshDataList()
   , mCurrentData()
   , mNodeStack()
   , mTriangleAnimations()
{
}

////////////////////////////////////////////////////////////////////////////////
PoseMeshFileHandler::~PoseMeshFileHandler()
{
}

////////////////////////////////////////////////////////////////////////////////
void PoseMeshFileHandler::startElement(const XMLCh* const uri,const XMLCh* const localname,
                                       const XMLCh* const qname, const XERCES_CPP_NAMESPACE_QUALIFIER Attributes& attrs)
{
   dtUtil::XMLStringConverter elementName(localname);
   const std::string elementStr = elementName.ToString();

   PoseNode currentNode(NODE_UNKNOWN);

   if (elementStr == POSE_NODE)
   {
      currentNode = NODE_POSEMESH;

      // Read in the attributes
      ReadPoseMeshNode(attrs);
   }
   else if (elementStr == TRIANGLE_NODE)
   {
      currentNode = NODE_TRIANGLE;
   }
   else if (elementStr == ANIMATION_NODE)
   {
      currentNode = NODE_ANIMATION;
   }

   mNodeStack.push(currentNode);
}

////////////////////////////////////////////////////////////////////////////////
void PoseMeshFileHandler::endElement(const XMLCh* const uri,const XMLCh* const localname,const XMLCh* const qname)
{
   dtUtil::XMLStringConverter elementName(localname);
   const std::string elementStr = elementName.ToString();

   if (elementStr == POSE_NODE)
   {
      mMeshDataList.push_back(mCurrentData);
      mCurrentData.mAnimations.clear();
   }
   else if (elementStr == TRIANGLE_NODE)
   {
      assert((mCurrentData.mAnimations.size() % 3) == 0);
   }

   mNodeStack.pop();
}

////////////////////////////////////////////////////////////////////////////////
void PoseMeshFileHandler::characters(const XMLCh* const chars, const unsigned int length)
{
   switch (mNodeStack.top())
   {
      case NODE_ANIMATION:
      {
         dtUtil::XMLStringConverter data(chars);
         mCurrentData.mAnimations.push_back(data.ToString());

      } break;

      case NODE_UNKNOWN:
      default:
      {
      } break;
   }
}

////////////////////////////////////////////////////////////////////////////////
void dtAnim::PoseMeshFileHandler::ReadPoseMeshNode(const XERCES_CPP_NAMESPACE_QUALIFIER Attributes& attrs)
{
   bool success = true;

   dtUtil::AttributeSearch search;
   dtUtil::AttributeSearch::ResultMap results;
   dtUtil::AttributeSearch::ResultMap::iterator resultIter;

   results    = search(attrs);
   resultIter = results.find(NAME_ATTRIBUTE);

   // Get the name attribute
   if (resultIter != results.end())
   {
      mCurrentData.mName = resultIter->second;
   }
   else
   {
      mCurrentData.mName = DEFAULT_VALUE;
   }

   // Get the root bone attribute
   resultIter = results.find(ROOT_ATTRIBUTE);
   success = success && (resultIter != results.end());

   if (resultIter != results.end())
   {
      mCurrentData.mRootName = resultIter->second;
   }

   // Get the root forward axis direction
   resultIter = results.find(ROOT_FORWARD_ATTRIBUTE);
   success = success && (resultIter != results.end());

   if (resultIter != results.end())
   {
      std::istringstream iss(resultIter->second);
      float xAxis, yAxis, zAxis;

      iss >> xAxis >> yAxis >> zAxis;
      mCurrentData.mRootForward.set(xAxis, yAxis, zAxis);
   }

   // Get the effector bone attribute
   resultIter = results.find(EFFECTOR_ATTRIBUTE);
   success = success && (resultIter != results.end());

   if (resultIter != results.end())
   {
      mCurrentData.mEffectorName = resultIter->second;
   }

   // Get the effector forward axis direction
   resultIter = results.find(EFFECTOR_FORWARD_ATTRIBUTE);
   success = success && (resultIter != results.end());

   if (resultIter != results.end())
   {
      std::istringstream iss(resultIter->second);
      float xAxis, yAxis, zAxis;

      iss >> xAxis >> yAxis >> zAxis;
      mCurrentData.mEffectorForward.set(xAxis, yAxis, zAxis);
   }

   // Report errors
   if (!success)
   {
      throw dtUtil::Exception(dtCore::ExceptionEnum::INVALID_PARAMETER,
         "Invalid or missing parameter in PoseMesh file.",
         __FILE__, __LINE__);
   }
}

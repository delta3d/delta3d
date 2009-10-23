/*
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2004-2008 MOVES Institute
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
 * Michael Guerrero
 */

#ifndef __POSE_MESH_XML_H__
#define __POSE_MESH_XML_H__

#include <xercesc/sax2/ContentHandler.hpp>  // for a base class
#include <vector>
#include <stack>
#include <string>
#include <osg/Vec3>


namespace dtAnim
{
   typedef std::vector<std::string> StringVector;

   struct PoseMeshData
   {
      std::string  mName;            ///< an identifier for this pose mesh instance
      std::string  mRootName;        ///< an identifier for the bone that will give the global forward
      osg::Vec3    mRootForward;     ///< the direction that end effectors are relative to
      std::string  mEffectorName;    ///< an identifier for the end effector bone     
      osg::Vec3    mEffectorForward; ///< the direction that is forward in the end effector's bone's space
      StringVector mAnimations;      ///< a list of name triples for animation triangles
   };


   class PoseMeshFileHandler : public XERCES_CPP_NAMESPACE_QUALIFIER ContentHandler
   {
   public:

      enum PoseNode
      {
         NODE_UNKNOWN,
         NODE_POSEMESH,
         NODE_TRIANGLE,
         NODE_ANIMATION
      };

      typedef std::stack<PoseNode> NodeStack;
      typedef std::vector<PoseMeshData> PoseMeshDataVector;
      
      // Node names
      static const char POSE_NODE[];
      static const char TRIANGLE_NODE[];
      static const char ANIMATION_NODE[];

      // Attribute names
      static const char NAME_ATTRIBUTE[];
      static const char ROOT_ATTRIBUTE[];
      static const char ROOT_FORWARD_ATTRIBUTE[];
      static const char EFFECTOR_ATTRIBUTE[];
      static const char EFFECTOR_FORWARD_ATTRIBUTE[];
      static const char DEFAULT_VALUE[];


      PoseMeshFileHandler();
      ~PoseMeshFileHandler();

      void characters(const XMLCh* const chars, const unsigned int length);

      void startElement(const XMLCh* const uri,const XMLCh* const localname,
         const XMLCh* const qname, const XERCES_CPP_NAMESPACE_QUALIFIER Attributes& attrs);

      void endElement(const XMLCh* const uri,const XMLCh* const localname,const XMLCh* const qname);

      // Required functions
      void ignorableWhitespace(const XMLCh* const chars, const unsigned int length) {}
      void processingInstruction(const XMLCh* const target, const XMLCh* const data) {}
      void setDocumentLocator(const XERCES_CPP_NAMESPACE_QUALIFIER Locator* const locator) {}
      void startPrefixMapping(const XMLCh* const prefix, const XMLCh* const uri) {};
      void endPrefixMapping(const XMLCh* const prefix) {};
      void skippedEntity(const XMLCh* const name) {};
      void startDocument() {}
      void endDocument() {}

      const PoseMeshDataVector& GetData() { return mMeshDataList; }

   private:
      PoseMeshFileHandler(const PoseMeshFileHandler&);

      PoseMeshDataVector mMeshDataList;
      PoseMeshData       mCurrentData;
      NodeStack          mNodeStack;
      StringVector       mTriangleAnimations;

      void ReadPoseMeshNode(const XERCES_CPP_NAMESPACE_QUALIFIER Attributes& attrs);
   };
}


#endif // __POSE_MESH_XML_H__

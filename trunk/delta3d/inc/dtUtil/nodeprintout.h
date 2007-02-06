/* -*-c++-*-
 * Delta3D Open Source Game and Simulation Engine 
 * Copyright (C) 2006 MOVES Institute 
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
 * allen -morgas on forums- danklefsen, Curtiss Murphy
 */

#ifndef __DELTA_NODE_PRINT_OUT__
#define __DELTA_NODE_PRINT_OUT__

#include <dtUtil/export.h>
#include <cstdio>
#include <osg/Referenced>
#include <osg/Node>
#include <sstream>

// Foward declarations
namespace osg
{
   //class Node;
   class Geode;
   class PrimitiveSet;
}

namespace dtUtil
{
   class DT_UTIL_EXPORT NodePrintOut : public osg::Referenced
   {
      public:
         NodePrintOut();

      protected:
         virtual ~NodePrintOut() {}

      public:
         /// Called from anyone that wants to print out a file, takes in a node, outputs file*
         void PrintOutNode(std::string& printOutFileName, osg::Node* nodeToPrint, bool PrintVerts = false, bool printToFile = true);
      
         /// Returns the file stream
         std::string GetFileOutput() const;

      protected:
         /// Called from printoutnode user should never call
         void Analyze(osg::Node *nd, std::string indent);

         /// Called from Analyze user should never call
         void AnalyzeGeode(osg::Geode *geode, std::string indent);

         /// Called from AnalyzeGeode user should never call
         void AnalyzePrimSet(osg::PrimitiveSet* prset, const osg::Vec3Array *verts, std::string indent);

      private:
         /// to keep track for file* sake
         unsigned int mTabAmount;

         /// What is used to print open close etc
         FILE* mFile;

         /// do we want to print out extensive vertice data?
         bool mPrintingVerts;

         std::ostringstream mOutputStream[3];
   };
}

#endif


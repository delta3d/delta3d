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
#include <osg/Referenced>
#include <osg/Node>
#include <sstream>

// Foward declarations
/// @cond DOXYGEN_SHOULD_SKIP_THIS
namespace osg
{
   class Geode;
   class PrimitiveSet;
}
/// @endcond

namespace dtUtil
{
   ///Utility class used to traverse a node and generate a formatted text output
   /** Example:
    * @code
    * osg::Node *node = LoadFile("myFile.ive");
    * RefPtr<NodePrintOut> printout = new NodePrintOut();
    * std::cout << printout->CollectNodeData(*node) << std::endl;
    * @endcode
    */

   class DT_UTIL_EXPORT NodePrintOut : public osg::Referenced
   {
      public:
         NodePrintOut();

      protected:
         virtual ~NodePrintOut() {}

      public:
         /** Traverse a node's graph and generate a text printout of the
           * hierarchy.
           * @param nodeToPrint : the node to traverse
           * @param outputFilename : an optional filename to save the output to (default = "")
           * @param printVertData : optionally print out vertex information (default = false)
           * @param nodeMask : will only print out nodes that have a node mask that share
           *                   at least one bit.
           * @return The formatted string output.
           *
           */
         std::string CollectNodeData(const osg::Node& nodeToPrint,
                                     const std::string& outputFilename = "",
                                     bool printVertData = false,
                                     unsigned int nodeMask = 0xFFFFFFFF
                                     );

         /// Returns the file stream
         std::string GetFileOutput() const;

         /// Dumps a node to an osg file
         void PrintNodeToOSGFile(const osg::Node& node, const std::string& fileName);

         /// Dumps a node to a stream in osgb format or the old .osg format if requested
         void PrintNodeToOSGFile(const osg::Node& node, std::ostringstream& oss, bool oldOsgFormat = false);

      protected:
         /// Called from printoutnode user should never call
         void Analyze(const osg::Node& nd, const std::string& indent, unsigned int nodeMask);

         /// Called from Analyze user should never call
         void AnalyzeGeode(const osg::Geode& geode, const std::string& indent);

         /// Called from AnalyzeGeode user should never call
         void AnalyzePrimSet(const osg::PrimitiveSet& prset, const osg::Vec3Array& verts, const std::string& indent);

      private:

         /// What is used to print open close etc
         FILE* mFile;

         /// do we want to print out extensive vertice data?
         bool mPrintingVerts;

         std::ostringstream mOutputStream[3];
   };
}

#endif


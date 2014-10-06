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

#include <prefix/dtutilprefix.h>
#include <dtUtil/nodeprintout.h>
#include <dtUtil/fileutils.h>
#include <dtUtil/log.h>
#include <dtUtil/bits.h>

#include <sstream>
#include <fstream>
#include <osg/Geode>
#include <osg/Geometry>
#include <osg/Group>
#include <osg/Node>
#include <osg/PrimitiveSet>
#include <osgDB/WriteFile>

namespace dtUtil
{

   NodePrintOut::NodePrintOut()
      : mFile(NULL)
      , mPrintingVerts(false)
   {
   }

   //////////////////////////////////////////////////////////////////////////
   std::string NodePrintOut::CollectNodeData(const osg::Node& nodeToPrint,
                                             const std::string& outputFilename /*= ""*/,
                                             bool printVertData /*= false */,
                                             unsigned int nodeMask /* 0xFFFFFFFF */
                                             )
   {
      // Clear any previous data
      for (int i = 0; i < 3; ++i)
      {
         mOutputStream[i].str("");
      }

      mPrintingVerts = printVertData;
      Analyze(nodeToPrint, "", nodeMask);

      if (!outputFilename.empty())
      {
         mFile = fopen(outputFilename.c_str(), "w");

         for (int i = 0; i < 3; ++i)
         {
            fwrite(mOutputStream[i].str().c_str(),
               mOutputStream[i].str().size() * sizeof(char),
               1,
               mFile);
         }

         fclose(mFile);
      }

      return GetFileOutput();
   }

   /// Called from printoutnode user should never call
   //////////////////////////////////////////////////////////////////////////
   void NodePrintOut::Analyze(const osg::Node& nd, const std::string& indent, unsigned int nodeMask)
   {
      if ((nd.getNodeMask() & nodeMask) == 0U)
      {
         return;
      }

      mOutputStream[0] << indent << nd.libraryName() << "::" <<  nd.className() <<
         " - Node Name [" << nd.getName() << "], Node Mask [0x" << std::hex <<
         nd.getNodeMask() << "]" << std::endl;

      const osg::Geode* geode = dynamic_cast<const osg::Geode*>(&nd);
      if (geode != NULL)
      {
         // Analyze the geode. If it isnt a geode the dynamic cast gives NULL.
         AnalyzeGeode(*geode, indent);
      }
      else
      {
         const osg::Group* gp = dynamic_cast<const osg::Group*>(&nd);
         if (gp != NULL)
         {
            for (unsigned int ic=0; ic < gp->getNumChildren(); ++ic)
            {
               std::string newIndent = indent + "   ";
               Analyze(*gp->getChild(ic), newIndent, nodeMask);
            }
         }
      }
   } // divide the geode into its drawables and primitivesets:

   /// Called from Analyze user should never call
   void NodePrintOut::AnalyzeGeode(const osg::Geode& geode, const std::string& indent)
   {
      if (mPrintingVerts)
      {
         for (unsigned int i = 0; i < geode.getNumDrawables(); ++i)
         {
            const osg::Drawable* drawable = geode.getDrawable(i);
            const osg::Geometry* geom = dynamic_cast<const osg::Geometry*>(drawable);
            if (geom)
            {
               for (unsigned int ipr = 0; ipr < geom->getNumPrimitiveSets(); ++ipr)
               {
                  const osg::PrimitiveSet* prset = geom->getPrimitiveSet(ipr);
                  mOutputStream[0] << indent << "Primitive Set " << ipr << std::endl;
                  AnalyzePrimSet(*prset, *static_cast<const osg::Vec3Array*>(geom->getVertexArray()), indent + "   ");
               }
            }
         }
      }
   }

   /// Called from AnalyzeGeode user should never call
   void NodePrintOut::AnalyzePrimSet(const osg::PrimitiveSet& prset, const osg::Vec3Array& verts, const std::string& indent)
   {
      mOutputStream[0] << indent << "Prim set type "<< prset.getMode() << std::endl;

      if (mPrintingVerts)
      {
         unsigned int ic;
         unsigned int nprim=0;
         for (ic = 0; ic < prset.getNumIndices(); ++ic)
         {
            // NB the vertices are held in the drawable -
            mOutputStream[1] << indent <<  "vertex "<< ic << " is index "<< prset.index(ic) << " at " <<
            (verts)[prset.index(ic)].x() << "," <<
            (verts)[prset.index(ic)].y() << "," <<
            (verts)[prset.index(ic)].z() << std::endl;

         }
         // you might want to handle each type of primset differently: such as:

         switch (prset.getMode())
         {
         case osg::PrimitiveSet::TRIANGLES: // get vertices of triangle
            {
               mOutputStream[2] << indent << "Triangles "<< nprim << " is index "<<prset.index(ic) << std::endl;
               for (unsigned int i2 = 0; i2 < prset.getNumIndices() - 2; i2 += 3)
               {
                  // This is where you would write your indices out with the information they have.
               }
            }
            break;

         default:
            break;
         }
      }
   }

   std::string NodePrintOut::GetFileOutput() const
   {
      std::string result;
      for (int i = 0; i < 3; ++i)
      {
         result += mOutputStream[i].str();
      }

      return result;
   }

   void NodePrintOut::PrintNodeToOSGFile(const osg::Node& node, const std::string& fileName)
   {
      if (!osgDB::writeNodeFile(node, fileName))
      {
         std::ostringstream oss;
         oss << "Failed to write node: " << node.getName() << " to file: " << fileName;
         LOG_ERROR(oss.str());
      }
   }

   void NodePrintOut::PrintNodeToOSGFile(const osg::Node& node, std::ostringstream& oss, bool oldOsgFormat)
   {
      oss.str("");
      std::string tempFile = "temp.osgt";
      if (oldOsgFormat)
      {
         tempFile = "temp.osg";
      }

      osgDB::writeNodeFile(node, tempFile);

      std::ifstream in(tempFile.c_str());
      if (!in.is_open())
      {
         return;
      }

      std::string toWrite;
      while (!in.eof())
      {
         char buffer[1024] = { 0 };
         in.getline(buffer, 1023);
         toWrite += buffer;
         toWrite += '\n';
      }

      oss << toWrite;

      in.close();

      dtUtil::FileUtils::GetInstance().FileDelete(tempFile);
   }

} // namespace dtUtil

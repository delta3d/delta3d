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

#include <prefix/dtutilprefix-src.h>
#include <dtUtil/nodeprintout.h>

#include <sstream>
#include <osg/Geode>
#include <osg/Geometry>
#include <osg/Group>
#include <osg/Node>
#include <osg/PrimitiveSet>
#include <osgDB/WriteFile>

namespace dtUtil
{

   NodePrintOut::NodePrintOut(): mTabAmount(0)
      , mFile(NULL)
      , mPrintingVerts(0)
   {

   }


   /// Called from anyone that wants to print out a file, takes in a node, outputs file*
   void NodePrintOut::PrintOutNode(std::string& printOutFileName, osg::Node* nodeToPrint, 
                                   bool PrintVerts, bool printToFile) 
   {
      if(nodeToPrint == NULL)
         return;

      // Clear any previous data
      for(int i = 0; i < 3; i++)
      {
         mOutputStream[i].str("");
      }

      mPrintingVerts = PrintVerts;
      Analyze(nodeToPrint, "");
      
      if(printToFile)
      {
         mFile = fopen(printOutFileName.c_str(), "w");

         for(int i = 0; i < 3; i++)
         {
            fwrite(mOutputStream[i].str().c_str(), 
                  mOutputStream[i].str().size() * sizeof(char), 
                  1, 
                  mFile);
         }

         fclose(mFile);
      }
   }

   /// Called from printoutnode user should never call
   void NodePrintOut::Analyze(osg::Node *nd, std::string indent)
   {
      mOutputStream[0] << indent << "Node - Class Name [" <<  nd->className() << "], Node Name [" << nd->getName() << "]" << std::endl;
      //fwrite((void*)StreamOne.str().c_str(), StreamOne.str().size() * sizeof(char), 1, mFile);

      osg::Geode *geode = dynamic_cast<osg::Geode *> (nd);
      if (geode != NULL) 
      { 
         // Analyze the geode. If it isnt a geode the dynamic cast gives NULL.
         AnalyzeGeode(geode, indent);
      } 
      else 
      {
         osg::Group *gp = dynamic_cast<osg::Group *> (nd);
         if (gp != NULL)
         {
            //if(gp->getName() != "")
            //{
            //   std::ostringstream StreamOne;
            //   StreamOne << indent << "Group Name = [" <<  gp->getName() << "]" << std::endl;
            //   fwrite((void*)StreamOne.str().c_str(), StreamOne.str().size() * sizeof(char), 1, mFile);
            //}

            for(unsigned int ic=0; ic<gp->getNumChildren(); ic++) 
            {
               Analyze(gp->getChild(ic), indent + "   ");
            }
         }
      }
   } // divide the geode into its drawables and primitivesets: 

   /// Called from Analyze user should never call
   void NodePrintOut::AnalyzeGeode(osg::Geode *geode, std::string indent)
   {
      if (mPrintingVerts)
      {

         for (unsigned int i=0; i<geode->getNumDrawables(); i++) 
         {
            osg::Drawable *drawable=geode->getDrawable(i);
            osg::Geometry *geom = dynamic_cast<osg::Geometry*>(drawable);
            for (unsigned int ipr=0; ipr<geom->getNumPrimitiveSets(); ipr++)
            {
               osg::PrimitiveSet* prset=geom->getPrimitiveSet(ipr);
               //std::ostringstream StreamOne;
               mOutputStream[0] << indent << "Primitive Set " << ipr << std::endl;
               //fwrite((void*)StreamOne.str().c_str(), StreamOne.str().size() * sizeof(char), 1, mFile);
               AnalyzePrimSet(prset, static_cast<const osg::Vec3Array*>(geom->getVertexArray()), indent + "   ");
            }
         }
      }
   }

   /// Called from AnalyzeGeode user should never call
   void NodePrintOut::AnalyzePrimSet(osg::PrimitiveSet* prset, const osg::Vec3Array *verts, std::string indent)
   {
      //std::ostringstream StreamOne;
      mOutputStream[0] << indent << "Prim set type "<< prset->getMode() << std::endl;
      //fwrite((void*)StreamOne.str().c_str(), StreamOne.str().size() * sizeof(char), 1, mFile);

      if(mPrintingVerts)
      {
         unsigned int ic;
         unsigned int nprim=0;
         for (ic=0; ic < prset->getNumIndices(); ic++)
         { 
            // NB the vertices are held in the drawable -
            //std::ostringstream StreamTwo;
            mOutputStream[1] << indent <<  "vertex "<< ic << " is index "<<prset->index(ic) << " at " <<
            (* verts)[prset->index(ic)].x() << "," <<
            (* verts)[prset->index(ic)].y() << "," << 
            (* verts)[prset->index(ic)].z() << std::endl;

            //fwrite((void*)StreamTwo.str().c_str(), StreamTwo.str().size() * sizeof(char), 1, mFile);
         }
         // you might want to handle each type of primset differently: such as:

         //std::ostringstream StreamThr;
         switch (prset->getMode()) 
         {
            case osg::PrimitiveSet::TRIANGLES: // get vertices of triangle
            {
               mOutputStream[2] << indent << "Triangles "<< nprim << " is index "<<prset->index(ic) << std::endl;
               for(unsigned int i2=0; i2<prset->getNumIndices()-2; i2+=3) 
               {
                  // This is where you would write your indices out with the information they have.
               }
               //fwrite((void*)StreamThr.str().c_str(), StreamThr.str().size() * sizeof(char), 1, mFile);
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
      for(int i = 0; i < 3; i++)
         result += mOutputStream[i].str();

      return result;
   }

   void NodePrintOut::PrintNodeToOSGFile(const osg::Node &node, const std::string &fileName)
   {
      if(!osgDB::writeNodeFile(node, fileName))
      {
         std::ostringstream oss;
         oss << "Failed to write node: " << node.getName() << " to file: " << fileName;
         LOG_ERROR(oss.str());
      }
   }

   void NodePrintOut::PrintNodeToOSGFile(const osg::Node &node, std::ostringstream &oss)
   {
      oss.str("");
      const std::string &tempFile = "temp.osg";
      osgDB::writeNodeFile(node, tempFile);

      std::ifstream in(tempFile.c_str());
      if(!in.is_open())
         return;

      std::string toWrite;
      while(!in.eof())
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
}

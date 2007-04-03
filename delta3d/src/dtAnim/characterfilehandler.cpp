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
 * 
 * Erik Johnson
 */
#include <dtAnim/characterfilehandler.h>
#include <dtUtil/xercesutils.h>
#include <dtUtil/log.h>

using namespace dtAnim;

void CharacterFileHandler::startElement( const XMLCh* const uri,const XMLCh* const localname,
                                                 const XMLCh* const qname, const XERCES_CPP_NAMESPACE_QUALIFIER Attributes& attrs )
{
   dtUtil::XMLStringConverter elementName(localname);
   const std::string elementStr = elementName.ToString();

   dtUtil::AttributeSearch search;
   dtUtil::AttributeSearch::ResultMap results;
   dtUtil::AttributeSearch::ResultMap::iterator resultIter;
   results = search(attrs);

   std::string errorString;

   if (elementStr == "character")
   {
      resultIter = results.find("name");

      if (resultIter != results.end())
      {
          mName = resultIter->second;
      }
      else
      {
         errorString = std::string("Invalid XML format: <character> missing <name> child");
      }     
   }
   else if (elementStr == "skeleton")
   {     
      resultIter = results.find("filename");

      if (resultIter != results.end())
      {
         mSkeletonFilename = resultIter->second;
      }
      else
      {
         errorString = std::string("Invalid XML format: <character> missing <name> child");
      }     
   }
   else if (elementStr == "animation")
   {      
      resultIter = results.find("filename");

      if (resultIter != results.end())
      {
         std::string filename = resultIter->second;
         
         //default the name of the animation to be the filename
         std::string name = resultIter->second; 

         resultIter = results.find("name");
         if (resultIter != results.end() )
         {
            name = resultIter->second;
         }
         AnimationStruct anim;
         anim.filename = filename;
         anim.name = name;
         mAnimations.push_back(anim);
      }
      else
      {
         errorString = std::string("Invalid XML format: <animation> missing <filename> child");
      }     
   }
   else if (elementStr == "mesh")
   {     
      resultIter = results.find("filename");

      if (resultIter != results.end())
      {
         std::string filename = resultIter->second;

         //default the mesh name to be the same as the filename
         std::string meshName = filename;

         resultIter = results.find("name");
         if (resultIter != results.end() )
         {
            meshName = resultIter->second;
         }
         
         MeshStruct mesh;
         mesh.filename = filename;
         mesh.name = meshName;

         mMeshes.push_back(mesh);
      }
      else
      {
         errorString = std::string("Invalid XML format: <mesh> missing <filename> child");
      }     
   }
   else if (elementStr == "material")
   {      
      resultIter = results.find("filename");

      if (resultIter != results.end())
      {
         mMaterialFilenames.push_back(resultIter->second);
      }
      else
      {
         errorString = std::string("Invalid XML format: <character> missing <name> child");
      }     
   }

   if (!errorString.empty())
   {      
      LOG_ERROR(errorString);
   }
}

CharacterFileHandler::CharacterFileHandler()
{

}

CharacterFileHandler::~CharacterFileHandler()
{

}
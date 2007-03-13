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
#include <dtChar/characterfilehandler.h>
#include <dtUtil/xercesutils.h>

using namespace dtChar;

void CharacterFileHandler::startElement( const XMLCh* const uri,const XMLCh* const localname,
                                                 const XMLCh* const qname, const XERCES_CPP_NAMESPACE_QUALIFIER Attributes& attrs )
{
   dtUtil::XMLStringConverter elementName(localname);
   const std::string elementStr = elementName.ToString();

   dtUtil::AttributeSearch search;
   dtUtil::AttributeSearch::ResultMap results;
   results = search(attrs);

   if (elementStr == "skeleton")
   {
      mSkeletonFilename = results.find("filename")->second;
   }
   else if (elementStr == "animation")
   {
      mAnimationFilenames.push_back(results.find("filename")->second);
   }
   else if (elementStr == "mesh")
   {
      mMeshFilenames.push_back(results.find("filename")->second);
   }
   else if (elementStr == "material")
   {
      mMaterialFilenames.push_back(results.find("filename")->second);
   }

}

CharacterFileHandler::CharacterFileHandler()
{

}

CharacterFileHandler::~CharacterFileHandler()
{

}
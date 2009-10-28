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

#include <dtAnim/posemeshloader.h>
#include <dtAnim/posemeshxml.h>

#include <dtUtil/xercesparser.h>
#include <dtUtil/log.h>
#include <dtUtil/exception.h>

using namespace dtAnim;

////////////////////////////////////////////////////////////////////////////////
PoseMeshLoader::PoseMeshLoader()
{
}

////////////////////////////////////////////////////////////////////////////////
PoseMeshLoader::~PoseMeshLoader()
{
}

////////////////////////////////////////////////////////////////////////////////
bool PoseMeshLoader::Load(const std::string& file, MeshDataContainer& toFill)
{
   PoseMeshFileHandler handler;
   dtUtil::XercesParser parser;
   bool result = false;

   try
   {
      result = parser.Parse(file, handler, "");
   }
   catch (const dtUtil::Exception& ex)
   {
      ex.LogException(dtUtil::Log::LOG_ERROR);
   }

   if (result)
   {
      toFill = handler.GetData();
   }
   else
   {
      LOG_ERROR("Unable to load pose mesh file: " + file);
   }

   return result;
}

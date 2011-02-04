/* -*-c++-*-
 * Delta3D
 * Copyright 2011, Alion Science and Technology
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
 * This software was developed by Alion Science and Technology Corporation under
 * circumstances in which the U. S. Government may have rights in the software.
 * 
 * David Guthrie
 */

#include <dtDAL/projectconfig.h>
#include <dtUtil/exception.h>

namespace dtDAL
{
   ContextData::ContextData(const std::string& path)
   : mPath(path)
   {
   }

   DT_IMPLEMENT_ACCESSOR(ContextData, std::string, Path);

   ProjectConfig::ProjectConfig()
   : mReadOnly(false)
   {
   }

   ProjectConfig::~ProjectConfig()
   {
   }

   DT_IMPLEMENT_ACCESSOR(ProjectConfig, std::string, Name);
   DT_IMPLEMENT_ACCESSOR(ProjectConfig, std::string, Description);
   DT_IMPLEMENT_ACCESSOR(ProjectConfig, std::string, Author);
   DT_IMPLEMENT_ACCESSOR(ProjectConfig, std::string, Comment);
   DT_IMPLEMENT_ACCESSOR(ProjectConfig, std::string, Copyright);
   DT_IMPLEMENT_ACCESSOR(ProjectConfig, bool, ReadOnly);

   DT_IMPLEMENT_ARRAY_ACCESSOR(ProjectConfig, ContextData, ContextData, ContextData, ContextData(std::string("")));
}

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

#include <prefix/dtcoreprefix.h>
#include <dtCore/projectconfig.h>
#include <dtUtil/exception.h>
#include <dtUtil/fileutils.h>

namespace dtCore
{
   ContextData::ContextData(const std::string& path)
   : mPath(path)
   {
   }

   DT_IMPLEMENT_ACCESSOR(ContextData, std::string, Path)

   bool ContextData::operator==(const ContextData& toCompare) const
   {
      return mPath == toCompare.mPath;
   }

   std::ostream& operator << (std::ostream& out, const ContextData& data)
   {
      out << data.GetPath();
      return out;
   }

   ProjectConfig::ProjectConfig()
   : mReadOnly(false)
   {
   }

   ProjectConfig::~ProjectConfig()
   {
   }

   void ProjectConfig::ConvertContextDataToRelativeOfBasePath()
   {
      if (mBasePath.empty()) { return; }

      dtUtil::FileUtils& fileUtils = dtUtil::FileUtils::GetInstance();
      if (!fileUtils.DirExists(mBasePath))
      {
         return;
      }


      // reusable temp string.
      std::string contextPath;
      for (unsigned i = 0; i < mContextData.size(); ++i)
      {
         contextPath = mContextData[i].GetPath();
         contextPath = fileUtils.GetAbsolutePath(contextPath);

         // Using current directory instead of the mBasePath because that will automatically be the absolute.
         mContextData[i] = fileUtils.RelativePath(fileUtils.GetAbsolutePath(mBasePath), contextPath);
      }
   }


   DT_IMPLEMENT_ACCESSOR(ProjectConfig, std::string, BasePath)

   DT_IMPLEMENT_ACCESSOR(ProjectConfig, std::string, Name)
   DT_IMPLEMENT_ACCESSOR(ProjectConfig, std::string, Description)
   DT_IMPLEMENT_ACCESSOR(ProjectConfig, std::string, Author)
   DT_IMPLEMENT_ACCESSOR(ProjectConfig, std::string, Comment)
   DT_IMPLEMENT_ACCESSOR(ProjectConfig, std::string, Copyright)
   DT_IMPLEMENT_ACCESSOR(ProjectConfig, bool, ReadOnly)

   DT_IMPLEMENT_ARRAY_ACCESSOR(ProjectConfig, ContextData, ContextData, ContextData, ContextData(std::string("")))
}

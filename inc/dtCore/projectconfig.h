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

#ifndef PROJECTCONFIG_H_
#define PROJECTCONFIG_H_

#include <osg/Referenced>
#include <dtCore/export.h>
#include <dtUtil/getsetmacros.h>
#include <string>
#include <algorithm> // for_each in ContextData

namespace dtCore
{

   class DT_CORE_EXPORT ContextData
   {
   public:
      ContextData(const std::string& path);
      DT_DECLARE_ACCESSOR(std::string, Path)
      bool operator==(const ContextData&) const;
   };

   DT_CORE_EXPORT std::ostream& operator << (std::ostream& out, const ContextData& data);

   /**
    * Defines a configuration for a project that could span multiple directories.
    */
   class DT_CORE_EXPORT ProjectConfig : public osg::Referenced
   {
   public:
      ProjectConfig();

      /// The base path is root from which the relative paths of the context data are calculated.
      DT_DECLARE_ACCESSOR(std::string, BasePath)

      DT_DECLARE_ACCESSOR(std::string, Name)
      DT_DECLARE_ACCESSOR(std::string, Description)
      DT_DECLARE_ACCESSOR(std::string, Author)
      DT_DECLARE_ACCESSOR(std::string, Comment)
      DT_DECLARE_ACCESSOR(std::string, Copyright)
      DT_DECLARE_ACCESSOR(bool, ReadOnly)

      void ConvertContextDataToRelativeOfBasePath();

      DT_DECLARE_ARRAY_ACCESSOR(ContextData, ContextData, ContextData)
   private:
      virtual ~ProjectConfig();
   };

}

#endif /* PROJECTCONFIG_H_ */

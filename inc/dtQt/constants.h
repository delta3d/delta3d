/* -*-c++-*-
* Copyright (C) 2015, Caper Holdings LLC
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
* Chris Rodgers
*/

#ifndef DELTA_CONSTANTS_H
#define DELTA_CONSTANTS_H

////////////////////////////////////////////////////////////////////////////////
// INCLUDE DIRECTIVES
////////////////////////////////////////////////////////////////////////////////
#include <dtQt/export.h>
#include <QtCore/QString>
#include <dtUtil/nodetypes.h>



namespace dtQt
{
   /////////////////////////////////////////////////////////////////////////////
   // CLASS CODE
   /////////////////////////////////////////////////////////////////////////////
   class DT_QT_EXPORT Constants
   {
   public:
      static const QString ICON_NODE;
      static const QString ICON_BONE;
      static const QString ICON_DOF;
      static const QString ICON_GEODE;
      static const QString ICON_GEOMETRY;
      static const QString ICON_GROUP;
      static const QString ICON_LOD;
      static const QString ICON_MATRIX;
      static const QString ICON_OCCLUDER;
      static const QString ICON_OCCLUSION_QUERY;
      static const QString ICON_SKELETON;
      static const QString ICON_STATESET;

      static const QString* GetIconPathByClassName(const std::string& className);

   private:
      Constants() {}
      virtual ~Constants() {}
   };
}

#endif

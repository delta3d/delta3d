/* 
* Delta3D Open Source Game and Simulation Engine 
* Copyright (C) 2005 MOVES Institute 
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
*/

#ifndef DTDAL_INCLUDED
#define DTDAL_INCLUDED

#include <dtDAL/actorpluginregistry.h>
#include <dtDAL/actorproperty.h>
#include <dtDAL/actorproxy.h>
#include <dtDAL/actorproxyicon.h>
#include <dtDAL/actortype.h>
#include <dtDAL/datatype.h>
#include <dtDAL/directoryresourcetypehandler.h>
#include <dtDAL/enginepropertytypes.h>
#include <dtDAL/exceptionenum.h>
#include <dtDAL/fileutils.h>
#include <dtDAL/functor.h>
#include <dtDAL/librarymanager.h>
#include <dtDAL/map.h>
#include <dtDAL/mapxml.h>
#include <dtDAL/physicalactorproxy.h>
#include <dtDAL/plugin_export.h>
#include <dtDAL/project.h>
#include <dtDAL/rbodyresourcetypehandler.h>
#include <dtDAL/resourcedescriptor.h>
#include <dtDAL/resourcehelper.h>
#include <dtDAL/resourcetreenode.h>
#include <dtDAL/transformableactorproxy.h>
#include <dtDAL/export.h>

/** The Dynamic Actor Layer contains functionality to support the loading of 
  * map files and Actor Proxy support.
  */
namespace dtDAL
{
}

#if defined(_WIN32) || defined(WIN32) || defined(__WIN32__)

/* You may turn off this include message by defining _NOAUTOLIB */
#undef _AUTOLIBNAME

#if defined(_DEBUG)
#define _AUTOLIBNAME   "dtDALd.lib"
   #else 
#define _AUTOLIBNAME   "dtDAL.lib"  
#endif

#ifndef _NOAUTOLIBMSG
   #pragma message( "Will automatically link with " _AUTOLIBNAME )
#endif

#pragma  comment( lib, _AUTOLIBNAME )

#endif  // defined(_WIN32) || defined(WIN32) || defined(__WIN32__)

#endif //DTDAL_INCLUDED

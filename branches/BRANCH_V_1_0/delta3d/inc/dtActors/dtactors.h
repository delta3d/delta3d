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

#ifndef DTACTORS_INCLUDED
#define DTACTORS_INCLUDED


#include <dtActors/baselightactorproxy.h>
#include <dtActors/cameraactorproxy.h>
#include <dtActors/characteractorproxy.h>
#include <dtActors/clouddomeactorproxy.h>
#include <dtActors/cloudplaneactorproxy.h>
#include <dtActors/deltaobjectactorproxy.h>
#include <dtActors/engineactorregistry.h>
#include <dtActors/enveffectactorproxy.h>
#include <dtActors/environmentactorproxy.h>
#include <dtActors/infinitelightactorproxy.h>
#include <dtActors/infiniteterrainactorproxy.h>
#include <dtActors/meshterrainactorproxy.h>
#include <dtActors/particlesystemactorproxy.h>
#include <dtActors/positionallightactorproxy.h>
#include <dtActors/skyboxactorproxy.h>
#include <dtActors/skydomeactorproxy.h>
#include <dtActors/soundactorproxy.h>
#include <dtActors/spotlightactorproxy.h>
#include <dtActors/staticmeshactorproxy.h>
#include <dtActors/triggeractorproxy.h>

/** Contains the dtDAL::ActorProxy classes for the core engine classes
*/
namespace dtActors
{
}

#if defined(_WIN32) || defined(WIN32) || defined(__WIN32__)

#undef _AUTOLIBNAME

#if defined(_DEBUG)
   #define _AUTOLIBNAME "dtActorsd.lib"
#else
   #define _AUTOLIBNAME "dtActors.lib"
#endif

#ifndef _NOAUTOLIBMSG
   #pragma message( "Will automatically link with " _AUTOLIBNAME )
#endif

#pragma  comment( lib, _AUTOLIBNAME )

#endif  // defined(_WIN32) || defined(WIN32) || defined(__WIN32__)

#endif //DTACTORS_INCLUDED

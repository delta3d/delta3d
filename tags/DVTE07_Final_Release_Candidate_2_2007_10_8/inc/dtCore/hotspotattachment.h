/* 
* Delta3D Open Source Game and Simulation Engine 
* Copyright (C) 2004-2005 MOVES Institute 
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
* John Grant
*/

#ifndef __DTCORE_HOT_SPOT_ATTACHMENT_H__
#define __DTCORE_HOT_SPOT_ATTACHMENT_H__

#include <dtCore/transformable.h>      // for base class
#include <dtUtil/hotspotdefinition.h>  // for member

#include <dtUtil/macros.h>             // for usage of management_layer
#include <dtCore/export.h>

namespace dtCore
{
   /// maintains a mapping from HotSpotData instances to Transformable instances.
   class DT_CORE_EXPORT HotSpotAttachment : public dtCore::Transformable
   {
      DECLARE_MANAGEMENT_LAYER(HotSpotAttachment)

   public:
      HotSpotAttachment(const dtUtil::HotSpotDefinition& defintion);
      ~HotSpotAttachment();

      const dtUtil::HotSpotDefinition& GetDefinition() const;

   private:
      HotSpotAttachment();  ///< not implemented by design
      HotSpotAttachment(const HotSpotAttachment&);  ///< not implemented by design
      HotSpotAttachment& operator =(const HotSpotAttachment&);  ///< not implemented by design

      dtUtil::HotSpotDefinition mDefinition;
   };
}

#endif // __DTCORE_HOT_SPOT_ATTACHMENT_H__

/* -*-c++-*-
 * Delta3D Open Source Game and Simulation Engine 
 * Copyright (C) 2006, Alion Science and Technology, BMH Operation.
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
 * @author Olen A. Bruce
 * @author David Guthrie
 */

#ifndef DELTA_HLA_TO_ACTOR_ID
#define DELTA_HLA_TO_ACTOR_ID

//
// hlafomtranslator.h: Declaration of the HLAFOMTranslator class.
//
///////////////////////////////////////////////////////////////////////

#include "dtHLAGM/export.h"
#include "dtCore/uniqueid.h"

namespace dtHLAGM
{
   class DT_HLAGM_EXPORT HLAtoActorID
   {
      public:
     
         HLAtoActorID();
         
         ~HLAtoActorID();
         
         EntityIdentifier GetHLAID()
         {
            return mHLAID;
         }
         
         void SetHLAID(EntityIdentifier hlaID)
         {
            mHLAID.SetSiteIdentifier(hlaID.GetSiteIdentifier());
            mHLAID.SetApplicationIdentifier(hlaID.GetApplicationIdentifier());
            mHLAID.SetEntityIdentifier(hlaID.GetEntityIdentifier());
         }
         
         dtCore::UniqueID GetGameID()
         {
            return mGameID;
         }
         
         void SetGameID(dtCore::UniqueID gameID)
         {
            mGameID = gameID;
         }
         
      private:
    
         EntityIdentifier mHLAID;
         
         dtCore::UniqueID mGameID;    
   };
};

#endif // DELTA_HLA_TO_ACTOR_ID

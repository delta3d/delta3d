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
*/

#ifndef DELTA_TRIGGER
#define DELTA_TRIGGER

#include <dtCore/base.h>
#include <dtCore/refptr.h>
#include <dtABC/action.h>
#include <dtABC/export.h>

namespace dtABC
{
   class DT_ABC_EXPORT Trigger : public dtCore::Base
   {
      DECLARE_MANAGEMENT_LAYER(Trigger)

   public:
      Trigger( const std::string& name = "Trigger" );
   protected:
      virtual ~Trigger();
   public:

      virtual void OnMessage( dtCore::Base::MessageData* data );

      void SetEnabled( bool enabled ) { mEnabled = enabled; }
      bool GetEnabled() const { return mEnabled; }

      void SetTimeDelay( double timeDelay ) { mTimeDelay = timeDelay; }
      double GetTimeDelay() const { return mTimeDelay; }

      double GetTimeLeft() const { return mTimeLeft; }

      void Fire();

      void SetAction( Action* action ) { mActionToFire = action; }
      const Action* GetAction() const { return mActionToFire.get(); }

   private:

      void Update( double time );

   private:

      bool mEnabled;
      double mTimeDelay;
      double mTimeLeft;
      dtCore::RefPtr<Action> mActionToFire;

   };
};

#endif //DELTA_TRIGGER

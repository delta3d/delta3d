/* -*-c++-*-
 * dtPhysics
 * Copyright 2010, Alion Science and Technology
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
 * David Guthrie
 */

#include <dtPhysics/action.h>
#include <pal/pal.h>

namespace dtPhysics
{
   //////////////////////////////////////////////////////////////
   class InternalAction: public palAction
   {
   public:
      InternalAction(Action& action)
      : mAction(action)
      {}

      virtual ~InternalAction() {}

      virtual void operator()(Float timeStep)
      {
         mAction(Real(timeStep));
      }

   private:
      Action& mAction;
   };

   //////////////////////////////////////////////////////////////
   class ActionImpl
   {
   public:
      ActionImpl(Action& action)
      : mPalAction(new InternalAction(action))
      {

      }

      ~ActionImpl()
      {
         delete mPalAction;
      }

      palAction* mPalAction;
   };

   //////////////////////////////////////////////////////////////
   Action::Action()
   : mImpl(new ActionImpl(*this))
   {
   }

   //////////////////////////////////////////////////////////////
   Action::~Action()
   {
      delete mImpl;
      mImpl = NULL;
   }

   //////////////////////////////////////////////////////////////
   palAction* Action::GetPalAction()
   {
      return mImpl->mPalAction;
   }

}

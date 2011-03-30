/*
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2011 MOVES Institute
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

#ifndef inputdevicefeature_h__
#define inputdevicefeature_h__

// inputdevicefeature.h: Declaration of the InputDeviceFeature class.
//
//////////////////////////////////////////////////////////////////////

#include <dtCore/export.h>
#include <dtCore/observerptr.h>
#include <osg/Referenced>

namespace dtCore
{
   class InputDevice;

   class DT_CORE_EXPORT InputDeviceFeature : public osg::Referenced
   {
   public:

      /**
       * Constructor.
       *
       * @param owner the owner of this feature
       * @param description a description of this feature
       */
      InputDeviceFeature(InputDevice* owner, const std::string& description);

   protected:

      /**
       * Destructor.
       */
      virtual ~InputDeviceFeature() = 0;

   public:

      /**
       * Returns a pointer to the owner of this feature.
       *
       * @return a pointer to the owner of this feature
       */
      InputDevice* GetOwner() const;

      /**
       * Sets the description of this feature.
       *
       * @param description the new description
       */
      void SetDescription(const std::string& description);

      /**
       * Returns a description of this feature.
       *
       * @return a description of this feature
       */
      std::string GetDescription() const;

   private:

      /// The owner of this feature.
      dtCore::ObserverPtr<InputDevice> mOwner;

      /// A description of this feature.
      std::string mDescription;
   };
}

#endif // inputdevicefeature_h__

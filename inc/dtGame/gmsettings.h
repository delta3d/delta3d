/* -*-c++-*-
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2010, Alion Science and Technology
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
 * Curtiss Murphy
 */

#ifndef DELTA_GMSETTINGS
#define DELTA_GMSETTINGS

#include <dtGame/export.h>
#include <dtCore/refptr.h>
#include <osg/Referenced>

/**
 * A simple data class that has configurable settings used by the GM. Separated out as a class
 * so that it is extensible (ie subclass it with your own values and give it to the GM).  
 */
namespace dtGame
{
   class DT_GAME_EXPORT GMSettings : public osg::Referenced
   {
   public:
      /// Constructor
      GMSettings();

   protected:
      /// Destructor
      virtual ~GMSettings();

   public:
      /**
       * Returns true if the gm was setup with the server role. Among other things,
       * the server role applies when loading a map to handle InitialOwnership. 
       * By default, the settings are true for both server and client.
       * @return true if server role is on. 
       */
      bool IsServerRole() const;

      /**
       * Sets whether the gm acts as a server or not. Among other things,
       * the server role applies when loading a map to handle InitialOwnership. 
       * By default, the settings are true for both server and client.
       * @param newValue true for server or false if not. 
       */
      void SetServerRole(bool newValue);

      /**
       * Returns true if the gm was setup with the client role. Among other things,
       * the client role applies when loading a map to handle InitialOwnership. 
       * By default, the settings are true for both server and client.
       * @return true if client role is on. 
       */
      bool IsClientRole() const;

      /**
       * Sets whether the gm acts as a client or not. Among other things,
       * the server role applies when loading a map to handle InitialOwnership. 
       * By default, the settings are true for both server and client.
       * @param newValue true for client or false if not. 
       */
      void SetClientRole(bool newValue);


   private:
      bool mServerRole; // defaults to true
      bool mClientRole; // defaults to true
   };

} // namespace dtGame

#endif // DELTA_GMSETTINGS

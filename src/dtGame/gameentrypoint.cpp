/* -*-c++-*-
 * Delta3D
 * Copyright 2013, David Guthrie
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
#include <prefix/dtgameprefix.h>
#include <dtGame/gameentrypoint.h>
#include <dtABC/application.h>
#include <dtCore/keyboard.h>

namespace dtGame
{
   dtCore::RefPtr<dtABC::BaseABC> GameEntryPoint::CreateApplication(const std::string& configFileName)
   {
      dtCore::RefPtr<dtABC::Application> result = new dtABC::Application(configFileName);
      return result.get();
   }
}

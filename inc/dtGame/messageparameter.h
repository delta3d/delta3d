/*
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2005, BMH Associates, Inc.
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
 * William E. Johnson II
 */

#ifndef DELTA_MESSAGEPARAMETER
#define DELTA_MESSAGEPARAMETER

////////////////////////////////////////////////////////////////////////////////

#include <dtCore/namedparameters.h>

namespace dtGame
{
   typedef dtCore::NamedParameter                  MessageParameter;
   typedef dtCore::NamedGroupParameter             GroupMessageParameter;
   typedef dtCore::NamedBooleanParameter           BooleanMessageParameter;
   typedef dtCore::NamedUnsignedCharParameter      UnsignedCharMessageParameter;
   typedef dtCore::NamedShortIntParameter          ShortIntMessageParameter;
   typedef dtCore::NamedUnsignedShortIntParameter  UnsignedShortIntMessageParameter;
   typedef dtCore::NamedUnsignedIntParameter       UnsignedIntMessageParameter;
   typedef dtCore::NamedIntParameter               IntMessageParameter;
   typedef dtCore::NamedUnsignedLongIntParameter   UnsignedLongIntMessageParameter;
   typedef dtCore::NamedLongIntParameter           LongIntMessageParameter;
   typedef dtCore::NamedFloatParameter             FloatMessageParameter;
   typedef dtCore::NamedDoubleParameter            DoubleMessageParameter;
   typedef dtCore::NamedStringParameter            StringMessageParameter;
   typedef dtCore::NamedArrayParameter             ArrayMessageParameter;
   typedef dtCore::NamedEnumParameter              EnumMessageParameter;
   typedef dtCore::NamedActorParameter             ActorMessageParameter;
   typedef dtCore::NamedGameEventParameter         GameEventMessageParameter;
   typedef dtCore::NamedVec2Parameter              Vec2MessageParameter;
   typedef dtCore::NamedVec2fParameter             Vec2fMessageParameter;
   typedef dtCore::NamedVec2dParameter             Vec2dMessageParameter;
   typedef dtCore::NamedVec3Parameter              Vec3MessageParameter;
   typedef dtCore::NamedRGBColorParameter          RGBColorMessageParameter;
   typedef dtCore::NamedVec3fParameter             Vec3fMessageParameter;
   typedef dtCore::NamedVec3dParameter             Vec3dMessageParameter;
   typedef dtCore::NamedVec4Parameter              Vec4MessageParameter;
   typedef dtCore::NamedRGBAColorParameter         RGBAColorMessageParameter;
   typedef dtCore::NamedVec4fParameter             Vec4fMessageParameter;
   typedef dtCore::NamedVec4dParameter             Vec4dMessageParameter;
   typedef dtCore::NamedResourceParameter          ResourceMessageParameter;
}

////////////////////////////////////////////////////////////////////////////////

#endif // DELTA_MESSAGEPARAMETER

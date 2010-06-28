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

#include <dtDAL/namedparameters.h>

namespace dtGame
{
   typedef dtDAL::NamedParameter                  MessageParameter;
   typedef dtDAL::NamedGroupParameter             GroupMessageParameter;
   typedef dtDAL::NamedBooleanParameter           BooleanMessageParameter;
   typedef dtDAL::NamedUnsignedCharParameter      UnsignedCharMessageParameter;
   typedef dtDAL::NamedShortIntParameter          ShortIntMessageParameter;
   typedef dtDAL::NamedUnsignedShortIntParameter  UnsignedShortIntMessageParameter;
   typedef dtDAL::NamedUnsignedIntParameter       UnsignedIntMessageParameter;
   typedef dtDAL::NamedIntParameter               IntMessageParameter;
   typedef dtDAL::NamedUnsignedLongIntParameter   UnsignedLongIntMessageParameter;
   typedef dtDAL::NamedLongIntParameter           LongIntMessageParameter;
   typedef dtDAL::NamedFloatParameter             FloatMessageParameter;
   typedef dtDAL::NamedDoubleParameter            DoubleMessageParameter;
   typedef dtDAL::NamedStringParameter            StringMessageParameter;
   typedef dtDAL::NamedArrayParameter             ArrayMessageParameter;
   typedef dtDAL::NamedEnumParameter              EnumMessageParameter;
   typedef dtDAL::NamedActorParameter             ActorMessageParameter;
   typedef dtDAL::NamedGameEventParameter         GameEventMessageParameter;
   typedef dtDAL::NamedVec2Parameter              Vec2MessageParameter;
   typedef dtDAL::NamedVec2fParameter             Vec2fMessageParameter;
   typedef dtDAL::NamedVec2dParameter             Vec2dMessageParameter;
   typedef dtDAL::NamedVec3Parameter              Vec3MessageParameter;
   typedef dtDAL::NamedRGBColorParameter          RGBColorMessageParameter;
   typedef dtDAL::NamedVec3fParameter             Vec3fMessageParameter;
   typedef dtDAL::NamedVec3dParameter             Vec3dMessageParameter;
   typedef dtDAL::NamedVec4Parameter              Vec4MessageParameter;
   typedef dtDAL::NamedRGBAColorParameter         RGBAColorMessageParameter;
   typedef dtDAL::NamedVec4fParameter             Vec4fMessageParameter;
   typedef dtDAL::NamedVec4dParameter             Vec4dMessageParameter;
   typedef dtDAL::NamedResourceParameter          ResourceMessageParameter;
}

////////////////////////////////////////////////////////////////////////////////

#endif // DELTA_MESSAGEPARAMETER

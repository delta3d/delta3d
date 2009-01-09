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
 * ANY WARRANTY; without even the implied warranty lof MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; if not, write to the Free Software Foundation, Inc.,
 * 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 *
 */

#ifndef DELTA_COLLISIONCATEGORYDEFAULTS
#define DELTA_COLLISIONCATEGORYDEFAULTS

////////////////////////////////////////////////////////////////////////////////

#include <dtUtil/macros.h> // for UNSIGNED_BIT macro

/**
 * The default category bits for collision geoms:
 *
 * dtABC::ProximityTrigger  0
 *
 * dtCore::Camera:          1
 * dtCore::Compass:         2
 * dtCore::InfiniteTerrain: 3
 * dtCore::ISector:         4
 * dtCore::Object:          5
 * dtCore::ParticleSsystem: 6
 * dtCore::Physical:        7
 * dtCore::PointAxis:       8
 * dtCore::PositionalLight: 9
 * dtCore::SpotLight:       10
 * dtCore::Transformable:   11
 *
 * dtAudio::Listener:       13
 * dtAudio::Sound:          14
 * dtHLA::Entity:           15
 * dtTerrain::Terrain:      16
 *
 */

enum
{
   COLLISION_CATEGORY_BIT_PROXIMITYTRIGGER = 0,
   COLLISION_CATEGORY_BIT_CAMERA           = 1,
   COLLISION_CATEGORY_BIT_COMPASS          = 2,
   COLLISION_CATEGORY_BIT_INFINITETERRAIN  = 3,
   COLLISION_CATEGORY_BIT_ISECTOR          = 4,
   COLLISION_CATEGORY_BIT_OBJECT           = 5,
   COLLISION_CATEGORY_BIT_PARTICLESYSTEM   = 6,
   COLLISION_CATEGORY_BIT_PHYSICAL         = 7,
   COLLISION_CATEGORY_BIT_POINTAXIS        = 8,
   COLLISION_CATEGORY_BIT_POSITIONALLIGHT  = 9,
   COLLISION_CATEGORY_BIT_SPOTLIGHT        = 10,
   COLLISION_CATEGORY_BIT_TRANSFORMABLE    = 11,

   COLLISION_CATEGORY_BIT_LISTENER         = 13,
   COLLISION_CATEGORY_BIT_SOUND            = 14,
   COLLISION_CATEGORY_BIT_ENTITY           = 15,
   COLLISION_CATEGORY_BIT_TERRAIN          = 16,

   // users can start at COLLISION_CATEGORY_BIT_DEFAULTMAX+1
   COLLISION_CATEGORY_BIT_DEFAULTMAX       = COLLISION_CATEGORY_BIT_TERRAIN
};

enum
{
   COLLISION_CATEGORY_MASK_PROXIMITYTRIGGER = UNSIGNED_BIT(COLLISION_CATEGORY_BIT_PROXIMITYTRIGGER),
   COLLISION_CATEGORY_MASK_CAMERA           = UNSIGNED_BIT(COLLISION_CATEGORY_BIT_CAMERA),
   COLLISION_CATEGORY_MASK_COMPASS          = UNSIGNED_BIT(COLLISION_CATEGORY_BIT_COMPASS),
   COLLISION_CATEGORY_MASK_INFINITETERRAIN  = UNSIGNED_BIT(COLLISION_CATEGORY_BIT_INFINITETERRAIN),
   COLLISION_CATEGORY_MASK_ISECTOR          = UNSIGNED_BIT(COLLISION_CATEGORY_BIT_ISECTOR),
   COLLISION_CATEGORY_MASK_OBJECT           = UNSIGNED_BIT(COLLISION_CATEGORY_BIT_OBJECT),
   COLLISION_CATEGORY_MASK_PARTICLESYSTEM   = UNSIGNED_BIT(COLLISION_CATEGORY_BIT_PARTICLESYSTEM),
   COLLISION_CATEGORY_MASK_PHYSICAL         = UNSIGNED_BIT(COLLISION_CATEGORY_BIT_PHYSICAL),
   COLLISION_CATEGORY_MASK_POINTAXIS        = UNSIGNED_BIT(COLLISION_CATEGORY_BIT_POINTAXIS),
   COLLISION_CATEGORY_MASK_POSITIONALLIGHT  = UNSIGNED_BIT(COLLISION_CATEGORY_BIT_POSITIONALLIGHT),
   COLLISION_CATEGORY_MASK_SPOTLIGHT        = UNSIGNED_BIT(COLLISION_CATEGORY_BIT_SPOTLIGHT),
   COLLISION_CATEGORY_MASK_TRANSFORMABLE    = UNSIGNED_BIT(COLLISION_CATEGORY_BIT_TRANSFORMABLE),
   COLLISION_CATEGORY_MASK_LISTENER         = UNSIGNED_BIT(COLLISION_CATEGORY_BIT_LISTENER),
   COLLISION_CATEGORY_MASK_SOUND            = UNSIGNED_BIT(COLLISION_CATEGORY_BIT_SOUND),
   COLLISION_CATEGORY_MASK_ENTITY           = UNSIGNED_BIT(COLLISION_CATEGORY_BIT_ENTITY),
   COLLISION_CATEGORY_MASK_TERRAIN          = UNSIGNED_BIT(COLLISION_CATEGORY_BIT_TERRAIN),

   COLLISION_CATEGORY_MASK_ALLDEFAULTS      = COLLISION_CATEGORY_MASK_PROXIMITYTRIGGER
                                            | COLLISION_CATEGORY_MASK_CAMERA
                                            | COLLISION_CATEGORY_MASK_COMPASS
                                            | COLLISION_CATEGORY_MASK_INFINITETERRAIN
                                            | COLLISION_CATEGORY_MASK_ISECTOR
                                            | COLLISION_CATEGORY_MASK_OBJECT
                                            | COLLISION_CATEGORY_MASK_PARTICLESYSTEM
                                            | COLLISION_CATEGORY_MASK_PHYSICAL
                                            | COLLISION_CATEGORY_MASK_POINTAXIS
                                            | COLLISION_CATEGORY_MASK_POSITIONALLIGHT
                                            | COLLISION_CATEGORY_MASK_SPOTLIGHT
                                            | COLLISION_CATEGORY_MASK_TRANSFORMABLE
                                            | COLLISION_CATEGORY_MASK_LISTENER
                                            | COLLISION_CATEGORY_MASK_SOUND
                                            | COLLISION_CATEGORY_MASK_ENTITY
                                            | COLLISION_CATEGORY_MASK_TERRAIN,

   COLLISION_CATEGORY_MASK_ALL              = 0xffffffff
};

////////////////////////////////////////////////////////////////////////////////

#endif // DELTA_COLLISIONCATEGORYDEFAULTS

/* -*-c++-*-
* Delta3D Open Source Game and Simulation Engine
* Copyright (C) 2014, Caper Holdings, LLC
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
* Bradley Anderegg
*/


#ifndef _DYNAMIC_LIGHT_ACTOR_H_
#define _DYNAMIC_LIGHT_ACTOR_H_

#include <dtRender/dtrenderexport.h>
#include <dtGame/gameactorproxy.h>
#include <dtUtil/getsetmacros.h>

namespace dtCore
{
   class Transformable;
}

namespace dtRender
{
      class DT_RENDER_EXPORT DynamicLight : public dtGame::GameActorProxy
      {
      public:

         typedef unsigned int LightID;


         class DT_RENDER_EXPORT LightType: public dtUtil::Enumeration
         {
            DECLARE_ENUM(LightType)
         public:
            static LightType OMNI_DIRECTIONAL;
            static LightType SPOT_LIGHT;

            LightType(const std::string& name) : dtUtil::Enumeration(name)
            {
               AddInstance(this);
            }
         };


         /// Constructor
         DynamicLight();

         DynamicLight(LightType& lt);


         void CreateDrawable();

         virtual void OnEnteredWorld();

         virtual void OnRemovedFromWorld();

         /// Adds the properties associated with this actor
         virtual void BuildPropertyMap();

         // Attenuation - this controls how far the light is visible from the vec3 represents (constant, linear, quadratic) attentions
         DT_DECLARE_ACCESSOR_INLINE(osg::Vec3, Attenuation)

         //the color of the light
         DT_DECLARE_ACCESSOR_INLINE(osg::Vec3, LightColor)

         //this will create a light similar to a fire changing intensities over times with the flicker scale
         DT_DECLARE_ACCESSOR_INLINE(bool, Flicker)                  
         
         // Flicker Scale - the maximum increase or decrease in the light intensity (something like 0.1-0.4)
         DT_DECLARE_ACCESSOR(float, FlickerScale)
         
         //delete after max time
         DT_DECLARE_ACCESSOR_INLINE(bool, DeleteAfterMaxTime)
         // Max Time - the maximum time (in seconds) that the light should exist. Set to 0 to have NO maximum time.
         DT_DECLARE_ACCESSOR(float, MaxTime)
         
         //fading out makes the light linger after it has been destroyed
         DT_DECLARE_ACCESSOR_INLINE(bool, FadeOut)
         // Fade Out Time - how long the light should take to fade out (in seconds). This occurs AFTER Max Time. 0 means no fade out. 
         DT_DECLARE_ACCESSOR(float, FadeOutTime)

         // Radius - The distance of the bounding sphere of the light. ie, in general how big is the light estimated to be. Used for light priority, not for rendering 
         DT_DECLARE_ACCESSOR_INLINE(float, Radius)

         // Auto Delete If Target Is Null - indicates to delete this light if the target ever becomes NULL.  
         DT_DECLARE_ACCESSOR_INLINE(bool, DeleteOnTargetIsNull)                  

         DT_DECLARE_ACCESSOR_INLINE(dtUtil::EnumerationPointer<LightType>, LightType)                  


         DT_DECLARE_ACCESSOR_INLINE(bool, DeleteMe)                  

         DT_DECLARE_ACCESSOR_INLINE(osg::Vec3, LightPosition)

         // Intensity scales light value
         DT_DECLARE_ACCESSOR_INLINE(float, Intensity)

         // Intensity- mostly used internally to do effects like fading out and flickering
         DT_DECLARE_ACCESSOR_INLINE(float, IntensityMod)


         dtCore::Transformable* GetTarget();
         const dtCore::Transformable* GetTarget() const;
         void SetTarget(dtCore::Transformable&);

         static LightID GetCurrentLightIdCounter() { return mLightCounter; }
         LightID GetLightId() const { return mId; }

      protected:
         /// Destructor
         virtual ~DynamicLight();

         dtCore::ObserverPtr<dtCore::Transformable> mTarget;

         LightID mId;
         static OpenThreads::Atomic mLightCounter;

      };

      class DT_RENDER_EXPORT SpotLight : public DynamicLight
      {
      public:

         /// Constructor
         SpotLight();

         /// Destructor
         virtual ~SpotLight();

         void CreateDrawable();

         /// Adds the properties associated with this actor
         virtual void BuildPropertyMap();

         //this flag implies the direction specified is absolute or in world space
         //so it does not accumulate its parents rotation
         DT_DECLARE_ACCESSOR_INLINE(bool, UseAbsoluteDirection)

         //mSpotExponent is the spot rate of decay and controls how
         //the lights intensity decays from the center of the cone it its borders. The larger the value the faster de decay, with zero meaning constant light within the light cone.
         DT_DECLARE_ACCESSOR_INLINE(float, SpotExponent)

         //The cosine of the angle between the light to vertex vector and the spot direction must be larger than spotCosCutoff
         DT_DECLARE_ACCESSOR_INLINE(float, SpotCosCutoff)

         //The local direction of the light
         DT_DECLARE_ACCESSOR_INLINE(osg::Vec3, Direction)

         DT_DECLARE_ACCESSOR_INLINE(osg::Vec3, CurrentDirection)

      private:

         
      };

}

#endif

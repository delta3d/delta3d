/* -*-c++-*-
 * dtRender
 * Copyright 2014, Caper Holdings LLC
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
 * This software was developed by Alion Science and Technology Corporation under
 * circumstances in which the U. S. Government may have rights in the software.
 * 
 * Bradley Anderegg
 */

#ifndef DELTA_OPTIMIZERACTCOMP_H
#define DELTA_OPTIMIZERACTCOMP_H

#include <dtRender/dtrenderexport.h>
#include <dtUtil/getsetmacros.h>
#include <dtGame/actorcomponent.h>

#include <dtUtil/refstring.h>
#include <dtUtil/assocvector.h>

namespace dtCore
{
   class BaseActorObject;
}

namespace dtGame
{
   class Message;
}

namespace dtRender
{

   /////////////////////////////////////////////////////////////////////////////
   // Class:
   /////////////////////////////////////////////////////////////////////////////
   class DT_RENDER_EXPORT OptimizerActComp: public dtGame::ActorComponent
   {
      public:
         typedef dtGame::ActorComponent BaseClass;

         static const dtGame::ActorComponent::ACType TYPE;

         static const dtUtil::RefString PROPERTY_OPTIMIZER_COMP_NAME;
         static const dtUtil::RefString INVOKABLE_MAP_LOADED;


      public:
         // constructors
         OptimizerActComp();
         
         void SetName(const dtUtil::RefString& n);
         const dtUtil::RefString& GetName() const;

         /*virtual*/ void OnEnteredWorld();
         /*virtual*/ void OnRemovedFromWorld();

         /*virtual*/ void OnAddedToActor(dtCore::BaseActorObject& /*actor*/);
         /*virtual*/ void OnRemovedFromActor(dtCore::BaseActorObject& /*actor*/);

         
         // build property maps, for editor.
         virtual void BuildPropertyMap();

         void OnAddedMap(const dtGame::Message&);

         void RegisterForMapLoaded();

         DT_DECLARE_ACCESSOR_INLINE(bool, SpatializeGroups)
         DT_DECLARE_ACCESSOR_INLINE(bool, FlattenStaticTransforms)
         DT_DECLARE_ACCESSOR_INLINE(bool, ShareDuplicateState)
         DT_DECLARE_ACCESSOR_INLINE(bool, MergeGeometry)
         DT_DECLARE_ACCESSOR_INLINE(bool, CheckGeometry)
         
         DT_DECLARE_ACCESSOR_INLINE(bool, DefaultOSGOptimizations)
         
         DT_DECLARE_ACCESSOR_INLINE(unsigned, OptimizerOptionsOverride)

         DT_DECLARE_ACCESSOR_INLINE(bool, CompressTextures)
         
         DT_DECLARE_ACCESSOR_INLINE(int, MinVertsForOccluders)
         DT_DECLARE_ACCESSOR_INLINE(bool, CreateOccluders)

         DT_DECLARE_ACCESSOR_INLINE(unsigned, TraversalMask)
         
      protected:
         ~OptimizerActComp();

         void CleanUp();
         void Optimize();

         //For the actor property
         void SetNameByString(const std::string& name);
         //For the actor property
         const std::string& GetNameAsString() const;

      private:
         /// hiding copy constructor and operator=
         OptimizerActComp(const OptimizerActComp&);
         /// hiding copy constructor and operator=
         const OptimizerActComp& operator=(const OptimizerActComp&);


         dtUtil::RefString mName;

   };

} // namespace


#endif /* DELTA_OPTIMIZERACTCOMP_H */

/*
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2004-2006 MOVES Institute
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

#ifndef AI_INTERFACE_ACTOR
#define AI_INTERFACE_ACTOR

#include <dtCore/refptr.h>
#include <dtAI/export.h>
#include <dtCore/actorproxy.h>
#include <dtCore/deltadrawable.h>
#include <dtCore/resourcedescriptor.h>
#include <dtUtil/getsetmacros.h>

namespace dtAI
{
   class AIPluginInterface;

   /**
    * @class AIInterfaceActor
    * @brief This proxy wraps the AI Interface class Delta3D object.
    */
   class DT_AI_EXPORT AIInterfaceActor : public dtCore::BaseActorObject
   {
   public:
      static const dtUtil::RefString CLASS_NAME;
      static const dtUtil::RefString PROPERTY_WAYPOINT_FILE_NAME;
      static const dtUtil::RefString PROPERTY_WAYPOINT_RENDER_INFO;

      /**
       * Constructor
       */
      AIInterfaceActor();

      /**
       * Adds the properties that are common to all Delta3D physical objects.
       */
      /*virtual*/ void BuildPropertyMap();

      /**
       * Loads in an AI file
       * @param fileName The file to load
       */
      void LoadFile(const std::string& fileName);

      DT_DECLARE_ACCESSOR(dtCore::ResourceDescriptor, AIFile)

      /*virtual*/ bool IsPlaceable() const;

      // Use the AIPluginInterface object to interface with dtAI through this actor
      // using this instead of dtAI directly will ensure you will be
      // compatible with future versions of dtAI
      AIPluginInterface* GetAIInterface();
      const AIPluginInterface* GetAIInterface() const;

   protected:
     /*virtual*/ void CreateDrawable();

     /**
      * This is called from CreateActor
      */
     virtual AIPluginInterface* CreateAIInterface();

     /*virtual*/ ~AIInterfaceActor();

   private:
      dtCore::RefPtr<AIPluginInterface> mAIInterface;
   };

} // namespace dtAI

#endif // AI_INTERFACE_ACTOR

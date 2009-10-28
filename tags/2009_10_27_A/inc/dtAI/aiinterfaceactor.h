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

#ifndef DELTA_AI_INTERFACE_ACTOR
#define DELTA_AI_INTERFACE_ACTOR

#include <dtCore/refptr.h>
#include <dtAI/export.h>
#include <dtDAL/exceptionenum.h>
#include <dtDAL/actorproxy.h>
#include <dtCore/deltadrawable.h>

namespace dtAI
{
   class AIPluginInterface;

   class DT_AI_EXPORT AIInterfaceActor : public dtCore::DeltaDrawable
   {
   public:
      AIInterfaceActor();

      osg::Node* GetOSGNode();
      const osg::Node* GetOSGNode() const;

   protected:
      /*virtual*/ ~AIInterfaceActor();

   private:
   };

   /**
    * @class AIInterfaceActorProxy
    * @brief This proxy wraps the AI Interface class Delta3D object.
    */
   class DT_AI_EXPORT AIInterfaceActorProxy : public dtDAL::ActorProxy
   {
   public:
      static const dtUtil::RefString CLASS_NAME;
      static const dtUtil::RefString PROPERTY_WAYPOINT_FILE_NAME;

      /**
       * Constructor
       */
      AIInterfaceActorProxy();

      /**
       * Adds the properties that are common to all Delta3D physical objects.
       */
      /*virtual*/ void BuildPropertyMap();

      /**
       * Loads in an AI file
       * @param fileName The file to load
       */
      void LoadFile(const std::string& fileName);

      /*virtual*/ bool IsPlaceable() const;


      // Use the AIPluginInterface object to interface with dtAI through this actor
      // using this instead of dtAI directly will ensure you will be
      // compatible with future versions of dtAI
      AIPluginInterface* GetAIInterface();
      const AIPluginInterface* GetAIInterface() const;

   protected:
     /*virtual*/ void CreateActor();

     /**
      * This is called from CreateActor
      */
     virtual AIPluginInterface* CreateAIInterface();


     /**
      * Destructor
      */
     /*virtual*/ ~AIInterfaceActorProxy();

   private:
      dtCore::RefPtr<AIPluginInterface> mAIInterface;
   };

} // namespace dtAI

#endif // DELTA_AI_INTERFACE_ACTOR

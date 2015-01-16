/* -*-c++-*-
 * testAPP - Using 'The MIT License'
 * Copyright (C) 2014, Caper Holdings LLC
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include "SelectDestination.h"

#include <dtAI/aiplugininterface.h>
#include <dtUtil/mathdefines.h>
#include <dtCore/propertymacros.h>
#include "../civilianaiactorcomponent.h"

namespace dtExample
{


   /////////////////////////////////////////////////////////////////////////////
   SelectDestination::SelectDestination()
   : BaseClass()
   , mRadius(2500.0f)
   {
      AddAuthor("Bradley Anderegg");
   }

   /////////////////////////////////////////////////////////////////////////////
   SelectDestination::~SelectDestination()
   {
   }

   /////////////////////////////////////////////////////////////////////////////
   void SelectDestination::Init(const dtDirector::NodeType& nodeType, dtDirector::DirectorGraph* graph)
   {
      BaseClass::Init(nodeType, graph);
      mOutputs.push_back(dtDirector::OutputLink(this, "Failed", "Activated no destination is found."));
   }

   /////////////////////////////////////////////////////////////////////////////
   void SelectDestination::BuildPropertyMap()
   {
      BaseClass::BuildPropertyMap();

      const dtUtil::RefString AI_GROUP("AI");
      typedef dtCore::PropertyRegHelper<SelectDestination&, SelectDestination> RegHelperType;
      RegHelperType propReg(*this, this, AI_GROUP);

      DT_REGISTER_PROPERTY(Radius, "The radius to search for a waypoint.", RegHelperType, propReg);
      DT_REGISTER_PROPERTY(DestinationName, "The name of the waypoint to find.", RegHelperType, propReg);

      DT_REGISTER_PROPERTY(DestinationPosition, "Output: The position of the selected waypoint.", RegHelperType, propReg);

      mValues.push_back(dtDirector::ValueLink(this, GetProperty("DestinationPosition"), true, true));
   }

   /////////////////////////////////////////////////////////////////////////////
   bool SelectDestination::Update(float simDelta, float delta, int input, bool firstUpdate)
   {

      bool cont = firstUpdate && IsValid();

      //temporarily sets random destination
      if(cont)//&& (unsigned(mDestinationIndex) < mDestinationArray.size()) )
      {
         dtAI::AIPluginInterface::WaypointArray waypoints;

         CivilianAIActorComponent* civAI = GetTargetActor()->GetComponent<CivilianAIActorComponent>();

         dtAI::WaypointInterface* waypoint = NULL;
         if (!GetDestinationName().empty())
         {
            waypoint = GetAIInterface()->GetClosestNamedWaypoint(GetDestinationName(), GetTargetActor()->GetTranslation(), 8000.0f);
            std::string emptyString;
            SetDestinationName(emptyString);
         }

         float distToWaypoint = waypoint == NULL ? 0.0f : (waypoint->GetPosition() - GetTargetActor()->GetTranslation()).length2();

         if (waypoint == NULL)// || distToWaypoint > GetRadius())
         {
            GetAIInterface()->GetWaypointsAtRadius(civAI->GetPosition(), GetRadius(), waypoints);

            if(!waypoints.empty())
            {
               unsigned size = waypoints.size();
               if (waypoint == NULL)
               {
                  int index = dtUtil::RandRange(0U, size-1);

                  waypoint = waypoints[index];

                  // Trigger default out.
                  BaseClass::Update(simDelta, delta, input, firstUpdate);
               }
               else
               {
                  float proposedDist = distToWaypoint * 2.0f;
                  int count = size;
                  while (count > 0 && proposedDist >= distToWaypoint)
                  {
                     --count;
                     int index = dtUtil::RandRange(0U, size-1);
                     proposedDist = (waypoints[index]->GetPosition() - civAI->GetPosition()).length2();
                     if (proposedDist < distToWaypoint)
                     {
                        waypoint = waypoints[index];
                     }
                  }
               }
            }
            else
            {
               LOG_ERROR("Cannot find waypoints within radius");
               ActivateOutput("Failed");
            }
         }

         if (waypoint != NULL)
         {
            civAI->SetDestByWaypoint(waypoint);
            SetDestinationPosition(waypoint->GetPosition());
            // Trigger default out.
            BaseClass::Update(simDelta, delta, input, firstUpdate);
         }
         else
         {
            ActivateOutput("Failed");
         }
      }
      else
      {
         ActivateOutput("Failed");
      }


      return false;
   }

   ///////////////////////////////////////////////////////////////////////////////
   bool SelectDestination::CanConnectValue(dtDirector::ValueLink* link, dtDirector::ValueNode* value)
   {
      return true;
   }

   ///////////////////////////////////////////////////////////////////////////////
   DT_IMPLEMENT_ACCESSOR(SelectDestination, osg::Vec3, DestinationPosition);
   DT_IMPLEMENT_ACCESSOR(SelectDestination, float, Radius);
   DT_IMPLEMENT_ACCESSOR(SelectDestination, std::string, DestinationName);


   //---------------------
}//namespace dtExample


//---------------------------------------------------------------------------------------------------------------------------------
// dtExample
// Originally created on 03/11/2014 by Bradley G Anderegg
// Copyright (C) 2013-2014 Caper Holdings LLC, all rights reserved.
// ---------------------------------------------------------------------------------------------------------------------------------
// SelectDestination.cpp - End of file
// ---------------------------------------------------------------------------------------------------------------------------------


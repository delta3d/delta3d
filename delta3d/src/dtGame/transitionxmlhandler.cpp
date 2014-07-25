/* -*-c++-*-
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2009, Alion Science and Technology, BMH Operation
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

////////////////////////////////////////////////////////////////////////////////
// INCLUDE DIRECTICVES
////////////////////////////////////////////////////////////////////////////////
#include <dtGame/transitionxmlhandler.h>



namespace dtGame
{
   //////////////////////////////////////////////////////////////////////////
   // TRANSITION XML HANDLER CODE
   //////////////////////////////////////////////////////////////////////////
   void TransitionXMLHandler::startElement(const XMLCh* const uri,
                                                            const XMLCh* const localname,
                                                            const XMLCh* const qname,
                                                            const XERCES_CPP_NAMESPACE_QUALIFIER Attributes& attrs)
   {
      char* elementName = XERCES_CPP_NAMESPACE_QUALIFIER XMLString::transcode(localname);
      std::string ename(elementName);
      XERCES_CPP_NAMESPACE_QUALIFIER XMLString::release( &elementName );

      dtUtil::AttributeSearch attrsearch;
      dtUtil::AttributeSearch::ResultMap results = attrsearch( attrs );

      dtUtil::AttributeSearch::ResultMap::iterator typeiter = results.find("Type");

      if (ename == "Transition")
      {
         LOG_INFO("Found a State Transition XML element.")
      }
      else if(ename == "Event")
      {
         if( typeiter != results.end() )
         {
            mEventType = EventType::GetValueForName( (*typeiter).second );
         }
         else
         {
            LOG_ERROR("Transition file not structured properly at Event, "+ename+", requires Type attributes.")
         }
      }
      else if(ename == "FromState")
      {
         if((typeiter==results.end()))
         {
            LOG_ERROR("Transition file not structured properly at State, "+ename+", requires Type attributes.")
               return;
         }

         std::string stateType("default");
         if( typeiter != results.end() )
         {
            stateType = (*typeiter).second;
         }

         StateType* st = StateType::GetValueForName(stateType);

         if(st == NULL)
         {
            mFromState = NULL;
            LOG_ERROR("The StateType '" + stateType + "' has not been defined.");
         }
         else
         {
            mFromState = st;
         }
      }
      else if(ename == "ToState")
      {
         if((typeiter==results.end()))
         {
            LOG_ERROR("Transition file not structured properly at State, "+ename+", requires Type and Name attributes.")
               return;
         }

         std::string stateType("default");
         if( typeiter != results.end() )
         {
            stateType = (*typeiter).second;
         }

         StateType* st = StateType::GetValueForName(stateType);

         if(st == NULL)
         {
            mToState = NULL;
            LOG_ERROR("The StateType '" + stateType + "' has not been defined.");
         }
         else
         {
            mToState = st;
         }

      }
      else if(ename == "StartState")
      {
         std::string stateType("default");
         if( typeiter != results.end() )
         {
            stateType = (*typeiter).second;
         }

         StateType* st = StateType::GetValueForName(stateType);
         if(st == NULL)
         {
            LOG_ERROR("Invalid state type '" + stateType + "' for StartState.")
         }
         else
         {
            mManager->SetInitialState(st);
         }
      }
   }

   //////////////////////////////////////////////////////////////////////////
   void TransitionXMLHandler::endElement(const XMLCh* const uri,
      const XMLCh* const localname,
      const XMLCh* const qname)
   {
      char* ename = XERCES_CPP_NAMESPACE_QUALIFIER XMLString::transcode(localname);
      std::string elementName(ename);

      if (elementName == "Transition")
      {
         if(mFromState != NULL && mToState != NULL && mEventType != NULL)
         {
            mManager->AddTransition(mEventType, mFromState, mToState);
         }
         else
         {
            LOG_ERROR("Invalid Transition")
         }
      }

      XERCES_CPP_NAMESPACE_QUALIFIER XMLString::release(&ename);
   }

}

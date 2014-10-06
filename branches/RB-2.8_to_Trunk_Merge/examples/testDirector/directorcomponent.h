/*
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2009 MOVES Institute
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
 * MG
 */

#ifndef DIRECTOR_COMPONENT_H
#define DIRECTOR_COMPONENT_H

////////////////////////////////////////////////////////////////////////////////

#include <dtGame/baseinputcomponent.h>

namespace dtInspectorQt { class InspectorQt; }

////////////////////////////////////////////////////////////////////////////////

class DirectorComponent : public dtGame::BaseInputComponent
{
public:

   static const dtCore::RefPtr<dtCore::SystemComponentType> TYPE;

   /// Constructor
   DirectorComponent();

   /**
    * Handles incoming messages
    */
   virtual void ProcessMessage(const dtGame::Message& message);

   /**
   * KeyboardListener call back- Called when a key is pressed.
   * Override this if you want to handle this listener event.
   * Default handles the Escape key to quit.
   *
   * @param keyboard the source of the event
   * @param key the key pressed
   * @param character the corresponding character
   */
   virtual bool HandleKeyPressed(const dtCore::Keyboard* keyboard, int key);

protected:

   virtual ~DirectorComponent();

private:   
   
   void OnMapLoaded();

#if defined(USE_INSPECTOR)
   dtInspectorQt::InspectorQt* mInspector;
#endif
};

////////////////////////////////////////////////////////////////////////////////

#endif  // DIRECTOR_COMPONENT_H

/* -*-c++-*-
 * TutorialLibrary - This source file (.h & .cpp) - Using 'The MIT License'
 * Copyright (C) 2006-2008, Alion Science and Technology Corporation
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
 * 
 * @author Curtiss Murphy
 */
#ifndef __GM_TUTORIAL_ACTORS_REGISTRY__
#define __GM_TUTORIAL_ACTORS_REGISTRY__

#include "export.h"
#include <dtCore/actorpluginregistry.h>

class TUTORIAL_TANK_EXPORT ActorsRegistry : public dtCore::ActorPluginRegistry
{
public:
   static dtCore::RefPtr<dtCore::ActorType> TANK_ACTOR_TYPE;
   static dtCore::RefPtr<dtCore::ActorType> KILLABLE_ACTOR_TYPE;

   // Constructs our registry.  Creates the actor types easy access when needed.
   ActorsRegistry();

   // Registers actor types with the actor factory in the super class.
   virtual void RegisterActorTypes();
};

#endif // __GM_TUTORIAL_ACTORS_REGISTRY__

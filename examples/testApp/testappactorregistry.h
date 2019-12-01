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

#ifndef DELTA_TEST_APP_ACTOR_REGISTRY
#define DELTA_TEST_APP_ACTOR_REGISTRY

#include "export.h"
#include <dtCore/actorpluginregistry.h>

namespace dtExample
{
   /**
    * Class that exports the applicable actor proxies to a library
    */
   class TEST_APP_EXPORT TestAppActorRegistry : public dtCore::ActorPluginRegistry
   {
      public:
         typedef dtCore::ActorPluginRegistry BaseClass;

         static dtCore::RefPtr<dtCore::ActorType> EMPTY_ACTOR_TYPE;
         static dtCore::RefPtr<dtCore::ActorType> TERRAIN_ACTOR_TYPE;
         static dtCore::RefPtr<dtCore::ActorType> FIREWORK_ACTOR_TYPE;
         
         /// Constructor
         TestAppActorRegistry();

         /// Destructor
         virtual ~TestAppActorRegistry();

         /// Registers all of the actor proxies to be exported
         void RegisterActorTypes();

         void GetReplacementActorTypes(ActorTypeReplacements& replacements) const override;

      private:

   };
}

#endif

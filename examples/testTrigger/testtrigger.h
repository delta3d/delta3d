/*
 * testTrigger - testtrigger (.h & .cpp) - Using 'The MIT License'
 * Copyright (C) 2004-2008, MOVES Institute
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
 * Chris Osborn
 */
 
 #ifndef DELTA_TEST_TRIGGER
#define DELTA_TEST_TRIGGER

#include <dtABC/application.h>
#include <dtABC/proximitytrigger.h>
#include <dtCore/object.h>
#include <dtCore/positionallight.h>
#include <dtCore/refptr.h>
#include <dtCore/walkmotionmodel.h>

namespace dtCore
{
   class Keyboard;
}

class TestTrigger : public dtABC::Application
{
   class LightAction: public dtABC::Action
   {
   public:

      LightAction(dtCore::PositionalLight* light);

   protected:

      virtual ~LightAction() {}
      LightAction(const LightAction&); // Not implemented by design

      //returning false will tell this not to update again
      virtual bool OnNextStep()  { return false; }

      virtual void OnStart();

      virtual void OnPause()     {}
      virtual void OnUnPause()   {}

   private:

      dtCore::RefPtr<dtCore::PositionalLight> mPositionalLight;

   };

public:

   TestTrigger(const std::string& configFilename = "config.xml");
   virtual ~TestTrigger() {}

   virtual void Config();

private:

   dtCore::RefPtr<dtCore::Object>          mWarehouse;
   dtCore::RefPtr<dtCore::Object>          mHappySphere;
   dtCore::RefPtr<dtCore::PositionalLight> mPositionalLight;
   dtCore::RefPtr<dtCore::WalkMotionModel> mWalkMotionModel;
   dtCore::RefPtr<dtABC::ProximityTrigger> mProximityTrigger;
};

#endif // DELTA_TEST_TRIGGER

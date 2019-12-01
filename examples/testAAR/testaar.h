/* -*-c++-*-
 * testAAR - testaar (.h & .cpp) - Using 'The MIT License'
 * Copyright (C) 2005-2008, Alion Science and Technology Corporation
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
 * This software was developed by Alion Science and Technology Corporation under
 * circumstances in which the U. S. Government may have rights in the software.
 *
 * Christopher DuBuc
 * William E. Johnson II
 */

#ifndef DELTA_TEST_AAR_ENTRY_POINT
#define DELTA_TEST_AAR_ENTRY_POINT

#include <dtGame/gameentrypoint.h>
#include "export.h"
#include <dtCore/refptr.h>

// Foward declarations
namespace dtCore
{
   class MotionModel;
}

namespace dtGame
{
   class GameManager;
}

namespace dtLMS
{
   class LmsComponent;
}

class TEST_AAR_EXPORT TestAAR : public dtGame::GameEntryPoint
{

   public:

      /**
       * Constructs testAARLms.
       */
      TestAAR();

      /**
       * Destructs testAARLms, and disconnects from LMS if connected.
       */
      virtual ~TestAAR();

      /** 
       * Called to initialize the game application.  You can pull any command line params here.
       */
      virtual void Initialize(dtABC::BaseABC& app, int argc, char **argv);


      /**
       * Called just before your application's game loop starts.  This is your main 
       * opportunity to create components, load maps, create unique actors, etc...
       */
      virtual void OnStartup(dtABC::BaseABC& app, dtGame::GameManager& gamemanager);

   private:

      /**
       * Helper method to parse command line options
       * @note This method will parse command line options 
       * and set values as necessary. For instance, it will
       * set the data path of the application
       */
      void ParseCommandLineOptions(int argc, char **argv) const;

      dtCore::RefPtr<dtCore::MotionModel> mFMM;
      dtCore::RefPtr<dtLMS::LmsComponent> mLmsComponent;
      mutable bool mUseLMS;
};

#endif

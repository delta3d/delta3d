/* -*-c++-*-
 * Stealth Viewer - DeltaStepper (.h & .cpp) - Using 'The MIT License'
 * Copyright (C) 2007-2008, Alion Science and Technology Corporation
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
 * David Guthrie
 */

#include <prefix/dtqtprefix.h>
#include <dtQt/deltastepper.h>
#include <dtCore/system.h>
#include <QtCore/QtGlobal>
 
namespace dtQt
{

   DeltaStepper::DeltaStepper()
   {
      int msDelay = 0;
   
      //For some reason, Qt 4.7.x causes some dialog windows to hang up when 
      //used with a 0 time delay.  This is a temporary fix until the real cause
      //is rooted out.
      #if defined(QT_VERSION) && (QT_VERSION > 0x040600)
      msDelay = 1;
      #endif

      mTimer.setInterval(msDelay);
      connect(&mTimer, SIGNAL(timeout()), this, SLOT(Tick()), Qt::QueuedConnection);
   }

   DeltaStepper::~DeltaStepper()
   {
   }

   void DeltaStepper::Start()
   {
      mTimer.start();
   }

   void DeltaStepper::Stop()
   {
      mTimer.stop();
   }

   void DeltaStepper::Tick()
   {
      dtCore::System& system = dtCore::System::GetInstance();
      if (system.IsRunning())
      {
         system.StepWindow();
         emit PostTick();
      }
   }

}

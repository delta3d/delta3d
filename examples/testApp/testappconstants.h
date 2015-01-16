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

#ifndef TEST_APP_CONSTANTS_H
#define TEST_APP_CONSTANTS_H

////////////////////////////////////////////////////////////////////////////////
// INCLUDE DIRECTIVES
////////////////////////////////////////////////////////////////////////////////
#include <dtUtil/enumeration.h>
#include <dtUtil/refstring.h>
#include "export.h"



namespace dtExample
{
   ////////////////////////////////////////////////////////////////////
   // CONSTANTS
   ////////////////////////////////////////////////////////////////////
   class TEST_APP_EXPORT UINames
   {
   public:
      static const dtUtil::RefString UI_HELP;
      static const dtUtil::RefString UI_CONTROL_PANEL;

   private:
      UINames() {}
      virtual ~UINames() {}
   };

   ////////////////////////////////////////////////////////////////////
   class TEST_APP_EXPORT MotionModelType : public dtUtil::Enumeration
   {
      DECLARE_ENUM(MotionModelType);

   public:
      typedef dtUtil::Enumeration BaseClass;

      static MotionModelType NONE;
      static MotionModelType FLY;
      static MotionModelType FPS;
      static MotionModelType ORBIT;
      static MotionModelType RTS;
      static MotionModelType UFO;
      static MotionModelType WALK;

   protected:
      MotionModelType(const std::string& name);
      virtual ~MotionModelType();
   };

}

#endif

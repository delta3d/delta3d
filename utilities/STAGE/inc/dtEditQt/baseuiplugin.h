/* -*-c++-*-
 * Delta3D Simulation Training And Game Editor (STAGE)
 * STAGE - This source file (.h & .cpp) - Using 'The MIT License'
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
 * David Guthrie
 */

#ifndef DELTA_BASE_UI_PLUGIN
#define DELTA_BASE_UI_PLUGIN

#include <dtEditQt/export.h>

#include <QtCore/QtPlugin>

#include <string>

class QWidget;

namespace dtCore
{
   class NamedGroupParameter;
}

namespace dtEditQt
{
   class DT_EDITQT_EXPORT BaseUIPlugin
   {
   public:
      virtual ~BaseUIPlugin() {}

      /**
       * @return the unique name of this BaseUIPlugin.
       * This should match ui class string on the group actor properties this wants to edit.
       */
      virtual const std::string& GetName() const = 0;

      ///@return a new instance of the widget this plugin manipulates giving it the specified UI parent.
      virtual QWidget* CreateWidget(QWidget* parent) = 0;
   };

} // namespace dtEditQt
Q_DECLARE_INTERFACE(dtEditQt::BaseUIPlugin,  "org.delta3d.BaseUIPlugin/1.0");

#endif // DELTA_BASE_UI_PLUGIN

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

#ifndef DELTA_GROUP_UI_PLUGIN
#define DELTA_GROUP_UI_PLUGIN

#include <dtEditQt/baseuiplugin.h>

class QWidget;

namespace dtDAL
{
   class NamedGroupParameter;
}

namespace dtEditQt
{
   class DT_EDITQT_EXPORT GroupUIPlugin: public BaseUIPlugin
   {
   public:
      virtual ~GroupUIPlugin() {}

      /**
       * @return the unique name of this GroupUIPlugin.
       * This should match ui class string on the group actor properties this wants to edit.
       */
      virtual const std::string& GetName() const = 0;

      /**
       * Implementations should update the data displayed in the widget with the given dtDAL::GroupNamedParameter.
       * @return true if the operation was successful.
       * @param widget The widget to update.
       * @param modelData The data to use to populate the widget.
       */
      virtual bool UpdateWidgetFromModel(QWidget& widget, const dtDAL::NamedGroupParameter& modelData) const = 0;

      /**
       * Implementations should fill the given dtDAL::GroupNamedParameter with the values in widget.
       * @return true if the operation was successful.
       * @param widget The widget to update from.
       * @param modelData The data object to populate.
       */
      virtual bool UpdateModelFromWidget(QWidget& widget, dtDAL::NamedGroupParameter& modelData) const = 0;

      ///@return a new instance of the widget this plugin manipulates giving it the specified UI parent.
      virtual QWidget* CreateWidget(QWidget* parent) = 0;
   };

} // namespace dtEditQt
Q_DECLARE_INTERFACE(dtEditQt::GroupUIPlugin,  "org.delta3d.GroupUIPlugin/1.0");

#endif // DELTA_GROUP_UI_PLUGIN

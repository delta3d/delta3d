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
 * William E. Johnson II
 */

#ifndef DELTA_TASK_UI_PLUGIN
#define DELTA_TASK_UI_PLUGIN

#include <QtCore/QObject>
#include <dtEditQt/groupuiplugin.h>

namespace dtEditQt
{
   /**
    * @class TaskUIPlugin
    * @brief Plugin implementation for a group ui to edit tasks.
    */
   class TaskUIPlugin : public QObject, public dtEditQt::GroupUIPlugin
   {
      Q_OBJECT;
      Q_INTERFACES(dtEditQt::GroupUIPlugin);

   public:
      TaskUIPlugin();
      virtual ~TaskUIPlugin() {}
      /**
       * @return the name of the task ui plugin.
       */
      virtual const std::string& GetName() const { return mName; }

      /**
       * @param widget The widget to update.  It should be a TaskEditor.
       * @param modelData The data to use to populate the widget.
       */
      virtual bool UpdateWidgetFromModel(QWidget& widget, const dtCore::NamedGroupParameter& modelData) const;

      /**
       * @param widget The widget to read from.  It should be a TaskEditor.
       * @param modelData The data object to populate.
       */
      virtual bool UpdateModelFromWidget(QWidget& widget, dtCore::NamedGroupParameter& modelData) const;

      ///@return a new instance of TaskEditor.
      virtual QWidget* CreateWidget(QWidget* parent);
   private:
      std::string mName;
   };

} // namespace dtEditQt

#endif // DELTA_TASK_UI_PLUGIN

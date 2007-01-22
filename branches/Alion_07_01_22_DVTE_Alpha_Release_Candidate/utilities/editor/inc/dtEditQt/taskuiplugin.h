/* -*-c++-*-
 * Delta3D Open Source Game and Simulation Engine 
 * Simulation, Training, and Game Editor (STAGE)
 * Copyright (C) 2006, Alion Science and Technology, BMH Operation 
 *
 * This program is free software; you can redistribute it and/or modify it under
 * the terms of the GNU General Public License as published by the Free 
 * Software Foundation; either version 2 of the License, or (at your option) 
 * any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS 
 * FOR A PARTICULAR PURPOSE. See the GNU General Public License for more 
 * details.
 *
 * You should have received a copy of the GNU General Public License 
 * along with this library; if not, write to the Free Software Foundation, Inc., 
 * 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA 
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
         virtual ~TaskUIPlugin() {};
         /**
          * @return the name of the task ui plugin.
          */
         virtual const std::string& GetName() const { return mName; }
         
         /**
          * @param widget The widget to update.  It should be a TaskEditor.
          * @param modelData The data to use to populate the widget.
          */
         virtual bool UpdateWidgetFromModel(QWidget& widget, const dtDAL::NamedGroupParameter& modelData) const;

         /**
          * @param widget The widget to read from.  It should be a TaskEditor.
          * @param modelData The data object to populate.
          */
         virtual bool UpdateModelFromWidget(QWidget& widget, dtDAL::NamedGroupParameter& modelData) const;
         
         ///@return a new instance of TaskEditor.
         virtual QWidget* CreateWidget(QWidget* parent);
      private:
         std::string mName;

   };
}

#endif

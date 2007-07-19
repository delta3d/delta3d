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
 * David Guthrie
 */
#include <prefix/dtstageprefix-src.h>

#define QT_STATICPLUGIN

#include <QtGui/QWidget>
#include <QtCore/QString>

#include <dtEditQt/taskuiplugin.h>

#include <dtEditQt/taskeditor.h>
#include <dtEditQt/editordata.h>

#include <dtDAL/map.h>
#include <dtDAL/namedparameter.h>
#include <dtDAL/actortype.h>
#include <dtCore/uniqueid.h>

namespace dtEditQt
{

   ///////////////////////////////////////////////////////////////////////////////
   /////////////    Task UI Plugin    ////////////////////////////////////////////
   ///////////////////////////////////////////////////////////////////////////////
   
   TaskUIPlugin::TaskUIPlugin(): mName("TaskChildren")
   {}
   
   ///////////////////////////////////////////////////////////////////////////////
   bool TaskUIPlugin::UpdateWidgetFromModel(QWidget& widget, const dtDAL::NamedGroupParameter& modelData) const
   {
      TaskEditor* taskEditor = dynamic_cast<TaskEditor*>(&widget);
      if (taskEditor == NULL)
         return false;
      
      taskEditor->SetTaskChildren(modelData);
      return true;
   }
    
   ///////////////////////////////////////////////////////////////////////////////
   bool TaskUIPlugin::UpdateModelFromWidget(QWidget& widget, dtDAL::NamedGroupParameter& modelData) const
   {
      TaskEditor* taskEditor = dynamic_cast<TaskEditor*>(&widget);
      if (taskEditor == NULL)
         return false;
      
      taskEditor->GetTaskChildren(modelData);
      return true; 
   }
   
   ///////////////////////////////////////////////////////////////////////////////
   QWidget* TaskUIPlugin::CreateWidget(QWidget* parent)
   {
      return new TaskEditor(parent);
   }

}
//This export line had to be done this way because the first name is used in a function name in the macro, but I have to 
//put the call outside a namespace, so the second parameter has to have the namespace of the actual class.  God help us.
Q_EXPORT_PLUGIN2(TaskUIPlugin, dtEditQt::TaskUIPlugin);
//The plugin is all static, so just export and import it all at the same time.
Q_IMPORT_PLUGIN(TaskUIPlugin);

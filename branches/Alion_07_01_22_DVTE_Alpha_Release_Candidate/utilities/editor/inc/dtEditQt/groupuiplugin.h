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
 * David Guthrie
 */

#ifndef DELTA_GROUP_UI_PLUGIN
#define DELTA_GROUP_UI_PLUGIN

#include <string>
#include <QtCore/QtPlugin>

class QWidget;

namespace dtDAL
{
   class NamedGroupParameter;
}

namespace dtEditQt 
{
   class GroupUIPlugin
   {
      public:
         virtual ~GroupUIPlugin() {};

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
}
Q_DECLARE_INTERFACE( dtEditQt::GroupUIPlugin,  "org.delta3d.GroupUIPlugin/1.0" );


#endif

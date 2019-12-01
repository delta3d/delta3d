/*
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2008 MOVES Institute
 *
 * This library is free software; you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by the Free
 * Software Foundation; either version 2.1 of the License, or (at your option)
 * any later version.
 *
 * This library is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; if not, write to the Free Software Foundation, Inc.,
 * 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 *
 * Author: Jeff P. Houde
 */

#ifndef INSPECTORQT_PROPERTY_EDITOR
#define INSPECTORQT_PROPERTY_EDITOR

#include <dtQt/basepropertyeditor.h>

/**
 * This class is the property editor for displaying and editing properties of selected objects.  It shows the
 * information about the selected actor(s) including type, name, location, rotation, etc...  It is a
 * dockable window.
 */
class PropertyEditor : public dtQt::BasePropertyEditor
{
   Q_OBJECT
public:
   typedef dtQt::BasePropertyEditor BaseClass;

   /**
    * Constructor
    */
   PropertyEditor(QMainWindow* parent);

   /**
    * Destructor
    */
   virtual ~PropertyEditor();

   void PropertyAboutToChangeFromControl(dtCore::PropertyContainer& propCon, dtCore::ActorProperty& prop,
            const std::string& oldValue, const std::string& newValue);

protected:

   /// Called on the close event
   virtual void closeEvent(QCloseEvent* e);
};

#endif // INSPECTORQT_PROPERTY_EDITOR

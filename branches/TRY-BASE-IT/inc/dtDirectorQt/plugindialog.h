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
 * Author:
 * Edited By: Jeff P. Houde
 */
#ifndef DIRECTOR_PLUGIN_DIALOG
#define DIRECTOR_PLUGIN_DIALOG

#include <dtDirectorQt/export.h>
#include <dtDirectorQt/directoreditor.h>
#include <dtDirectorQt/plugininterface.h>
#include "ui_plugindialog.h"

namespace dtDirector
{
   class DT_DIRECTOR_QT_EXPORT PluginDialog: public QObject
   {
      Q_OBJECT

   public:

      PluginDialog(DirectorEditor* editor);
      virtual ~PluginDialog();

   public slots:

      /** when user chooses plugin manager in menu */
      void OnOpenDialog();

      /** when user checks/unchecks a plugin in the list */
      void OnPluginChanged(QListWidgetItem* item);

   private:
      DirectorEditor* mEditor;

      /** when user presses OK button */
      void OnApplyChanges(QListWidget* listWidget);
      
   };
}

#endif //PLUGIN_DIALOG_PLUGIN

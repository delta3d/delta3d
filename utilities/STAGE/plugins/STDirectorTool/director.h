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

#ifndef DIRECTOR_TOOL_PLUGIN
#define DIRECTOR_TOOL_PLUGIN

#include "export.h"
#include <dtEditQt/mainwindow.h>
#include <dtEditQt/plugininterface.h>

#include <dtDirector/director.h>
#include <dtDirectorQt/directoreditor.h>

#include <QtGui/QDockWidget>


using namespace dtEditQt;

class QAction;

/**
 * The DirectorToolPlugin is a plugin that is used as a tool
 * to place a LinkedPointsActor into the world.
 */
class DT_DIRECTOR_TOOL_EXPORT DirectorToolPlugin 
   : public dtDirector::DirectorEditor
   , public Plugin
{
   Q_OBJECT

public:

   const static std::string PLUGIN_NAME;
   
   DirectorToolPlugin(MainWindow* mw);

   ~DirectorToolPlugin();

   virtual void Destroy();

   /** override close event to get notified when user closes the dock */
   virtual void closeEvent(QCloseEvent* event);

public slots:

   /**
    * Event handler when the map has changed.
    */
   void OnMapChanged();

   /**
   * User has pressed the tool button.
   */
   void OnToolButtonPressed();

private:

   MainWindow*    mMainWindow;

   QAction*       mToolButton;
};

#endif // DIRECTOR_TOOL_PLUGIN

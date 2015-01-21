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

#include <QtGui/QWidget>

#include <vector>

using namespace dtEditQt;

class QAction;
class DirectorToolEditor;

/**
 * The DirectorToolPlugin is a plugin that is used as a tool
 * to edit Director scripts.
 */
class DT_DIRECTOR_TOOL_EXPORT DirectorToolPlugin
   : public QWidget
   , public Plugin
{
   Q_OBJECT

public:

   const static std::string PLUGIN_NAME;
   
   DirectorToolPlugin(MainWindow* mw);

   ~DirectorToolPlugin();

   virtual void Destroy();

public slots:

   /**
    * User has pressed the tool button.
    */
   void OnToolButtonPressed();

private:

   MainWindow*    mMainWindow;

   QAction*       mToolButton;
};


/**
 * The STAGE Director Editor Implementation.
 */
class DT_DIRECTOR_TOOL_EXPORT DirectorToolEditor
   : public dtDirector::DirectorEditor
{
   Q_OBJECT

public:
   
   DirectorToolEditor();

   ~DirectorToolEditor();

   /**
    * Retrieves the current actor selection from STAGE.
    *
    * @return  The current actor selection.
    */
   virtual dtCore::ActorPtrVector GetActorSelection();

public slots:

   /**
    * Event handler when the map has changed.
    */
   void OnMapChanged();

   /**
   * Event handler when we want to center on an actor.
   * Note: Use the mProxy member variable to store the actor to go to.
   */
   void OnGotoActor();

   /**
   * Event handler when the current value node should be set to the current actor.
   * Note: Use the mNode member variable to store the current node to be set.
   */
   void OnUseCurrentActor();

protected:

   /**
   * Event handler when viewing the context menu of a value node.
   *
   * @param[in]  node  The value node.
   *
   * @return     True if a default action was set.
   */
   virtual bool OnContextValueNode(dtCore::RefPtr<dtDirector::Node> node, QMenu& menu);

   /**
   * Event handler when a node is double clicked.
   *
   * @param[in]  node  The node that was double clicked.
   *
   * @return     True if an action was performed.
   */
   virtual bool OnDoubleClickValueNode(dtCore::RefPtr<dtDirector::Node> node);

private:

   dtCore::RefPtr<dtCore::BaseActorObject> mProxy;
   dtDirector::Node*  mNode;
};

#endif // DIRECTOR_TOOL_PLUGIN

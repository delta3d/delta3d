/* -*-c++-*-
* Copyright (C) 2015, Caper Holdings LLC
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
*/

#ifndef DELTA_SCENE_HIERARCHY_EDITOR_PLUGIN_H
#define DELTA_SCENE_HIERARCHY_EDITOR_PLUGIN_H

////////////////////////////////////////////////////////////////////////////////
// INCLUDE DIRECTIVES
////////////////////////////////////////////////////////////////////////////////
#include "export.h"
#include <QtWidgets/QDockWidget>
#include <dtEditQt/plugininterface.h>
#include <dtQt/nodegraphview.h>
#include "ui_maphierarchyeditor.h"




////////////////////////////////////////////////////////////////////////////////
// FORWARD DECLARATIONS
////////////////////////////////////////////////////////////////////////////////
class Ui_SceneHierarcyEditor;

namespace dtEditQt
{
   class MainWindow;
}

typedef dtCore::ActorRefPtrVector ActorRefPtrVector;



////////////////////////////////////////////////////////////////////////////////
// CLASS CODE
////////////////////////////////////////////////////////////////////////////////  
class DELTA_SCENE_HIERARCHY_EDITOR_EXPORT SceneHierarchyEditorPlugin
   : public QDockWidget   
   , public dtEditQt::Plugin
{
   Q_OBJECT

public:
   const static std::string PLUGIN_NAME;

   SceneHierarchyEditorPlugin(dtEditQt::MainWindow* mw);
   ~SceneHierarchyEditorPlugin();

   virtual void Destroy();

   /** override close event to get notified when user closes the dock */
   virtual void closeEvent(QCloseEvent* event);
   
   void UpdateUI();

   void GetRootActorNodeWrappers(dtQt::BaseNodeWrapperArray& outNodes);

   void ConvertNodesToActors(const dtQt::BaseNodeWrapperArray& nodes, dtCore::ActorRefPtrVector& outActors);
   void ConvertActorsToNodes(const dtCore::ActorRefPtrVector& actors, dtQt::BaseNodeWrapperArray& outNodes);

public slots:
   void onCurrentMapChanged();

   void OnNodeSelectionChanged(const dtQt::BaseNodeWrapperArray& nodes);
   void OnNodesDetached(const dtQt::BaseNodeWrapperArray& nodes);
   void OnNodesAttached(const dtQt::BaseNodeWrapperArray& nodes, const dtQt::BaseNodeWrapper& parentNode);
   void OnActorCreated(dtCore::ActorPtr actor, bool forceNoAdjustments);
   void OnActorDestroyed(dtCore::ActorPtr actor);
   void OnActorsSelected(ActorRefPtrVector& actors);

private:
   Ui_SceneHierarchyEditor mUI;
   dtEditQt::MainWindow* mMainWindow;
   dtQt::NodeGraphViewerPanel*  mNodeGraphViewer;
};

#endif

#ifndef STAGE_MAP_HIERARCHY_EDITOR_PLUGIN_H__
#define STAGE_MAP_HIERARCHY_EDITOR_PLUGIN_H__

#include "export.h"

#include "ui_maphierarchyeditor.h"

#include <dtEditQt/plugininterface.h>

#include <QtGui/QDockWidget>

//forward declarations
class Ui_MapHierarcyEditor;
class QGridLayout;

class NodeButton;

namespace dtCore
{
   class ActorHierarchyNode;
}

namespace dtEditQt
{
   class MainWindow;
}

/**
 The Map Hierarchy Editor STAGE plugin.
*/      
class STAGE_MAP_HIERARCHY_EDITOR_EXPORT MapHierarchyEditorPlugin
   : public QDockWidget   
   , public dtEditQt::Plugin
{
   Q_OBJECT

public:
   const static std::string PLUGIN_NAME;

   MapHierarchyEditorPlugin(dtEditQt::MainWindow* mw);
   ~MapHierarchyEditorPlugin();

   virtual void Destroy();

   /** override close event to get notified when user closes the dock */
   virtual void closeEvent(QCloseEvent* event);
   
   void BuildTreeFromMap();
   void BuildTreeGUI();
   void BuildGUIBranch(NodeButton* GUIparent, dtCore::ActorHierarchyNode* branch);
   void RebuildTree();
   QGridLayout* GetGridLayout();

public slots:
   void onCurrentMapChanged();

private:
   Ui_MapHierarchyEditor                              mUI;
   dtEditQt::MainWindow*                              mMainWindow;
};

#endif //STAGE_MAP_HIERARCHY_EDITOR_PLUGIN_H__

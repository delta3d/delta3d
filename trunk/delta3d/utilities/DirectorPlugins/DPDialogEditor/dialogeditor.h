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

#ifndef DIRECTOR_DIALOG_EDITOR_PLUGIN
#define DIRECTOR_DIALOG_EDITOR_PLUGIN

#include <export.h>

#include <dtDirectorQt/plugininterface.h>
#include <dtDirectorQt/customeditortool.h>

#include <dtCore/uniqueid.h>

#include <QtGui/QMainWindow>

#include "ui_dialogeditor.h"

namespace dtDirector
{
   class DirectorEditor;
}

class DialogRootItem;
class DialogRefLineType;

/**
 * The DirectorDialogEditorPlugin is a plugin that is used as a tool
 * to generate a Director Dialog Macro Node.
 */
class DT_DIRECTOR_DIALOG_EDITOR_EXPORT DirectorDialogEditorPlugin
   : public QMainWindow
   , public dtDirector::CustomEditorTool
   , public dtDirector::Plugin
{
   Q_OBJECT

public:

   enum EVENT_TYPE
   {
      PRE_EVENT = 1,
      DURING_EVENT = 2,
      POST_EVENT = 4,
   };

   const static std::string PLUGIN_NAME;

   /**
    * Constructor
    *
    * @param[in]  editor  The editor.
    */
   DirectorDialogEditorPlugin();

   /**
    * Deconstructor.
    */
   ~DirectorDialogEditorPlugin();

   /**
    * Initializes the window.
    */
   void Initialize();

   /**
    * Event handler when this tool should be opened for the given graph.
    * @Note:  This method should be overloaded to perform any initial
    * operations when this tool is activated.
    *
    * @param[in]  editor  The editor that is using this tool.
    * @param[in]  graph   The graph to open the tool for.
    */
   virtual void Open(dtDirector::DirectorEditor* editor, dtDirector::DirectorGraph* graph);

   /**
    * Event handler to close the tool.
    * @Note:  This method should be overloaded to perform any shut down
    * operations when this tool is deactivated.
    */
   virtual void Close();

   /**
    * Destroys the window.
    */
   virtual void Destroy();

   /**
    * override close event to get notified when user closes the dock
    *
    * @param[in]  event  The close event.
    */
   virtual void closeEvent(QCloseEvent* event);

   /**
    * Retrieves the dialog tree.
    */
   DialogTreeWidget* GetTree() const;

   /**
    * Maps a reference event name with the given line ID.
    * This is used when loading the dialog tree.
    *
    * @param[in]  refName  The name of the reference.
    * @param[in]  id       The id of the line.
    */
   void MapReference(const QString& refName, const dtCore::UniqueId& id);

   /**
    * Registers a reference line with the given name.
    * This is used when loading the dialog tree.
    *
    * @param[in]  refLine  The reference line.
    * @param[in]  refName  The reference name.
    */
   void RegisterReference(DialogLineItem* refLine, const QString& refName);

   /**
    * Registers an event.
    * This is used when saving a dialog line that contains an event.
    *
    * @param[in]  name  The name of the event.
    */
   void RegisterEvent(const QString& name, int eventType);

public slots:

   /**
    * Event handler when an item has been selected in the tree.
    */
   void OnCurrentTreeItemChanged(QTreeWidgetItem* current, QTreeWidgetItem* previous);

   /**
    * Event handler to load the current graph.
    */
   void OnLoad();

   /**
    * Event handler when the save button is pressed.
    */
   void OnSave();

   /**
    * Event handler when the save and close button is pressed.
    */
   void OnSaveAndClose();

   /**
    * Event handler when the cancel button is pressed.
    */
   void OnCancel();

protected:

private:

   Ui_DialogEditor mUI;

   DialogRootItem* mRoot;
   DialogRootItem* mEndDialog;

   QWidget*        mEditWidget;

   std::map<QString, dtCore::UniqueId> mRefMap;
   std::map<QString, std::vector<DialogLineItem*> > mRefRegister;

   std::map<QString, int> mEventRegister;
};

#endif // DIRECTOR_DIALOG_EDITOR_PLUGIN

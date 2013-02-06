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

#ifndef DIRECTOR_DIALOG_TREE_ITEMS
#define DIRECTOR_DIALOG_TREE_ITEMS

#include <export.h>

#include <QtGui/QTreeWidget>
#include <QtGui/QListWidget>
#include <QtGui/QTreeWidgetItem>
#include <QtGui/QListWidgetItem>
#include <QtGui/QItemDelegate>

#include <dtCore/uniqueid.h>

class DialogLineType;
class DialogChoiceItem;
class DialogTreeDelegate;
class DialogSpeakerList;
class DirectorDialogEditorPlugin;

/**
 * Dialog tree.
 */
class DT_DIRECTOR_DIALOG_EDITOR_EXPORT DialogTreeWidget: public QTreeWidget
{
   Q_OBJECT
public:

   DialogTreeWidget(QWidget* parent = NULL);
   ~DialogTreeWidget();

   void SetEditor(DirectorDialogEditorPlugin* editor);
   DirectorDialogEditorPlugin* GetEditor() const;

   void Reset();

   QTreeWidgetItem* GetItem(const QModelIndex& index) const;

   QStringList GetSpeakerList() const;

   int CreateIndex();

   void UpdateLabels() const;

   void OnSpeakerRemoved(const QString& speaker);
   void OnSpeakerRenamed(const QString& oldName, const QString& newName);

protected:

   void startDrag(Qt::DropActions supportedActions);
   void dropEvent(QDropEvent* event);

   void mousePressEvent(QMouseEvent* event);

   void contextMenuEvent(QContextMenuEvent* event);

public slots:

   void RecurseStartDrag(QTreeWidgetItem* item, QTreeWidgetItem* moving);

   void OnAddNewLine(QAction* action);
   void OnAddNewChoice();

   void OnRemoveLine();

   friend class DialogSpeakerList;

private:

   void RecurseSpeakerRemoved(QTreeWidgetItem* item, const QString& speaker);
   void RecurseSpeakerRenamed(QTreeWidgetItem* item, const QString& oldName, const QString& newName);

   DirectorDialogEditorPlugin* mEditor;
   DialogTreeDelegate* mDelegate;
   int mIndex;
};

/**
 * Item Delegate.
 */
class DT_DIRECTOR_DIALOG_EDITOR_EXPORT DialogTreeDelegate: public QItemDelegate
{
   Q_OBJECT
public:

   DialogTreeDelegate(DialogTreeWidget* tree);

   QWidget* createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const;
   void setEditorData(QWidget *editor, const QModelIndex &index) const;
   void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const;

   void UpdateLabels() const;

private:

   void RecurseUpdateLabels(QTreeWidgetItem* item) const;

   DialogTreeWidget* mTree;
};

/**
 * A dialog line item listed in the dialog tree.
 */
class DT_DIRECTOR_DIALOG_EDITOR_EXPORT DialogLineItem: public QTreeWidgetItem
{
public:

   DialogLineItem(const QString& name, const DialogLineType* type, int index, DirectorDialogEditorPlugin* editor);
   ~DialogLineItem();

   DialogLineType*         GetType()     const {return mType;}
   const QString&          GetTypeName() const {return mTypeName;}
   DialogTreeWidget*       GetTree()     const {return mTree;}
   const dtCore::UniqueId& GetID()       const {return mID;}
   int                     GetIndex()    const {return mIndex;}

   virtual void UpdateLabel();

   bool CanHaveSubLine() const;
   bool IsChoice() const;
   int  GetChoiceLimit() const;

   DialogLineItem*   GetChildLine() const;
   DialogChoiceItem* GetChildChoice(int index) const;

protected:

private:

   DialogLineType*   mType;

   QString           mTypeName;
   DialogTreeWidget* mTree;

   dtCore::UniqueId  mID;
   int               mIndex;
};


/**
 * A dialog choice item listed as a choice to a line item.
 */
class DT_DIRECTOR_DIALOG_EDITOR_EXPORT DialogChoiceItem: public QTreeWidgetItem
{
public:

   DialogChoiceItem(int index, const QString& label, bool moveable, bool nameable);
   ~DialogChoiceItem();

   bool IsMoveable() const {return mMoveable;}
   bool IsNameable() const {return mNameable;}

   void UpdateLabel();

   void SetLabel(const QString& label);
   const QString& GetLabel() const;

   DialogLineItem* GetChildLine() const;

protected:

private:

   QString  mLabel;
   int      mIndex;
   bool     mMoveable;
   bool     mNameable;
};


/**
 * The root dialog tree item.
 */
class DT_DIRECTOR_DIALOG_EDITOR_EXPORT DialogRootItem: public DialogLineItem
{
public:

   DialogRootItem(const QString& name, const DialogLineType* type, bool allowChildren, DirectorDialogEditorPlugin* editor);
   ~DialogRootItem();

   virtual void UpdateLabel();
   bool CanHaveSubLine() const;

protected:

private:

   bool mAllowChildren;
};

/**
 * The speaker list widget.
 */
class DT_DIRECTOR_DIALOG_EDITOR_EXPORT DialogSpeakerList: public QListWidget
{
   Q_OBJECT
public:

   DialogSpeakerList(QWidget* parent = NULL);
   ~DialogSpeakerList();

   void Reset();

   void SetTree(DialogTreeWidget* tree);

   QStringList GetSpeakerList() const;
   void AddSpeaker(const QString& speaker);

protected:

   void startDrag(Qt::DropActions supportedActions);
   void dropEvent(QDropEvent* event);
   void dragEnterEvent(QDragEnterEvent* event);
   void dragLeaveEvent(QDragLeaveEvent* event);

   void mouseDoubleClickEvent(QMouseEvent *event);
   void contextMenuEvent(QContextMenuEvent* event);

public slots:

   void OnSelectionChanged(QListWidgetItem* current, QListWidgetItem* previous);
   void OnItemChanged(QListWidgetItem* changedItem);
   void OnRemoveSpeaker();

private:

   QString mOldName;
   DialogTreeWidget* mTree;
};

#endif // DIRECTOR_DIALOG_TREE_ITEMS

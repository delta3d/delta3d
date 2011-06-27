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
#include <QtGui/QTreeWidgetItem>
#include <QtGui/QItemDelegate>

#include <dtCore/uniqueid.h>

class DialogLineType;
class DialogChoiceItem;
class DialogTreeDelegate;
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

   void Reset()
   {
      mIndex = 1;
   }

   QTreeWidgetItem* GetItem(const QModelIndex& index) const
   {
      return itemFromIndex(index);
   }

   int CreateIndex()
   {
      return mIndex++;
   }

   void UpdateLabels() const;

protected:

   void startDrag(Qt::DropActions supportedActions);

   //void dragMoveEvent(QDragMoveEvent* event);
   //void dropEvent(QDropEvent* event);

   void contextMenuEvent(QContextMenuEvent* event);

public slots:

   void RecurseStartDrag(QTreeWidgetItem* item, QTreeWidgetItem* moving);

   void OnAddNewLine(QAction* action);
   void OnAddNewChoice();

   void OnRemoveLine();

protected slots:

private:

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

   DialogRootItem(const QString& name, const DialogLineType* type, bool allowChildren);
   ~DialogRootItem();

   virtual void UpdateLabel();
   bool CanHaveSubLine() const;

protected:

private:

   bool mAllowChildren;
};

#endif // DIRECTOR_DIALOG_TREE_ITEMS

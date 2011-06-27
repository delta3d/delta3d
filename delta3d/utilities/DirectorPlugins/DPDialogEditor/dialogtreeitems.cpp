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

#include <dialogtreeitems.h>
#include <dialogeditor.h>

#include <dialoglineregistry.h>
#include <dialoglinetype.h>

#include <QtGui/QMenu>
#include <QtGui/QAction>
#include <QtGui/QDragMoveEvent>
#include <QtGui/QLineEdit>



////////////////////////////////////////////////////////////////////////////////
DialogTreeWidget::DialogTreeWidget(QWidget* parent)
   : QTreeWidget(parent)
   , mEditor(NULL)
   , mDelegate(NULL)
   , mIndex(1)
{
   invisibleRootItem()->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);

   setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
   setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

   mDelegate = new DialogTreeDelegate(this);
   setItemDelegate(mDelegate);
}

////////////////////////////////////////////////////////////////////////////////
DialogTreeWidget::~DialogTreeWidget()
{
   if (mDelegate)
   {
      delete mDelegate;
      mDelegate = NULL;
   }
}

////////////////////////////////////////////////////////////////////////////////
void DialogTreeWidget::SetEditor(DirectorDialogEditorPlugin* editor)
{
   mEditor = editor;
}

////////////////////////////////////////////////////////////////////////////////
DirectorDialogEditorPlugin* DialogTreeWidget::GetEditor() const
{
   return mEditor;
}

////////////////////////////////////////////////////////////////////////////////
void DialogTreeWidget::UpdateLabels() const
{
   if (mDelegate)
   {
      mDelegate->UpdateLabels();
   }
}

////////////////////////////////////////////////////////////////////////////////
void DialogTreeWidget::startDrag(Qt::DropActions supportedActions)
{
   QTreeWidgetItem* movingItem = currentItem();
   RecurseStartDrag(topLevelItem(0), movingItem);

   QTreeWidget::startDrag(supportedActions);
}

//////////////////////////////////////////////////////////////////////////////////
//void DialogTreeWidget::dragMoveEvent(QDragMoveEvent* event)
//{
//   event->ignore();
//
//   QTreeWidgetItem* source = currentItem();
//   QTreeWidgetItem* dest = dynamic_cast<QTreeWidgetItem*>(itemAt(event->pos()));
//
//   if (source && dest && source != dest)
//   {
//      DialogLineItem* sourceLine = dynamic_cast<DialogLineItem*>(source);
//      DialogLineItem* destLine = dynamic_cast<DialogLineItem*>(dest);
//
//      DialogChoiceItem* sourceChoice = dynamic_cast<DialogChoiceItem*>(source);
//      DialogChoiceItem* destChoice = dynamic_cast<DialogChoiceItem*>(dest);
//
//      // If you are moving a line item.
//      if (sourceLine)
//      {
//         // Lines can only be moved to items with no children.
//         if (dest->childCount() > 0)
//         {
//            return;
//         }
//
//         // Lines can only be moved to other lines that are not choice lines.
//         if (destLine && (!destLine->CanHaveSubLine() || destLine->IsChoice()))
//         {
//            return;
//         }
//      }
//      // If you are moving a choice item.
//      else if (sourceChoice)
//      {
//         // Choice items can not be moved to other choice items.
//         if (destChoice)
//         {
//            return;
//         }
//
//         // Choice items can only be moved to choice line nodes.
//         if (destLine && !destLine->IsChoice())
//         {
//            return;
//         }
//
//         // Choice items can only be moved to a choice line that isn't at it's choice limit.
//         if (destLine && destLine->GetChoiceLimit() > -1 &&
//            destLine->childCount() >= destLine->GetChoiceLimit())
//         {
//            return;
//         }
//      }
//   }
//
//   event->accept();
//}

//////////////////////////////////////////////////////////////////////////////////
//void DialogTreeWidget::dropEvent(QDropEvent* event)
//{
//   QTreeWidget::dropEvent(event);
//}

////////////////////////////////////////////////////////////////////////////////
void DialogTreeWidget::contextMenuEvent(QContextMenuEvent* event)
{
   QTreeWidgetItem* item = itemAt(event->pos());

   bool addNewLine   = false;
   bool addNewChoice = false;
   bool removeLine   = false;
   bool removeChoice = false;

   QMenu menu;
   
   DialogLineItem* lineItem = dynamic_cast<DialogLineItem*>(item);
   if (lineItem)
   {
      removeLine = true;

      if (lineItem->IsChoice())
      {
         // Add a new choice only if we have not reached our limit.
         addNewChoice = true;

         int choiceLimit = lineItem->GetChoiceLimit();
         if (choiceLimit > -1 && choiceLimit <= lineItem->childCount())
         {
            addNewChoice = false;
         }
      }
      else if (lineItem->CanHaveSubLine())
      {
         // Add a new follow up line if this line doesn't currently have one.
         if (lineItem->childCount() == 0)
         {
            addNewLine = true;
         }
      }
   }

   DialogRootItem* rootItem = dynamic_cast<DialogRootItem*>(item);
   if (rootItem)
   {
      if (!rootItem->CanHaveSubLine())
      {
         addNewLine = false;
         addNewChoice = false;
      }

      removeLine = false;
   }

   DialogChoiceItem* choiceItem = dynamic_cast<DialogChoiceItem*>(item);
   if (choiceItem)
   {
      if (choiceItem->childCount() == 0)
      {
         addNewLine = true;
      }

      removeChoice = true;
   }

   if (addNewLine)
   {
      QMenu* newLineMenu = menu.addMenu("Add New Line");
      if (newLineMenu)
      {
         connect(newLineMenu, SIGNAL(triggered(QAction*)), this, SLOT(OnAddNewLine(QAction*)));

         std::vector<QString> lineTypes = DialogLineRegistry::GetInstance().GetLineTypes();
         int count = (int)lineTypes.size();
         for (int index = 0; index < count; ++index)
         {
            newLineMenu->addAction(lineTypes[index]);
         }
      }
   }
   
   if (addNewChoice)
   {
      QAction* choiceAction = menu.addAction("Add New Choice");
      if (choiceAction)
      {
         connect(choiceAction, SIGNAL(triggered()), this, SLOT(OnAddNewChoice()));
      }
   }

   if (removeLine)
   {
      QAction* removeAction = menu.addAction("Remove This Line");
      if (removeAction)
      {
         connect(removeAction, SIGNAL(triggered()), this, SLOT(OnRemoveLine()));
      }
   }

   if (removeChoice)
   {
      QAction* removeAction = menu.addAction("Remove This Choice");
      if (removeAction)
      {
         connect(removeAction, SIGNAL(triggered()), this, SLOT(OnRemoveLine()));
      }
   }

   menu.exec(event->globalPos());
}

////////////////////////////////////////////////////////////////////////////////
void DialogTreeWidget::RecurseStartDrag(QTreeWidgetItem* item, QTreeWidgetItem* moving)
{
   int count = item->childCount();
   for (int index = 0; index < count; ++index)
   {
      bool allowTarget = true;
      bool allowDrag = true;

      QTreeWidgetItem* child = item->child(index);

      DialogLineItem* sourceLine = dynamic_cast<DialogLineItem*>(moving);
      DialogLineItem* destLine = dynamic_cast<DialogLineItem*>(child);

      DialogChoiceItem* sourceChoice = dynamic_cast<DialogChoiceItem*>(moving);
      DialogChoiceItem* destChoice = dynamic_cast<DialogChoiceItem*>(child);

      // If you are moving a line item.
      if (sourceLine)
      {
         // Lines can only be moved to items with no children.
         if (child->childCount() > 0)
         {
            allowTarget = false;
         }

         // Lines can only be moved to other lines that are not choice lines.
         if (destLine && (!destLine->CanHaveSubLine() || destLine->IsChoice()))
         {
            allowTarget = false;
         }
      }
      // If you are moving a choice item.
      else if (sourceChoice)
      {
         // Choice items can not be moved to other choice items.
         if (destChoice)
         {
            allowTarget = false;
         }

         // Choice items can only be moved to choice line nodes.
         if (destLine && !destLine->IsChoice())
         {
            allowTarget = false;
         }

         // Choice items can only be moved to a choice line that isn't at it's choice limit.
         if (destLine && destLine->GetChoiceLimit() > -1 &&
            destLine->childCount() >= destLine->GetChoiceLimit())
         {
            allowTarget = false;
         }
      }

      // Make sure non-moveable choices stay non-moveable.
      if (destChoice && !destChoice->IsMoveable())
      {
         allowDrag = false;
      }

      Qt::ItemFlags flags = Qt::ItemIsEditable | Qt::ItemIsSelectable | Qt::ItemIsEnabled;

      if (allowTarget)
      {
         flags |= Qt::ItemIsDropEnabled;
      }

      if (allowDrag)
      {
         flags |= Qt::ItemIsDragEnabled;
      }

      child->setFlags(flags);

      RecurseStartDrag(child, moving);
   }
}

////////////////////////////////////////////////////////////////////////////////
void DialogTreeWidget::OnAddNewLine(QAction* action)
{
   if (!action)
   {
      return;
   }

   QString name = action->text();

   const DialogLineType* lineType = DialogLineRegistry::GetInstance().GetLineTypeForName(name);
   if (lineType)
   {
      QTreeWidgetItem* cur = currentItem();
      if (cur)
      {
         DialogLineItem* newLine = new DialogLineItem(lineType->GetName(), lineType, CreateIndex(), mEditor);
         cur->addChild(newLine);
         newLine->GetType()->Init(newLine, mEditor);
         cur->setExpanded(true);
      }
   }
}

////////////////////////////////////////////////////////////////////////////////
void DialogTreeWidget::OnAddNewChoice()
{
   QTreeWidgetItem* cur = currentItem();
   if (cur)
   {
      DialogChoiceItem* newChoice = new DialogChoiceItem(CreateIndex(), "", true, true);
      cur->addChild(newChoice);
      cur->setExpanded(true);
   }
}

////////////////////////////////////////////////////////////////////////////////
void DialogTreeWidget::OnRemoveLine()
{
   QTreeWidgetItem* cur = currentItem();
   if (cur)
   {
      QTreeWidgetItem* parent = cur->parent();
      if (parent)
      {
         parent->removeChild(cur);
         delete cur;

         UpdateLabels();
      }
   }
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
DialogTreeDelegate::DialogTreeDelegate(DialogTreeWidget* tree)
   : mTree(tree)
{
}

////////////////////////////////////////////////////////////////////////////////
QWidget* DialogTreeDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
   QTreeWidgetItem* item = mTree->GetItem(index);
   if (item)
   {
      DialogLineItem* lineItem = dynamic_cast<DialogLineItem*>(item);
      if (lineItem)
      {
         DialogLineType* lineType = lineItem->GetType();
         if (lineType)
         {
            return lineType->CreateInlineEditor(parent, mTree->GetEditor());
         }
      }
      else
      {
         DialogChoiceItem* choiceItem = dynamic_cast<DialogChoiceItem*>(item);
         if (choiceItem && !choiceItem->IsNameable())
         {
            return NULL;
         }
      }
   }

   return QItemDelegate::createEditor(parent, option, index);
}

////////////////////////////////////////////////////////////////////////////////
void DialogTreeDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
   QTreeWidgetItem* item = mTree->GetItem(index);
   if (item)
   {
      DialogLineItem* lineItem = dynamic_cast<DialogLineItem*>(item);
      if (lineItem)
      {
         DialogLineType* lineType = lineItem->GetType();
         if (lineType)
         {
            lineType->RefreshInlineEditor(editor);
            return;
         }
      }

      DialogChoiceItem* choiceItem = dynamic_cast<DialogChoiceItem*>(item);
      if (choiceItem)
      {
         QLineEdit* lineEdit = dynamic_cast<QLineEdit*>(editor);
         if (lineEdit)
         {
            lineEdit->setText(choiceItem->GetLabel());
            return;
         }
      }
   }

   QItemDelegate::setEditorData(editor, index);
}

////////////////////////////////////////////////////////////////////////////////
void DialogTreeDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
   QTreeWidgetItem* item = mTree->GetItem(index);
   if (item)
   {
      DialogLineItem* lineItem = dynamic_cast<DialogLineItem*>(item);
      if (lineItem)
      {
         DialogLineType* lineType = lineItem->GetType();
         if (lineType)
         {
            lineType->RefreshInlineData(editor);
            UpdateLabels();
            return;
         }
      }

      DialogChoiceItem* choiceItem = dynamic_cast<DialogChoiceItem*>(item);
      if (choiceItem)
      {
         QLineEdit* lineEdit = dynamic_cast<QLineEdit*>(editor);
         if (lineEdit)
         {
            choiceItem->SetLabel(lineEdit->text());
            UpdateLabels();
            return;
         }
      }
   }

   QItemDelegate::setModelData(editor, model, index);
}

////////////////////////////////////////////////////////////////////////////////
void DialogTreeDelegate::UpdateLabels() const
{
   int count = mTree->topLevelItemCount();
   for (int index = 0; index < count; ++index)
   {
      QTreeWidgetItem* child = mTree->topLevelItem(index);
      RecurseUpdateLabels(child);
   }

   mTree->resizeColumnToContents(0);
}

////////////////////////////////////////////////////////////////////////////////
void DialogTreeDelegate::RecurseUpdateLabels(QTreeWidgetItem* item) const
{
   if (!item)
   {
      return;
   }

   int count = item->childCount();
   for (int index = 0; index < count; ++index)
   {
      QTreeWidgetItem* child = item->child(index);
      DialogLineItem* line = dynamic_cast<DialogLineItem*>(child);
      if (line)
      {
         line->UpdateLabel();
      }
      else
      {
         DialogChoiceItem* choice = dynamic_cast<DialogChoiceItem*>(child);
         if (choice)
         {
            choice->UpdateLabel();
         }
      }

      RecurseUpdateLabels(child);
   }
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
DialogLineItem::DialogLineItem(const QString& name, const DialogLineType* type, int index, DirectorDialogEditorPlugin* editor)
   : mType(NULL)
   , mTypeName(name)
   , mTree(NULL)
   , mIndex(index)
{
   setFlags(Qt::ItemIsEditable | Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled);

   if (editor)
   {
      mTree = editor->GetTree();
   }

   if (type)
   {
      mType = type->Create();

      QColor color = type->GetColor();
      setBackgroundColor(0, color);
   }

   UpdateLabel();
}

////////////////////////////////////////////////////////////////////////////////
DialogLineItem::~DialogLineItem()
{
   if (mType)
   {
      delete mType;
   }
}

////////////////////////////////////////////////////////////////////////////////
void DialogLineItem::UpdateLabel()
{
   if (mType)
   {
      setText(0, QString("[") + QString::number(mIndex) + ":" + mTypeName + "] " +
         mType->GetDisplayName(mTree->GetEditor()));
   }
}

////////////////////////////////////////////////////////////////////////////////
bool DialogLineItem::CanHaveSubLine() const
{
   if (mType)
   {
      return mType->CanHaveSubLine();
   }

   return true;
}

////////////////////////////////////////////////////////////////////////////////
bool DialogLineItem::IsChoice() const
{
   if (mType)
   {
      return mType->IsChoice();
   }

   return false;
}

////////////////////////////////////////////////////////////////////////////////
int DialogLineItem::GetChoiceLimit() const
{
   if (mType)
   {
      return mType->GetChoiceLimit();
   }

   return -1;
}

////////////////////////////////////////////////////////////////////////////////
DialogLineItem* DialogLineItem::GetChildLine() const
{
   if (childCount() > 0)
   {
      return dynamic_cast<DialogLineItem*>(child(0));
   }

   return NULL;
}

////////////////////////////////////////////////////////////////////////////////
DialogChoiceItem* DialogLineItem::GetChildChoice(int index) const
{
   if (index < childCount())
   {
      return dynamic_cast<DialogChoiceItem*>(child(index));
   }

   return NULL;
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
DialogChoiceItem::DialogChoiceItem(int index, const QString& label, bool moveable, bool nameable)
   : mLabel(label)
   , mIndex(index)
   , mMoveable(moveable)
   , mNameable(nameable)
{
   QColor color = Qt::yellow;
   color.setAlphaF(0.25f);
   setBackgroundColor(0, color);

   Qt::ItemFlags flags = Qt::ItemIsEditable | Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsDropEnabled;
   if (moveable)
   {
      flags |= Qt::ItemIsDragEnabled;
   }

   setFlags(flags);
   UpdateLabel();
}

////////////////////////////////////////////////////////////////////////////////
DialogChoiceItem::~DialogChoiceItem()
{
}

////////////////////////////////////////////////////////////////////////////////
void DialogChoiceItem::UpdateLabel()
{
   setText(0, QString("[") + QString::number(mIndex) + ":Choice] " + mLabel);
}

////////////////////////////////////////////////////////////////////////////////
void DialogChoiceItem::SetLabel(const QString& label)
{
   mLabel = label;
}

////////////////////////////////////////////////////////////////////////////////
const QString& DialogChoiceItem::GetLabel() const
{
   return mLabel;
}

////////////////////////////////////////////////////////////////////////////////
DialogLineItem* DialogChoiceItem::GetChildLine() const
{
   if (childCount() > 0)
   {
      return dynamic_cast<DialogLineItem*>(child(0));
   }

   return NULL;
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
DialogRootItem::DialogRootItem(const QString& name, const DialogLineType* type, bool allowChildren)
   : DialogLineItem(name, type, 0, NULL)
   , mAllowChildren(allowChildren)
{
   QColor color = Qt::red;
   color.setAlphaF(0.15f);
   setBackgroundColor(0, color);

   // Root items can never move.
   setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);

   UpdateLabel();
}

////////////////////////////////////////////////////////////////////////////////
DialogRootItem::~DialogRootItem()
{
}

////////////////////////////////////////////////////////////////////////////////
void DialogRootItem::UpdateLabel()
{
   setText(0, GetTypeName());
}

////////////////////////////////////////////////////////////////////////////////
bool DialogRootItem::CanHaveSubLine() const
{
   return mAllowChildren;
}

////////////////////////////////////////////////////////////////////////////////

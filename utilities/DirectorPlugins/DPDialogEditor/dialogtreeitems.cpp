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
void DialogTreeWidget::Reset()
{
   mIndex = 1;
}

////////////////////////////////////////////////////////////////////////////////
QTreeWidgetItem* DialogTreeWidget::GetItem(const QModelIndex& index) const
{
   return itemFromIndex(index);
}

////////////////////////////////////////////////////////////////////////////////
QStringList DialogTreeWidget::GetSpeakerList() const
{
   if (mEditor)
   {
      return mEditor->GetSpeakerList();
   }

   return QStringList();
}

////////////////////////////////////////////////////////////////////////////////
int DialogTreeWidget::CreateIndex()
{
   return mIndex++;
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
void DialogTreeWidget::OnSpeakerRemoved(const QString& speaker)
{
   int count = topLevelItemCount();
   for (int index = 0; index < count; ++index)
   {
      QTreeWidgetItem* item = topLevelItem(index);
      RecurseSpeakerRemoved(item, speaker);
   }
}

////////////////////////////////////////////////////////////////////////////////
void DialogTreeWidget::OnSpeakerRenamed(const QString& oldName, const QString& newName)
{
   int count = topLevelItemCount();
   for (int index = 0; index < count; ++index)
   {
      QTreeWidgetItem* item = topLevelItem(index);
      RecurseSpeakerRenamed(item, oldName, newName);
   }
}

////////////////////////////////////////////////////////////////////////////////
void DialogTreeWidget::startDrag(Qt::DropActions supportedActions)
{
   QTreeWidgetItem* movingItem = currentItem();
   RecurseStartDrag(topLevelItem(0), movingItem);

   QTreeWidget::startDrag(supportedActions);
}

////////////////////////////////////////////////////////////////////////////////
void DialogTreeWidget::dropEvent(QDropEvent* event)
{
   DialogChoiceItem* item = dynamic_cast<DialogChoiceItem*>(currentItem());

   // Remember the old position of the item.
   DialogLineItem* oldParent = NULL;
   int             oldIndex = -1;

   if (item)
   {
      oldParent = dynamic_cast<DialogLineItem*>(item->parent());
      if (oldParent)
      {
         oldIndex = oldParent->indexOfChild(item);
      }
   }

   // Perform the drop event.
   QTreeWidget::dropEvent(event);

   // Retrieve the new position of the item.
   DialogLineItem* newParent = NULL;
   int             newIndex = -1;

   if (item)
   {
      newParent = dynamic_cast<DialogLineItem*>(item->parent());
      if (newParent)
      {
         newIndex = newParent->indexOfChild(item);
      }
   }

   // Notify both the old and new parents that the child item has moved.
   if (oldParent != newParent)
   {
      if (oldParent && oldParent->GetType())
      {
         oldParent->GetType()->OnChildChoiceRemoved(item, oldIndex);
      }

      if (newParent && newParent->GetType())
      {
         newParent->GetType()->OnChildChoiceAdded(item, newIndex);
      }
   }
   else if (newParent && newParent->GetType() &&
      oldIndex > -1 && newIndex > -1 &&
      oldIndex != newIndex)
   {
      newParent->GetType()->OnChildChoiceMoved(item, oldIndex, newIndex);
   }
}

////////////////////////////////////////////////////////////////////////////////
void DialogTreeWidget::mousePressEvent(QMouseEvent* event)
{
   QTreeWidgetItem* clickedItem = itemAt(event->pos());
   if (clickedItem)
   {
      QTreeWidget::mousePressEvent(event);
   }
   else
   {
      QTreeWidgetItem* current = currentItem();
      clearSelection();
      mEditor->OnCurrentTreeItemChanged(NULL, current);
      event->accept();
   }
}

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

   event->ignore();
   if (!menu.actions().empty())
   {
      menu.exec(event->globalPos());
      event->accept();
   }
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

      DialogLineItem* curLine = dynamic_cast<DialogLineItem*>(cur);
      if (curLine && curLine->GetType())
      {
         curLine->GetType()->OnChildChoiceAdded(newChoice, cur->childCount()-1);
      }
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
         int index = parent->indexOfChild(cur);
         parent->removeChild(cur);

         DialogLineItem* parentLine  = dynamic_cast<DialogLineItem*>(parent);
         DialogChoiceItem* curChoice = dynamic_cast<DialogChoiceItem*>(cur);

         if (parentLine && curChoice && parentLine->GetType())
         {
            parentLine->GetType()->OnChildChoiceRemoved(curChoice, index);
         }

         delete cur;

         UpdateLabels();
      }
   }
}

////////////////////////////////////////////////////////////////////////////////
void DialogTreeWidget::RecurseSpeakerRemoved(QTreeWidgetItem* item, const QString& speaker)
{
   if (!item)
   {
      return;
   }

   DialogLineItem* lineItem = dynamic_cast<DialogLineItem*>(item);
   if (lineItem && lineItem->GetType())
   {
      lineItem->GetType()->OnSpeakerRemoved(speaker);
   }

   int count = item->childCount();
   for (int index = 0; index < count; ++index)
   {
      QTreeWidgetItem* child = item->child(index);
      RecurseSpeakerRemoved(child, speaker);
   }
}

////////////////////////////////////////////////////////////////////////////////
void DialogTreeWidget::RecurseSpeakerRenamed(QTreeWidgetItem* item, const QString& oldName, const QString& newName)
{
   if (!item)
   {
      return;
   }

   DialogLineItem* lineItem = dynamic_cast<DialogLineItem*>(item);
   if (lineItem && lineItem->GetType())
   {
      lineItem->GetType()->OnSpeakerRenamed(oldName, newName);
   }

   int count = item->childCount();
   for (int index = 0; index < count; ++index)
   {
      QTreeWidgetItem* child = item->child(index);
      RecurseSpeakerRenamed(child, oldName, newName);
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
   if (mType && mTree)
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
DialogRootItem::DialogRootItem(const QString& name, const DialogLineType* type, bool allowChildren, DirectorDialogEditorPlugin* editor)
   : DialogLineItem(name, type, 0, editor)
   , mAllowChildren(allowChildren)
{
   QColor color = Qt::red;
   color.setAlphaF(0.15f);
   setBackgroundColor(0, color);

   // Root items can never move.
   Qt::ItemFlags flags = Qt::ItemIsSelectable | Qt::ItemIsEnabled;
   if (type != NULL)
   {
      flags |= Qt::ItemIsEditable;
   }
   setFlags(flags);

   UpdateLabel();
}

////////////////////////////////////////////////////////////////////////////////
DialogRootItem::~DialogRootItem()
{
}

////////////////////////////////////////////////////////////////////////////////
void DialogRootItem::UpdateLabel()
{
   if (GetType() != NULL && GetTree() != NULL)
   {
      setText(0, QString("[") + GetTypeName() + "] " +
         GetType()->GetDisplayName(GetTree()->GetEditor()));
   }
   else
   {
      setText(0, GetTypeName());
   }
}

////////////////////////////////////////////////////////////////////////////////
bool DialogRootItem::CanHaveSubLine() const
{
   return mAllowChildren;
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

static const QString ADD_SPEAKER_TEXT("<Add Speaker...>");

////////////////////////////////////////////////////////////////////////////////
DialogSpeakerList::DialogSpeakerList(QWidget* parent)
   : QListWidget(parent)
   , mTree(NULL)
{
   connect(this, SIGNAL(currentItemChanged(QListWidgetItem*, QListWidgetItem*)), this, SLOT(OnSelectionChanged(QListWidgetItem*, QListWidgetItem*)));
   connect(this, SIGNAL(itemChanged(QListWidgetItem*)), this, SLOT(OnItemChanged(QListWidgetItem*)));
}

////////////////////////////////////////////////////////////////////////////////
DialogSpeakerList::~DialogSpeakerList()
{
}

////////////////////////////////////////////////////////////////////////////////
void DialogSpeakerList::Reset()
{
   clear();

   QListWidgetItem* newItem = new QListWidgetItem();
   if (newItem)
   {
      newItem->setText(ADD_SPEAKER_TEXT);
      newItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsEditable);
      addItem(newItem);
   }
}

////////////////////////////////////////////////////////////////////////////////
void DialogSpeakerList::SetTree(DialogTreeWidget* tree)
{
   mTree = tree;
}

////////////////////////////////////////////////////////////////////////////////
QStringList DialogSpeakerList::GetSpeakerList() const
{
   QStringList speakerList;

   for (int index = 0; index < count() - 1; ++index)
   {
      QListWidgetItem* speakerItem = item(index);
      if (speakerItem)
      {
         speakerList.append(speakerItem->text());
      }
   }

   return speakerList;
}

////////////////////////////////////////////////////////////////////////////////
void DialogSpeakerList::AddSpeaker(const QString& speaker)
{
   QListWidgetItem* speakerItem = new QListWidgetItem();
   if (speakerItem)
   {
      speakerItem->setText(speaker);
      speakerItem->setFlags(speakerItem->flags() | Qt::ItemIsEditable);
      insertItem(count() - 1, speakerItem);
   }
}

////////////////////////////////////////////////////////////////////////////////
void DialogSpeakerList::startDrag(Qt::DropActions supportedActions)
{
   QListWidget::startDrag(supportedActions);
}

////////////////////////////////////////////////////////////////////////////////
void DialogSpeakerList::dropEvent(QDropEvent* event)
{
   QListWidget::dropEvent(event);

   for (int index = 0; index < count() - 1; ++index)
   {
      QListWidgetItem* current = item(index);
      if (current)
      {
         current->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsEditable | Qt::ItemIsDragEnabled);
      }
   }

   QListWidgetItem* newItem = new QListWidgetItem();
   if (newItem)
   {
      newItem->setText(ADD_SPEAKER_TEXT);
      newItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsEditable);
      addItem(newItem);
   }
}

////////////////////////////////////////////////////////////////////////////////
void DialogSpeakerList::dragEnterEvent(QDragEnterEvent* event)
{
   QListWidgetItem* lastItem = item(count() - 1);
   if (lastItem)
   {
      delete lastItem;
   }

   QListWidget::dragEnterEvent(event);
}

////////////////////////////////////////////////////////////////////////////////
void DialogSpeakerList::dragLeaveEvent(QDragLeaveEvent* event)
{
   QListWidgetItem* newItem = new QListWidgetItem();
   if (newItem)
   {
      newItem->setText(ADD_SPEAKER_TEXT);
      newItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsEditable);
      addItem(newItem);
   }

   QListWidget::dragLeaveEvent(event);
}

////////////////////////////////////////////////////////////////////////////////
void DialogSpeakerList::mouseDoubleClickEvent(QMouseEvent *event)
{
   event->ignore();

   QListWidgetItem* speakerItem = itemAt(event->pos());
   if (!speakerItem)
   {
      speakerItem = item(count() - 1);
      if (speakerItem)
      {
         clearSelection();
         speakerItem->setSelected(true);
         editItem(speakerItem);
      }

      event->accept();
   }
   else
   {
      QListWidget::mouseDoubleClickEvent(event);
   }
}

////////////////////////////////////////////////////////////////////////////////
void DialogSpeakerList::contextMenuEvent(QContextMenuEvent* event)
{
   event->ignore();

   QMenu menu;

   QListWidgetItem* speakerItem = itemAt(event->pos());
   if (speakerItem && speakerItem != item(count() - 1))
   {
      QAction* removeAction = menu.addAction("Remove Speaker");
      connect(removeAction, SIGNAL(triggered()), this, SLOT(OnRemoveSpeaker()));
   }

   if (!menu.actions().empty())
   {
      menu.exec(event->globalPos());
      event->accept();
   }
}

////////////////////////////////////////////////////////////////////////////////
void DialogSpeakerList::OnSelectionChanged(QListWidgetItem* current, QListWidgetItem* previous)
{
   if (current)
   {
      mOldName = current->text();
   }
}

////////////////////////////////////////////////////////////////////////////////
void DialogSpeakerList::OnItemChanged(QListWidgetItem* changedItem)
{
   if (!changedItem)
   {
      return;
   }

   QString newName = changedItem->text();
   if (newName != mOldName && count() > 0)
   {
      QListWidgetItem* lastItem = item(count() - 1);
      if (lastItem == changedItem)
      {
         QListWidgetItem* newItem = new QListWidgetItem();
         if (newItem)
         {
            newItem->setText(ADD_SPEAKER_TEXT);
            newItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsEditable);
            addItem(newItem);
         }
      }
      else
      {
         mTree->OnSpeakerRenamed(mOldName, newName);
         mOldName = newName;
      }

      changedItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsEditable | Qt::ItemIsDragEnabled);
   }
}

////////////////////////////////////////////////////////////////////////////////
void DialogSpeakerList::OnRemoveSpeaker()
{
   QListWidgetItem* item = currentItem();
   if (item)
   {
      mTree->OnSpeakerRemoved(mOldName);
      delete item;
   }
}

////////////////////////////////////////////////////////////////////////////////

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

////////////////////////////////////////////////////////////////////////////////
// INCLUDE DIRECTIVES
////////////////////////////////////////////////////////////////////////////////
#include "AnimationControlDock.h"
// QT
#include <QtGui/QTableWidgetItem>



using namespace dtAnim;



/////////////////////////////////////////////////////////////////////////////////
// ANIMATION PARAMS STRUCT CODE
/////////////////////////////////////////////////////////////////////////////////
AnimParams::AnimParams()
   : mBeginFrame(0.0)
   , mEndFrame(0.0)
   , mBeginFrameOffset(0.0)
   , mSpeed(1.0)
   , mLoopLimit(0)
   , mPlayMode(osg::AnimationPath::NO_LOOPING)
{}



/////////////////////////////////////////////////////////////////////////////////
// ANIMATION PARAMS TABLE ROW CLASS CODE
/////////////////////////////////////////////////////////////////////////////////
AnimationParamsTableRow::AnimationParamsTableRow()
{
}

void AnimationParamsTableRow::InsertValuesToTable(QTableWidget& table, int rowIndex)
{
   for (int col = 0; col < MAX_COLUMNS; ++col)
   {
      table.setItem(rowIndex, col, new QTableWidgetItem);
   }

   SetRowValues(table, rowIndex);
}

void AnimationParamsTableRow::SetRowValues(QTableWidget& table, int rowIndex)
{
   QString curStr;
   QTableWidgetItem* curItem = nullptr;
   for (int col = 0; col < MAX_COLUMNS; ++col)
   {
      curItem = table.item(rowIndex, col);

      switch (col)
      {
      case NAME:
         curStr = mParams.mName.c_str();
         curItem->setText(curStr);
         break;
      case BEGIN: 
         curStr = QString::number(mParams.mBeginFrame);
         curItem->setText(curStr);
         break;
      case END: 
         curStr = QString::number(mParams.mEndFrame);
         curItem->setText(curStr);
         break;
      case OFFSET: 
         curStr = QString::number(mParams.mBeginFrameOffset);
         curItem->setText(curStr);
         break;
      case SPEED: 
         curStr = QString::number(mParams.mSpeed);
         curItem->setText(curStr);
         break;
      case LIMIT:
         curStr = QString::number(mParams.mLoopLimit);
         curItem->setText(curStr);
         break;
      case PLAYMODE:
         curStr = "ONCE";

         if (mParams.mPlayMode == osg::AnimationPath::LOOP)
         {
            curStr = "LOOP";
         }
         else if (mParams.mPlayMode == osg::AnimationPath::SWING)
         {
            curStr = "SWING";
         }

         curItem->setText(curStr);
         break;
      default:
         break;
      }
   }

   table.update();
}

AnimParams& AnimationParamsTableRow::GetParams()
{
   return mParams;
}

const AnimParams& AnimationParamsTableRow::GetParams() const
{
   return mParams;
}



/////////////////////////////////////////////////////////////////////////////////
// ANIMATION CONTROL DOCK CLASS CODE
/////////////////////////////////////////////////////////////////////////////////
AnimationControlDock::AnimationControlDock(QWidget* parent)
   : BaseClass(parent)
   , mPaused(false)
{
   mUI.setupUi(this);

   CreateConnections();

   UpdateUI();
}

void AnimationControlDock::CreateConnections()
{
   // BUTTONS
   connect(mUI.mButtonPlay, SIGNAL(pressed()), this, SLOT(OnPlay()));
   connect(mUI.mButtonPause, SIGNAL(pressed()), this, SLOT(OnPause()));
   connect(mUI.mButtonReset, SIGNAL(pressed()), this, SLOT(OnReset()));
   connect(mUI.mButtonAdd, SIGNAL(pressed()), this, SLOT(OnAdd()));
   connect(mUI.mButtonRemove, SIGNAL(pressed()), this, SLOT(OnRemove()));

   // RADIO BUTTONS
   connect(mUI.mRadioPlayMode_Once, SIGNAL(clicked(bool)), this, SLOT(OnPlayModeChanged(bool)));
   connect(mUI.mRadioPlayMode_Loop, SIGNAL(clicked(bool)), this, SLOT(OnPlayModeChanged(bool)));
   connect(mUI.mRadioPlayMode_Swing, SIGNAL(clicked(bool)), this, SLOT(OnPlayModeChanged(bool)));

   // SPINNERS
   connect(mUI.mAnimSpeed, SIGNAL(editingFinished()), this, SLOT(OnSpeedChanged()));
   connect(mUI.mAnimFrameBeginOffset, SIGNAL(editingFinished()), this, SLOT(OnBeginFrameOffsetChanged()));
   connect(mUI.mAnimLoopLimit, SIGNAL(editingFinished()), this, SLOT(OnLoopLimitChanged()));

   // TABLE ITEMS
   connect(mUI.mTableAnimations, SIGNAL(cellClicked(int, int)), this, SLOT(OnItemSelect(int, int)));
}

void AnimationControlDock::ResetUI()
{
   mUI.mOutputAnimClipName->setText(tr(""));
   mUI.mAnimName->setText(tr(""));
   mUI.mOutputObjectCount->setText(tr("0"));
   mUI.mOutputFrameTotal->setText(tr("0"));
   mUI.mAnimFrameBegin->setValue(0.0);
   mUI.mAnimFrameEnd->setValue(0.0);
   mUI.mAnimFrameBeginOffset->setValue(0.0);
   mUI.mAnimSpeed->setValue(1.0);
   mUI.mAnimLoopLimit->setValue(0);
   mUI.mTableAnimations->clear();

   mAnimParams.clear();
}

void AnimationControlDock::UpdateUI()
{
   osg::Node* node = mObject.valid() ? mObject->GetOSGNode() : nullptr;
   if (node != nullptr)
   {
      AnimCallbackVisitor visitor;
      node->accept(visitor);

      int maxFrames = 0;
      double timeEarliest = -1.0;
      double timeLatest = 0.0;
      osg::AnimationPath* curPath = nullptr;
      typedef AnimCallbackVisitor::AnimCallbackVector AnimCallbackVector;
      AnimCallbackVector::iterator iter = visitor.GetAnimCallbacks().begin();
      AnimCallbackVector::iterator iterEnd = visitor.GetAnimCallbacks().end();
      for (; iter != iterEnd; ++iter)
      {
         curPath = iter->get()->getAnimationPath();
         double firstTime = curPath->getFirstTime();
         double lastTime = curPath->getLastTime();
         double timePeriod = lastTime - firstTime;

         if (timeEarliest == -1.0f || firstTime < timeEarliest)
            timeEarliest = firstTime;
         if (lastTime > timeLatest)
            timeLatest = lastTime;

         if (maxFrames < (int)(curPath->getTimeControlPointMap().size()))
         {
            maxFrames = (int)(curPath->getTimeControlPointMap().size());
         }
      }

      QString str;
      str = str.number(visitor.GetAnimCallbacks().size());
      mUI.mOutputObjectCount->setText(str);
      
      str.clear();
      str = str.number(maxFrames);
      mUI.mOutputFrameTotal->setText(str);

      str.clear();
      double period = timeLatest - timeEarliest;
      str = str.number(period);
      mUI.mOutputTotalSeconds->setText(str);
   }
}

void AnimationControlDock::SetPlayMode(int playMode) const
{
   osg::AnimationPath::LoopMode loopMode = ConvertToPlayMode(playMode);

   if (loopMode != GetPlayMode())
   {
      if (loopMode == osg::AnimationPath::LOOP)
      {
         mUI.mRadioPlayMode_Loop->setChecked(true);
      }
      else if (loopMode == osg::AnimationPath::SWING)
      {
         mUI.mRadioPlayMode_Swing->setChecked(true);
      }
      else
      {
         mUI.mRadioPlayMode_Once->setChecked(true);
      }
   }
}

osg::AnimationPath::LoopMode AnimationControlDock::GetPlayMode() const
{
   osg::AnimationPath::LoopMode playMode = osg::AnimationPath::NO_LOOPING; // ONCE
   if (mUI.mRadioPlayMode_Loop->isChecked())
   {
      playMode = osg::AnimationPath::LOOP;
   }
   else if (mUI.mRadioPlayMode_Swing->isChecked())
   {
      playMode = osg::AnimationPath::SWING;
   }

   return playMode;
}

osg::AnimationPath::LoopMode AnimationControlDock::ConvertToPlayMode(int playMode) const
{
   osg::AnimationPath::LoopMode loopMode = osg::AnimationPath::NO_LOOPING; // ONCE
   switch (playMode)
   {
   case osg::AnimationPath::LOOP:
      loopMode = osg::AnimationPath::LOOP;
      break;
   case osg::AnimationPath::SWING:
      loopMode = osg::AnimationPath::SWING;
      break;
   default:
      break;
   }
   return loopMode;
}

void AnimationControlDock::ApplyAnimationParameters(dtAnim::AnimCallbackVisitor& visitor)
{
   AnimClipPath* curPath = nullptr;
   typedef AnimCallbackVisitor::AnimCallbackVector AnimCallbackVector;
   AnimCallbackVector::iterator iter = visitor.GetAnimCallbacks().begin();
   AnimCallbackVector::iterator iterEnd = visitor.GetAnimCallbacks().end();
   for (; iter != iterEnd; ++iter)
   {
      curPath = dynamic_cast<AnimClipPath*>(iter->get()->getAnimationPath());
      curPath->setBeginTime(mUI.mAnimFrameBegin->value());
      curPath->setEndTime(mUI.mAnimFrameEnd->value());
      curPath->setTimeOffset(mUI.mAnimFrameBeginOffset->value());
      curPath->setLoopLimit(mUI.mAnimLoopLimit->value());
      curPath->setLoopMode(GetPlayMode());
   }
   visitor.SetSpeed(mUI.mAnimSpeed->value());
}

osg::Node* AnimationControlDock::GetRootNode()
{
   return mObject.valid() ? mObject->GetOSGNode() : nullptr;
}

void AnimationControlDock::OnPause()
{
   InternalPause();
}

void AnimationControlDock::InternalPause()
{
   osg::Node* node = GetRootNode();
   if (node != nullptr)
   {
      AnimCallbackVisitor visitor;
      node->accept(visitor);

      visitor.SetPaused(true);

      mPaused = true;
   }
}

void AnimationControlDock::OnPlay()
{
    InternalPlay(true);
}

void AnimationControlDock::InternalPlay(bool reset)
{
   osg::Node* node = GetRootNode();
   if (node != nullptr)
   {
      AnimCallbackVisitor visitor;
      node->accept(visitor);

      visitor.SetPaused(false);
      mPaused = false;

      if (reset)
      {
          visitor.ResetCallbacks();
      }

      ApplyAnimationParameters(visitor);
   }
}

void AnimationControlDock::OnReset()
{
   osg::Node* node = GetRootNode();
   if (node != nullptr)
   {
      AnimCallbackVisitor visitor;
      node->accept(visitor);

      visitor.ResetCallbacks();
   }
}

std::string AnimationControlDock::GetRowID(int rowIndex) const
{
   std::string name;
   QTableWidgetItem* item = mUI.mTableAnimations->item(rowIndex, AnimationParamsTableRow::NAME);
   if (item != nullptr)
   {
      name = item->text().toStdString();
   }

   return name;
}

int AnimationControlDock::GetRowIndex(const std::string& animName) const
{
   int rowIndex = -1;
   
   QTableWidgetItem* curItem = nullptr;
   int numRows = mUI.mTableAnimations->rowCount();
   for (int i = 0; i < numRows; ++i)
   {
      curItem = mUI.mTableAnimations->item(i, AnimationParamsTableRow::NAME);
      if (curItem->text().toStdString() == animName)
      {
         rowIndex = i;
         break;
      }
   }

   return rowIndex;
}

void AnimationControlDock::OnAdd()
{
   QString animName(mUI.mAnimName->text().trimmed());
   bool valid = ! animName.isEmpty()
      && mUI.mAnimFrameBegin->value() >= 0.0
      && mUI.mAnimFrameBegin->value() < mUI.mAnimFrameEnd->value();

   if (valid)
   {
      // Determine if an animation has already be specified.
      AnimParamRows::iterator foundIter = mAnimParams.find(animName.toStdString());
      bool createRow = foundIter == mAnimParams.end();

      // Gather the parameters from the UI.
      AnimParams params;
      UpdateAnimParamsFromUI(params);

      // Create or modify the parameters of the specified animation.
      dtCore::RefPtr<AnimationParamsTableRow> row;
      row = createRow ? new AnimationParamsTableRow() : foundIter->second.get();
      row->GetParams() = params;

      if (createRow)
      {
         int rowIndex = mUI.mTableAnimations->rowCount();
         mUI.mTableAnimations->insertRow(rowIndex);
         row->InsertValuesToTable(*mUI.mTableAnimations, rowIndex);

         mAnimParams.insert(std::make_pair(params.mName, row));
      }
      else
      {
         int rowIndex = GetRowIndex(animName.toStdString());
         if (rowIndex >= 0)
         {
            row->SetRowValues(*mUI.mTableAnimations, rowIndex);
         }
      }
   }
}

void AnimationControlDock::OnRemove()
{
   InternalPause();

   typedef QList<QTableWidgetItem *> ItemList;
   ItemList items = mUI.mTableAnimations->selectedItems();

   std::set<int> rows;
   
   QTableWidgetItem* curItem = nullptr;
   while (!items.empty())
   {
      curItem = items.front();
      int rowIndex = mUI.mTableAnimations->row(curItem);

      if (rows.find(rowIndex) == rows.end())
      {
         rows.insert(rowIndex);
      }

      // Clear the item from the list of removable items.
      items.removeFirst();
   }

   while(!rows.empty())
   {
      // Remove the last selected row, then remove upward.
      int rowIndex = *rows.rbegin();

      if (rowIndex >= 0)
      {
         // Remove the object containg the parameter data.
         std::string rowID = GetRowID(rowIndex);
         AnimParamRows::iterator foundIter = mAnimParams.find(rowID);
         if (foundIter != mAnimParams.end())
         {
            mAnimParams.erase(foundIter);
         }

         // Remove the row from the table UI.
         mUI.mTableAnimations->removeRow(rowIndex);
      }

      // Clear the index from the remove list so that subsequent
      // rows in the table are not accidentally removed.
      rows.erase(rowIndex);
   }
}

void AnimationControlDock::OnSpeedChanged()
{
   osg::Node* node = GetRootNode();
   if (node != nullptr && mUI.mAnimSpeed->value() != 0.0)
   {
      AnimCallbackVisitor visitor;
      node->accept(visitor);

      visitor.SetSpeed(mUI.mAnimSpeed->value());
   }
}

void AnimationControlDock::OnBeginFrameOffsetChanged()
{
   osg::Node* node = GetRootNode();
   if (node != nullptr && mUI.mAnimSpeed->value() != 0.0)
   {
      AnimCallbackVisitor visitor;
      node->accept(visitor);

      visitor.SetTimeOffset(mUI.mAnimFrameBeginOffset->value());
   }
}

void AnimationControlDock::OnLoopLimitChanged()
{
   osg::Node* node = GetRootNode();
   if (node != nullptr && mUI.mAnimSpeed->value() != 0.0)
   {
      AnimCallbackVisitor visitor;
      node->accept(visitor);

      visitor.SetLoopLimit(mUI.mAnimLoopLimit->value());
   }
}

void AnimationControlDock::OnPlayModeChanged(bool checked)
{
    osg::Node* node = GetRootNode();
    AnimCallbackVisitor visitor;
    if (node != nullptr && mUI.mAnimSpeed->value() != 0.0)
    {
        node->accept(visitor);
    }
    
    osg::AnimationPath::LoopMode curPlayMode = GetPlayMode();

    visitor.SetPlayMode(curPlayMode);
}

void AnimationControlDock::OnItemSelect(int row, int column)
{
   std::string name = GetRowID(row);
   AnimParamRows::iterator foundIter = mAnimParams.find(name);

   if (foundIter != mAnimParams.end())
   {
      AnimationParamsTableRow* rowObject = foundIter->second.get();

      InternalPause();

      AnimParams& params = rowObject->GetParams();

      UpdateUIFromAnimParams(params);

      if (mUI.mChkPlayOnClick->checkState() == Qt::Checked)
      {
         InternalPlay(true);
      }
   }
}

void AnimationControlDock::OnGeometryLoaded(dtCore::Object* object)
{
   ResetUI();

   mObject = object;

   bool objectValid = mObject.valid() && mObject->GetOSGNode() != nullptr;
   if (objectValid)
   {
      osg::Node* node = mObject->GetOSGNode();

      // Swap out animation paths with subclassed version
      // that allows for better interpolation control, such
      // as for playing only a segment of  animation.
      AnimCallbackVisitor visitor;
      node->accept(visitor);

      osg::AnimationPath* curPath = nullptr;
      typedef AnimCallbackVisitor::AnimCallbackVector AnimCallbackVector;
      AnimCallbackVector::iterator iter = visitor.GetAnimCallbacks().begin();
      AnimCallbackVector::iterator iterEnd = visitor.GetAnimCallbacks().end();
      for (; iter != iterEnd; ++iter)
      {
         curPath = iter->get()->getAnimationPath();
         osg::ref_ptr<osg::AnimationPath> newPath = new AnimClipPath(*curPath);
         iter->get()->setAnimationPath(newPath.get());
      }
   }

   mPaused = false;

   UpdateUI();

   if (objectValid)
   {
       mUI.mAnimFrameEnd->setValue(mUI.mOutputTotalSeconds->text().toDouble());
   }
}

void AnimationControlDock::UpdateAnimParamsFromUI(AnimParams& outParams)
{
   outParams.mName = mUI.mAnimName->text().toStdString();
   outParams.mBeginFrame = mUI.mAnimFrameBegin->value();
   outParams.mEndFrame = mUI.mAnimFrameEnd->value();
   outParams.mBeginFrameOffset = mUI.mAnimFrameBeginOffset->value();
   outParams.mSpeed = mUI.mAnimSpeed->value();
   outParams.mLoopLimit = mUI.mAnimLoopLimit->value();
   outParams.mPlayMode = GetPlayMode();

   outParams.mPlayMode = osg::AnimationPath::NO_LOOPING; // ONCE
   if(mUI.mRadioPlayMode_Loop->isChecked())
   {
      outParams.mPlayMode = osg::AnimationPath::LOOP;
   }
   else if(mUI.mRadioPlayMode_Swing->isChecked())
   {
      outParams.mPlayMode = osg::AnimationPath::SWING;
   }
}

void AnimationControlDock::UpdateUIFromAnimParams(const AnimParams& params)
{
   mUI.mOutputAnimClipName->setText(tr(params.mName.c_str()));
   mUI.mAnimName->setText(tr(params.mName.c_str()));
   mUI.mAnimFrameBegin->setValue(params.mBeginFrame);
   mUI.mAnimFrameEnd->setValue(params.mEndFrame);
   mUI.mAnimFrameBeginOffset->setValue(params.mBeginFrameOffset);
   mUI.mAnimSpeed->setValue(params.mSpeed);
   mUI.mAnimLoopLimit->setValue(params.mLoopLimit);
   SetPlayMode(params.mPlayMode);
}

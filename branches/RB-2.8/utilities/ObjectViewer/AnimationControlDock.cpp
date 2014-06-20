
////////////////////////////////////////////////////////////////////////////////
// INCLUDE DIRECTIVES
////////////////////////////////////////////////////////////////////////////////
#include "AnimationControlDock.h"
// DELTA3D
#include <dtAnim/animclippath.h>
// QT
#include <QtGui/QListWidgetItem>



using namespace dtAnim;



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
   
   // LIST ITEMS
   connect(mUI.mListAnimations, SIGNAL(itemClicked(QListWidgetItem*)), this, SLOT(OnItemSelect(QListWidgetItem*)));
}

void AnimationControlDock::ResetUI()
{
   mUI.mOutputAnimClipName->setText(tr(""));
   mUI.mAnimName->setText(tr(""));
   mUI.mOutputObjectCount->setText(tr("0"));
   mUI.mOutputFrameTotal->setText(tr("0"));
   mUI.mAnimFrameBegin->setValue(0.0);
   mUI.mAnimFrameEnd->setValue(0.0);
   mUI.mListAnimations->clear();

   mParamsMap.clear();
}

void AnimationControlDock::UpdateUI()
{
   osg::Node* node = mObject.valid() ? mObject->GetOSGNode() : NULL;
   if (node != NULL)
   {
      AnimCallbackVisitor visitor;
      node->accept(visitor);

      int maxFrames = 0;
      double timeEarliest = -1.0;
      double timeLatest = 0.0;
      osg::AnimationPath* curPath = NULL;
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

      mCurrentParams.mName = "Default";
      mCurrentParams.mBeginFrame = timeEarliest;
      mCurrentParams.mEndFrame = timeLatest;

      QString str;
      str = str.number(visitor.GetAnimCallbacks().size());
      mUI.mOutputObjectCount->setText(str);
      
      str.clear();
      str = str.number(maxFrames);
      mUI.mOutputFrameTotal->setText(str);

      str.clear();
      double period = mCurrentParams.mEndFrame - mCurrentParams.mBeginFrame;
      str = str.number(period);
      mUI.mOutputTotalSeconds->setText(str);
   }
}

void AnimationControlDock::ApplyAnimationParameters(dtAnim::AnimCallbackVisitor& visitor)
{
   AnimClipPath* curPath = NULL;
   typedef AnimCallbackVisitor::AnimCallbackVector AnimCallbackVector;
   AnimCallbackVector::iterator iter = visitor.GetAnimCallbacks().begin();
   AnimCallbackVector::iterator iterEnd = visitor.GetAnimCallbacks().end();
   for (; iter != iterEnd; ++iter)
   {
      curPath = dynamic_cast<AnimClipPath*>(iter->get()->getAnimationPath());
      curPath->setBeginTime(mUI.mAnimFrameBegin->value());
      curPath->setEndTime(mUI.mAnimFrameEnd->value());
   }
}

osg::Node* AnimationControlDock::GetRootNode()
{
   return mObject.valid() ? mObject->GetOSGNode() : NULL;
}

void AnimationControlDock::OnPause()
{
   InternalPause();
}

void AnimationControlDock::InternalPause()
{
   osg::Node* node = GetRootNode();
   if (node != NULL)
   {
      AnimCallbackVisitor visitor;
      node->accept(visitor);

      visitor.SetPaused(true);

      mPaused = true;
   }
}

void AnimationControlDock::OnPlay()
{
    InternalPlay(false);
}

void AnimationControlDock::InternalPlay(bool reset)
{
   osg::Node* node = GetRootNode();
   if (node != NULL && mPaused)
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
   if (node != NULL)
   {
      AnimCallbackVisitor visitor;
      node->accept(visitor);

      visitor.ResetCallbacks();
   }
}

void AnimationControlDock::OnAdd()
{
   QString animName(mUI.mAnimName->text().trimmed());
   bool valid = ! animName.isEmpty()
      && mUI.mAnimFrameBegin->value() >= 0.0
      && mUI.mAnimFrameBegin->value() < mUI.mAnimFrameEnd->value();

   if (valid)
   {
      AnimParams params;
      params.mName = mUI.mAnimName->text().toStdString();
      params.mBeginFrame = mUI.mAnimFrameBegin->value();
      params.mEndFrame = mUI.mAnimFrameEnd->value();

      QString str(animName);
      str += ": ";
      str += mUI.mAnimFrameBegin->text();
      str += " to ";
      str += mUI.mAnimFrameEnd->text();

      QListWidgetItem* item = new QListWidgetItem;
      item->setText(str);
      mUI.mListAnimations->addItem(item);

      if (mParamsMap.find(str.toStdString()) == mParamsMap.end())
      {
         mParamsMap.insert(std::make_pair(str.toStdString(), params));
      }
   }
}

void AnimationControlDock::OnRemove()
{
   InternalPause();

   typedef QList<QListWidgetItem *> ItemList;
   ItemList items = mUI.mListAnimations->selectedItems();
   while (!items.empty())
   {
      mUI.mListAnimations->removeItemWidget(items.first());
      items.removeFirst();
   }
}

void AnimationControlDock::OnSpeedChanged()
{
   osg::Node* node = GetRootNode();
   if (node != NULL && mUI.mAnimSpeed->value() != 0.0)
   {
      AnimCallbackVisitor visitor;
      node->accept(visitor);

      visitor.SetSpeed(mUI.mAnimSpeed->value());
   }
}

void AnimationControlDock::OnPlayModeChanged(bool checked)
{
    osg::Node* node = GetRootNode();
    AnimCallbackVisitor visitor;
    if (node != NULL && mUI.mAnimSpeed->value() != 0.0)
    {
        node->accept(visitor);
    }
    
    osg::AnimationPath::LoopMode curPlayMode = osg::AnimationPath::NO_LOOPING; // ONCE
    if (mUI.mRadioPlayMode_Loop->isChecked())
    {
        curPlayMode = osg::AnimationPath::LOOP;
    }
    else if (mUI.mRadioPlayMode_Swing->isChecked())
    {
        curPlayMode = osg::AnimationPath::SWING;
    }

    visitor.SetPlayMode(curPlayMode);
}

void AnimationControlDock::OnItemSelect(QListWidgetItem* item)
{
   if (item != NULL)
   {
      AnimParamMap::iterator foundIter = mParamsMap.find(item->text().toStdString());

      if (foundIter != mParamsMap.end())
      {
         InternalPause();

         AnimParams& params = foundIter->second;
         SetOutput(params);

         if (mUI.mChkPlayOnClick->checkState() == Qt::Checked)
         {
             InternalPlay(true);
         }
      }
   }
}

void AnimationControlDock::OnGeometryLoaded(dtCore::Object* object)
{
   ResetUI();

   mObject = object;

   bool objectValid = mObject.valid() && mObject->GetOSGNode() != NULL;
   if (objectValid)
   {
      osg::Node* node = mObject->GetOSGNode();

      // Swap out animation paths with subclassed version
      // that allows for better interpolation control, such
      // as for playing only a segment of  animation.
      AnimCallbackVisitor visitor;
      node->accept(visitor);

      osg::AnimationPath* curPath = NULL;
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

void AnimationControlDock::SetOutput(const AnimParams& params)
{
   mUI.mOutputAnimClipName->setText(tr(params.mName.c_str()));
   mUI.mAnimName->setText(tr(params.mName.c_str()));
   mUI.mAnimFrameBegin->setValue(params.mBeginFrame);
   mUI.mAnimFrameEnd->setValue(params.mEndFrame);
}

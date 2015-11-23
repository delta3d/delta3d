
////////////////////////////////////////////////////////////////////////////////
// INCLUDE DIRECTIVES
////////////////////////////////////////////////////////////////////////////////
#include "AnimationSliderPanel.h"
#include <QtGui/QIcon>



////////////////////////////////////////////////////////////////////////////////
// CONSTANTS
////////////////////////////////////////////////////////////////////////////////
const QString AnimationSliderPanel::ICON_PAUSE("./images/pause.png");
const QString AnimationSliderPanel::ICON_PLAY("./images/play.png");



////////////////////////////////////////////////////////////////////////////////
// CLASS CODE
////////////////////////////////////////////////////////////////////////////////
AnimationSliderPanel::AnimationSliderPanel(QWidget* parent)
   : BaseClass(parent)
   , mFrame(0)
   , mFrameStart(0)
   , mFrameEnd(0)
   , mFrameTotal(0)
   , mFramesPerSec(DEFAULT_FPS)
{
   mUI.setupUi(this);

   CreateConnections();

   UpdateUI();
}

AnimationSliderPanel::~AnimationSliderPanel()
{}

void AnimationSliderPanel::CreateConnections()
{
   // Buttons
   connect(mUI.mButtonPlayPause, SIGNAL(clicked()),
      this, SLOT(OnPlayPausePressed()));
   connect(mUI.mButtonReset, SIGNAL(clicked()),
      this, SLOT(OnResetPressed()));

   // Frame Controls
   connect(mUI.mSpinFrameStart, SIGNAL(valueChanged(int)),
      this, SLOT(OnFrameStartChanged(int)));
   connect(mUI.mSpinFrameEnd, SIGNAL(valueChanged(int)),
      this, SLOT(OnFrameEndChanged(int)));
   connect(mUI.mSpinFrameTotal, SIGNAL(valueChanged(int)),
      this, SLOT(OnFrameTotalChanged(int)));
   connect(mUI.mTimeSlider, SIGNAL(valueChanged(int)),
      this, SLOT(OnTimeSliderChanged(int)));
}

void AnimationSliderPanel::UpdateUI()
{
   if (mAnimator.valid() && ! mAnimator->IsPaused())
   {
      float time = mAnimator->GetAnimationTime();

      // TODO: Convert time to frame.
   }
}

void AnimationSliderPanel::SetAnimator(dtAnim::AnimationUpdaterInterface* animator)
{
   mAnimator = animator;
}

dtAnim::AnimationUpdaterInterface* AnimationSliderPanel::GetAnimator() const
{
   return mAnimator;
}

void AnimationSliderPanel::OnModelChanged()
{
   // TODO:
}

void AnimationSliderPanel::OnPlayPausePressed()
{
   bool isPlaying = mAnimator.valid() && ! mAnimator->IsPaused();
   
   if (mAnimator.valid())
   {
      mAnimator->SetPaused(isPlaying);

      isPlaying = ! mAnimator->IsPaused();
   }

   if (isPlaying)
   {
      QIcon icon(ICON_PAUSE);
      mUI.mButtonPlayPause->setIcon(icon);
      mUI.mButtonPlayPause->setToolTip("Pause");
   }
   else
   {
      QIcon icon(ICON_PLAY);
      mUI.mButtonPlayPause->setIcon(icon);
      mUI.mButtonPlayPause->setToolTip("Play");
   }
}

void AnimationSliderPanel::OnResetPressed()
{
   mUI.mTimeSlider->setValue(0);
}

void AnimationSliderPanel::OnTimeSliderChanged(int frame)
{
   if (mFrame != frame)
   {
      mFrame = frame;
      if (mAnimator.valid())
      {
         float time = 0.0f;

         // TODO: Convert frames to time.

         mAnimator->SetAnimationTime(time);
      }
   }
}

void AnimationSliderPanel::OnFrameStartChanged(int frame)
{
   if (mFrameStart != frame)
   {
      mFrameStart = frame;
      mUI.mTimeSlider->setMinimum(frame);

      // Update the end spinner so that the same number
      // of frames are between the start and the end.
      // NOTE: The change signal will update end frame variable.
      mUI.mSpinFrameEnd->setValue(mFrameStart + mFrameTotal);
   }
}

void AnimationSliderPanel::OnFrameEndChanged(int frame)
{
   if (mFrameEnd != frame)
   {
      if (frame <= mFrameStart)
      {
         frame = mFrameStart + 1;

         // Since the change value is now modified,
         // ensure the widget displays the new value
         // but does not re-signal to this method.
         mUI.mSpinFrameEnd->blockSignals(true);
         mUI.mSpinFrameEnd->setValue(frame);
         mUI.mSpinFrameEnd->blockSignals(false);
      }

      mFrameEnd = frame;
      mUI.mTimeSlider->setMaximum(frame);

      int diff = mFrameEnd - mFrameStart;
      mUI.mSpinFrameTotal->setValue(diff);
   }
}

void AnimationSliderPanel::OnFrameTotalChanged(int frameTotal)
{
   if (mFrameTotal != frameTotal)
   {
      mFrameTotal = frameTotal;

      // Update the end frame spinner.
      // NOTE: This should send a change signal.
      mUI.mSpinFrameEnd->setValue(mFrameStart + mFrameTotal);
   }
}

void AnimationSliderPanel::OnFramesPerSecondChanged(int fps)
{
   if (mFramesPerSec != fps)
   {
      mFramesPerSec = fps;
   }
}

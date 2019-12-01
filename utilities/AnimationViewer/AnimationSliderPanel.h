
#ifndef __ANIMATION_SLIDER_PANEL_H__
#define __ANIMATION_SLIDER_PANEL_H__

////////////////////////////////////////////////////////////////////////////////
// INCLUDE DIRECTIVES
////////////////////////////////////////////////////////////////////////////////
#include "ui_AnimationSliderPanel.h"
#include <QtGui/QWidget>
#include <dtAnim/animationupdaterinterface.h>
#include <dtCore/refptr.h>



////////////////////////////////////////////////////////////////////////////////
// FORWARD DECLARATIONS
////////////////////////////////////////////////////////////////////////////////
class QString;

namespace Ui
{
   class AnimationSliderPanel;
}



////////////////////////////////////////////////////////////////////////////////
// CLASS CODE
////////////////////////////////////////////////////////////////////////////////
class AnimationSliderPanel : public QWidget
{
   Q_OBJECT
public:
   typedef QWidget BaseClass;
    
   static const QString ICON_PAUSE;
   static const QString ICON_PLAY;
   static const int DEFAULT_FPS = 30;

   AnimationSliderPanel(QWidget* parent = NULL);
   virtual ~AnimationSliderPanel();

   void CreateConnections();

   void UpdateUI();

   void SetAnimator(dtAnim::AnimationUpdaterInterface* animator);
   dtAnim::AnimationUpdaterInterface* GetAnimator() const;

public slots:
   void OnModelChanged();

   void OnPlayPausePressed();
   void OnResetPressed();
   void OnTimeSliderChanged(int frame);

   void OnFrameStartChanged(int frame);
   void OnFrameEndChanged(int frame);
   void OnFrameTotalChanged(int frameTotal);
   void OnFramesPerSecondChanged(int fps);

protected:
   Ui::AnimationSliderPanel mUI;

   dtCore::RefPtr<dtAnim::AnimationUpdaterInterface> mAnimator;

   int mFrame;
   int mFrameStart;
   int mFrameEnd;
   int mFrameTotal;
   int mFramesPerSec;
};

#endif

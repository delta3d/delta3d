
#ifndef __ATTACHMENT_PANEL_H__
#define __ATTACHMENT_PANEL_H__

////////////////////////////////////////////////////////////////////////////////
// INCLUDE DIRECTIVES
////////////////////////////////////////////////////////////////////////////////
#include "ui_AttachmentPanel.h"
#include <QtGui/QWidget>
#include <osg/Node>
#include <dtAnim/boneinterface.h>
#include <dtAnim/chardrawable.h>
#include <dtCore/refptr.h>
#include <dtCore/observerptr.h>



////////////////////////////////////////////////////////////////////////////////
// FORWARD DECLARATIONS
////////////////////////////////////////////////////////////////////////////////
class QString;

namespace Ui
{
   class AttachmentPanel;
}



////////////////////////////////////////////////////////////////////////////////
// TYPE DEFINITIONS
////////////////////////////////////////////////////////////////////////////////
typedef dtCore::RefPtr<osg::Node> OsgNodePtr;
typedef dtCore::RefPtr<dtAnim::BoneInterface> BonePtr;
typedef dtCore::RefPtr<dtAnim::CharDrawable> CharacterPtr;

struct AttachmentInfo
{
   BonePtr mBone;
   osg::Vec3 mOffset;
   osg::Vec3 mRotation;
};



////////////////////////////////////////////////////////////////////////////////
// CLASS CODE
////////////////////////////////////////////////////////////////////////////////
class AttachmentPanel : public QWidget
{
   Q_OBJECT
public:
   typedef QWidget BaseClass;

   AttachmentPanel(QWidget* parent = NULL);
   virtual ~AttachmentPanel();

   void CreateConnections();

   void UpdateUI();

   void UpdateSelectedBone();

signals:
   void SignalBoneSelected(BonePtr bone);
   void SignalOffsetChanged(osg::Vec3 offset);
   void SignalRotationChanged(osg::Vec3 rotation);

   void SignalAttachmentChanged(AttachmentInfo info);

   void SignalLoadAttachment(const QString file);

public slots:
   void OnAttachmentLoaded();
   void OnBoneSelectedFromList(int index);
   void OnBoneSelected(BonePtr bone);
   void OnCharacterUpdated(CharacterPtr character);
   void OnLoadAttachmentClicked();
   void OnNodeSelected(OsgNodePtr node);
   void OnOffsetChanged();
   void OnRotationChanged();

   void OnOffsetSliderChanged();
   void OnRotationSliderChanged();

protected:
   Ui::AttachmentPanel mUI;

   AttachmentInfo mInfo;

   dtCore::ObserverPtr<dtAnim::CharDrawable> mCharacter;

   float mSliderScale;
};

#endif

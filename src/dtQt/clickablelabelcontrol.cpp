
////////////////////////////////////////////////////////////////////////////////
// INCLUDE DIRECTIVES
////////////////////////////////////////////////////////////////////////////////
#include <dtQt/clickablelabelcontrol.h>
#include <QtCore/QEvent>
#include <QtGui/QHBoxLayout>



namespace dtQt
{
   /////////////////////////////////////////////////////////////////////////////
   // CLASS CODE
   /////////////////////////////////////////////////////////////////////////////
   ClickableLabelControl::ClickableLabelControl(QLabel& label)
      : mLabel(&label)
   {
      mLabel->installEventFilter(this);
      QHBoxLayout* layout = new QHBoxLayout;
      layout->setMargin(0);
      setLayout(layout);
      setMinimumSize(mLabel->minimumSize());
      setSizePolicy(mLabel->sizePolicy());

      mLabel->setParent(this);
   }

   ClickableLabelControl::~ClickableLabelControl()
   {}

   bool ClickableLabelControl::eventFilter(QObject* obj, QEvent* qevent)
   {
      if (qevent->type() == QEvent::MouseButtonPress)
      {
         emit SignalClick();
         this->OnClick();
         return true;
      }

      // standard event processing
      return QObject::eventFilter(obj, qevent);
   }

   void ClickableLabelControl::OnClick()
   {
      // OVERRIDE:
   }

}

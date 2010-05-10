#ifndef AIUTILITY_QT_GL_FRAME
#define AIUTILITY_QT_GL_FRAME

#include <QtGui/QFrame>

class QtGLFrame : public QFrame
{
   Q_OBJECT

public:
   QtGLFrame(QObject* parent = NULL);
   virtual ~QtGLFrame(void);

protected:
   void enterEvent(QEvent* event);
   void resizeEvent(QResizeEvent* event);
};

#endif // AIUTILITY_QT_GL_FRAME


#ifndef stageglwidget_h__
#define stageglwidget_h__

#include <dtQt/osgadapterwidget.h>

class QMouseEvent;

namespace dtEditQt
{
   class EditorViewport;

   /** Special QGLWidgets used by STAGE.  These route incoming Qt events
     * to base dtQt::OSGAdapterWidget as well as the associated dtEditQt::Viewport. 
     */
   class STAGEGLWidget : public dtQt::OSGAdapterWidget
   {
   public:
      STAGEGLWidget(bool drawOnSeparateThread,  QWidget* parent = NULL,
                    const QGLWidget* shareWidget = NULL, Qt::WindowFlags f = NULL);

      virtual ~STAGEGLWidget();

      ///Which EditorViewport is this widget associated with?
      void SetViewport(EditorViewport* viewport);

   protected:
      /**
      * Called when the user moves the mouse while pressing any combination of
      * mouse buttons.  Based on the current mode, the camera is updated.
      */
      virtual void mouseMoveEvent(QMouseEvent* e);

      /**
      * Called when the user presses a mouse button in the viewport.  Based on
      * the combination of buttons pressed, the viewport's current mode will
      * be set.
      * @param e
      * @see ModeType
      */
      virtual void mousePressEvent(QMouseEvent* e);

      /**
      * Called when the user releases a mouse button in the viewport.  Based on
      * the buttons released, the viewport's current mode is updated
      * accordingly.
      * @param e
      */
      virtual void mouseReleaseEvent(QMouseEvent* e);

      /**
      * Called when the user double clicks the mouse button in the viewport.
      */
      virtual void mouseDoubleClickEvent(QMouseEvent *e);

      /**
      * Called when the user moves the wheel on a mouse containing a scroll wheel.
      * This causes the scene to be zoomed in and out.
      * @param e
      */
      virtual void wheelEvent(QWheelEvent* e);

      /**
      * Called when the user presses a key on the keyboard in the viewport.
      * Based on the combination of keys pressed, the viewport's current
      * mode will be set.
      * @param e
      */
      virtual void keyPressEvent(QKeyEvent* e);

      /**
      * Called when the user releases a key on the keyboard in the viewport.
      * Based on the keys released, the viewport's current mode is
      * updated accordingly.
      * @param e
      */
      virtual void keyReleaseEvent(QKeyEvent* e);


      /**
      * Drag events.
      */
      virtual void dragEnterEvent(QDragEnterEvent* event);
      virtual void dragLeaveEvent(QDragLeaveEvent* event);
      virtual void dragMoveEvent(QDragMoveEvent* event);
      virtual void dropEvent(QDropEvent* event);


      virtual void enterEvent(QEvent *e);
      virtual void leaveEvent(QEvent *e);

      /**
      * Called when the viewport needs to redraw itself.
      */
      virtual void paintGL();

      virtual void initializeGL();

      virtual void resizeGL(int width, int height);

   private:

      dtEditQt::EditorViewport* mViewport; ///<The Viewport that's associated with this widget
   };
}
#endif // stageglwidget_h__


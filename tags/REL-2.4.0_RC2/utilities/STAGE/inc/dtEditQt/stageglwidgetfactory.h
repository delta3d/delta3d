#ifndef stageglwidgetfactory_h__
#define stageglwidgetfactory_h__

#include <dtQt/glwidgetfactory.h>
#include <dtQt/osgadapterwidget.h>

#include <dtEditQt/stageglwidget.h>

namespace dtEditQt
{

   ///Factory used to create the special QGLWidgets used in STAGE
   class STAGEGLWidgetFactory : public dtQt::GLWidgetFactory
   {
   public:
      STAGEGLWidgetFactory()
      {
      }

      virtual dtQt::OSGAdapterWidget* CreateWidget(bool drawOnSeparateThread,  QWidget* parent = NULL,
                                           const QGLWidget* shareWidget = NULL, Qt::WindowFlags f = NULL)
      {
         return new dtEditQt::STAGEGLWidget(drawOnSeparateThread, parent,
                                            shareWidget, f);
      }
   protected:
      virtual ~STAGEGLWidgetFactory()
      {
      }
   };
}
#endif // stageglwidgetfactory_h__

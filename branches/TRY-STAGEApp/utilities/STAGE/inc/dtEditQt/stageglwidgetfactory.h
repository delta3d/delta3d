#ifndef stageglwidgetfactory_h__
#define stageglwidgetfactory_h__

#include <dtQt/glwidgetfactory.h>
#include <dtQt/osgadapterwidget.h>

#include <dtEditQt/stageglwidget.h>

namespace dtEditQt
{
   
   class STAGEGLWidgetFactory : public dtQt::GLWidgetFactory
   {
   public:
   	STAGEGLWidgetFactory()
      {
      }
   	
      virtual ~STAGEGLWidgetFactory()
      {
      }

      dtQt::OSGAdapterWidget* CreateWidget()
      {
         return new dtEditQt::STAGEGLWidget(false);
      }
   };
}
#endif // stageglwidgetfactory_h__

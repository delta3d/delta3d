#ifndef inspectorqt_h__
#define inspectorqt_h__

#include <dtInspectorQt/export.h>
#include <dtCore/base.h>

class QApplication;

namespace dtInspectorQt
{

   class InspectorWindow;

   /** This utility library is used to inspect and tweak Delta3D class
     * instances found at runtime.
     * To use, just create an instance of InspectorQt in your application, sometime
     * after all Delta3D instances have been created.  Note: dtInspectorQt will
     * not find any instances that have been created after dtInspectorQt has 
     * been created.
     * @code
     * #include <dtInspectorQt/inspectorqt.h>
     * dtCore::RefPtr<dtInspectorQt::InspectorQt> mgr = new dtInspectorQt::InspectorQt(argc, argv);
     * @endcode
     * @note InspectorQt requires modification to your project settings to add the
     * Qt include folders, plus linking with Qt libraries.
     */
   class DT_INSPECTORQT_EXPORT InspectorQt : public dtCore::Base
   {
   public:
   	InspectorQt(int& argc, char **argv);

      virtual void OnMessage(MessageData*);

   protected:
      virtual ~InspectorQt();
   	
   private:
      InspectorWindow *mInspector;
      QApplication *mApp;
   };
}
#endif // inspectorqt_h__

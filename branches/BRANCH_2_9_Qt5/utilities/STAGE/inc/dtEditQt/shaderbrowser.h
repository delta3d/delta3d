#ifndef shaderbrowser_h__
#define shaderbrowser_h__

#include <dtEditQt/resourceabstractbrowser.h>

class QGridLayout;

namespace dtEditQt
{
   class ShaderBrowser : public ResourceAbstractBrowser
   {
      Q_OBJECT

   public:
      ShaderBrowser(dtCore::DataType& type, QWidget* parent = 0);
      virtual ~ShaderBrowser();
   protected:

   private:
      // Layout Objects
      QGridLayout* mGrid;

   };
}
#endif // shaderbrowser_h__

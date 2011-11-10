#include <dtGUI/guiexceptionenum.h>

namespace dtGUI
{
   ////////////////////////////////////////////////////////////////////////////////
   GenericCEGUIException::GenericCEGUIException(const std::string& message, const std::string& filename, unsigned int linenum)
      : dtUtil::Exception(message, filename, linenum)
   {
   }
}//namespace dtGUI

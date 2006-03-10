#ifndef LOADABLE_INCLUDED
#define LOADABLE_INCLUDED

#include <string>
#include "dtCore/export.h"

namespace osg
{
   class Node;
}

namespace dtCore
{
   
   /** The Loadable class is a simple mechanism for loading files.  Inheriting
    *  from this class will supply the derived class the API to load any type
    *  of file in a generic fashion.  The LoadFile()
    *  method stores the supplied filename for later retrieval.  The default 
    *  behavior will pass the supplied filename to the OSG reader functions
    *  and return back the osg::Node.  The LoadFile() method is intended to be
    *  overwritten if the default behavior doesn't support the needs of the derive
    *  class.
    */
   class DT_CORE_EXPORT Loadable
   {
   public:
      ///Load a file from disk
      virtual osg::Node* LoadFile(const std::string& filename, bool useCache = true);

      ///Get the filename of the last loaded file
      const std::string& GetFilename(void) const {return mFilename;}

   protected:
	   Loadable(void);
		virtual ~Loadable(void);

      std::string mFilename; ///<The filename of the last file loaded
   };

}

#endif //LOADABLE_INCLUDED

#ifndef LOADABLE_INCLUDED
#define LOADABLE_INCLUDED

#include "dtCore/export.h"
#include "osg/Node"
#include <iostream>

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
   class DT_EXPORT Loadable
   {
   public:
      ///Load a file from disk
      virtual osg::Node* LoadFile( std::string filename, bool useCache = true);

      ///Get the filename of the last loaded file
      std::string GetFilename(void) const {return mFilename;}

   protected:
   Loadable(void);
   virtual ~Loadable(void);

   std::string mFilename; ///<The filename of the last file loaded
   };

}

#endif //LOADABLE_INCLUDED
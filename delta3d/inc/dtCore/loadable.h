#ifndef LOADABLE_INCLUDED
#define LOADABLE_INCLUDED

#include "dtCore/export.h"
#include "osg/Node"
#include <iostream>

namespace dtCore
{
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
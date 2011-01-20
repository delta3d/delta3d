#ifndef DATAFILTER_INCLUDED
#define DATAFILTER_INCLUDED

#include <strstream>

namespace dtCore
{
   /**
    *  Simple interface to allow for the filtering of raw data from a file.
    */
   class DataFilter
   {

   public:
      
      virtual std::istrstream& FilterData(std::ifstream& unfilteredFileStream) = 0;

      virtual void ShutdownFilter(std::ifstream& unfilteredFileStream) = 0;
   };
}

#endif

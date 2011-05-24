#ifndef LOADABLE_INCLUDED
#define LOADABLE_INCLUDED

#include <string>
#include <dtCore/export.h>

/// @cond DOXYGEN_SHOULD_SKIP_THIS
namespace osg
{
   class Node;
}
/// @endcond


namespace dtCore
{
   /**
    * The Loadable class is a simple mechanism for loading files.  Inheriting
    * from this class will supply the derived class the API to load any type
    * of file in a generic fashion.  The LoadFile()
    * method stores the supplied filename for later retrieval.  The default
    * behavior will pass the supplied filename to the OSG reader functions
    * and return back the osg::Node.  The LoadFile() method is intended to be
    * overwritten if the default behavior doesn't support the needs of the derive
    * class.
    */
   class DT_CORE_EXPORT Loadable
   {
   public:

      /*!
      * Load a geometry from a file using any supplied data file paths set in
      * dtCore::SetDataFilePathList().  Additional calls to this method will just combine
      * the loaded files together.
      *
      * @param filename : The name of the file to be loaded
      * @param useCache : If true, use OSG's object cache
      */
      virtual osg::Node* LoadFile(const std::string& filename, bool useCache = true);

      ///Get the filename of the last loaded file
      const std::string& GetFilename() const { return mFilename; }

      ///flush all objects that have been previously cached.
      static void FlushObjectCache();

   protected:
      Loadable();
      virtual ~Loadable();

      std::string mFilename; ///<The filename of the last file loaded
   };

}

#endif //LOADABLE_INCLUDED


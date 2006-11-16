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

      /*
      /// See file CopyOp in osg for further reading.
      class CopyOp
         enum Options      
         {
         SHALLOW_COPY                = 0,
         DEEP_COPY_OBJECTS           = 1,
         DEEP_COPY_NODES             = 2,
         DEEP_COPY_DRAWABLES         = 4,
         DEEP_COPY_STATESETS         = 8,
         DEEP_COPY_STATEATTRIBUTES   = 16,
         DEEP_COPY_TEXTURES          = 32,
         DEEP_COPY_IMAGES            = 64,
         DEEP_COPY_ARRAYS            = 128,
         DEEP_COPY_PRIMITIVES        = 256,
         DEEP_COPY_SHAPES            = 512,
         DEEP_COPY_UNIFORMS          = 1024,
         DEEP_COPY_ALL               = 0xffffffff
         };
      */
      
      /// Uses the basic osg::CopyOp SHALLOW_COPY.
      static const unsigned int COPY_OPS_DEFAULT = osg::CopyOp::SHALLOW_COPY;
      
      /// This will share images, textures, and vertices between models. 
      /// Good use case is for articulated vehicles or characters.
      static const unsigned int COPY_OPS_SHARED_GEOMETRY = (osg::CopyOp::DEEP_COPY_OBJECTS 
                                                          | osg::CopyOp::DEEP_COPY_NODES 
                                                          | osg::CopyOp::DEEP_COPY_STATESETS 
                                                          | osg::CopyOp::DEEP_COPY_STATEATTRIBUTES 
                                                          | osg::CopyOp::DEEP_COPY_UNIFORMS );
      
      /// Makes a deep copy of everything of the model. This is a memory hit and not a hard drive lookup hit.
      static const unsigned int COPY_OPS_DEEP_COPY_ALL = osg::CopyOp::DEEP_COPY_ALL;

      ///Load a file from disk, or memory. If you are new to this function, Look at the COPY_OPS Flags in this class or osg::CopyOp
      virtual osg::Node* LoadFile( const std::string& filename, bool useCache = true, unsigned int cacheFlags = COPY_OPS_DEFAULT);

      ///Get the filename of the last loaded file
      const std::string& GetFilename() const { return mFilename; }

   protected:
	   Loadable();
		virtual ~Loadable();

      std::string mFilename; ///<The filename of the last file loaded
   };

}

#endif //LOADABLE_INCLUDED

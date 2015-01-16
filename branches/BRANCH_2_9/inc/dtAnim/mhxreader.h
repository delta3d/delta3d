
#ifndef __DELTA_MHX_READER_H__
#define __DELTA_MHX_READER_H__

////////////////////////////////////////////////////////////////////////////////
// INCLUDE DIRECTIVES
////////////////////////////////////////////////////////////////////////////////
#include <dtAnim/export.h>
#include <osgDB/ReaderWriter>



namespace dtAnim
{
   /////////////////////////////////////////////////////////////////////////////
   // CLASS CODE
   /////////////////////////////////////////////////////////////////////////////
   class DT_ANIM_EXPORT MHXReader : public osgDB::ReaderWriter
   {
   public:
      typedef osgDB::ReaderWriter BaseClass;

      MHXReader();

      const char* className() const;

      virtual osgDB::ReaderWriter::ReadResult readObject(const std::string& fileName, const osgDB::ReaderWriter::Options* options = NULL) const;
   
      virtual osgDB::ReaderWriter::ReadResult readObject(std::istream& fin, const osgDB::ReaderWriter::Options* options) const;

   protected:
      virtual ~MHXReader();
   };

}

#endif
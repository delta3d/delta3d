
////////////////////////////////////////////////////////////////////////////////
// INCLUDE DIRECTIVES
////////////////////////////////////////////////////////////////////////////////
#include <dtAnim/morphtargetreaderwriter.h>
#include <dtAnim/osgloader.h>
#include <dtCore/refptr.h>
#include <dtUtil/fileutils.h>
#include <osgAnimation/MorphGeometry>
#include <osgDB/FileNameUtils>



namespace dtAnim
{
   /////////////////////////////////////////////////////////////////////////////
   // CLASS CODE
   /////////////////////////////////////////////////////////////////////////////
   MorphTargetReaderWriter::MorphTargetReaderWriter()
   {
      supportsExtension(dtAnim::OsgExtEnum::MAKE_HUMAN_TARGET.GetName(),
         dtAnim::OsgExtEnum::MAKE_HUMAN_TARGET.GetDescription());
   }

   MorphTargetReaderWriter::~MorphTargetReaderWriter()
   {}

   const char* MorphTargetReaderWriter::className() const
   {
      return "MorphTargetReaderWriter";
   }

   osgDB::ReaderWriter::ReadResult MorphTargetReaderWriter::readObject(
      const std::string& fileName, const osgDB::ReaderWriter::Options* options) const
   {
      std::string ext = osgDB::getLowerCaseFileExtension(fileName);

      if (!acceptsExtension(ext)) return ReadResult::FILE_NOT_HANDLED;

      if (!dtUtil::FileUtils::GetInstance().FileExists(fileName))
      {
         return osgDB::ReaderWriter::ReadResult(osgDB::ReaderWriter::ReadResult::FILE_NOT_FOUND);
      }

      osgDB::ifstream istream(fileName.c_str(), std::ios::in);

      if (!istream.is_open())
      {
         return osgDB::ReaderWriter::ReadResult(osgDB::ReaderWriter::ReadResult::ERROR_IN_READING_FILE);
      }

      osgDB::ReaderWriter::ReadResult result = readObject(istream, options);
      if (istream.is_open())
      {
         istream.close();
      }

      return result;
   }

   osgDB::ReaderWriter::ReadResult MorphTargetReaderWriter::readObject(
      std::istream& fin, const osgDB::ReaderWriter::Options* options ) const
   {
      typedef std::map<unsigned int, osg::Vec3> VertexMap;
      VertexMap vertMap;

      dtCore::RefPtr<osg::Geometry> geom = new osgAnimation::MorphGeometry;
      geom->setName("MorphTarget");

      dtCore::RefPtr<osg::Vec3Array> verts;
      geom->setVertexArray(verts);

      int loop = 0;
      int index = 0;
      osg::Vec3 offset;
      while (fin.good())
      {
         if (fin.peek() == '#')
         {
            std::string commentLine;
            std::getline(fin, commentLine);
            continue;
         }

         std::string str;
         fin >> str;

         switch (loop % 4)
         {
         case 0:
            index = atoi(str.c_str());
            break;
         case 1:
            offset.x() = atof(str.c_str());
            break;
         case 2:
            offset.y() = atof(str.c_str());
            break;
         case 3:
            offset.z() = atof(str.c_str());
            break;
         default:
            break;
         }

         if (loop == 3)
         {
            vertMap[index] = offset;
         }

         ++loop;
      }

      return geom.valid()
         ? osgDB::ReaderWriter::ReadResult(geom.get(), ReaderWriter::ReadResult::FILE_LOADED)
         : osgDB::ReaderWriter::ReadResult::ERROR_IN_READING_FILE;
   }

   // Register this ReaderWriter with the OSG plugin registry.
   REGISTER_OSGPLUGIN(target, MorphTargetReaderWriter)

}
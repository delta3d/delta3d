/* -*-c++-*-
* dtPhysics
* Copyright 2007-2010, Alion Science and Technology
*
* This library is free software; you can redistribute it and/or modify it under
* the terms of the GNU Lesser General Public License as published by the Free
* Software Foundation; either version 2.1 of the License, or (at your option)
* any later version.
*
* This library is distributed in the hope that it will be useful, but WITHOUT
* ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
* FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more
* details.
*
* You should have received a copy of the GNU Lesser General Public License
* along with this library; if not, write to the Free Software Foundation, Inc.,
* 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
*
* This software was developed by Alion Science and Technology Corporation under
* circumstances in which the U. S. Government may have rights in the software.
*
* Bradley Anderegg
*/



#include <osgDB/FileNameUtils>
#include <osgDB/ReadFile>

#include <osg/ComputeBoundsVisitor>

#include <dtPhysics/physicsreaderwriter.h>
#include <dtPhysics/trianglerecorder.h>
#include <dtPhysics/geometry.h>


#include <dtUtil/datastream.h>
#include <dtUtil/exception.h>
#include <dtUtil/fileutils.h>
#include <dtUtil/refstring.h>
#include <dtCore/exceptionenum.h>

#include <fstream>

namespace dtPhysics
{
   //////////////////////////////////////////////////////////////////////////
   //file saving and loading utils
   //////////////////////////////////////////////////////////////////////////
   namespace PhysicsFileHeader
   {
      const unsigned TRIANGLE_DATA_FILE_IDENT = 3130317;
      
      const unsigned VERSION_MAJOR = 1;
      const unsigned VERSION_MINOR = 1; // 1 - added material table section

      const char FILE_START_END_CHAR = '!';
   };



   /////////////////////////////////////////////////////////////////////////////
   // CONSTANTS
   /////////////////////////////////////////////////////////////////////////////
   static const dtUtil::RefString PHYSICS_PLUGIN_DATA("dtPhysicsPluginData");



   /////////////////////////////////////////////////////////////////////////////
   // CLASS CODE
   /////////////////////////////////////////////////////////////////////////////
   class PhysOptions : public osg::Object
   {
   public:
      typedef osg::Object BaseClass;
      typedef dtPhysics::VertexData PhysTriData;

      PhysOptions(PhysTriData& triangleData, const std::string& fileName)
         : mDataContainer(&triangleData)
         , mFileName(fileName)
      {}

      PhysTriData& GetDataContainer()
      {
         return *mDataContainer;
      }

      const std::string& GetFileName() const
      {
         return mFileName;
      }

      static dtCore::RefPtr<osgDB::ReaderWriter::Options>
         CreateOSGOptions(PhysOptions& optionData)
      {
         dtCore::RefPtr<osgDB::ReaderWriter::Options> newOptions;
         const osgDB::ReaderWriter::Options* globalOptions = osgDB::Registry::instance()->getOptions();

         if (globalOptions != NULL)
         {
            newOptions = static_cast<osgDB::ReaderWriter::Options*>(globalOptions->clone(0));
         }
         else
         {
            newOptions = new osgDB::ReaderWriter::Options;
         }

         newOptions->setObjectCacheHint(osgDB::ReaderWriter::Options::CACHE_ALL);

         newOptions->setPluginData(PHYSICS_PLUGIN_DATA, &optionData);

         return newOptions; 
      }

      META_Object("dtPhysics", PhysOptions);

   private:
      PhysTriData* mDataContainer;
      std::string mFileName;

      PhysOptions()
         : mDataContainer(NULL)
      {}

      PhysOptions(const osg::Object& obj,const osg::CopyOp& copyop=osg::CopyOp::SHALLOW_COPY)
         : BaseClass(obj, copyop)
         , mDataContainer(NULL)
      {}
   };



   //////////////////////////////////////////////////////////////////////////
   // OSG READER WRITER
   //////////////////////////////////////////////////////////////////////////
   class PhysicsOSGReaderWriterPlugin : public osgDB::ReaderWriter
   {
   public:
      //////////////////////////////////////////////////////////////////////////
      PhysicsOSGReaderWriterPlugin()
      {
         supportsExtension("phys","dtPhysics File");
         supportsExtension("dtphys","dtPhysics File");
      }

      //////////////////////////////////////////////////////////////////////////
      const char* className() const
      {
         return "dtPhysics Reader/Writer"; 
      }

      //////////////////////////////////////////////////////////////////////////
      PhysOptions* GetPhysOptions(const osgDB::ReaderWriter::Options& options) const
      {
         const PhysOptions* physOptionsConst
            = static_cast<const PhysOptions*>(options.getPluginData(PHYSICS_PLUGIN_DATA));
         return const_cast<PhysOptions*>(physOptionsConst);
      }

      //////////////////////////////////////////////////////////////////////////
      osgDB::ReaderWriter::ReadResult readObject(const std::string& fileName, const osgDB::ReaderWriter::Options* options = NULL) const
      {
         std::string ext = osgDB::getLowerCaseFileExtension(fileName);

         if (!acceptsExtension(ext)) return ReadResult::FILE_NOT_HANDLED;

         if (!dtUtil::FileUtils::GetInstance().FileExists(fileName))
         {
            return osgDB::ReaderWriter::ReadResult(osgDB::ReaderWriter::ReadResult::FILE_NOT_FOUND);
         }

         std::ifstream infile(fileName.c_str(), std::ios_base::binary | std::ios_base::in);

         bool streamValid = infile.is_open() && infile.good() && !infile.eof();
         if (!streamValid)
         {
            if (infile.is_open())
            {
               infile.close();
            }
            return osgDB::ReaderWriter::ReadResult(osgDB::ReaderWriter::ReadResult::ERROR_IN_READING_FILE);
         }

         osgDB::ReaderWriter::ReadResult result = readObject(infile, options);
         if (infile.is_open())
         {
            infile.close();
         }
         return result;
      }

      //////////////////////////////////////////////////////////////////////////
      virtual osgDB::ReaderWriter::ReadResult readObject(std::istream& fin, const osgDB::ReaderWriter::Options* options = NULL) const
      {
         // Get the PhysOptions object that is holding onto the loaded data.
         PhysOptions* physOptions = GetPhysOptions(*options);

         if (LoadFile(fin, *physOptions))
         {
            // Return the PhysOptions as the success object. This is so that
            // callers of the ReadObjectFile method will be returned a non-NULL
            // object pointer as a flag that the file loading has succeeded.
            return osgDB::ReaderWriter::ReadResult(physOptions, ReaderWriter::ReadResult::FILE_LOADED);
         }

         return osgDB::ReaderWriter::ReadResult::ERROR_IN_READING_FILE;
      }

      //////////////////////////////////////////////////////////////////////////
      bool LoadFile(std::istream& infile, PhysOptions& physOptions) const
      {
         bool streamValid = infile.good() && !infile.eof();
         if(!streamValid)
         {
            return false;
         }

         bool read_file_ok = false;
         PhysOptions::PhysTriData& triangleData = physOptions.GetDataContainer();
         const std::string& filename = physOptions.GetFileName();

         try
         {
            // Get length of file
            infile.seekg (0, std::ios::end);
            int length = infile.tellg();
            infile.seekg (0, std::ios::beg);

            // Read Data
            char* buffer = new char [length];
            infile.read(buffer, length);

            dtUtil::DataStream ds(buffer, length);

            char fileStart;
            unsigned fileIdent, vMajor, vMinor;

            ds >> fileStart >> fileIdent >> vMajor >> vMinor;

            if(fileStart == PhysicsFileHeader::FILE_START_END_CHAR &&
               fileIdent == PhysicsFileHeader::TRIANGLE_DATA_FILE_IDENT &&
               vMajor == 1)
            {

               // Vertex Data
               unsigned numVerts = 0;
               unsigned numIndices = 0;
               unsigned numMaterials = 0;

               ds.Read(numVerts);
               {
                  triangleData.mVertices.reserve(triangleData.mVertices.size() + numVerts);
                  for(unsigned i = 0; i < numVerts; ++i)
                  {
                     osg::Vec3 vert;
                     ds.Read(vert);
                     triangleData.mVertices.push_back(vert);
                  }
               }

               // Triangle Data
               ds.Read(numIndices);
               {
                  for(unsigned i = 0; i < numIndices; ++i)
                  {
                     unsigned face = 0;
                     ds.Read(face);
                     triangleData.mIndices.push_back(face);
                  }
               }

               // Material Data
               ds.Read(numMaterials);
               {
                  for(unsigned i = 0; i < numMaterials; ++i)
                  {
                     unsigned materialID = 0;
                     ds.Read(materialID);
                     triangleData.mMaterialFlags.push_back(materialID);
                  }
               }

               // New material section data
               // Check for old material data section
               bool oldVersion = vMinor == 0;
               if ( ! oldVersion)
               {
                  unsigned numMaterialEntries = 0;
                  ds.Read(numMaterialEntries);

                  // Check for a material table (added in version 1.1).

                  // Material Data

                  for(unsigned i = 0; i < numMaterialEntries; ++i)
                  {
                     unsigned materialID = 0;
                     std::string materialName;

                     ds.Read(materialID);
                     ds.Read(materialName);

                     // Add actual material id-name pairs.
                     triangleData.SetMaterialName(materialID, materialName);
                  }
               }

            }
            else
            {
               throw dtCore::BaseException(
                  "Error reading Physics file '" + filename + ".", __FILE__, __LINE__);
            }


            ds.Read(fileStart);
            if(fileStart != PhysicsFileHeader::FILE_START_END_CHAR)
            {
               throw dtCore::BaseException(
                  "Error reading Physics file '" + filename + ".", __FILE__, __LINE__);
            }

            //read successful
            read_file_ok = true;
         }
         catch(dtUtil::Exception& e)
         {
            e.LogException();
         }

         //infile.close();
         return read_file_ok;
      }

   };

   REGISTER_OSGPLUGIN(dtphys, PhysicsOSGReaderWriterPlugin);



   //////////////////////////////////////////////////////////////////////////
   //PhysicsReaderWriter
   //////////////////////////////////////////////////////////////////////////
   bool PhysicsReaderWriter::LoadFileGetExtents(osg::BoundingBox& bound, const std::string& filename)
   {
      dtCore::RefPtr<VertexData> triangleData = new VertexData;
      dtUtil::FileUtils& fileUtils = dtUtil::FileUtils::GetInstance();
      dtCore::RefPtr<PhysOptions> physOptions = new PhysOptions(*triangleData, filename);
      dtCore::RefPtr<osgDB::ReaderWriter::Options> options = PhysOptions::CreateOSGOptions(*physOptions);

      bool result = false;
      fileUtils.ReadObject(filename, options.get());
      if (!triangleData->mVertices.empty())
      {
         std::vector<VectorType>::iterator i,iend;
         i = triangleData->mVertices.begin();
         iend = triangleData->mVertices.end();
         for (;i != iend; ++i)
         {
            bound.expandBy(*i);
         }
      }
      else
      {
         osgDB::Registry* reg = osgDB::Registry::instance();

         options = reg->getOptions() ?
            static_cast<osgDB::ReaderWriter::Options*>(reg->getOptions()->clone(osg::CopyOp::SHALLOW_COPY)) :
         new osgDB::ReaderWriter::Options;

         options->setObjectCacheHint(osgDB::ReaderWriter::Options::CACHE_ALL);

         dtCore::RefPtr<osg::Node> node = fileUtils.ReadNode(filename, options);
         if (node.valid())
         {
            osg::ComputeBoundsVisitor bbv;

            // sorry about the const cast.  The node SHOULD be const since we aren't changing it
            // but accept doesn't work as const.
            const_cast<osg::Node&>(*node).accept(bbv);
            bound.expandBy(bbv.getBoundingBox());
            result = true;
         }
      }
      return result;

   }

   bool PhysicsReaderWriter::LoadTriangleDataFile(VertexData& triangleData, const std::string& filename)
   {
      dtUtil::FileUtils& fileUtils = dtUtil::FileUtils::GetInstance();
      dtCore::RefPtr<PhysOptions> physOptions = new PhysOptions(triangleData, filename);
      dtCore::RefPtr<osgDB::ReaderWriter::Options> options = PhysOptions::CreateOSGOptions(*physOptions);

      dtCore::RefPtr<osg::Object> objResult = fileUtils.ReadObject(filename, options.get());

      if (triangleData.mVertices.empty())
      {
         dtCore::RefPtr<osg::Node> node = dynamic_cast<osg::Node*>(objResult.get());
         if (node.valid())
         {
            TriangleRecorder tr;
            tr.Record(*node);
            if (!tr.mData.back()->mVertices.empty() && !tr.mData.back()->mIndices.empty())
            {
               triangleData.mVertices = tr.mData.back()->mVertices;
               triangleData.mIndices.swap(tr.mData.back()->mIndices);
               triangleData.SwapMaterialTable(*tr.mData.back());
            }
         }
      }
      return !triangleData.mVertices.empty();
   }
  
   bool PhysicsReaderWriter::SaveTriangleDataFile(const VertexData& triangleData, const std::string& filename)
   {
      std::ofstream outfile;

      outfile.open(filename.c_str(), std::ios_base::binary | std::ofstream::out);
      if (outfile.fail())
      {
         LOG_ERROR(std::string("Unable to open filename: ") + filename + std::string(" for writing"));
         return false;
      }

      dtUtil::DataStream ds;
      ds.Write(PhysicsFileHeader::FILE_START_END_CHAR);
      ds.Write(PhysicsFileHeader::TRIANGLE_DATA_FILE_IDENT);
      ds.Write(PhysicsFileHeader::VERSION_MAJOR);
      ds.Write(PhysicsFileHeader::VERSION_MINOR);

      // Vertex Data
      ds.Write(unsigned(triangleData.mVertices.size()));
      {
         std::vector<VectorType>::const_iterator iter = triangleData.mVertices.begin();
         std::vector<VectorType>::const_iterator iterEnd = triangleData.mVertices.end();
         for(;iter != iterEnd; ++iter)
         {
            ds.Write(*iter);
         }
      }

      // Triangle Data
      ds.Write(unsigned(triangleData.mIndices.size()));
      {
         osg::UIntArray::const_iterator iter = triangleData.mIndices.begin();
         osg::UIntArray::const_iterator iterEnd = triangleData.mIndices.end();
         for(;iter != iterEnd; ++iter)
         {
            ds.Write(*iter);
         }
      }

      // Material Data
      ds.Write(unsigned(triangleData.mMaterialFlags.size()));
      {
         osg::UIntArray::const_iterator iter = triangleData.mMaterialFlags.begin();
         osg::UIntArray::const_iterator iterEnd = triangleData.mMaterialFlags.end();
         for(;iter != iterEnd; ++iter)
         {
            ds.Write(*iter);
         }
      }

      // Material Name Table (added in version 1.1)
      // Check for the old way of writing materials.
      bool oldFormat = PhysicsFileHeader::VERSION_MAJOR < 1
            || (PhysicsFileHeader::VERSION_MAJOR == 1 && PhysicsFileHeader::VERSION_MINOR == 0);
      if ( ! oldFormat)
      {
         // List materials associated with the geometry,
         // names mapped to local material indices/flags.
         const MaterialNameTable& matTable = triangleData.GetMaterialTable();

         ds.Write(unsigned(matTable.size()));

         MaterialIndex matIndex = 0;
         std::string matName;
         MaterialNameTable::const_iterator curIter = matTable.begin();
         MaterialNameTable::const_iterator endIter = matTable.end();
         for (; curIter != endIter; ++curIter)
         {
            matIndex = curIter->first;
            matName = curIter->second;

            ds.Write(matIndex);
            ds.Write(matName);
         }
      }


      ds.Write(PhysicsFileHeader::FILE_START_END_CHAR);
      outfile.write(ds.GetBuffer(), ds.GetBufferSize());
      outfile.flush();
      outfile.close();

      return !outfile.fail();
   }

}//namespace dtPhysics


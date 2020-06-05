///////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2012-2014 DreamWorks Animation LLC
//
// All rights reserved. This software is distributed under the
// Mozilla Public License 2.0 ( http://www.mozilla.org/MPL/2.0/ )
//
// Redistributions of source code must retain the above copyright
// and license notice and the following restrictions and disclaimer.
//
// *     Neither the name of DreamWorks Animation nor the names of
// its contributors may be used to endorse or promote products derived
// from this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// OWNER OR CONTRIBUTORS BE LIABLE FOR ANY INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
// IN NO EVENT SHALL THE COPYRIGHT HOLDERS' AND CONTRIBUTORS' AGGREGATE
// LIABILITY FOR ALL CLAIMS REGARDLESS OF THEIR BASIS EXCEED US$250.00.
//
///////////////////////////////////////////////////////////////////////////

#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <openvdb/openvdb.h>
#ifdef DWA_OPENVDB
#include <logging_base/logging.h>
#include <usagetrack.h>
#endif
#include <openvdb/tools/MeshToVolume.h>
#include <openvdb/tools/ValueTransformer.h>


#include <dtUtil/mathdefines.h>
#include <dtUtil/stringutils.h>
#include <dtPhysics/trianglerecorder.h>

#include <osgDB/ReadFile>
#include <osg/NodeVisitor>
#include <osg/TriangleFunctor>
#include <osg/NodeVisitor>
#include <osg/Geode>
#include <osg/Geometry>
#include <map>
#include <osg/io_utils>

using namespace openvdb;


namespace {

   typedef std::vector<std::string> StringVec;

   //const char* INDENT = "   ";
   const char* gProgName = "";


   void
   usage(int exitStatus = EXIT_FAILURE)
   {
      std::cerr <<
            "Usage: " << gProgName << " inFile1 [inFile2 ...] [options]\n" <<
            "Which: Converts meshes into voxel grids\n" <<
            "Options:\n" <<
            "    -b,            convert to a bool grid or tree (default).\n" <<
            "    -f,            convert to a float grid or tree.\n" <<
            "    -r{N},         voxel size in x,y,z (Z can be overridden).\n" <<
            "    -z{N},         z Thickness, or use the -r value by default.\n"<<
            "    -t{N},         Mesh thickness (internal).  How thick to make the mesh walls internally.\n" <<
            "    -x{N},         Exterior mesh thickness.  Thickness on the outward side of the mesh.\n" <<
            "    -s{N},         subdivisions to break the file into.  This will also reduce he memory usage at create time..\n";
      exit(exitStatus);
   }


   //std::string
   //sizeAsString(openvdb::Index64 n, const std::string& units)
   //{
   //    std::ostringstream ostr;
   //    ostr << std::setprecision(3);
   //    if (n < 1000) {
   //        ostr << n;
   //    } else if (n < 1000000) {
   //        ostr << (double(n) / 1.0e3) << "K";
   //    } else if (n < 1000000000) {
   //        ostr << (double(n) / 1.0e6) << "M";
   //    } else {
   //        ostr << (double(n) / 1.0e9) << "G";
   //    }
   //    ostr << units;
   //    return ostr.str();
   //}
   //
   //
   //std::string
   //bytesAsString(openvdb::Index64 n)
   //{
   //    std::ostringstream ostr;
   //    ostr << std::setprecision(3);
   //    if (n >> 30) {
   //        ostr << (double(n) / double(uint64_t(1) << 30)) << "GB";
   //    } else if (n >> 20) {
   //        ostr << (double(n) / double(uint64_t(1) << 20)) << "MB";
   //    } else if (n >> 10) {
   //        ostr << (double(n) / double(uint64_t(1) << 10)) << "KB";
   //    } else {
   //        ostr << n << "B";
   //    }
   //    return ostr.str();
   //}


   //std::string
   //coordAsString(const openvdb::Coord ijk, const std::string& sep)
   //{
   //    std::ostringstream ostr;
   //    ostr << ijk[0] << sep << ijk[1] << sep << ijk[2];
   //    return ostr.str();
   //}
   //
   //
   //std::string
   //bkgdValueAsString(const openvdb::GridBase::ConstPtr& grid)
   //{
   //    std::ostringstream ostr;
   //    if (grid) {
   //        const openvdb::TreeBase& tree = grid->baseTree();
   //        ostr << "background: ";
   //        openvdb::Metadata::Ptr background = tree.getBackgroundValue();
   //        if (background) ostr << background->str();
   //    }
   //    return ostr.str();
   //}
   struct PrintInterrupter
   {
       /// Default constructor
       PrintInterrupter (): mLastPercent(0) {}
       /// Signal the start of an interruptible operation.
       /// @param name  an optional descriptive name for the operation
       void start(const char* name = NULL) { (void)name; }
       /// Signal the end of an interruptible operation.
       void end() {}
       /// Check if an interruptible operation should be aborted.
       /// @param percent  an optional (when >= 0) percentage indicating
       ///     the fraction of the operation that has been completed
       /// @note this method is assumed to be thread-safe. The current
       /// implementation is clearly a NOOP and should compile out during
       /// optimization!
       inline bool wasInterrupted(int percent = -1)
       {
          if (percent > mLastPercent)
          {
             mLastPercent = percent;
             std::cout << percent << "%" << std::endl; return false;
          }
          return false;
       }
       int mLastPercent;
   };

   FloatGrid::Ptr convertToLevelSet(std::vector<Vec3s> pointList, std::vector<Vec4I> triList, float resolution, float zres, float interiorThickness, float exteriorThickness, const std::string& filename)
   {

      Mat4R m;
      m.setToScale(Vec3R(resolution, resolution, zres));
      openvdb::math::Transform::Ptr xform = openvdb::math::Transform::createLinearTransform(m);
      xform->voxelSize(Vec3R(resolution, resolution, zres));

      //xform.voxelSize(openvdb::Vec3d(resolution, resolution, resolution));
      FloatGrid::Ptr result;
      try {
         std::vector<Vec3s> indexSpacePoints(pointList.size());

         { // Copy and transform (required for MeshToVolume) points to grid space.
            openvdb::tools::internal::PointTransform ptnXForm(pointList, indexSpacePoints, *xform);
            ptnXForm.run();
         }

         float exWidth(exteriorThickness);
         float inWidth(interiorThickness);


         math::Transform::Ptr transform = xform->copy();
         openvdb::tools::MeshToVolume<FloatGrid, PrintInterrupter> vol(transform, 0, new PrintInterrupter);

         vol.convertToLevelSet(indexSpacePoints, triList, exWidth, inWidth);

         return vol.distGridPtr();
         //result = openvdb::tools::meshToLevelSet<FloatGrid>(xform, pointList, triList);
         //std::vector<Vec4I> quads(0);
         //result = openvdb::tools::doMeshConversion<openvdb::FloatGrid>(*xform, pointList, triList, quads, 0.01f, 0.5f, false);
      } catch (openvdb::Exception& e) {
         OPENVDB_LOG_ERROR(e.what() << " (" << filename << ")");
      }
      return result;
   }
   /// Print detailed information about the given VDB files.
   /// If @a metadata is true, include file-level metadata key, value pairs.
   template<typename LevelSetType>
   inline
   void printLongListing(const StringVec& filenames, float resolution, float zres, size_t splits, float interiorThickness, float exteriorThickness)
   {
      bool oneFile = (filenames.size() == 1), firstFile = true;

      std::vector<std::string> outFiles;

      for (size_t f = 0, N = filenames.size(); f < N; ++f, firstFile = false)
      {
         outFiles.clear();

         const std::string& filename = filenames[f];
         std::cout << filename << ' ' << resolution << ' ' << zres << std::endl;


         osg::ref_ptr<osg::Node> node = osgDB::readNodeFile(filename, NULL);
         if (!node.valid())
         {
            std::cout << "error loading." << std::endl;
            return;
         }

         dtPhysics::TriangleRecorder tr;
         tr.Record(*node, 200.0f);

         std::vector<Vec3s> pointList;
         std::vector<Vec4I> triList;
         pointList.reserve(tr.mData.back()->mVertices.size());
         size_t numTriangles = tr.mData.back()->mIndices.size() / 3;
         size_t numTrisPerSplit = (numTriangles / splits) + (numTriangles / splits != 0 ? 1 : 0);
         triList.reserve(numTrisPerSplit);

         for (unsigned split = 0; split < splits; ++split)
         {
            openvdb::GridPtrVecPtr grids(new openvdb::GridPtrVec);

            for (unsigned i = 0; i < tr.mData.back()->mVertices.size(); ++i)
            {
               const osg::Vec3& point = tr.mData.back()->mVertices[i];
               Vec3s pointvdb(point.x(), point.y(), point.z());
               pointList.push_back(pointvdb);
            }

            for (unsigned i = split * numTrisPerSplit * 3; i < tr.mData.back()->mIndices.size() && i < (split + 1) * numTrisPerSplit * 3; i+=3)
            {
               Vec4I tri(tr.mData.back()->mIndices[i], tr.mData.back()->mIndices[i+1], tr.mData.back()->mIndices[i+2], util::INVALID_IDX);
               triList.push_back(tri);
            }

            FloatGrid::Ptr gridInitial = convertToLevelSet(pointList, triList, resolution, zres, interiorThickness, exteriorThickness, filename);

            pointList.clear();
            triList.clear();

            if (gridInitial && gridInitial->isType<LevelSetType>())
            {
               gridInitial->setSaveFloatAsHalf(true);
               grids->push_back(gridInitial);
               gridInitial = NULL;
            }
            else if (gridInitial)
            {
               // Define a local function that, given an iterator pointing to a vector value
               // in an input grid, sets the corresponding tile or voxel in a scalar,
               // floating-point output grid to the length of the vector.
               struct Local {
                  static inline void op(
                        const openvdb::FloatGrid::ValueOnCIter& iter,
                        typename LevelSetType::Accessor& accessor)
                  {
                     if (iter.isVoxelValue()) { // set a single voxel
                        accessor.setValue(iter.getCoord(), openvdb::math::Abs(*iter) > FLT_EPSILON);
                     } else { // fill an entire tile
                        openvdb::CoordBBox bbox;
                        iter.getBoundingBox(bbox);
                        accessor.getTree()->fill(bbox, openvdb::math::Abs(*iter) > FLT_EPSILON);
                     }
                  }
               };

               // Create a scalar grid to hold the transformed values.
               typename LevelSetType::Ptr outGrid = LevelSetType::create();
               outGrid->setTransform(gridInitial->transformPtr());
               // Populate the output grid with transformed values.
               openvdb::tools::transformValues(gridInitial->cbeginValueOn(), *outGrid, Local::op);
               gridInitial = NULL;
               outGrid->setSaveFloatAsHalf(true);
               grids->push_back(outGrid);
            }

            if (grids->empty()) continue;

            std::string outFile;
            if (splits > 1)
            {
               outFile = filename + dtUtil::ToString(split) + ".vdb";
               outFiles.push_back(outFile);
            }
            else
            {
               outFile = filename + ".vdb";
            }


            openvdb::io::File file(outFile);
            file.write(*grids);

            if (!oneFile) {
               if (!firstFile) {
                  std::cout << "\n" << std::string(40, '-') << "\n\n";
               }
               std::cout << filename << "\n\n";
            }

            std::cout << "\n";

            // For each grid in the file...
            bool firstGrid = true;
            for (openvdb::GridPtrVec::const_iterator it = grids->begin(); it != grids->end(); ++it) {
               if (openvdb::GridBase::ConstPtr grid = *it) {
                  if (!firstGrid) std::cout << "\n\n";
                  std::cout << "Name: " << grid->getName() << std::endl;
                  grid->print(std::cout, /*verboseLevel=*/11);
                  firstGrid = false;
               }
            }
         } // For each split

         if (!outFiles.empty())
         {
            GridPtrVec grids;
            for (auto i = outFiles.cbegin(), iend = outFiles.cend(); i != iend; ++i)
            {
               const std::string& name = *i;
               try
               {
                  openvdb::io::File file(name);
                  file.open();
                  GridPtrVecPtr fileGrids = file.getGrids();
                  file.close();
                  grids.insert(grids.end(), fileGrids->begin(), fileGrids->end());
               }
               catch (const openvdb::IoError& ioe)
               {
                  std::cerr << "Error reopening: " << name << " for merging.  Aborting." << std::endl;
                  return;
               }
            }
            while (grids.size() > 1)
            {
               typename LevelSetType::Ptr bg = boost::dynamic_pointer_cast<LevelSetType>(grids.front());
               typename LevelSetType::Ptr bg2 = boost::dynamic_pointer_cast<LevelSetType>(grids.back());
               bg->merge(*bg2);
               grids.pop_back();
            }
            if (!grids.empty())
            {
               openvdb::io::File file(filename + ".vdb");
               file.write(grids);
            }
         }

      } // For each file

   }

} // unnamed namespace


int
main(int argc, char *argv[])
{
#ifdef DWA_OPENVDB
   USAGETRACK_report_basic_tool_usage(argc, argv, /*duration=*/0);
   logging_base::configure(argc, argv);
#endif

   OPENVDB_START_THREADSAFE_STATIC_WRITE
   gProgName = argv[0];
   if (const char* ptr = ::strrchr(gProgName, '/')) gProgName = ptr + 1;
   OPENVDB_FINISH_THREADSAFE_STATIC_WRITE

   int exitStatus = EXIT_SUCCESS;

   if (argc == 1) usage();

   bool useBoolean = false, useFloat = false, useGrid = true;
   float gridResolution = 1.0f, zres = -1.0f, thickness = 0.15f, exterior = 0.15f;
   unsigned subdivisions = 1;
   StringVec filenames;
   for (int i = 1; i < argc; ++i) {
      std::string arg = argv[i];
      if (arg[0] == '-' && arg.length() > 1) {
         if (arg == "-b") {
            useBoolean = true;
         } else if (arg == "-f") {
            useFloat = true;
            //            } else if (arg == "-g") {
            //                useGrid = true;
            //            } else if (arg == "-t") {
            //                useGrid = false;
         } else if (arg.substr(0,2) == "-r") {
            if (arg.length() == 2 && i + 1 < argc)
            {
               gridResolution = dtUtil::ToType<float>(argv[i+1]);
               ++i;
            }
            else if (arg.length() > 2)
            {
               gridResolution = dtUtil::ToType<float>(arg.substr(2));
            }
            else
            {
               std::cerr << gProgName << ": \"" << arg << "\" requires a float as argument.\n";
               usage();
            }
         } else if (arg.substr(0,2) == "-z") {
            if (arg.length() == 2 && i + 1 < argc)
            {
               zres = dtUtil::ToType<float>(argv[i+1]);
               ++i;
            }
            else if (arg.length() > 2)
            {
               zres = dtUtil::ToType<float>(arg.substr(2));
            }
            else
            {
               std::cerr << gProgName << ": \"" << arg << "\" requires a float as argument.\n";
               usage();
            }
         } else if (arg.substr(0,2) == "-s") {
            if (arg.length() == 2 && i + 1 < argc)
            {
               subdivisions = dtUtil::ToType<int>(argv[i+1]);
               ++i;
            }
            else if (arg.length() > 2)
            {
               subdivisions = dtUtil::ToType<int>(arg.substr(2));
            }
            else
            {
               std::cerr << gProgName << ": \"" << arg << "\" requires an int as argument.\n";
               usage();
            }
         } else if (arg.substr(0,2) == "-t") {
            if (arg.length() == 2 && i + 1 < argc)
            {
               thickness = dtUtil::ToType<int>(argv[i+1]);
               ++i;
            }
            else if (arg.length() > 2)
            {
               thickness = dtUtil::ToType<int>(arg.substr(2));
            }
            else
            {
               std::cerr << gProgName << ": \"" << arg << "\" requires an int as argument.\n";
               usage();
            }
         } else if (arg.substr(0,2) == "-x") {
            if (arg.length() == 2 && i + 1 < argc)
            {
               exterior = dtUtil::ToType<int>(argv[i+1]);
               ++i;
            }
            else if (arg.length() > 2)
            {
               exterior = dtUtil::ToType<int>(arg.substr(2));
            }
            else
            {
               std::cerr << gProgName << ": \"" << arg << "\" requires an int as argument.\n";
               usage();
            }
         } else if (arg == "-h" || arg == "-help" || arg == "--help") {
            usage(EXIT_SUCCESS);
         } else {
            std::cerr << gProgName << ": \"" << arg << "\" is not a valid option\n";
            usage();
         }
      } else if (!arg.empty()) {
         filenames.push_back(arg);
      }
   }
   if (filenames.empty()) {
      std::cerr << gProgName << ": expected one or more mesh files\n";
      usage();
   }
   if (zres < 0.0f) zres = gridResolution;

   try {
      openvdb::initialize();

      /// @todo Remove the following at some point:
      openvdb::Grid<openvdb::tree::Tree4<bool, 4, 3, 3>::Type>::registerGrid();
      openvdb::Grid<openvdb::tree::Tree4<float, 4, 3, 3>::Type>::registerGrid();
      openvdb::Grid<openvdb::tree::Tree4<double, 4, 3, 3>::Type>::registerGrid();
      openvdb::Grid<openvdb::tree::Tree4<int32_t, 4, 3, 3>::Type>::registerGrid();
      openvdb::Grid<openvdb::tree::Tree4<int64_t, 4, 3, 3>::Type>::registerGrid();
      openvdb::Grid<openvdb::tree::Tree4<openvdb::Vec2i, 4, 3, 3>::Type>::registerGrid();
      openvdb::Grid<openvdb::tree::Tree4<openvdb::Vec2s, 4, 3, 3>::Type>::registerGrid();
      openvdb::Grid<openvdb::tree::Tree4<openvdb::Vec2d, 4, 3, 3>::Type>::registerGrid();
      openvdb::Grid<openvdb::tree::Tree4<openvdb::Vec3i, 4, 3, 3>::Type>::registerGrid();
      openvdb::Grid<openvdb::tree::Tree4<openvdb::Vec3f, 4, 3, 3>::Type>::registerGrid();
      openvdb::Grid<openvdb::tree::Tree4<openvdb::Vec3d, 4, 3, 3>::Type>::registerGrid();

      if (useGrid)
      {
         if (useFloat)
         {
            printLongListing<FloatGrid>(filenames, gridResolution, zres, subdivisions, thickness, exterior);
         }
         else
         {
            printLongListing<BoolGrid>(filenames, gridResolution, zres, subdivisions, thickness, exterior);
         }
      }
      //        else
      //        {
      //           if (useFloat)
      //           {
      //              printLongListing<FloatTree>(filenames, gridResolution);
      //           }
      //           else
      //           {
      //              printLongListing<BoolTree>(filenames, gridResolution);
      //           }
      //        }
   }
   catch (const std::exception& e) {
      OPENVDB_LOG_FATAL(e.what());
      exitStatus = EXIT_FAILURE;
   }
   catch (...) {
      OPENVDB_LOG_FATAL("Exception caught (unexpected type)");
      std::unexpected();
   }

   return exitStatus;
}


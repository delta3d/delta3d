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


#include <osgDB/ReadFile>
#include <osg/NodeVisitor>
#include <osg/TriangleFunctor>
#include <osg/NodeVisitor>
#include <osg/Geode>
#include <osg/Geometry>
#include <map>
#include <osg/io_utils>

using namespace openvdb;

namespace dtUtil
{
template <typename T>
bool IsFinite(const T value)
{
  #ifdef DELTA_WIN32
      return _finite(value) ? true : false;
   #else
      return std::isfinite(value) ? true : false;
   #endif
}

template <typename VecType>
bool IsFiniteVec(const VecType value)
{
   for (size_t i = 0; i < VecType::num_components; ++i)
   {
#ifdef _MSC_VER
      if (!_finite(value[i]))
      {
         return false;
      }
#else
      if (!std::isfinite(value[i]))
      {
         return false;
      }
#endif
   }
   return true;
}
}

   class VertexData : public osg::Referenced
   {
   public:
      VertexData() {}


      std::vector<osg::Vec3> mVertices;
      std::vector<unsigned> mIndices;
   protected:
      ~VertexData() {}
   };

   class TriangleRecorder
   {
      public:

         TriangleRecorder();
         ~TriangleRecorder();

         /**
          * Records all the triangles in the buffors on this object for the given node.
          * @param node The node to traverse.
          * @param maxEdgeSize  The largest size of a triangle edge before the code will split the triangle in half recursively.
          *                     Large triangles can give physics engine trouble.
          */
         void Record(const osg::Node& node, Real maxEdgeLength = -1);

         typedef std::map<osg::Vec3, int> VertexMap;

         VertexMap mVertIndexSet;
         osg::ref_ptr<VertexData> mData;

         const osg::Matrix& GetMatrix() const;
         void SetMatrix(const osg::Matrix& m);

         /**
          * Called once for each visited triangle.
          *
          * @param v1 the triangle's first vertex
          * @param v2 the triangle's second vertex
          * @param v3 the triangle's third vertex
          * @param treatVertexDataAsTemporary whether or not to treat the vertex data
          * as temporary
          */
         void operator()(const osg::Vec3& v1,
                  const osg::Vec3& v2,
                  const osg::Vec3& v3,
                  bool treatVertexDataAsTemporary);
      private:
         osg::Matrix mMatrix;
         int mSplitCount;
         int mReuseCount;
         bool mMatrixIsIdentity;
         float mMaxEdgeLength;
   };


template< class T >
class TriangleRecorderVisitor : public osg::NodeVisitor
{
public:

   int mSplit, mSplitCount, mNumGeodes, mGeodeExportCounter;
   bool mExportSpecificMaterial, mSkipSpecificMaterial;
   std::string mSpecificDescription;
   std::string mCurrentDescription;
   osg::TriangleFunctor<T> mFunctor;

   TriangleRecorderVisitor();
   virtual ~TriangleRecorderVisitor() {}

   // Returns a description string that may be contained in the specified node.
   static std::string GetDescription(const osg::Node& node);

   // Returns the nearest parent description.
   static std::string CheckDescriptionInAncestors(const osg::Node& node);

   // Acquires the current description from the specified node.
   void CheckDesc(osg::Node& node);

   virtual void apply(osg::Node& node);

   virtual void apply(osg::Group& gnode);

   /**
   * Applies this visitor to a geode.
   *
   * @param node the geode to visit
   */
   virtual void apply(osg::Geode& node);

   virtual void apply(osg::Billboard& node);
};

template<class T>
TriangleRecorderVisitor<T>::TriangleRecorderVisitor()
   : osg::NodeVisitor(osg::NodeVisitor::TRAVERSE_ACTIVE_CHILDREN)
   , mSplit(-1)
   , mSplitCount(1)
   , mNumGeodes(0)
   , mGeodeExportCounter(0)
   , mExportSpecificMaterial(false)
   , mSkipSpecificMaterial(false)
{}

template<class T>
std::string TriangleRecorderVisitor<T>::GetDescription(const osg::Node& node)
{
   std::string desc;

   if ( ! node.getDescriptions().empty())
   {
      // Use *last* description as material tag
      desc = node.getDescription(node.getNumDescriptions()-1);
   }

   return desc;
}

template<class T>
void TriangleRecorderVisitor<T>::CheckDesc(osg::Node& node)
{
   mCurrentDescription = GetDescription(node);
}

template<class T>
std::string TriangleRecorderVisitor<T>::CheckDescriptionInAncestors(const osg::Node& node)
{
   std::string desc;

   const osg::Node* curNode = &node;
   while (curNode != NULL)
   {
      desc = GetDescription(*curNode);
      if ( ! desc.empty())
      {
         break;
      }

      // Assume a single parent.
      if ( ! curNode->getParents().empty())
      {
         curNode = curNode->getParent(0);
      }
      else
      {
         curNode = NULL;
      }
   }

   return desc;
}


template<class T>
void TriangleRecorderVisitor<T>::apply(osg::Geode& node)
{
   // Obtain the description for the current node.
   CheckDesc(node);

   if(mSplit != -1)
   {
      ++mGeodeExportCounter;
      int divCount = mNumGeodes / mSplitCount;
      if (divCount == 0) divCount = 1;
      if(((mGeodeExportCounter - 1) / divCount) != mSplit)
      {
         //skip this one since we are breaking it up into multiple parts
         return;
         //std::cout << "Skipping tile number: " << mGeodeExportCounter << std::endl;
      }
      else
      {
         //std::cout << "Exporting tile number: " << mGeodeExportCounter << std::endl;
      }
   }

   //for some reason if we do this on the whole scene it crashes, so we are doing it per geode
   //Simplify(&node);

   osg::NodePath nodePath = getNodePath();
   mFunctor.SetMatrix(osg::computeLocalToWorld(nodePath));

   for(size_t i=0;i<node.getNumDrawables();i++)
   {
      osg::Drawable* d = node.getDrawable(i);

      if (d->supports(mFunctor))
      {
         d->accept(mFunctor);
      }
   }
}

template<class T>
void TriangleRecorderVisitor<T>::apply(osg::Node& node)
{
   CheckDesc(node);
   osg::NodeVisitor::apply(node);
}

template<class T>
void TriangleRecorderVisitor<T>::apply(osg::Group& g)
{
   CheckDesc(g);
   osg::NodeVisitor::apply(g);
}

template<class T>
void TriangleRecorderVisitor<T>::apply(osg::Billboard& node)
{
   //do nothing
}


   //////////////////////////////////////////////////////
   TriangleRecorder::TriangleRecorder()
   : mData(new VertexData)
   , mSplitCount(0)
   , mReuseCount(0)
   , mMatrixIsIdentity(true)
   , mMaxEdgeLength(200.0f)
   {
   }

   //////////////////////////////////////////////////////
   TriangleRecorder::~TriangleRecorder()
   {

   }

   //////////////////////////////////////////////////////
   void TriangleRecorder::Record(const osg::Node& node, Real maxEdgeLength)
   {
      TriangleRecorderVisitor<TriangleRecorder> visitor;
      if (maxEdgeLength > 0)
      {
         mMaxEdgeLength = maxEdgeLength;
      }
      // sorry about the const cast.  The node SHOULD be const since we aren't changing it
      // but accept doesn't work as const.
      const_cast<osg::Node&>(node).accept(visitor);
      mData = visitor.mFunctor.mData;
      visitor.mFunctor.mVertIndexSet.swap(mVertIndexSet);
   }

   //////////////////////////////////////////////////////
   const osg::Matrix& TriangleRecorder::GetMatrix() const
   {
      return mMatrix;
   }

   //////////////////////////////////////////////////////
   void TriangleRecorder::SetMatrix(const osg::Matrix& m)
   {
      mMatrix = m;
      mMatrixIsIdentity = mMatrix.isIdentity();
   }

   struct Triangle
   {
      Triangle(const osg::Vec3& v1, const osg::Vec3& v2, const osg::Vec3& v3)
      {
         mV[0] = v1;
         mV[1] = v2;
         mV[2] = v3;
      }

      bool SplitIf(Real maxEdgeLength, Triangle& newT)
      {
         if (maxEdgeLength == FLT_MAX || maxEdgeLength <= 0.0f)
         {
            return false;
         }

         int longest = 1;

         osg::Vec3 side1(mV[1] - mV[0]);
         osg::Vec3 side2(mV[2] - mV[1]);
         osg::Vec3 side3(mV[0] - mV[2]);

         Real length2_1 = side1.length2();
         Real length2_2 = side2.length2();
         Real length2_3 = side3.length2();

         Real maxLength2 = maxEdgeLength * maxEdgeLength;

         bool split = length2_1 > maxLength2;

         if (length2_1 < length2_2)
         {
            longest = 2;
            split = length2_2 > maxLength2;
         }

         if ((longest == 2 && length2_2 < length2_3) || (longest == 1 && length2_1 < length2_3))
         {
            longest = 3;
            split = length2_3 > maxLength2;
         }

         if (split)
         {
            newT = *this;

            if (longest == 1)
            {
               osg::Vec3 newVertex = mV[0] + (side1 * 0.5f);
               mV[1] = newVertex;
               newT.mV[0] = newVertex;
            }
            else if (longest == 2)
            {
               osg::Vec3 newVertex = mV[1] + (side2 * 0.5f);
               mV[2] = newVertex;
               newT.mV[1] = newVertex;
            }
            else if (longest == 3)
            {
               osg::Vec3 newVertex = mV[2] + (side3 * 0.5f);
               mV[0] = newVertex;
               newT.mV[2] = newVertex;
            }
         }
         return split;
      }

      osg::Vec3 mV[3];
   };

   //////////////////////////////////////////////////////
   void TriangleRecorder::operator()(const osg::Vec3& v1,
            const osg::Vec3& v2,
            const osg::Vec3& v3,
            bool treatVertexDataAsTemporary)
   {
      //std::cerr << "New Vertex: " << v1 << "\n" << v2 << "\n" << v3 << std::endl;
      osg::Vec3 tv[3];
      if (mMatrixIsIdentity)
      {
         tv[0] = v1;
         tv[1] = v2;
         tv[2] = v3;
      }
      else
      {
         tv[0] = v1*mMatrix;
         tv[1] = v2*mMatrix;
         tv[2] = v3*mMatrix;
      }
      //std::cerr << tv[0] << "\n" << tv[1] << "\n" << tv[2] << std::endl;

      if (dtUtil::IsFiniteVec(tv[0]) && dtUtil::IsFiniteVec(tv[1]) && dtUtil::IsFiniteVec(tv[2]))
      {
         std::vector<Triangle> mTriangles;
         Triangle initial(tv[0], tv[1], tv[2]);

         mTriangles.push_back(initial);

         Triangle newT(initial);

         for (size_t i = 0; i < mTriangles.size(); ++i)
         {
            Triangle t = mTriangles[i];
            while (t.SplitIf(mMaxEdgeLength, newT))
            {
               mTriangles.push_back(newT);
               ++mSplitCount;
            }
            mTriangles[i] = t;

            VertexMap::iterator vertIter;
            int index = 0;
            for (unsigned j = 0; j < 3; ++j)
            {
               vertIter = mVertIndexSet.find(t.mV[j]);

               if(vertIter != mVertIndexSet.end())
               {
                  index = (*vertIter).second;
                  ++mReuseCount;
               }
               else
               {
                  index = mData->mVertices.size();
                  mData->mVertices.push_back(t.mV[j]);
                  mVertIndexSet.insert(std::make_pair(t.mV[j], index));
               }
               mData->mIndices.push_back(index);
               //std::cerr << mData->mVertices[index] << "\n";
            }
            //std::cerr << std::endl;
         }

      }
      else
      {
         std::ostringstream ss;
         ss << "Found non-finite triangle data.  The three vertices of the triangle are \"";
         ss << tv[0] << "\", \"" << tv[1] << "\", and \"" << tv[2] << "\".";
         std::cout << ss.str() << std::endl;
      }
   }



namespace {

typedef std::vector<std::string> StringVec;

const char* INDENT = "   ";
const char* gProgName = "";


void
usage(int exitStatus = EXIT_FAILURE)
{
    std::cerr <<
"Usage: " << gProgName << " in.vdb [in.vdb ...] [options]\n" <<
"Which: prints information about OpenVDB grids\n" <<
"Options:\n" <<
"    -l, -stats     long printout, including grid statistics\n" <<
"    -m, -metadata  print per-file and per-grid metadata\n";
    exit(exitStatus);
}


std::string
sizeAsString(openvdb::Index64 n, const std::string& units)
{
    std::ostringstream ostr;
    ostr << std::setprecision(3);
    if (n < 1000) {
        ostr << n;
    } else if (n < 1000000) {
        ostr << (double(n) / 1.0e3) << "K";
    } else if (n < 1000000000) {
        ostr << (double(n) / 1.0e6) << "M";
    } else {
        ostr << (double(n) / 1.0e9) << "G";
    }
    ostr << units;
    return ostr.str();
}


std::string
bytesAsString(openvdb::Index64 n)
{
    std::ostringstream ostr;
    ostr << std::setprecision(3);
    if (n >> 30) {
        ostr << (double(n) / double(uint64_t(1) << 30)) << "GB";
    } else if (n >> 20) {
        ostr << (double(n) / double(uint64_t(1) << 20)) << "MB";
    } else if (n >> 10) {
        ostr << (double(n) / double(uint64_t(1) << 10)) << "KB";
    } else {
        ostr << n << "B";
    }
    return ostr.str();
}


std::string
coordAsString(const openvdb::Coord ijk, const std::string& sep)
{
    std::ostringstream ostr;
    ostr << ijk[0] << sep << ijk[1] << sep << ijk[2];
    return ostr.str();
}


std::string
bkgdValueAsString(const openvdb::GridBase::ConstPtr& grid)
{
    std::ostringstream ostr;
    if (grid) {
        const openvdb::TreeBase& tree = grid->baseTree();
        ostr << "background: ";
        openvdb::Metadata::Ptr background = tree.getBackgroundValue();
        if (background) ostr << background->str();
    }
    return ostr.str();
}

FloatGrid::Ptr convertToGrid(const std::string& filename)
{
   osg::ref_ptr<osg::Node> result = osgDB::readNodeFile(filename, NULL);
   if (!result.valid())
   {
      std::cout << "error loading." << std::endl;
      return FloatGrid::Ptr();
   }

   TriangleRecorder tr;
   tr.Record(*result, 200.0f);

   std::vector<Vec3s> pointList;
   std::vector<Vec4I> polygonList;
   pointList.reserve(tr.mData->mVertices.size());
   polygonList.reserve(tr.mData->mIndices.size() / 3);
   for (unsigned i = 0; i < tr.mData->mVertices.size(); ++i)
   {
      const osg::Vec3& point = tr.mData->mVertices[i];
      Vec3s pointvdb(point.x(), point.y(), point.z());
      pointList.push_back(pointvdb);
   }

   for (unsigned i = 0; i < tr.mData->mIndices.size(); i+=3)
   {
      Vec4I poly(tr.mData->mIndices[i], tr.mData->mIndices[i+1], tr.mData->mIndices[i+2], util::INVALID_IDX);
      polygonList.push_back(poly);
   }

   openvdb::math::Transform xform;
   return openvdb::tools::meshToLevelSet<FloatGrid>(xform, pointList, polygonList);

}



/// Print detailed information about the given VDB files.
/// If @a metadata is true, include file-level metadata key, value pairs.
void
printLongListing(const StringVec& filenames)
{
    bool oneFile = (filenames.size() == 1), firstFile = true;

    for (size_t i = 0, N = filenames.size(); i < N; ++i, firstFile = false) {
        openvdb::io::File file(filenames[i]);
        std::string version;
        openvdb::GridPtrVecPtr grids;
        openvdb::MetaMap::Ptr meta;
        try {
            file.open();
            grids = file.getGrids();
            meta = file.getMetadata();
            version = file.version();
            file.close();
        } catch (openvdb::Exception& e) {
            OPENVDB_LOG_ERROR(e.what() << " (" << filenames[i] << ")");
        }
        if (!grids) continue;

        if (!oneFile) {
            if (!firstFile) {
                std::cout << "\n" << std::string(40, '-') << "\n\n";
            }
            std::cout << filenames[i] << "\n\n";
        }

        // Print file-level metadata.
        std::cout << "VDB version: " << version << "\n";
        if (meta) {
            std::string str = meta->str();
            if (!str.empty()) std::cout << str << "\n";
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
    }
}


/// Print condensed information about the given VDB files.
/// If @a metadata is true, include file- and grid-level metadata.
void
printShortListing(const StringVec& filenames, bool metadata)
{
    bool oneFile = (filenames.size() == 1), firstFile = true;

    for (size_t i = 0, N = filenames.size(); i < N; ++i, firstFile = false) {

        const std::string
            indent(oneFile ? "": INDENT),
            indent2(indent + INDENT);

        if (!oneFile) {
            if (metadata && !firstFile) std::cout << "\n";
            std::cout << filenames[i] << ":\n";
        }


        openvdb::GridPtrVec gridsvec;
        openvdb::GridPtrVecPtr grids(&gridsvec);
        FloatGrid::Ptr grid = convertToGrid(filenames[i]);
        if (grid)
        {
           grids->push_back(grid);
        }
        openvdb::MetaMap::Ptr meta;


/*        openvdb::io::File file(filenames[i]);
        try {
            file.open();
            grids = file.getGrids();
            meta = file.getMetadata();
            file.close();
        } catch (openvdb::Exception& e) {
            OPENVDB_LOG_ERROR(e.what() << " (" << filenames[i] << ")");
        }
*/
        if (!grids) continue;

        openvdb::io::File file(filenames[i] + ".vdb");
        file.write(*grids);

        if (metadata) {
            // Print file-level metadata.
            std::string str = meta->str(indent);
            if (!str.empty()) std::cout << str << "\n";
        }

        // For each grid in the file...
        for (openvdb::GridPtrVec::const_iterator it = grids->begin(); it != grids->end(); ++it) {
            const openvdb::GridBase::ConstPtr grid = *it;
            if (!grid) continue;

            // Print the grid name and its voxel value datatype.
            std::cout << indent << std::left << std::setw(11) << grid->getName()
                << " " << std::right << std::setw(6) << grid->valueType();

            // Print the grid's bounding box and dimensions.
            openvdb::CoordBBox bbox = grid->evalActiveVoxelBoundingBox();
            std::string
                boxStr = coordAsString(bbox.min()," ") + "  " + coordAsString(bbox.max()," "),
                dimStr = coordAsString(bbox.extents(), "x");
            boxStr += std::string(
                std::max(1, int(40 - boxStr.size() - dimStr.size())), ' ') + dimStr;
            std::cout << " " << std::left << std::setw(40) << boxStr;

            // Print the number of active voxels.
            std::cout << "  " << std::right << std::setw(8)
                << sizeAsString(grid->activeVoxelCount(), "Vox");

            // Print the grid's in-core size, in bytes.
            std::cout << " " << std::right << std::setw(6) << bytesAsString(grid->memUsage());

            std::cout << std::endl;

            // Print grid-specific metadata.
            if (metadata) {
                // Print background value.
                std::string str = bkgdValueAsString(grid);
                if (!str.empty()) {
                    std::cout << indent2 << str << "\n";
                }
                // Print local and world transforms.
                grid->transform().print(std::cout, indent2);
                // Print custom metadata.
                str = grid->str(indent2);
                if (!str.empty()) std::cout << str << "\n";
                std::cout << std::flush;
            }
        }
    }
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

    bool stats = false, metadata = false;
    StringVec filenames;
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg[0] == '-') {
            if (arg == "-m" || arg == "-metadata") {
                metadata = true;
            } else if (arg == "-l" || arg == "-stats") {
                stats = true;
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

        if (stats) {
            printLongListing(filenames);
        } else {
            printShortListing(filenames, metadata);
        }
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


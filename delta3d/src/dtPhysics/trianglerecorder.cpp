#include <dtPhysics/trianglerecorder.h>
#include <dtPhysics/trianglerecordervisitor.h>

#include <dtUtil/log.h>
#include <dtUtil/mathdefines.h>
#include <dtUtil/stringutils.h>
#include <sstream>
#include <osg/io_utils>

namespace dtPhysics
{
   //////////////////////////////////////////////////////
   TriangleRecorder::TriangleRecorder()
   : mData()
   , mCurrentMaterial(0)
   , mMaxEdgeLength(20.0)
   , mMode(TriangleRecorder::COMBINED)
   , mMaxSizePerBuffer(0)
   , mGeodeCount(0)
   , mSplitCount(0)
   , mReuseCount(0)
   , mMatrixIsIdentity(true)
   {
      mData.push_back(new VertexData);
   }

   //////////////////////////////////////////////////////
   TriangleRecorder::~TriangleRecorder()
   {

   }

   //////////////////////////////////////////////////////
   void TriangleRecorder::Record(const osg::Node& node, Real maxEdgeLength, MaterialLookupFunc materialLookup)
   {
      TriangleRecorderVisitor<TriangleRecorder> visitor(materialLookup);
      if (maxEdgeLength > 0)
      {
         mMaxEdgeLength = maxEdgeLength;
      }
      visitor.mFunctor.SetMode(GetMode());
      visitor.mFunctor.SetMaxEdgeLength(GetMaxEdgeLength());
      visitor.mFunctor.SetCurrentMaterial(GetCurrentMaterial());
      visitor.mFunctor.SetMaxSizePerBuffer(GetMaxSizePerBuffer());
      visitor.mFunctor.SetPhysicsNodeNamePattern(GetPhysicsNodeNamePattern());

      // sorry about the const cast.  The node SHOULD be const since we aren't changing it
      // but accept doesn't work as const.
      const_cast<osg::Node&>(node).accept(visitor);
      mData = visitor.mFunctor.mData;
      visitor.mFunctor.mVertIndexSet.swap(mVertIndexSet);
   }

   //////////////////////////////////////////////////////
   const MatrixType& TriangleRecorder::GetMatrix() const
   {
      return mMatrix;
   }

   //////////////////////////////////////////////////////
   void TriangleRecorder::SetMatrix(const MatrixType& m)
   {
      mMatrix = m;
      mMatrixIsIdentity = mMatrix.isIdentity();
   }

   struct Triangle
   {
      Triangle(const VectorType& v1, const VectorType& v2, const VectorType& v3)
      {
         mV[0] = v1;
         mV[1] = v2;
         mV[2] = v3;
      }

      bool HasEquivalentVertices() const
      {
         VectorType::value_type epsilon(0.00001);
         return dtUtil::Equivalent(mV[0], mV[1], epsilon) || dtUtil::Equivalent(mV[1], mV[2], epsilon) || dtUtil::Equivalent(mV[2], mV[0], epsilon);
      }

      bool IsFinite() const
      {
         return dtUtil::IsFiniteVec(mV[0]) && dtUtil::IsFiniteVec(mV[1]) && dtUtil::IsFiniteVec(mV[2]);
      }

      bool SplitIf(Real maxEdgeLength, Triangle& newT)
      {
         if (maxEdgeLength == FLT_MAX || maxEdgeLength <= 0.0f)
         {
            return false;
         }

         int longest = 1;

         VectorType side1(mV[1] - mV[0]);
         VectorType side2(mV[2] - mV[1]);
         VectorType side3(mV[0] - mV[2]);

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
               VectorType newVertex = mV[0] + (side1 * 0.5f);
               mV[1] = newVertex;
               newT.mV[0] = newVertex;
            }
            else if (longest == 2)
            {
               VectorType newVertex = mV[1] + (side2 * 0.5f);
               mV[2] = newVertex;
               newT.mV[1] = newVertex;
            }
            else if (longest == 3)
            {
               VectorType newVertex = mV[2] + (side3 * 0.5f);
               mV[0] = newVertex;
               newT.mV[2] = newVertex;
            }
         }
         return split;
      }

      VectorType mV[3];
   };

   DT_IMPLEMENT_ACCESSOR(TriangleRecorder, dtPhysics::MaterialIndex, CurrentMaterial);
   DT_IMPLEMENT_ACCESSOR(TriangleRecorder, std::string, CurrentMaterialName);
   DT_IMPLEMENT_ACCESSOR(TriangleRecorder, float, MaxEdgeLength);
   DT_IMPLEMENT_ACCESSOR(TriangleRecorder, TriangleRecorder::Mode, Mode);
   DT_IMPLEMENT_ACCESSOR(TriangleRecorder, size_t, MaxSizePerBuffer);
   DT_IMPLEMENT_ACCESSOR(TriangleRecorder, size_t, GeodeCount);

   //////////////////////////////////////////////////////
   bool TriangleRecorder::operator()(osg::Geode& geode)
   {
      if (!mPhysicsNodeNamePattern.empty())
      {
         if (!dtUtil::Match(mPhysicsNodeNamePattern.c_str(), geode.getName().c_str()))
            return false;
      }
      ++mGeodeCount;
      size_t currentVertCount = mData.back()->mIndices.size();
      bool split = (mMode == TriangleRecorder::PER_GEODE && geode.getNumDrawables() > 0U && mGeodeCount > 1) ||
            (mMaxSizePerBuffer > 0 && currentVertCount >= mMaxSizePerBuffer);

      if (split) // Don't split on the first one.
      {
         mData.push_back(new VertexData);
      }
      return true;
   }

   //////////////////////////////////////////////////////
   void TriangleRecorder::operator()(const VectorType& v1,
            const VectorType& v2,
            const VectorType& v3,
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

      Triangle initial(tv[0], tv[1], tv[2]);
      if (!initial.HasEquivalentVertices() && initial.IsFinite())
      {
         std::vector<Triangle> mTriangles;

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
                  index = mData.back()->mVertices.size();
                  mData.back()->mVertices.push_back(t.mV[j]);
                  mVertIndexSet.insert(std::make_pair(t.mV[j], index));
               }
               mData.back()->mIndices.push_back(index);
               //std::cerr << mData->mVertices[index] << "\n";
            }
            //std::cerr << std::endl;
         }

         // For now only one material can be applied to an object.
         // NOTE: This should be done per vertex when per-vertex
         // material support becomes available.
         mData.back()->mMaterialFlags.push_back(mCurrentMaterial);

         // Add a mapping of the material index to the material name
         mData.back()->SetMaterialName(mCurrentMaterial, mCurrentMaterialName);
      }
      else
      {
         std::ostringstream ss;
         ss << "Found non-finite triangle data or one with equivalent vertices.  The three vertices of the triangle are \"";
         ss << tv[0] << "\", \"" << tv[1] << "\", and \"" << tv[2] << "\".";
         LOG_ERROR(ss.str());
      }
   }

}

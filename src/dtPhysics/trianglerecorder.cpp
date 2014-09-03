#include <dtPhysics/trianglerecorder.h>
#include <dtPhysics/trianglerecordervisitor.h>

#include <dtUtil/log.h>
#include <dtUtil/mathdefines.h>
#include <sstream>
#include <osg/io_utils>

#include <iostream>

namespace dtPhysics
{

   //////////////////////////////////////////////////////
   TriangleRecorder::TriangleRecorder()
   : mCurrentMaterial(0)
   , mMaxEdgeSize(20.0)
   , mMatrixIsIdentity()
   , mData(new VertexData)
   {
   }

   //////////////////////////////////////////////////////
   TriangleRecorder::~TriangleRecorder()
   {

   }

   //////////////////////////////////////////////////////
   void TriangleRecorder::Record(const osg::Node& node, Real maxEdgeSize, MaterialLookupFunc materialLookup)
   {
      TriangleRecorderVisitor<TriangleRecorder> visitor(materialLookup);
      // sorry about the const cast.  The node SHOULD be const since we aren't changing it
      // but accept doesn't work as const.
      const_cast<osg::Node&>(node).accept(visitor);
      mMaxEdgeSize = maxEdgeSize;
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

      bool SplitIf(Real maxEdgeLength, Triangle& newT)
      {
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

   //////////////////////////////////////////////////////
   void TriangleRecorder::operator()(const VectorType& v1,
            const VectorType& v2,
            const VectorType& v3,
            bool treatVertexDataAsTemporary)
   {
      osg::Vec3 tv[3];
      if (mMatrixIsIdentity)
      {
         tv[0]= v1*mMatrix;
         tv[1] = v2*mMatrix;
         tv[2] = v3*mMatrix;
      }
      else
      {
         tv[0] = v1*mMatrix;
         tv[1] = v2*mMatrix;
         tv[2] = v3*mMatrix;
      }

      if (dtUtil::IsFiniteVec(tv[0]) && dtUtil::IsFiniteVec(tv[1]) && dtUtil::IsFiniteVec(tv[2]))
      {
         std::vector<Triangle> mTriangles;
         Triangle initial(tv[0], tv[1], tv[0]);

         mTriangles.push_back(initial);

         Triangle newT(initial);

         for (size_t i = 0; i < mTriangles.size(); ++i)
         {
            Triangle t = mTriangles[i];
            while (t.SplitIf(20.0f, newT))
            {
               mTriangles.push_back(newT);
               std::cout << "Splitting." << std::endl;
            }

            mTriangles[i] = t;

            VertexMap::iterator vertIters[3];
            int indices[3];
            for (unsigned j = 0; j < 2; ++j)
            {
               vertIters[j] = mVertIndexSet.find(mTriangles[i].mV[j]);

               if(vertIters[j] != mVertIndexSet.end())
               {
                  indices[j] = (*vertIters[j]).second;
               }
               else
               {
                  indices[j] = mData->mVertices.size();
                  mData->mVertices.push_back(mTriangles[i].mV[j]);
                  mVertIndexSet.insert(std::make_pair(mTriangles[i].mV[j], indices[j]));
               }
               mData->mIndices.push_back(indices[j]);
            }
         }

         mData->mMaterialFlags.push_back(mCurrentMaterial);
      }
      else
      {
         std::ostringstream ss;
         ss << "Found non-finite triangle data.  The three vertices of the triangle are \"";
         ss << tv[0] << "\", \"" << tv[1] << "\", and \"" << tv[2] << "\".";
         LOG_ERROR(ss.str());
      }
   }

}

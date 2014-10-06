#include <dtPhysics/trianglerecorder.h>
#include <osg/NodeVisitor>
#include <osg/TriangleFunctor>
#include <osg/Geode>
#include <osg/Geometry>

#include <dtUtil/log.h>
#include <dtUtil/mathdefines.h>
#include <sstream>
#include <osg/io_utils>

namespace dtPhysics
{
   //////////////////////////////////////////////////////
   template< class T >
   class DrawableVisitor : public osg::NodeVisitor
   {
      public:

         /**
          * Constructor.
          */
         DrawableVisitor()
         : osg::NodeVisitor(osg::NodeVisitor::TRAVERSE_ACTIVE_CHILDREN)
         {}

         /**
          * Applies this visitor to a geode.
          *
          * @param node the geode to visit
          */
         virtual void apply(osg::Geode& node)
         {
            for(size_t i=0; i<node.getNumDrawables(); i++)
            {
               osg::Drawable* d = node.getDrawable(i);
               if (!d)
                  continue;
/*
               osg::Geometry* geom = d->asGeometry();
               
               bool wroteWithGeom = false;
               if (geom != NULL)
               {
                  osg::IndexArray* ind = geom->getVertexIndices();
                  osg::Array* arrayBase = geom->getVertexArray();


                  if (ind != NULL && arrayBase != NULL)
                  {
                     int indexBase = int(mFunctor.mVertices.size());

                     bool success = false;
                     if (arrayBase->getType() == osg::Array::Vec3ArrayType)
                     {
                        osg::Vec3Array* array= static_cast<osg::Vec3Array*>(arrayBase);
                        mFunctor.mVertices.reserve(mFunctor.mVertices.size() + array->size());
                        for (unsigned i = 0; i < array->size(); ++i)
                        {
                           mFunctor.mVertices.push_back((*array)[i]);
                        }
                        success = true;
                     }
                     else if (arrayBase->getType() == osg::Array::Vec3dArrayType)
                     {
                        osg::Vec3dArray* array= static_cast<osg::Vec3dArray*>(arrayBase);
                        mFunctor.mVertices.reserve(mFunctor.mVertices.size() + array->size());
                        for (unsigned i = 0; i < array->size(); ++i)
                        {
                           mFunctor.mVertices.push_back((*array)[i]);
                        }
                        success = true;
                     }

                     if (success)
                     {
                        mFunctor.mIndices.reserve(mFunctor.mIndices.size() + ind->getNumElements());

                        if (ind->getDataSize() == sizeof(char))
                        {
                           char* byteArray = (char*)(ind->getDataPointer());
                           for (unsigned i = 0; i < ind->getNumElements(); ++i)
                           {
                              mFunctor.mIndices.push_back(int(byteArray[i]) + indexBase);
                           }
                        }
                        else if (ind->getDataSize() == sizeof(short))
                        {
                           short* shortArray = (short*)(ind->getDataPointer());
                           for (unsigned i = 0; i < ind->getNumElements(); ++i)
                           {
                              mFunctor.mIndices.push_back(int(shortArray[i]) + indexBase);
                           }
                        }
                        else if (ind->getDataSize() == sizeof(int))
                        {
                           int* intArray = (int*)(ind->getDataPointer());
                           for (unsigned i = 0; i < ind->getNumElements(); ++i)
                           {
                              mFunctor.mIndices.push_back(int(intArray[i]) + indexBase);
                           }
                        }
                        else
                        {
                           std::ostringstream ss;
                           ss << ind->getDataSize();
                           LOG_ERROR("The index size is " + ss.str());
                        }
                        wroteWithGeom = true;
                     }
                  }

               }

               if (wroteWithGeom)
               {
                  return;
               }
*/
               if (d->supports(mFunctor))
               {
                  osg::NodePath nodePath = getNodePath();
                  mFunctor.SetMatrix(osg::computeLocalToWorld(nodePath));
                  d->accept(mFunctor);
               }
               else
                  LOG_WARNING("Geometry "+d->getName()+" does not support conversion.")
            }
         }

      public:
         osg::TriangleFunctor<T> mFunctor;
   };

   //////////////////////////////////////////////////////
   TriangleRecorder::TriangleRecorder():
      mMatrixIsIdentity(true)
   {
      mMatrix.makeIdentity();
   }

   //////////////////////////////////////////////////////
   TriangleRecorder::~TriangleRecorder()
   {

   }

   //////////////////////////////////////////////////////
   void TriangleRecorder::Record(const osg::Node& node)
   {
      DrawableVisitor<TriangleRecorder> visitor;
      // sorry about the const cast.  The node SHOULD be const since we aren't changing it
      // but accept doesn't work as const.
      const_cast<osg::Node&>(node).accept(visitor);
      visitor.mFunctor.mIndices.swap(mIndices);
      visitor.mFunctor.mVertices.swap(mVertices);
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
      : mV1(v1)
      , mV2(v2)
      , mV3(v3)
      {
      }

      bool SplitIf(Real maxEdgeLength, Triangle& newT)
      {
         int longest = 1;

         VectorType side1(mV2 - mV1);
         VectorType side2(mV3 - mV2);
         VectorType side3(mV1 - mV3);

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
               VectorType newVertex = mV1 + (side1 * 0.5f);
               mV2 = newVertex;
               newT.mV1 = newVertex;
            }
            else if (longest == 2)
            {
               VectorType newVertex = mV2 + (side2 * 0.5f);
               mV3 = newVertex;
               newT.mV2 = newVertex;
            }
            else if (longest == 3)
            {
               VectorType newVertex = mV3 + (side3 * 0.5f);
               mV1 = newVertex;
               newT.mV3 = newVertex;
            }
         }
         return split;
      }

      VectorType mV1, mV2, mV3;
   };

   //////////////////////////////////////////////////////
   void TriangleRecorder::operator()(const VectorType& v1,
            const VectorType& v2,
            const VectorType& v3,
            bool treatVertexDataAsTemporary)
   {
      VectorType tv1, tv2, tv3;

      if (!mMatrixIsIdentity)
      {
         tv1 = v1*mMatrix;
         tv2 = v2*mMatrix;
         tv3 = v3*mMatrix;
      }
      else
      {
         tv1 = v1;
         tv2 = v2;
         tv3 = v3;
      }

      if (dtUtil::IsFiniteVec(tv1) && dtUtil::IsFiniteVec(tv2) && dtUtil::IsFiniteVec(tv3))
      {

         std::vector<Triangle> mTriangles;
         Triangle initial(tv1, tv2, tv3);

         mTriangles.push_back(initial);

         Triangle newT(initial);

         for (size_t i = 0; i < mTriangles.size(); ++i)
         {
            int indexBase = int(mVertices.size());

            Triangle t = mTriangles[i];
            while (t.SplitIf(20.0f, newT))
            {
               mTriangles.push_back(newT);
            }

            mTriangles[i] = t;

            mIndices.push_back(indexBase);
            mIndices.push_back(indexBase + 1);
            mIndices.push_back(indexBase + 2);

            mVertices.push_back(t.mV1);
            mVertices.push_back(t.mV2);
            mVertices.push_back(t.mV3);
         }
      }
      else
      {
         std::ostringstream ss;
         ss << "Found non-finite triangle data.  The three vertices of the triangle are \"";
         ss << tv1 << "\", \"" << tv2 << "\", and \"" << tv3 << "\".";
         LOG_ERROR(ss.str());
      }
   }

}

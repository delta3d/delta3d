// splitgeode.cpp: Implementation of the SplitGeode function.
//
//////////////////////////////////////////////////////////////////////

#include "splitgeode.h"

/// @cond DOXYGEN_SHOULD_SKIP_THIS
using namespace std;
using namespace osg;
/// @endcond

/**
 * A small value, used for determining whether a point lies on a plane.
 */
const float epsilon = 0.0001f;


/**
 * A set of vertex attributes.
 */
struct VertexAttributes
{
   /** 
    * The vertices.
    */
   vector<Vec3> mVertices;
   
   /**
    * The normals.
    */
   vector<Vec3> mNormals;
   
   /**
    * The texture coordinates.
    */
   vector< vector<Vec2> > mTextureCoordinates;
   
   
   /**
    * Adds an attribute from another set of vertex attributes.
    *
    * @param attributes the other set of attributes
    * @param index the index of the attribute to add
    */ 
   void addAttribute(const VertexAttributes& attributes, int index)
   {
      mVertices.push_back(attributes.mVertices[index]);
      
      if(attributes.mNormals.size() > 0)
      {
         mNormals.push_back(attributes.mNormals[index]);
      }
      
      if(mTextureCoordinates.size() < attributes.mTextureCoordinates.size())
      {
         mTextureCoordinates.resize(
            attributes.mTextureCoordinates.size()
         );
      }
      
      for(unsigned int i=0;i<attributes.mTextureCoordinates.size();i++)
      {
         if(attributes.mTextureCoordinates[i].size() > 0)
         {
            mTextureCoordinates[i].push_back(
               attributes.mTextureCoordinates[i][index]
            );
         }
      }
   }
   
   /**
    * Adds an attribute between two attributes from another set of
    * vertex attributes.
    *
    * @param attributes the other set of attributes
    * @param index1 the index of the first attribute
    * @param index2 the index of the second attribute
    * @param t the interpolation parameter
    */
   void addAttribute(const VertexAttributes& attributes,
                     int index1, int index2, float t)
   {
      mVertices.push_back(
         attributes.mVertices[index1]*(1-t) + attributes.mVertices[index2]*t
      );
      
      if(attributes.mNormals.size() > 0)
      {
         mNormals.push_back(
            attributes.mNormals[index1]*(1-t) + attributes.mNormals[index2]*t
         );
         
         mNormals.back().normalize();
      }
      
      if(mTextureCoordinates.size() < attributes.mTextureCoordinates.size())
      {
         mTextureCoordinates.resize(
            attributes.mTextureCoordinates.size()
         );
      }
      
      for(unsigned int i=0;i<attributes.mTextureCoordinates.size();i++)
      {
         if(attributes.mTextureCoordinates[i].size() > 0)
         {
            mTextureCoordinates[i].push_back(
               attributes.mTextureCoordinates[i][index1]*(1-t) + 
               attributes.mTextureCoordinates[i][index2]*t
            );
         }
      }
   }
};

/**
 * Splits the primitives of a Drawable.
 */
class SplitFunctor : public Drawable::AttributeFunctor,
                     public osg::PrimitiveFunctor
{
   public:
      
      /**
       * The split plane.
       */
      Plane mPlane;
      
      /**
       * The original vertex attributes.
       */
      VertexAttributes mVertexAttributes;
      
      /**
       * The vertex attributes above the plane.
       */
      VertexAttributes mAboveVertexAttributes;
 
      /**
       * The vertex attributes below the plane.
       */
      VertexAttributes mBelowVertexAttributes;
           
      /**
       * The primitive sets above the plane.
       */
      vector< osg::ref_ptr<PrimitiveSet> > mAbovePrimitiveSets;
      
      /**
       * The primitive sets below the plane.
       */
      vector< osg::ref_ptr<PrimitiveSet> > mBelowPrimitiveSets;
      
      
      /**
       * Applies this functor to an array of Vec2 attributes.
       *
       * @param at the type of the attributes
       * @param count the number of attributes
       * @param attr the attribute array
       */
      virtual void apply(Drawable::AttributeType at, unsigned int count, Vec2* attr)
      {
         if(at >= Drawable::TEXTURE_COORDS_0)
         {
            unsigned int textureUnit = at - Drawable::TEXTURE_COORDS_0;
            
            if(mVertexAttributes.mTextureCoordinates.size() <= textureUnit)
            {
               mVertexAttributes.mTextureCoordinates.resize(textureUnit + 1);
            }
            
            mVertexAttributes.mTextureCoordinates[textureUnit].resize(count);
            
            for(unsigned int i = 0;i<count;i++)
            {
               mVertexAttributes.mTextureCoordinates[textureUnit][i] = attr[i];
            }
         }
      }
      
      /**
       * Applies this functor to an array of Vec3 attributes.
       *
       * @param at the type of the attributes
       * @param count the number of attributes
       * @param attr the attribute array
       */
      virtual void apply(Drawable::AttributeType at, unsigned int count, Vec3* attr)
      {
         if(at == Drawable::NORMALS)
         {
            mVertexAttributes.mNormals.resize(count);
            
            for(unsigned int i=0;i<count;i++)
            {
               mVertexAttributes.mNormals[i] = attr[i];
            }
         }
      }
      
      /**
       * Sets the vertex array to an array of Vec2 vertices.
       *
       * @param count the number of vertices in the array
       * @param vertices the array of vertices
       */
      virtual void setVertexArray(unsigned int count, const Vec2* vertices) {}
      
      /**
       * Sets the vertex array to an array of Vec3 vertices.
       *
       * @param count the number of vertices in the array
       * @param vertices the array of vertices
       */
      virtual void setVertexArray(unsigned int count, const Vec3* vertices)
      {
         mVertexAttributes.mVertices.resize(count);
         
         for(unsigned int i=0;i<count;i++)
         {
            mVertexAttributes.mVertices[i] = vertices[i];
         }
      }
      
      /**
       * Sets the vertex array to an array of Vec4 vertices.
       *
       * @param count the number of vertices in the array
       * @param vertices the array of vertices
       */
      virtual void setVertexArray(unsigned int count, const Vec4* vertices) {}
      
      /**
       * Computes the interpolation parameter corresponding to the intersection
       * of the clipping plane and a line segment.
       *
       * @param v1 the first vertex
       * @param v2 the second vertex
       * @return the plane parameter
       */ 
      float getPlaneParameter(const Vec3& v1, const Vec3& v2)
      {
         return (-mPlane[0]*v1[0] - mPlane[1]*v1[1] - mPlane[2]*v1[2] - mPlane[3]) /
                (mPlane[0]*(v2[0]-v1[0]) + mPlane[1]*(v2[1]-v1[1]) + mPlane[2]*(v2[2]-v1[2]));
      }
      
      /**
       * Draws a batch of primitives based on part of the vertex attribute array
       *
       * @param mode the draw mode
       * @param first the first index
       * @param count the number of elements
       */
      virtual void drawArrays(GLenum mode, GLint first, GLsizei count)
      {
         vector<GLuint> indices;
         
         indices.resize(count);
         
         for(int i=0;i<count;i++)
         {
            indices[i] = first + i;
         }
         
         drawElements(mode, indices.size(), &indices.front());
      }
      
      /**
       * Draws a batch of indexed elements.
       *
       * @param mode the draw mode
       * @param count the number of indices
       * @param indices the array of indices
       */
      virtual void drawElements(GLenum mode, GLsizei count, const GLubyte* indices)
      {
         vector<GLuint> newIndices;
         
         newIndices.resize(count);
         
         for(int i=0;i<count;i++)
         {
            newIndices[i] = indices[i];
         }
         
         drawElements(mode, newIndices.size(), &newIndices.front());
      }
      
      /**
       * Draws a batch of indexed elements.
       *
       * @param mode the draw mode
       * @param count the number of indices
       * @param indices the array of indices
       */
      virtual void drawElements(GLenum mode, GLsizei count, const GLushort* indices)
      {
         vector<GLuint> newIndices;
         
         newIndices.resize(count);
         
         for(int i=0;i<count;i++)
         {
            newIndices[i] = indices[i];
         }
         
         drawElements(mode, newIndices.size(), &newIndices.front());
      }
      
      /**
       * Draws a batch of points.
       *
       * @param indices the indices of the points to draw
       */
      void drawPoints(vector<int>& indices)
      {
         unsigned int prevAbove = mAboveVertexAttributes.mVertices.size(),
                      prevBelow = mBelowVertexAttributes.mVertices.size();
         
         for(unsigned int i=0;i<indices.size();i++)
         {
            float dist = mPlane.distance(
               mVertexAttributes.mVertices[indices[i]]
            );
            
            if(dist < 0.0f)
            {
               mBelowVertexAttributes.addAttribute(
                  mVertexAttributes, indices[i]
               );
            }
            else
            {
               mAboveVertexAttributes.addAttribute(
                  mVertexAttributes, indices[i]
               );
            }
         }
         
         if(mAboveVertexAttributes.mVertices.size() != prevAbove)
         {
            mAbovePrimitiveSets.push_back(
               new DrawArrays(
                  GL_POINTS, 
                  prevAbove,
                  mAboveVertexAttributes.mVertices.size() - prevAbove
               )
            );
         }
         
         if(mBelowVertexAttributes.mVertices.size() != prevBelow)
         {
            mBelowPrimitiveSets.push_back(
               new DrawArrays(
                  GL_POINTS, 
                  prevBelow,
                  mBelowVertexAttributes.mVertices.size() - prevBelow
               )
            );
         }
      }
      
      /**
       * Draws a batch of lines.
       *
       * @param indices the indices of the lines to draw
       */
      void drawLines(vector<int>& indices)
      {
         unsigned int prevAbove = mAboveVertexAttributes.mVertices.size(),
                      prevBelow = mBelowVertexAttributes.mVertices.size();
         
         for(unsigned int i=0;i<indices.size();i+=2)
         {
            float dist1 = mPlane.distance(
               mVertexAttributes.mVertices[indices[i]]
            );
            
            float dist2 = mPlane.distance(
               mVertexAttributes.mVertices[indices[i+1]]
            );
            
            if(dist1 < 0.0f)
            {
               mBelowVertexAttributes.addAttribute(
                  mVertexAttributes, indices[i]
               );
                  
               if(dist2 > 0.0f)
               {
                  float t = getPlaneParameter(
                     mVertexAttributes.mVertices[indices[i]],
                     mVertexAttributes.mVertices[indices[i+1]]
                  );
                  
                  mBelowVertexAttributes.addAttribute(
                     mVertexAttributes, indices[i], indices[i+1], t
                  );
                  
                  mAboveVertexAttributes.addAttribute(
                     mVertexAttributes, indices[i], indices[i+1], t
                  );
                  
                  mAboveVertexAttributes.addAttribute(
                     mVertexAttributes, indices[i+1]
                  );
               }
               else
               {
                  mBelowVertexAttributes.addAttribute(
                     mVertexAttributes, indices[i+1]
                  );
               }
            }
            else
            {
               mAboveVertexAttributes.addAttribute(
                  mVertexAttributes, indices[i]
               );
               
               if(dist2 < 0.0f)
               {
                  float t = getPlaneParameter(
                     mVertexAttributes.mVertices[indices[i]],
                     mVertexAttributes.mVertices[indices[i+1]]
                  );
                  
                  mAboveVertexAttributes.addAttribute(
                     mVertexAttributes, indices[i], indices[i+1], t
                  );
                  
                  mBelowVertexAttributes.addAttribute(
                     mVertexAttributes, indices[i], indices[i+1], t
                  );
                  
                  mBelowVertexAttributes.addAttribute(
                     mVertexAttributes, indices[i+1]
                  );
               }
               else
               {
                  mAboveVertexAttributes.addAttribute(
                     mVertexAttributes, indices[i+1]
                  );
               }
            }
         }
         
         if(mAboveVertexAttributes.mVertices.size() != prevAbove)
         {
            mAbovePrimitiveSets.push_back(
               new DrawArrays(
                  GL_LINES, 
                  prevAbove,
                  mAboveVertexAttributes.mVertices.size() - prevAbove
               )
            );
         }
         
         if(mBelowVertexAttributes.mVertices.size() != prevBelow)
         {
            mBelowPrimitiveSets.push_back(
               new DrawArrays(
                  GL_LINES, 
                  prevBelow,
                  mBelowVertexAttributes.mVertices.size() - prevBelow
               )
            );
         }
      }
      
      /**
       * Represents an interpolated vertex used in the construction of a
       * set of clipped triangles.
       */
      struct InterpolatedVertex
      {
         /**
          * The indices of the vertex.
          */
         int mIndices[2];
         
         /**
          * The interpolation parameter.
          */
         float mT;
      };
      
      /**
       * Draws a batch of polygons.
       *
       * @param indices the indices of the polygons to draw
       */
      void drawPolygons(vector<int>& indices)
      {
         unsigned int prevAbove = mAboveVertexAttributes.mVertices.size(),
                      prevBelow = mBelowVertexAttributes.mVertices.size();
         
         for(unsigned int i=0;i<indices.size();i++)
         {
            vector<int> polyIndices;
            
            vector<InterpolatedVertex> aboveVertices, belowVertices;
            
            bool above = false, below = false;
            
            unsigned int j, k;
            
            for(;indices[i]!=-1;i++)
            {
               float dist = mPlane.distance(
                  mVertexAttributes.mVertices[indices[i]]
               );
               
               if(dist < -epsilon)
               {
                  below = true;
               }
               else if(dist > +epsilon)
               {
                  above = true;
               }
               
               polyIndices.push_back(indices[i]);
            }
            
            if(!above && !below)
            {
               Plane polyPlane;
               
               for(j=0;j<polyIndices.size()-2;j++)
               {
                  polyPlane.set(
                     mVertexAttributes.mVertices[polyIndices[j]],
                     mVertexAttributes.mVertices[polyIndices[j+1]],
                     mVertexAttributes.mVertices[polyIndices[j+2]]
                  );
                  
                  if(polyPlane.getNormal().length() > epsilon)
                  {
                     break;
                  }
               }
               
               if(polyPlane.getNormal()*mPlane.getNormal() > 0.0f)
               {
                  above = true;
               }
               else
               {
                  below = true;
               }
            }
            
            if(above && below)
            {
               for(j=0;j<polyIndices.size();j++)
               {
                  k = (j+1)%polyIndices.size();
               
                  float jDist = mPlane.distance(
                     mVertexAttributes.mVertices[polyIndices[j]]
                  );
               
                  float kDist = mPlane.distance(
                     mVertexAttributes.mVertices[polyIndices[k]]
                  );
               
                  InterpolatedVertex iv;
                  
                  iv.mIndices[0] = iv.mIndices[1] = polyIndices[j];
                  iv.mT = 0.0f;
                     
                  if(jDist < -epsilon)
                  {
                     belowVertices.push_back(iv);
                  
                     if(kDist > +epsilon)
                     {
                        InterpolatedVertex iv2;
                        
                        iv2.mIndices[0] = polyIndices[j];
                        iv2.mIndices[1] = polyIndices[k];
                        
                        iv2.mT = getPlaneParameter(
                           mVertexAttributes.mVertices[polyIndices[j]],
                           mVertexAttributes.mVertices[polyIndices[k]]
                        );
                        
                        aboveVertices.push_back(iv2);
                        belowVertices.push_back(iv2);
                     }
                  }
                  else if(jDist > +epsilon)
                  {
                     aboveVertices.push_back(iv);
                  
                     if(kDist < -epsilon)
                     {
                        InterpolatedVertex iv2;
                        
                        iv2.mIndices[0] = polyIndices[j];
                        iv2.mIndices[1] = polyIndices[k];
                        
                        iv2.mT = getPlaneParameter(
                           mVertexAttributes.mVertices[polyIndices[j]],
                           mVertexAttributes.mVertices[polyIndices[k]]
                        );
                        
                        aboveVertices.push_back(iv2);
                        belowVertices.push_back(iv2);
                     }
                  }
                  else
                  {
                     belowVertices.push_back(iv);
                     aboveVertices.push_back(iv);
                  }
               }
            }
            else if(above)
            {
               for(j=0;j<polyIndices.size();j++)
               {
                  InterpolatedVertex iv;
                  
                  iv.mIndices[0] = iv.mIndices[1] = polyIndices[j];
                  iv.mT = 0.0f;
                  
                  aboveVertices.push_back(iv);
               }
            }
            else if(below)
            {
               for(j=0;j<polyIndices.size();j++)
               {
                  InterpolatedVertex iv;
                  
                  iv.mIndices[0] = iv.mIndices[1] = polyIndices[j];
                  iv.mT = 0.0f;
                  
                  belowVertices.push_back(iv);
               }
            }
            
            if(aboveVertices.size() > 0)
            {
               for(j=1;j<aboveVertices.size()-1;j++)
               {
                  mAboveVertexAttributes.addAttribute(
                     mVertexAttributes, 
                     aboveVertices[0].mIndices[0], 
                     aboveVertices[0].mIndices[1],
                     aboveVertices[0].mT 
                  );
                  
                  mAboveVertexAttributes.addAttribute(
                     mVertexAttributes, 
                     aboveVertices[j].mIndices[0], 
                     aboveVertices[j].mIndices[1],
                     aboveVertices[j].mT 
                  );
                  
                  mAboveVertexAttributes.addAttribute(
                     mVertexAttributes, 
                     aboveVertices[j+1].mIndices[0], 
                     aboveVertices[j+1].mIndices[1],
                     aboveVertices[j+1].mT 
                  );
               }
            }
            
            if(belowVertices.size() > 0)
            {
               for(j=1;j<belowVertices.size()-1;j++)
               {
                  mBelowVertexAttributes.addAttribute(
                     mVertexAttributes, 
                     belowVertices[0].mIndices[0], 
                     belowVertices[0].mIndices[1],
                     belowVertices[0].mT 
                  );
                  
                  mBelowVertexAttributes.addAttribute(
                     mVertexAttributes, 
                     belowVertices[j].mIndices[0], 
                     belowVertices[j].mIndices[1],
                     belowVertices[j].mT 
                  );
                  
                  mBelowVertexAttributes.addAttribute(
                     mVertexAttributes, 
                     belowVertices[j+1].mIndices[0], 
                     belowVertices[j+1].mIndices[1],
                     belowVertices[j+1].mT 
                  );
               }
            }
         }
         
         if(mAboveVertexAttributes.mVertices.size() != prevAbove)
         {
            mAbovePrimitiveSets.push_back(
               new DrawArrays(
                  GL_TRIANGLES, 
                  prevAbove,
                  mAboveVertexAttributes.mVertices.size() - prevAbove
               )
            );
         }
         
         if(mBelowVertexAttributes.mVertices.size() != prevBelow)
         {
            mBelowPrimitiveSets.push_back(
               new DrawArrays(
                  GL_TRIANGLES, 
                  prevBelow,
                  mBelowVertexAttributes.mVertices.size() - prevBelow
               )
            );
         }
      }
      
      /**
       * Draws a batch of indexed elements.
       *
       * @param mode the draw mode
       * @param count the number of indices
       * @param indices the array of indices
       */
      virtual void drawElements(GLenum mode, GLsizei count, const GLuint* indices)
      {
         int i;
         
         vector<int> newIndices;
         
         switch(mode)
         {
            case GL_POINTS:
               for(i=0;i<count;i++)
               {
                  newIndices.push_back(indices[i]);
               }
               drawPoints(newIndices);
               break;
               
            case GL_LINE_STRIP:
               for(i=0;i<count-1;i++)
               {
                  newIndices.push_back(indices[i]);
                  newIndices.push_back(indices[i+1]);
               }
               drawLines(newIndices);
               break;
               
            case GL_LINE_LOOP:
               for(i=0;i<count;i++)
               {
                  newIndices.push_back(indices[i]);
                  newIndices.push_back(indices[(i+1)%count]);
               }
               drawLines(newIndices);
               break;
               
            case GL_LINES:
               for(i=0;i<count;i+=2)
               {
                  newIndices.push_back(indices[i]);
                  newIndices.push_back(indices[i+1]);
               }
               drawLines(newIndices);
               break;
               
            case GL_TRIANGLE_STRIP:
               for(i=0;i<count-2;i++)
               {
                  if(i%2 == 0)
                  {
                     newIndices.push_back(indices[i]);
                     newIndices.push_back(indices[i+1]);
                     newIndices.push_back(indices[i+2]);
                     newIndices.push_back(-1);
                  }
                  else
                  {
                     newIndices.push_back(indices[i+2]);
                     newIndices.push_back(indices[i+1]);
                     newIndices.push_back(indices[i]);
                     newIndices.push_back(-1);
                  }
               }
               drawPolygons(newIndices);
               break;
               
            case GL_TRIANGLE_FAN:
               for(i=1;i<count-1;i++)
               {
                  newIndices.push_back(indices[0]);
                  newIndices.push_back(indices[i]);
                  newIndices.push_back(indices[i+1]);
                  newIndices.push_back(-1);
               }
               drawPolygons(newIndices);
               break;
               
            case GL_TRIANGLES:
               for(i=0;i<count;i+=3)
               {
                  newIndices.push_back(indices[i]);
                  newIndices.push_back(indices[i+1]);
                  newIndices.push_back(indices[i+2]);
                  newIndices.push_back(-1);
               }
               drawPolygons(newIndices);
               break;
               
            case GL_QUAD_STRIP:
               for(i=0;i<count-2;i+=2)
               {
                  if(i%4 == 0)
                  {
                     newIndices.push_back(indices[i]);
                     newIndices.push_back(indices[i+1]);
                     newIndices.push_back(indices[i+2]);
                     newIndices.push_back(indices[i+3]);
                     newIndices.push_back(-1);
                  }
                  else
                  {
                     newIndices.push_back(indices[i+3]);
                     newIndices.push_back(indices[i+2]);
                     newIndices.push_back(indices[i+1]);
                     newIndices.push_back(indices[i]);
                     newIndices.push_back(-1);
                  }
               }
               drawPolygons(newIndices);
               break;
               
            case GL_QUADS:
               for(i=0;i<count;i+=4)
               {
                  newIndices.push_back(indices[i]);
                  newIndices.push_back(indices[i+1]);
                  newIndices.push_back(indices[i+2]);
                  newIndices.push_back(indices[i+3]);
                  newIndices.push_back(-1);
               }
               drawPolygons(newIndices);
               break;
               
            case GL_POLYGON:
               for(i=0;i<count;i++)
               {
                  newIndices.push_back(indices[i]);
               }
               newIndices.push_back(-1);
               drawPolygons(newIndices);
               break;
         }
      }
      
      virtual void begin(GLenum mode) {}
      virtual void vertex(const Vec2& vert) {}
      virtual void vertex(const Vec3& vert) {}
      virtual void vertex(const Vec4& vert) {}
      virtual void vertex(float x, float y) {}
      virtual void vertex(float x, float y, float z) {}
      virtual void vertex(float x, float y, float z, float w) {}
      virtual void end() {}
};


/**
 * Splits a geode across a plane.
 *
 * @param plane the plane to split across
 * @param path the path to the geode
 * @param aboveGeodes the paths to geodes above the plane
 * @param belowGeodes the paths to geodes below the plane
 */
void SplitGeode(const Plane& plane,
                NodePath& path,
                vector<NodePath>& abovePaths,
                vector<NodePath>& belowPaths)
{
   Matrix matrix;
   
   for(NodePath::iterator it = path.begin();
       it != path.end();
       it++)
   {
      Transform* transform = dynamic_cast<Transform*>(*it);
                  
      if(transform != NULL)
      {
         transform->computeLocalToWorldMatrix(matrix, NULL);
      }
   }
   
   Plane transformedPlane = plane;
   
   transformedPlane.transformProvidingInverse(matrix);
   
   Geode* geode = dynamic_cast<Geode*>(path.back());
   
   switch(transformedPlane.intersect(geode->getBound()))
   {
      case -1:
         belowPaths.push_back(path);
         return;
         
      case +1:
         abovePaths.push_back(path);
         return;
   }
   
   unsigned int j;
   
   osg::ref_ptr<Geode> aboveGeode = new Geode,
                  belowGeode = new Geode;
   
   while(geode->getNumDrawables() > 0)
   {
      SplitFunctor splitter;
      
      splitter.mPlane = transformedPlane;
      
      osg::ref_ptr<Drawable> drawable = geode->getDrawable(0);
      
      drawable->accept((Drawable::AttributeFunctor&)splitter);
      drawable->accept((osg::PrimitiveFunctor&)splitter);
      
      geode->removeDrawable(drawable.get());
      
      if(splitter.mAbovePrimitiveSets.size() == 0)
      {
         belowGeode->addDrawable(drawable.get());
      }
      else if(splitter.mBelowPrimitiveSets.size() == 0)
      {
         aboveGeode->addDrawable(drawable.get());
      }
      else
      {
         osg::ref_ptr<Geometry> aboveGeom = new Geometry,
                           belowGeom = new Geometry;
         
         aboveGeom->setStateSet(drawable->getStateSet());
                   
         aboveGeom->setVertexArray(
            new Vec3Array(
               splitter.mAboveVertexAttributes.mVertices.begin(),
               splitter.mAboveVertexAttributes.mVertices.end()
            )
         );
         
         if(splitter.mAboveVertexAttributes.mNormals.size() > 0)
         {
            aboveGeom->setNormalArray(
               new Vec3Array(
                  splitter.mAboveVertexAttributes.mNormals.begin(),
                  splitter.mAboveVertexAttributes.mNormals.end()
               )
            );
         }
         
         for(j=0;j<splitter.mAboveVertexAttributes.mTextureCoordinates.size();j++)
         {
            if(splitter.mAboveVertexAttributes.mTextureCoordinates[j].size() > 0)
            {
               aboveGeom->setTexCoordArray(
                  j,
                  new Vec2Array(
                     splitter.mAboveVertexAttributes.mTextureCoordinates[j].begin(),
                     splitter.mAboveVertexAttributes.mTextureCoordinates[j].end()
                  )
               );
            }
         }
         
         belowGeom->setStateSet(drawable->getStateSet());
         
         belowGeom->setVertexArray(
            new Vec3Array(
               splitter.mBelowVertexAttributes.mVertices.begin(),
               splitter.mBelowVertexAttributes.mVertices.end()
            )
         );
         
         if(splitter.mBelowVertexAttributes.mNormals.size() > 0)
         {
            belowGeom->setNormalArray(
               new Vec3Array(
                  splitter.mBelowVertexAttributes.mNormals.begin(),
                  splitter.mBelowVertexAttributes.mNormals.end()
               )
            );
         }
         
         for(j=0;j<splitter.mBelowVertexAttributes.mTextureCoordinates.size();j++)
         {
            if(splitter.mBelowVertexAttributes.mTextureCoordinates[j].size() > 0)
            {
               belowGeom->setTexCoordArray(
                  j,
                  new Vec2Array(
                     splitter.mBelowVertexAttributes.mTextureCoordinates[j].begin(),
                     splitter.mBelowVertexAttributes.mTextureCoordinates[j].end()
                  )
               );
            }
         }
         
         aboveGeom->getPrimitiveSetList() = splitter.mAbovePrimitiveSets;
         belowGeom->getPrimitiveSetList() = splitter.mBelowPrimitiveSets;
         
         aboveGeode->addDrawable(aboveGeom.get());
         belowGeode->addDrawable(belowGeom.get());
      }
   }

   Group* group = dynamic_cast<Group*>(path[path.size() - 2]);
   
   if(aboveGeode->getNumDrawables() > 0)
   {
      aboveGeode->setStateSet(geode->getStateSet());
      
      group->addChild(aboveGeode.get());

      NodePath abovePath = path;
      
      abovePath.back() = aboveGeode.get();
      
      abovePaths.push_back(abovePath);
   }
   
   if(belowGeode->getNumDrawables() > 0)
   {
      belowGeode->setStateSet(geode->getStateSet());
      
      group->addChild(belowGeode.get());
      
      NodePath belowPath = path;
      
      belowPath.back() = belowGeode.get();
      
      belowPaths.push_back(belowPath);
   }
   
   group->removeChild(geode);
}

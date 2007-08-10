/*
 * 
 * 
 */
#ifndef DELTA_TANGENTSPACEVISITOR
#define DELTA_TANGENTSPACEVISITOR

#include <osg/NodeVisitor>

namespace dtCore
{
   /**
    * This visitor is used to generate tangents for your node.  It will stick the 
    * tangents on a Vertex Attribute Array.  In Open GL Shader Language, each vertex
    * can have a number of attributes (minimum supported is 16 = 0 to 15).  You can use these 
    * attributes to put a single value for each vertex. This visitor generates tangents
    * for each vertex and puts them in the specified array number (0 to 15). If you pass
    * in the shader program, it will also bind the array number to a name, so you can use 
    * it in your Vertex Shader like this: 
    *       attribute vec4 vTangent;
    * Typically, tangents are put in index 6. An example usage looks like this: 
    *       dtCore::RefPtr<TangentSpaceVisitor> visitor = new TangentSpaceVisitor
    *             (defaultShader, 6, "vTangent");
    *       mMyCoolNode->accept(*visitor.get());
    * This visitor was based on osgFx::BumpMapping.cpp 
    */
   class TangentSpaceVisitor: public osg::NodeVisitor 
   {
   public:
      // constructor
      TangentSpaceVisitor(const std::string &vertexAttributeName, 
            osg::Program *shaderProgram = NULL, int tangentVertexAttribNumber = 6);

      // apply - called for each node
      void apply(osg::Geode& geode);

   private:
      std::string mVertexAttributeName;
      osg::Program *mShaderProgram;
      int mTangentVertexAttribNumber;
   };
}

#endif /*DELTA_TANGENTSPACEVISITOR*/

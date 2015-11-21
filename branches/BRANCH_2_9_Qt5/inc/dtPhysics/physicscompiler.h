/* -*-c++-*-
 * testAPP - Using 'The MIT License'
 * Copyright (C) 2014, Caper Holdings LLC
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#ifndef DELTA_PHYSICS_COMPILER_H
#define DELTA_PHYSICS_COMPILER_H

////////////////////////////////////////////////////////////////////////////////
// INCLUDE DIRECTIVES
////////////////////////////////////////////////////////////////////////////////
#include <dtPhysics/physicsexport.h>
#include <dtCore/propertymacros.h>
#include <dtPhysics/geometry.h>
#include <dtPhysics/physicsobject.h>
#include <dtPhysics/physicsmaterials.h>
#include <dtPhysics/trianglerecorder.h>
#include <osg/NodeVisitor>



////////////////////////////////////////////////////////////////////////////////
// FORWARD DECLARATIONS
////////////////////////////////////////////////////////////////////////////////
namespace dtGame
{
   class GameManager;
}



namespace dtPhysics
{
   typedef std::vector<dtCore::RefPtr<dtPhysics::PhysicsObject> > PhysicsObjectArray;



   /////////////////////////////////////////////////////////////////////////////
   // CLASS CODE
   /////////////////////////////////////////////////////////////////////////////
   class DT_PHYSICS_EXPORT PhysicsCompileResult : public osg::Referenced
   {
   public:
      dtCore::RefPtr<dtPhysics::VertexData> mVertData;
      int mPartIndex;
      int mPartTotalInProgress;
      std::string mMaterialName;

      PhysicsCompileResult();

   protected:
      virtual ~PhysicsCompileResult();
   };



   /////////////////////////////////////////////////////////////////////////////
   // TYPE DEFINITIONS
   /////////////////////////////////////////////////////////////////////////////
   typedef std::string VertexDataTableKey;
   typedef std::map<VertexDataTableKey, TriangleRecorder::VertexDataArray> VertexDataTable;

   typedef dtUtil::Functor<std::string, TYPELIST_1(const osg::Node&)> NodeDescriptionSearchFunc;
   typedef dtUtil::Functor<std::string, TYPELIST_1(const std::string&)> FilterStringFunc;
   typedef dtUtil::Functor<dtPhysics::MaterialIndex, TYPELIST_1(const std::string&)> MaterialSearchFunc;
   typedef dtUtil::Functor<void, TYPELIST_1(PhysicsCompileResult&)> GeometryCompiledCallback;



   /////////////////////////////////////////////////////////////////////////////
   // CLASS CODE
   /////////////////////////////////////////////////////////////////////////////
   struct DT_PHYSICS_EXPORT PhysicsCompileOptions
   {
      static const float DEFAULT_MAX_EDGE_LENGTH;
      static const unsigned int DEFAULT_MAX_VERTS_PER_MESH = 300000;

      PhysicsCompileOptions();

      unsigned int mMaxVertsPerMesh;
      float mMaxEdgeLength;
      bool mAllowDefaultMaterial;
      bool mSplitUpGeodes;
   };



   /////////////////////////////////////////////////////////////////////////////
   // CLASS CODE
   /////////////////////////////////////////////////////////////////////////////
   struct DT_PHYSICS_EXPORT PhysicsObjectOptions
   {
      static const dtPhysics::Real DEFAULT_COLLISION_MARGIN;
      static const dtPhysics::Real DEFAULT_MASS;
      static PrimitiveType *const DEFAULT_PRIMITIVE_TYPE;
      static MechanicsType *const DEFAULT_MECHANICS_TYPE;

      PhysicsObjectOptions();

      // Geometry & Object Setup Options
      PrimitiveType* mPrimitiveType;
      MechanicsType* mMechanicsType;
      bool mIsPolytope; // For convex hull only
      bool mClearExistingObjects;
      dtPhysics::Real mMass;
      dtPhysics::Real mCollisionMargin;
      dtPhysics::VectorType mDimensions;
   };


   ////////////////////////////////////////////////////////////////////////////////
   // CLASS CODE
   ////////////////////////////////////////////////////////////////////////////////
   class DT_PHYSICS_EXPORT NodeDescriptionCollector : public osg::NodeVisitor
   {
   public:
      typedef osg::NodeVisitor BaseClass;

      NodeDescriptionCollector(NodeDescriptionSearchFunc func);

      virtual ~NodeDescriptionCollector();

      virtual void apply(osg::Node& node);

      virtual void apply(osg::Billboard& node);
      
      NodeDescriptionSearchFunc mNodeDescSearchFunc;
      unsigned mNodesWithDescriptionsCount;
      std::set<std::string> mDescriptionList;
   };



   /////////////////////////////////////////////////////////////////////////////
   // CLASS CODE
   /////////////////////////////////////////////////////////////////////////////
   class DT_PHYSICS_EXPORT PhysicsCompiler : public osg::Referenced
   {
   public:
      PhysicsCompiler();

      /**
       * Sets the default material and material index to use for compiled
       * physics geometries for which requested materials cannot be found.
       */
      DT_DECLARE_ACCESSOR(std::string, DefaultMaterialName)

      /**
       * Set whether the GameManager actor search method will be used for
       * locating physics materials.
       * @param gm GameManager to be used for locating material actors.
       */
      void SetMaterialSearchByActor(dtGame::GameManager* gm);

      /**
       * Determines if the GameManager actor search method will be used for
       * locating physics materials.
       * @return TRUE if the GameManager actor search method will be used.
       */
      bool IsMaterialSearchByActor() const;

      /**
       * Specialized method that will search for a material index by material name
       * using the GameManager actor search method.
       * @param materialName Unique name of the physics material to be found.
       * @return Index of the material if found; -1 if not found.
       */
      dtPhysics::MaterialIndex GetMaterialIndexByMaterialActor(const std::string& materialName) const;

      /**
       * Specialized method that will search for a material index by material name
       * using the physics world search method.
       * @param materialName Unique name of the physics material to be found.
       * @return Index of the material if found; -1 if not found.
       */
      dtPhysics::MaterialIndex GetMaterialIndexByPhysicsWorld(const std::string& materialName) const;

      /**
       * Primary method that will search for a material index by material name
       * using the search mode that had been set for this object.
       * @param materialName Unique name of the physics material to be found.
       * @return Index of the material if found; -1 if not found.
       */
      dtPhysics::MaterialIndex GetMaterialIndex(const std::string& materialName) const;

      /**
       * Gets the default material index that will be assigned to compiled
       * physics geometries for which requested materials cannot be found.
       * @return Index of the default physics material.
       */
      dtPhysics::MaterialIndex GetDefaultMaterialIndex() const;

      /**
       * Convenience method for determining the material associated with
       * the specified geometry data.
       * @param geometry Geometric data that may have material data.
       * @return Name of the physics material associated with the geometry data.
       */
      std::string GetMaterialNameForGeometry(const VertexData& geometry) const;

      /**
       * Convenience method for acquiring the material object associated with
       * the specified geometry data.
       * @param geometry Geometric data that may have material data.
       * @return Index of the physics material associated with the geometry data.
       */
      dtPhysics::Material* GetMaterialForGeometry(const VertexData& geometry) const;

      /**
       * Sets a custom string filter function that attempts parsing a valid
       * material name from node description strings.
       * @param filterFunc Custom function to filter processed node descriptions.
       */
      void SetNodeDescriptionFilter(FilterStringFunc filterFunc);
   
      /**
       * Convenience method for filtering and trimming a material name
       * from a specified string, which typically will come from a node description.
       * @return Parsed and trimmed material name.
       */
      std::string GetMaterialNameFiltered(const std::string& nodeCommentString) const;

      /**
       * Convenience method for searching for node description strings.
       * @return Parsed and trimmed material name.
       */
      std::string GetMaterialName(const osg::Node& node) const;
   
      /**
       * Convenience method for acquiring all description strings of a specified node.
       * @param node Node to be searched for description strings.
       * @param outDescriptions Container to capture the acquire description strings.
       * @return Number of description strings acquired.
       */
      int GetNodeDescriptions(osg::Node& node,
         std::set<std::string>& outDescriptions) const;

      /**
       * Attempts compilating physics geometries for a specified node.
       * @param node Root node of a mesh model for which to compile physics geometries.
       * @param options Struct that contains parameters that control the compilation results.
       * @param outData Container to capture the compiled physics geometries, keyed on material name.
       * @return Number of geometries compiled.
       */
      int CompilePhysicsForNode(osg::Node& node,
         const PhysicsCompileOptions& options,
         VertexDataTable& outData);

      /**
       * Attempts compilating physics geometries for a specified node for a specified material.
       * @param node Root node of a mesh model for which to compile physics geometries.
       * @param options Struct that contains parameters that control the compilation results.
       * @param materialName Name of the explicit material for which to compile geometries.
       * @param outData Container to capture the compiled physics geometries, keyed on material name.
       * @return Number of geometries compiled.
       */
      int CompilePhysicsForNodeMaterial(osg::Node& node,
         const PhysicsCompileOptions& options,
         const std::string& materialName,
         VertexDataTable& outData);

      /**
       * Sets a custom method to be called when a geometry has finished compiling.
       * The specified callback will be passed a PhysicsCompileResult struct that
       * contains the compiled geometry, along with other information relevant to
       * to compilation of the geometry.
       * @param geomCompiledCallback Custom callback method to be called when a geometry has finished compiling.
       */
      void SetGeometryCompiledCallback(GeometryCompiledCallback geomCompiledCallback);

      /**
       * Convenience method for general setup of physics objects
       * via physics geometry as primary input.
       * @param options Collection of object/geometry creation parameters.
       * @param vertData Collection of geometry data with which to create physics objects.
       * @param outObjects Collection to capture the generated physics objects.
       * @return Number of objects created.
       */
      int CreatePhysicsObjectsForGeometry(const PhysicsObjectOptions& options,
         TriangleRecorder::VertexDataArray& vertData, PhysicsObjectArray& outObjects) const;
      
      /**
       * Method for creating geometry using options.
       * @param options Collection of object/geometry creation parameters.
       * @param xform Position and orientation for the new geometry.
       * @param vertData* Geometry data with which to create a geometry object; this is used for complex shapes such as terrain.
       * @return New geometry object; NULL if creation failed.
       */
      dtCore::RefPtr<Geometry> CreateGeometry(
         const PhysicsObjectOptions& options,
         const TransformType& xform,
         VertexData* vertData = NULL) const;

   protected:
      virtual ~PhysicsCompiler();

      /**
       * Default convenience method for filtering and trimming a material name
       * from a specified string, which typically will come from a node description.
       * @return Parsed and trimmed material name.
       */
      std::string GetMaterialNameFiltered_Internal(const std::string& nodeCommentString) const;

      MaterialSearchFunc mMatSearchFunc;
      FilterStringFunc mNodeDescFilterFunc;
      GeometryCompiledCallback mGeometryCompiledCallback;

      dtPhysics::MaterialIndex mDefaultMatId;
      dtCore::ObserverPtr<dtGame::GameManager> mGM;
   };

}

#endif

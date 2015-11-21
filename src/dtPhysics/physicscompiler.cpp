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

////////////////////////////////////////////////////////////////////////////////
// INCLUDE DIRECTIVES
////////////////////////////////////////////////////////////////////////////////
#include <dtPhysics/physicscompiler.h>
#include <dtGame/gamemanager.h>
#include <dtPhysics/palphysicsworld.h>
#include <dtPhysics/physicsactorregistry.h>
#include <dtPhysics/physicsmaterialactor.h>
#include <dtPhysics/trianglerecorder.h>
#include <dtPhysics/trianglerecordervisitor.h>
#include <dtUtil/log.h>
#include <dtUtil/stringutils.h>
#include <pal/pal.h> // for Material



namespace dtPhysics
{
   ////////////////////////////////////////////////////////////////////////////////
   // TYPE DEFINITIONS
   ////////////////////////////////////////////////////////////////////////////////
   typedef dtPhysics::TriangleRecorderVisitor<dtPhysics::TriangleRecorder> TriangleVisitor;

   ////////////////////////////////////////////////////////////////////////////////
   // TYPE DEFINITIONS
   ////////////////////////////////////////////////////////////////////////////////
   PhysicsCompileResult::PhysicsCompileResult()
      : mPartIndex(0)
      , mPartTotalInProgress(0)
   {}

   PhysicsCompileResult::~PhysicsCompileResult()
   {}



   ////////////////////////////////////////////////////////////////////////////////
   // CLASSS CODE
   ////////////////////////////////////////////////////////////////////////////////
   const float PhysicsCompileOptions::DEFAULT_MAX_EDGE_LENGTH = 20.0f;

   PhysicsCompileOptions::PhysicsCompileOptions()
      : mMaxVertsPerMesh(DEFAULT_MAX_VERTS_PER_MESH)
      , mMaxEdgeLength(DEFAULT_MAX_EDGE_LENGTH)
      , mAllowDefaultMaterial(true)
      , mSplitUpGeodes(false)
   {}



   /////////////////////////////////////////////////////////////////////////////
   // CLASS CODE
   /////////////////////////////////////////////////////////////////////////////
   const dtPhysics::Real PhysicsObjectOptions::DEFAULT_COLLISION_MARGIN(0.02);
   const dtPhysics::Real PhysicsObjectOptions::DEFAULT_MASS(1.0);
   PrimitiveType* const PhysicsObjectOptions::DEFAULT_PRIMITIVE_TYPE = &PrimitiveType::TRIANGLE_MESH;
   MechanicsType* const PhysicsObjectOptions::DEFAULT_MECHANICS_TYPE = &MechanicsType::STATIC;

   PhysicsObjectOptions::PhysicsObjectOptions()
      : mPrimitiveType(DEFAULT_PRIMITIVE_TYPE)
      , mMechanicsType(DEFAULT_MECHANICS_TYPE)
      , mIsPolytope(true)
      , mClearExistingObjects(true)
      , mMass(DEFAULT_MASS)
      , mCollisionMargin(DEFAULT_COLLISION_MARGIN)
      , mDimensions()
   {}


   /////////////////////////////////////////////////////////////////////////////
   // CLASS CODE
   /////////////////////////////////////////////////////////////////////////////
   NodeDescriptionCollector::NodeDescriptionCollector(NodeDescriptionSearchFunc func)
      : BaseClass(osg::NodeVisitor::TRAVERSE_ALL_CHILDREN)
      , mNodeDescSearchFunc(func)
      , mNodesWithDescriptionsCount(0)
   {}

   NodeDescriptionCollector::~NodeDescriptionCollector()
   {}

   void NodeDescriptionCollector::apply(osg::Node& node)
   {
      if ( ! mNodeDescSearchFunc.valid())
      {
         return;
      }

      std::string desc = mNodeDescSearchFunc(node);

      if ( ! desc.empty())
      {
         mDescriptionList.insert(desc);

         ++mNodesWithDescriptionsCount;
      }

      traverse(node);
   }

   void NodeDescriptionCollector::apply(osg::Billboard& node)
   {
      //do nothing
   }



   /////////////////////////////////////////////////////////////////////////////
   // CLASS CODE
   /////////////////////////////////////////////////////////////////////////////
   PhysicsCompiler::PhysicsCompiler()
      : mDefaultMatId(0)
   {
      // Ensure the default material search mode function is set.
      SetMaterialSearchByActor(NULL);

      // Ensure the default string filtering method is set.
      FilterStringFunc filterFunc = FilterStringFunc(this, &PhysicsCompiler::GetMaterialNameFiltered_Internal);
      SetNodeDescriptionFilter(filterFunc);

      mDefaultMaterialName = PhysicsMaterials::DEFAULT_MATERIAL_NAME;
   }

   PhysicsCompiler::~PhysicsCompiler()
   {}

   void PhysicsCompiler::SetMaterialSearchByActor(dtGame::GameManager* gm)
   {
      mGM = gm;

      if (mGM.valid())
      {
         mMatSearchFunc = MaterialSearchFunc(this, &PhysicsCompiler::GetMaterialIndexByMaterialActor);
      }
      else // default search by physics world
      {
         mMatSearchFunc = MaterialSearchFunc(this, &PhysicsCompiler::GetMaterialIndexByPhysicsWorld);
      }
   }

   bool PhysicsCompiler::IsMaterialSearchByActor() const
   {
      return mGM.valid();
   }

   dtPhysics::MaterialIndex PhysicsCompiler::GetMaterialIndex(const std::string& materialName) const
   {
      return mMatSearchFunc(materialName);
   }

   dtPhysics::MaterialIndex PhysicsCompiler::GetMaterialIndexByMaterialActor(const std::string& materialName) const
   {
      dtPhysics::MaterialIndex index = mDefaultMatId;

      if(mGM.valid() && !materialName.empty())
      {
         // The string for material name could be a key/value pair.
         // Use GetMaterialNameFiltered to ensure the key and assignment oprator
         // are removed and white space trimmed.
         std::string desc(GetMaterialNameFiltered(materialName));

         typedef std::vector<dtCore::ActorProxy* > ActorArray;
         ActorArray actors;

         mGM->FindActorsByType(*dtPhysics::PhysicsActorRegistry::PHYSICS_MATERIAL_ACTOR_TYPE, actors);

         dtPhysics::MaterialActor* material = NULL;
         if ( ! actors.empty())
         {
            ActorArray::iterator iter = actors.begin();
            ActorArray::iterator iterEnd = actors.end();

            for (; iter != iterEnd; ++iter)
            {
               material = dynamic_cast<dtPhysics::MaterialActor*>(*iter);
               if(material != NULL && material->GetName() == desc)
               {
                  index = dtPhysics::MaterialIndex(material->GetMaterialDef().GetMaterialIndex());
                  break;
               }
            }
         }

         if (material == NULL)
         {
            LOG_WARNING("Could not find physics MaterialActor with the name: " + desc + ".");
         }
      }

      return index;
   }

   dtPhysics::MaterialIndex PhysicsCompiler::GetMaterialIndexByPhysicsWorld(const std::string& materialName) const
   {
      dtPhysics::MaterialIndex index = mDefaultMatId;

      if ( ! materialName.empty())
      {
         // The string for material name could be a key/value pair.
         // Use GetMaterialNameFiltered to ensure the key and assignment operator
         // are removed and white space trimmed.
         std::string matName(GetMaterialNameFiltered(materialName));

         // If the physics world exists...
         if (dtPhysics::PhysicsWorld::IsInitialized())
         {
            dtPhysics::PhysicsWorld& world = dtPhysics::PhysicsWorld::GetInstance();

            // ...get the requested material by name.
            dtPhysics::Material* mat = world.GetMaterials().GetMaterial(matName);

            if (mat != NULL)
            {
               index = mat->GetId();
            }
         }
      }

      return index;
   }

   void PhysicsCompiler::SetDefaultMaterialName(const std::string& materialName)
   {
      mDefaultMaterialName = materialName;
      mDefaultMatId = GetMaterialIndex(mDefaultMaterialName);
   }

   const std::string& PhysicsCompiler::GetDefaultMaterialName() const
   {
      return mDefaultMaterialName;
   }

   dtPhysics::MaterialIndex PhysicsCompiler::GetDefaultMaterialIndex() const
   {
      return mDefaultMatId;
   }

   std::string PhysicsCompiler::GetMaterialNameForGeometry(const VertexData& geometry) const
   {
      std::string matName;

      if (geometry.GetMaterialCount() > 0)
      {
         matName = geometry.GetMaterialName(geometry.GetFirstMaterialIndex());
      }

      return matName;
   }

   void PhysicsCompiler::SetNodeDescriptionFilter(FilterStringFunc filterFunc)
   {
      if (filterFunc.valid())
      {
         mNodeDescFilterFunc = filterFunc;
      }
      else // Set the default method
      {
         mNodeDescFilterFunc = FilterStringFunc(this, &PhysicsCompiler::GetMaterialNameFiltered_Internal);
      }
   }

   std::string PhysicsCompiler::GetMaterialNameFiltered(const std::string& nodeDescription) const
   {
      return mNodeDescFilterFunc(nodeDescription);
   }

   std::string PhysicsCompiler::GetMaterialNameFiltered_Internal(const std::string& propertyString) const
   {
      std::string matName(propertyString);

      // Determine if a physics property name string needs to be removed.
      static const std::string PROP_ASSIGNMENT(" = ");
      size_t foundIndex = propertyString.find(PROP_ASSIGNMENT);
      if (foundIndex != std::string::npos)
      {
         foundIndex += PROP_ASSIGNMENT.size();
         matName = propertyString.substr(foundIndex);
         matName = dtUtil::Trim(matName);
      }

      return matName;
   }

   std::string PhysicsCompiler::GetMaterialName(const osg::Node& node) const
   {
      std::string desc;

      if ( ! node.getDescriptions().empty())
      {
         // Use the last description as material tag
         desc = node.getDescription(node.getNumDescriptions()-1);
      }

      return desc;
   }

   int PhysicsCompiler::GetNodeDescriptions(osg::Node& node,
      std::set<std::string>& outDescriptions) const
   {
      NodeDescriptionSearchFunc getMatNameFunc
         = NodeDescriptionSearchFunc(this, &PhysicsCompiler::GetMaterialName);

      // Gather all the node descriptions contained in the node tree.
      // The descriptions will be used as material names.
      NodeDescriptionCollector cdv(getMatNameFunc);
      node.accept(cdv);
      outDescriptions = cdv.mDescriptionList;

      return (int)outDescriptions.size();
   }

   int PhysicsCompiler::CompilePhysicsForNode(osg::Node& node,
      const PhysicsCompileOptions& options, VertexDataTable& outData)
   {
      int results = 0;

      std::set<std::string> matNameList;
      GetNodeDescriptions(node, matNameList);
      
      // Determine if the default material should be allowed.
      if (options.mAllowDefaultMaterial)
      {
         // This will trigger use of default material for unmarked nodes.
         matNameList.insert("");
      }

      // For each description (used as material name), create a separate physics mesh.
      std::set<std::string>::iterator iter = matNameList.begin();
      std::set<std::string>::iterator iterEnd = matNameList.end();
      for(; iter != iterEnd; ++iter)
      {
         std::string materialName = (*iter);

         results += CompilePhysicsForNodeMaterial(node, options, materialName, outData);
      }

      return results;
   }

   int PhysicsCompiler::CompilePhysicsForNodeMaterial(osg::Node& node,
      const PhysicsCompileOptions& options, const std::string& materialName, VertexDataTable& outData)
   {
      int results = 0;

      // Determine if default material compilation is allowed.
      // Default material use can be triggered by using an empty string.
      if ( ! options.mAllowDefaultMaterial && materialName.empty())
      {
         return results;
      }

      std::string matName(materialName);

      // Define the function for searching descriptions on nodes.
      NodeDescriptionSearchFunc getMatNameFunc
         = NodeDescriptionSearchFunc(this, &PhysicsCompiler::GetMaterialName);

      // Define the function for TriangleRecorders to use for determining
      // node material indices.
      dtPhysics::TriangleRecorder::MaterialLookupFunc materialLookup(this, &PhysicsCompiler::GetMaterialIndex);
      dtPhysics::TriangleRecorder::MaterialNameFilterFunc matNameFilter(this, &PhysicsCompiler::GetMaterialNameFiltered);

      TriangleVisitor mv(materialLookup);
      mv.mMaterialNameFilter = matNameFilter;
      mv.mFunctor.SetMaxEdgeLength(options.mMaxEdgeLength);
      mv.mFunctor.SetMaxSizePerBuffer(options.mMaxVertsPerMesh);
      mv.mExportSpecificMaterial = true;
      // The material name (node description) remains the same as found on a node.
      // The description could be a key/value pair string, depending how the
      // information was exported from an art tool, such as 3DS Max.
      mv.mSpecificDescription = materialName; // use original string

      // Search for geodes related to the current material name.
      node.accept(mv);

      // Ensure a valid name.
      if ( matName.empty() )
      {
         matName = mDefaultMaterialName;
      }

      // The material name (from a node description) may be a key/value pair.
      // Ensure that the key and delimiter are removed and whitespace trimmed.
      matName = GetMaterialNameFiltered(matName);
      
      // Acquire a container for the current material for capturing new data.
      TriangleRecorder::VertexDataArray* outVertArray = &outData[matName];

      for (unsigned i = 0, iend = mv.mFunctor.mData.size(); i < iend; ++i)
      {
         VertexData* vertData = mv.mFunctor.mData[i];

         if (vertData->mIndices.empty())
         {
            // DEBUG:
            LOG_ERROR("Error cooking mesh for material: " + matName);
         }
         else
         {
            outVertArray->push_back(vertData);
            ++results;

            // Notify that a geometry was compiled.
            if (mGeometryCompiledCallback.valid())
            {
               dtCore::RefPtr<PhysicsCompileResult> result = new PhysicsCompileResult;
               result->mPartIndex = int(i);
               result->mPartTotalInProgress = int(iend);
               result->mMaterialName = matName;
               result->mVertData = vertData;

               mGeometryCompiledCallback(*result);
            }
         }
      }

      // Remove the table row if it is empty.
      if (outVertArray->empty())
      {
         VertexDataTable::iterator foundIter = outData.find(matName);
         if (foundIter != outData.end() && foundIter->second.empty())
         {
            outData.erase(foundIter);
            outVertArray = NULL;
         }
      }

      return results;
   }

   void PhysicsCompiler::SetGeometryCompiledCallback(GeometryCompiledCallback geomCompiledCallback)
   {
      mGeometryCompiledCallback = geomCompiledCallback;
   }

   int PhysicsCompiler::CreatePhysicsObjectsForGeometry(
      const PhysicsObjectOptions& options,
      TriangleRecorder::VertexDataArray& vertData,
      PhysicsObjectArray& outObjects) const
   {
      int results = 0;
            
      PrimitiveType* primType = options.mPrimitiveType;
      MechanicsType* mechType = options.mMechanicsType;

      std::string idxBuffer;
      VertexData* curData = NULL;
      unsigned padding = unsigned(std::log10(float(vertData.size())));
      TriangleRecorder::VertexDataArray::const_iterator curIter = vertData.begin();
      TriangleRecorder::VertexDataArray::const_iterator endIter = vertData.end();
      for (; curIter != endIter; ++curIter)
      {
         curData = curIter->get();

         dtCore::Transform xform;
         dtCore::RefPtr<dtPhysics::Geometry> geom
            = CreateGeometry(options, xform, curData);

         if (geom.valid())
         {
            dtUtil::MakeIndexString(unsigned(results), idxBuffer, padding);
            dtCore::RefPtr<PhysicsObject> po = PhysicsObject::CreateNew("PhysicsObject" + idxBuffer);

            std::string matName = GetMaterialNameForGeometry(*curData);

            if ( ! curData->mOutputFile.IsEmpty())
            {
               po->SetMeshResource(curData->mOutputFile);
            }

            po->SetPrimitiveType(*primType);
            po->SetMechanicsType(*mechType);
            po->SetMaterialByName(matName);
            po->CreateFromGeometry(*geom);

            outObjects.push_back(po.get());
         
            ++results;
         }
         else
         {
            LOG_ERROR("Could not create physics object.");
         }
      }

      return results;
   }

   dtCore::RefPtr<Geometry> PhysicsCompiler::CreateGeometry(
      const PhysicsObjectOptions& options,
      const TransformType& xform,
      VertexData* vertData) const
   {
      dtCore::RefPtr<Geometry> geom;

      const PrimitiveType& primType = *options.mPrimitiveType;
      Real mass = options.mMass;
      const VectorType& dimensions = options.mDimensions;

      // --- SIMPLE TYPES --- //
      if (PrimitiveType::BOX == primType)
      {
         geom = Geometry::CreateBoxGeometry(xform, dimensions, mass);
      }
      else if(PrimitiveType::SPHERE == primType)
      {
         geom = Geometry::CreateSphereGeometry(xform, dimensions[0], mass);
      }
      else if(PrimitiveType::CYLINDER == primType)
      {
         geom = Geometry::CreateCylinderGeometry(xform, dimensions[0], dimensions[1], mass);
      }
      else if(PrimitiveType::CAPSULE == primType)
      {
         geom = Geometry::CreateCapsuleGeometry(xform, dimensions[0], dimensions[1], mass);
      }
      else if (vertData != NULL)// --- COMPLEX TYPES --- //
      {
         if(PrimitiveType::CONVEX_HULL == primType)
         {
            geom = Geometry::CreateConvexGeometry(xform, *vertData, mass, options.mIsPolytope);
         }
         else if(PrimitiveType::TRIANGLE_MESH == primType)
         {
            geom = Geometry::CreateConcaveGeometry(xform, *vertData, mass);
         }
         else if (PrimitiveType::TERRAIN_MESH == primType)
         {
            geom = Geometry::CreateConcaveGeometry(xform, *vertData, mass);
         }
      }

      if (geom.valid())
      {
         geom->SetMargin(options.mCollisionMargin);
      }
      else
      {
         LOG_WARNING("Could not create geometry. Check options or vertex data.");
      }

      return geom;
   }

} // END - namespace dtPhysics

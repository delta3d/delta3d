/* -*-c++-*-
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2006, Alion Science and Technology, BMH Operation
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
 * David Guthrie
 * Bradley Anderegg
 * Allen Danklefsen
 */

//Must include this before pal.
#include <dtCore/enginepropertytypes.h>
#include <dtCore/propertymacros.h>

#include <dtPhysics/physicsobject.h>
#include <dtPhysics/physicsmaterialactor.h>
#include <dtPhysics/physicsactcomp.h>
#include <dtPhysics/palphysicsworld.h>
#include <dtPhysics/bodywrapper.h>
#include <dtPhysics/geometry.h>
#include <dtPhysics/physicsreaderwriter.h>
#include <dtGame/gameactor.h>
#include <dtCore/project.h>
#include <dtUtil/mathdefines.h>
#include <dtPhysics/palutil.h>

#include <dtPhysics/jointdesc.h>
#include <dtPhysics/jointtype.h>

#include <osg/ComputeBoundsVisitor>

#include <pal/palBodyBase.h>
#include <pal/palActivation.h>
#include <pal/palBodies.h>
#include <pal/palFactory.h>
#include <pal/palLinks.h>

#include <cstring>

namespace dtPhysics
{

   struct PhysicsObjectDataMembers
   {
      PhysicsObjectDataMembers(const std::string& name)
      :  mMechanicsType(&MechanicsType::DYNAMIC)
      ,  mCollisionGroup(0)
      ,  mPrimitiveType(&PrimitiveType::SPHERE)
      ,  mExtents(Real(-1.0), Real(-1.0), Real(-1.0))
      ,  mOriginOffset(Real(0.0), Real(0.0), Real(0.0))
      ,  mMomentOfInertia(Real(-1.0), Real(-1.0), Real(-1.0))
      ,  mName(name)
      ,  mMassOfObject(Real(1.0))
      ,  mSkinThickness(Real(0.02))
      ,  mTransform()
      ,  mNotifyCollisions(false)
      ,  mCollisionResponseEnabled(true)
      ,  mVisualToBodyIsIdentity(true)
      ,  mMaterialId(false)
      ,  mActivationLinearVelocityThreshold(Real(-1.0))
      ,  mActivationAngularVelocityThreshold(Real(-1.0))
      ,  mActivationTimeThreshold(Real(-1.0))
      ,  mLinearDamping(Real(0.01))
      ,  mAngularDamping(Real(0.01))
      ,  mActivationSettings(nullptr)
      ,  mMeshScale(Real(1.0), Real(1.0), Real(1.0))
      {
      }
      // the mechanics enum  that can say what type of physics object is in the world
      // and how it related to everyone else
      MechanicsType* mMechanicsType;

      // the collision group for the object.  This is a number, not a mask.
      // In some physics engines it is called a scene. objects in different groups
      // will not collide with each other unless the groups marked to do so via a call on the
      // PhysicsWorld
      CollisionGroup mCollisionGroup;

      // enum for type
      PrimitiveType* mPrimitiveType;

      // so we can go up the chain
      dtCore::ObserverPtr<osg::Referenced> mUserData;

      // extents of the object?
      VectorType mExtents;

      // offset of geometry from the origin, used to offset the center of mass.
      VectorType mOriginOffset;

      // The moment of inertia tensor.
      VectorType mMomentOfInertia;

      // name of the physics object
      dtUtil::RefString mName;

      // the implementation can not always hold onto this.
      Real mMassOfObject;

      // the implementation can not always hold onto this.
      Real mSkinThickness;

      // we need to make a copy of this for stage.
      TransformType mTransform;

      MatrixType mVisualToBodyTransform;

      bool mNotifyCollisions;
      bool mCollisionResponseEnabled;
      bool mVisualToBodyIsIdentity;

      dtCore::UniqueId mMaterialId;

      Real mActivationLinearVelocityThreshold, mActivationAngularVelocityThreshold, mActivationTimeThreshold;
      Real mLinearDamping, mAngularDamping;

      dtCore::RefPtr<GenericBodyWrapper> mGenericBody;
      palActivationSettings* mActivationSettings;

      dtCore::ResourceDescriptor mMeshResource;
      VectorType mMeshScale;

      std::vector<dtCore::RefPtr<Geometry> > mGeometries;

      void CreateSimpleGeometry(const PrimitiveType& primType, const VectorType& dimensions,
            const TransformType& geomWorldTransform,
            Real mass)
      {

         dtCore::RefPtr<Geometry> geom;
         if (PrimitiveType::BOX == primType)
         {
            geom = Geometry::CreateBoxGeometry(geomWorldTransform, dimensions, mass);
         }
         else if(PrimitiveType::SPHERE == primType)
         {
            geom = Geometry::CreateSphereGeometry(geomWorldTransform, dimensions[0], mass);
         }
         else if(PrimitiveType::CYLINDER == primType)
         {
            geom = Geometry::CreateCylinderGeometry(geomWorldTransform, dimensions[0], dimensions[1], mass);
         }
         else if(PrimitiveType::CAPSULE == primType)
         {
            geom = Geometry::CreateCapsuleGeometry(geomWorldTransform, dimensions[0], dimensions[1], mass);
         }

         if (geom == nullptr)
         {
            LOG_ERROR("Unsupported primitive type passed to CreateSimpleGeometry: " + primType.GetName());
            return;
         }

         geom->SetMargin(mSkinThickness);

         mGeometries.push_back(geom);
         mGenericBody->ConnectGeometry(*geom);
      }


      void CreateComplexGeometry(const PrimitiveType& primType,
            const TransformType& geomWorldTransform,
            VertexData& vertexData,
            Real mass)
      {
         dtCore::RefPtr<Geometry> geom;
         if(PrimitiveType::CONVEX_HULL == primType)
         {
            geom = Geometry::CreateConvexGeometry(geomWorldTransform, vertexData, mass, true);
         }
         else if(PrimitiveType::TRIANGLE_MESH == primType)
         {
            geom = Geometry::CreateConcaveGeometry(geomWorldTransform, vertexData, mass);
         }
         else if (PrimitiveType::TERRAIN_MESH == primType)
         {
            geom = Geometry::CreateConcaveGeometry(geomWorldTransform, vertexData, mass);
         }

         geom->SetMargin(mSkinThickness);

         mGeometries.push_back(geom);
         mGenericBody->ConnectGeometry(*geom);
      }

      void ApplyActivationSettings()
      {
         if (mActivationSettings != nullptr)
         {
            if (mActivationLinearVelocityThreshold > -FLT_EPSILON)
            {
               mActivationSettings->SetActivationLinearVelocityThreshold(Float(mActivationLinearVelocityThreshold));
            }
            if (mActivationAngularVelocityThreshold > -FLT_EPSILON)
            {
               mActivationSettings->SetActivationAngularVelocityThreshold(Float(mActivationAngularVelocityThreshold));
            }
            if (mActivationTimeThreshold > -FLT_EPSILON)
            {
               mActivationSettings->SetActivationTimeThreshold(Float(mActivationTimeThreshold));
            }
         }
      }
   };
   /////////////////////////////////////////////////////////////////////////////
   // Physics Object
   /////////////////////////////////////////////////////////////////////////////

   /////////////////////////////////////////////////////////////////////////////
   PhysicsObject::PhysicsObject(const std::string& name)
   :  mDataMembers(new PhysicsObjectDataMembers(name))
   {
   }

   /////////////////////////////////////////////////////////////////////////////
   PhysicsObject::PhysicsObject()
   :  mDataMembers(new PhysicsObjectDataMembers("Default"))
   {
   }

   dtCore::RefPtr<dtCore::ObjectType> PhysicsObject::TYPE(new dtCore::ObjectType("PhysicsObject", "dtPhysics"));

   /*override*/ const dtCore::ObjectType& PhysicsObject::GetObjectType() const { return *TYPE; }

   /////////////////////////////////////////////////////////////////////////////
   PhysicsObject::~PhysicsObject()
   {
      CleanUp();
      delete mDataMembers;
      mDataMembers = nullptr;
   }

   /////////////////////////////////////////////////////////////////////////////
   void PhysicsObject::SetNotifyCollisions(bool value)
   {
      if (mDataMembers->mGenericBody.valid())
         PhysicsWorld::GetInstance().NotifyCollision(*this, value);
      mDataMembers->mNotifyCollisions = value;
   }

   /////////////////////////////////////////////////////////////////////////////
   bool PhysicsObject::GetNotifyCollisions() const
   {
      return mDataMembers->mNotifyCollisions;
   }

   /////////////////////////////////////////////////////////////////////////////
   const dtCore::UniqueId& PhysicsObject::GetMaterialId() const
   {
      return mDataMembers->mMaterialId;
   }

   /////////////////////////////////////////////////////////////////////////////
   void PhysicsObject::SetMaterialId(const dtCore::UniqueId& matId)
   {
      if (mDataMembers->mMaterialId != matId)
      {
         mDataMembers->mMaterialId = matId;
         SetMaterialById(matId);
      }
   }

   /////////////////////////////////////////////////////////////////////////////
   void PhysicsObject::SetMaterial(Material* mat)
   {
      if (mat!= nullptr && mDataMembers->mGenericBody.valid())
      {
         mDataMembers->mGenericBody->SetMaterial(mat);
      }
   }

   /////////////////////////////////////////////////////////////////////////////
   Material* PhysicsObject::GetMaterial()
   {
      if (mDataMembers->mGenericBody.valid())
      {
         return mDataMembers->mGenericBody->GetMaterial();
      }
      return nullptr;
   }

   /////////////////////////////////////////////////////////////////////////////
   bool PhysicsObject::SetMaterialByName(const std::string& matName)
   {
      bool success = false;

      if (PhysicsWorld::IsInitialized())
      {
         Material* mat = nullptr;
         dtPhysics::PhysicsActComp* owner = dynamic_cast<dtPhysics::PhysicsActComp*>(GetUserData());
         if (owner != nullptr)
         {
            const MaterialActor* matActor = owner->LookupMaterialActor(matName);
            if (matActor != nullptr)
            {
               mDataMembers->mMaterialId = matActor->GetId();
               mat = matActor->GetMaterial();
            }
         }

         // If the actor can't be found the id can't be assigned.
         if (mat == nullptr)
         {
            dtPhysics::PhysicsMaterials& materials = PhysicsWorld::GetInstance().GetMaterials();

            mat = materials.GetMaterial(matName);
         }

         if (mat != nullptr)
         {
            SetMaterial(mat);
            success = true;
         }
      }

      return success;
   }

   /////////////////////////////////////////////////////////////////////////////
   bool PhysicsObject::SetMaterialByIndex(dtPhysics::MaterialIndex index)
   {
      bool success = false;

      if (PhysicsWorld::IsInitialized())
      {
         dtPhysics::PhysicsMaterials& materials = PhysicsWorld::GetInstance().GetMaterials();

         Material* mat = materials.GetMaterialByIndex(index);
         if (mat != nullptr)
         {
            SetMaterial(mat);
            success = true;
         }
      }

      return success;
   }

   /////////////////////////////////////////////////////////////////////////////
   bool PhysicsObject::SetMaterialById(const dtCore::UniqueId& id)
   {
      bool result = false;
      // Set the default material if nothing is set.
      if (mDataMembers->mGenericBody.valid())
      {
         PhysicsMaterials& materials = PhysicsWorld::GetInstance().GetMaterials();
         if (!id.IsNull())
         {
            PhysicsActComp* owner = dynamic_cast<dtPhysics::PhysicsActComp*>(GetUserData());
            if (owner != nullptr)
            {
               const MaterialActor* actor = owner->LookupMaterialActor(id);
               if (actor != nullptr)
               {
                  Material* matObj = materials.GetMaterial(actor->GetName());
                  if (matObj != nullptr)
                  {
                     SetMaterial(matObj);
                     result = true;
                  }
               }
            }
         }
         if (!result)
         {
            Material* matObj = materials.GetMaterial(PhysicsMaterials::DEFAULT_MATERIAL_NAME);
            if (matObj == nullptr)
            {
               LOG_ERROR("No default material exists")
            }
            else
            {
               SetMaterial(matObj);
               result = true;
            }
         }
      }
      return result;
   }


   /////////////////////////////////////////////////////////////////////////////
   void PhysicsObject::BuildPropertyMap()
   {
      static const dtUtil::RefString GROUP("PhysicsObject");

      typedef dtCore::PropertyRegHelper<PhysicsObject> PropRegType;
      PropRegType propRegHelper(*this, this, GROUP);

      DT_REGISTER_PROPERTY(Name,
            "The name of this physics object.  This can be used to look it up at runtime", PropRegType, propRegHelper);

      DT_REGISTER_PROPERTY_WITH_NAME(CollisionGroup, "Collision Group", "The numeric collision group for this object."
            "Groups can be configured to collide or not collide with each other."
            , PropRegType, propRegHelper);

      DT_REGISTER_PROPERTY_WITH_NAME(MechanicsType, "Mechanics Type",
            "Sets which collision type this actor will use", PropRegType, propRegHelper);

      DT_REGISTER_PROPERTY_WITH_NAME(PrimitiveType, "Primitive Type",
            "Sets which primitive type this actor will use", PropRegType, propRegHelper);

      DT_REGISTER_PROPERTY_WITH_NAME(Translation, "Translation",
            "Initial Translation", PropRegType, propRegHelper);

      DT_REGISTER_PROPERTY_WITH_NAME(Rotation, "Rotation",
            "Initial Rotation HPR", PropRegType, propRegHelper);

      DT_REGISTER_PROPERTY_WITH_NAME(OriginOffset, "Origin Offset",
            "Offsets the origin of the collision geometry relative to the center of mass."
            , PropRegType, propRegHelper);

      DT_REGISTER_PROPERTY_WITH_NAME(Extents, "Dimensions",
            "Used for determining extents of the collision volume"
            , PropRegType, propRegHelper);

      DT_REGISTER_PROPERTY_WITH_NAME(Mass, "Mass",
            "Total Mass of the object"
            , PropRegType, propRegHelper);

      DT_REGISTER_PROPERTY_WITH_NAME(SkinThickness, "Skin Thickness",
            "How far things may penetrate this physics object. Improves stability. Don't make this 0."
            , PropRegType, propRegHelper);

      DT_REGISTER_PROPERTY_WITH_NAME(NotifyCollisions, "Notify Collisions",
            "Set to true to enable collisions notifications for this object."
            , PropRegType, propRegHelper);

      DT_REGISTER_PROPERTY_WITH_NAME(MomentOfInertia, "Moment Of Inertia",
            "Sets the moment of inerta tensor 3x3 matrix diagonals.  If any value of this is 0 or negative, "
            "it will be computed from the geometry."
            , PropRegType, propRegHelper);

      DT_REGISTER_PROPERTY_WITH_NAME(ActivationLinearVelocityThreshold, "ActivationLinearVelocityThreshold",
            "the linear velocity threshold under which the object will auto-deactivate. -1 means engine default."
            , PropRegType, propRegHelper);

      DT_REGISTER_PROPERTY_WITH_NAME(ActivationAngularVelocityThreshold, "ActivationAngularVelocityThreshold",
            "the angular velocity threshold under which the object will auto-deactivate. -1 means engine default."
            , PropRegType, propRegHelper);

      DT_REGISTER_PROPERTY_WITH_NAME(ActivationTimeThreshold, "ActivationTimeThreshold",
            "The minimum time the object must be under all the activation thresholds before the object will auto-deactivate. -1 means engine default."
            , PropRegType, propRegHelper);

      DT_REGISTER_PROPERTY_WITH_NAME(LinearDamping, "LinearDamping",
            "Artifical linear body damping. 0 means off, 1 means pretty much don't move."
            , PropRegType, propRegHelper);

      DT_REGISTER_PROPERTY_WITH_NAME(AngularDamping, "AngularDamping",
            "Artifical angular body damping. 0 means off, 1 means pretty much don't move."
            , PropRegType, propRegHelper);

      DT_REGISTER_RESOURCE_PROPERTY_WITH_NAME(dtCore::DataType::STATIC_MESH, MeshResource, "PhysicsMesh", "PhysicsMesh",
            "Geometry file to load for the physics to use.  It can be either a renderable mesh or a compiled physics mesh.",
            PropRegType, propRegHelper);

      DT_REGISTER_PROPERTY_WITH_NAME(MeshScale, "PhysicsMeshScale",
            "If a physics mesh is set, it can be scaled with this property.",
            PropRegType, propRegHelper);

      DT_REGISTER_PROPERTY(CollisionResponseEnabled,
            "True for normal collision response.  False for collision detection only.",
            PropRegType, propRegHelper);

      DT_REGISTER_ACTOR_ID_PROPERTY("dtPhysics::MaterialActor", MaterialId,
            "Material", "The material actor that defines the physics materials for this object.",
            PropRegType, propRegHelper);
   }

   void PhysicsObject::CalculateBoundsAndOrigin(const osg::Node* nodeToLoad, bool calcDimensions, bool adjustOriginOffsetForGeometry)
   {
      VectorType center, newDimensions;
      osg::BoundingBox bBox;

      if (nodeToLoad != nullptr)
      {
         osg::ComputeBoundsVisitor bb;
         // sorry about the const cast.  The node SHOULD be const since we aren't changing it
         // but accept doesn't work as const.
         const_cast<osg::Node&>(*nodeToLoad).accept(bb);
         bBox = bb.getBoundingBox();
      }
      else if (GetMeshResource() != dtCore::ResourceDescriptor::NULL_RESOURCE)
      {
         std::string fileToLoad = dtCore::Project::GetInstance().GetResourcePath(GetMeshResource());
         dtPhysics::PhysicsReaderWriter::LoadFileGetExtents(bBox, fileToLoad);
      }

      if (calcDimensions)
      {
         CalculateOriginAndExtentsForNode(GetPrimitiveType(), bBox, center, newDimensions);
         for (unsigned i = 0; i < VectorType::num_components; ++i)
         {
            newDimensions[i] *= mDataMembers->mMeshScale[i];
            center[i] *= mDataMembers->mMeshScale[i];
         }
         SetExtents(newDimensions);
      }
      else
      {
         center = bBox.center();
      }

      if (adjustOriginOffsetForGeometry)
      {
         SetOriginOffset(GetOriginOffset() + center);
      }


   }

   static const std::string POLYTOPE_SUFFIX = "_Polytope";

   /////////////////////////////////////////////////////////////////////////////
   bool PhysicsObject::Create(const osg::Node* nodeToLoad, bool adjustOriginOffsetForGeometry,
         const std::string& cachingKey)
   {
      try
      {
         VectorType dimensions = GetExtents();
         bool calcDimensions = GetPrimitiveType().IsSimpleShape() && dimensions[0] <= 0.0f && dimensions[1] <= 0.0f && dimensions[2] <= 0.0f;

         if (!GetPrimitiveType().IsSimpleShape())
         {
            adjustOriginOffsetForGeometry = false;
         }

         if ((calcDimensions || adjustOriginOffsetForGeometry))
         {
            CalculateBoundsAndOrigin(nodeToLoad, calcDimensions, adjustOriginOffsetForGeometry);
         }

         dimensions = GetExtents();
         bool changedDimensions = false;
         for (int i = 0; i < VectorType::num_components; ++i)
         {
            if (dimensions[i] <= 0.0)
            {
               dimensions[i] = 1.0f;
               changedDimensions = true;
            }
         }

         if (changedDimensions)
         {
            SetExtents(dimensions);
         }

         dtCore::RefPtr<VertexData> data;
         if (!GetPrimitiveType().IsSimpleShape())
         {
            if (nodeToLoad != nullptr)
            {
               bool polytope = GetPrimitiveType() == PrimitiveType::CONVEX_HULL;
               VertexData::GetOrCreateCachedDataForNode(data, nodeToLoad, polytope && !cachingKey.empty() ? cachingKey + POLYTOPE_SUFFIX : cachingKey, polytope);
            }
            else
            {
               GetVertexDataForResource(data, cachingKey);
            }
         }

         return CreateInternal(data);
      }
      catch (const dtUtil::Exception& ex)
      {
         ex.LogException(dtUtil::Log::LOG_ERROR);
      }
      return false;
   }

   /////////////////////////////////////////////////////////////////////////////
   void PhysicsObject::GetVertexDataForResource(dtCore::RefPtr<VertexData>& vertDataOut, const std::string& cachingKey)
   {
      if (mDataMembers->mMeshResource == dtCore::ResourceDescriptor::NULL_RESOURCE)
         return;

      bool polytope = GetPrimitiveType() == PrimitiveType::CONVEX_HULL;

      std::string fileToLoad;

      bool dataNew = true;
      if (cachingKey != VertexData::NO_CACHE_KEY)
      {
         dataNew = VertexData::GetOrCreateCachedData(vertDataOut, polytope ? cachingKey + POLYTOPE_SUFFIX : cachingKey );
      }
      else
      {
         dataNew = VertexData::GetOrCreateCachedData(vertDataOut, polytope ? GetMeshResource().GetResourceIdentifier() + POLYTOPE_SUFFIX : GetMeshResource().GetResourceIdentifier() );
      }

      if (dataNew)
      {
         // throw the exception
         fileToLoad = dtCore::Project::GetInstance().GetResourcePath(GetMeshResource());

         if (!fileToLoad.empty())
         {
            dtCore::RefPtr<VertexData> readerData = new VertexData;

            if (dtPhysics::PhysicsReaderWriter::LoadTriangleDataFile(*readerData, fileToLoad))
            {
               dtCore::Transform geometryWorld;
               GetTransform(geometryWorld);

               vertDataOut->Swap(*readerData);
               if (polytope)
               {
                  vertDataOut->ConvertToPolytope();
               }
            }
            else
            {
               vertDataOut = nullptr;
               throw dtUtil::Exception("Unable to load triangle data from existing file resource: "
                     + GetMeshResource().GetResourceIdentifier(), __FILE__, __LINE__);
            }
         }
      }
   }

   /////////////////////////////////////////////////////////////////////////////
   bool PhysicsObject::CreateFromGeometry(dtPhysics::Geometry& geometry)
   {
      std::vector<GeometryPtr> geometryVec;
      geometryVec.push_back(&geometry);
      return CreateFromGeometry(geometryVec);
   }

   /////////////////////////////////////////////////////////////////////////////
   bool PhysicsObject::CreateFromGeometry(std::vector<GeometryPtr> geometry)
   {
      if (geometry.empty()) return false;

      dtCore::Transform transform;
      GetTransform(transform);

      mDataMembers->mGeometries.clear();

      mDataMembers->mGenericBody = BaseBodyWrapper::CreateGenericBody(transform, *mDataMembers->mMechanicsType, mDataMembers->mCollisionGroup, mDataMembers->mMassOfObject);

      if (mDataMembers->mGenericBody.valid())
      {
         mDataMembers->mGenericBody->Init();

         for (auto i = geometry.begin(), iend = geometry.end(); i != iend; ++i)
         {
            AddGeometry(**i);
         }

         //Reset this because it has to be done after geometry is added.
         SetMomentOfInertia(mDataMembers->mMomentOfInertia);
      }
      else
      {
         LOGN_ERROR("physicsobject.cpp", "Created a GenericBodyWrapper, but dynamic casting it to one failed.  Bailing out.");
         mDataMembers->mGenericBody = NULL;
         return false;
      }

      bool materialWasSet = false;
      // Attempt to assign a material from the arbitrary triangle data that may have been loaded.
      const dtPhysics::VertexData* vertData = geometry[0]->GetVertexData();
      if (GetMaterialId().IsNull() && vertData != nullptr && vertData->GetMaterialCount() > 0)
      {
         // For now one material can be applied per object.
         std::string matName = vertData->GetMaterialName(vertData->GetFirstMaterialIndex());

         if (! matName.empty())
         {
            // Try to set the physics material by name.
            materialWasSet = SetMaterialByName(matName);
            if (!materialWasSet)
            {
               LOG_ERROR("Could not assign a physics material by name \"" + matName
                     + "\" for object \"" + GetName() + "\" the property configured");
            }
         }
      }

      // Set the default material if nothing is set.
      if (!materialWasSet)
      {
         // If the id is not found or is empty, it will just assign the default material.
         SetMaterialById(GetMaterialId());
      }

      SetSkinThickness(mDataMembers->mSkinThickness);
      SetNotifyCollisions(GetNotifyCollisions());
      SetCollisionResponseEnabled(mDataMembers->mCollisionResponseEnabled);

      CreateWithBody(*mDataMembers->mGenericBody);

      mDataMembers->ApplyActivationSettings();
      SetLinearDamping(mDataMembers->mLinearDamping);
      SetAngularDamping(mDataMembers->mAngularDamping);

      return true;
   }

   /////////////////////////////////////////////////////////////////////////////
   bool PhysicsObject::CreateInternal(VertexData* vertData)
   {
      TransformType xform;
      GetTransform(xform);
      mDataMembers->mGeometries.clear();

      mDataMembers->mGenericBody = BaseBodyWrapper::CreateGenericBody(xform, *mDataMembers->mMechanicsType, mDataMembers->mCollisionGroup, mDataMembers->mMassOfObject);

      bool materialWasSet = false;
      if (mDataMembers->mGenericBody.valid())
      {
         mDataMembers->mGenericBody->Init();
         dtCore::Transform geometryWorld;
         GetOriginOffsetInWorldSpace(geometryWorld);
         if (GetPrimitiveType().IsSimpleShape())
         {
            mDataMembers->CreateSimpleGeometry(GetPrimitiveType(), GetExtents(), geometryWorld, GetMass());
         }
         else if (vertData != nullptr)
         {
            // Scale the data in place, which is actually the cached one.
            // Each user will just rescale the data to what they need, but
            // chances are good that the same scale will end up being used over and over.
            vertData->Scale(mDataMembers->mMeshScale);

            mDataMembers->CreateComplexGeometry(GetPrimitiveType(), geometryWorld, *vertData, GetMass());

            if (GetMaterialId().IsNull() && vertData->GetMaterialCount() > 0)
            {
               // For now one material can be applied per object.
               std::string matName = vertData->GetMaterialName(vertData->GetFirstMaterialIndex());

               if (! matName.empty())
               {
                  // Try to set the physics material by name.
                  materialWasSet = SetMaterialByName(matName);
                  if (!materialWasSet)
                  {
                     LOG_ERROR("Could not assign a physics material by name \"" + matName
                           + "\" for object \"" + GetName() + "\" the property configured");
                  }
               }
            }
         }
         else
         {
            throw dtUtil::Exception("Unable to create a geometry for PhysicsObject \"" + GetName() + "\" because it neither has bounds for a shape nor vertex data for a mesh.",
                  __FILE__, __LINE__);
         }
         //Reset this because it has to be done after geometry is added.
         SetMomentOfInertia(mDataMembers->mMomentOfInertia);
      }
      else
      {
         LOGN_ERROR("physicsobject.cpp", "Created a GenericBodyWrapper, but dynamic casting it to one failed.  Bailing out.");
         mDataMembers->mGenericBody = nullptr;
         return false;
      }

      if (!materialWasSet)
      {
         // If the id is not found or is empty, it will just assign the default material.
         SetMaterialById(GetMaterialId());
      }

      SetSkinThickness(mDataMembers->mSkinThickness);
      SetNotifyCollisions(GetNotifyCollisions());
      SetCollisionResponseEnabled(mDataMembers->mCollisionResponseEnabled);

      CreateWithBody(*mDataMembers->mGenericBody);

      mDataMembers->ApplyActivationSettings();
      SetLinearDamping(mDataMembers->mLinearDamping);
      SetAngularDamping(mDataMembers->mAngularDamping);

      return mDataMembers->mGenericBody.valid();
   }

   /////////////////////////////////////////////////////////////////////////////
   void PhysicsObject::CreateWithBody(GenericBodyWrapper& body)
   {
      // TODO, if this is called on an already configured body, this won't clear the geometries
      // list, and that list may be wrong. It can't clear the list because this method is called
      // from the other create methods that setup that list before calling this one.
      mDataMembers->mGenericBody = &body;
      mDataMembers->mGenericBody->GetPalBodyBase().SetUserData(this);
      mDataMembers->mActivationSettings = dynamic_cast<palActivationSettings*>(&body.GetPalBodyBase());
   }

   /////////////////////////////////////////////////////////////////////////////
   void PhysicsObject::CleanUp()
   {
      mDataMembers->mGeometries.clear();
      // This is just a pointer to a dynamic_cast version of the body, so no need to delete.
      mDataMembers->mActivationSettings = nullptr;
      // These are ref ptrs, so they should get cleaned up
      mDataMembers->mGenericBody = nullptr;
   }

   /////////////////////////////////////////////////////////////////////////////
   Real PhysicsObject::GetSkinThickness() const
   {
      Real thickness = mDataMembers->mSkinThickness;
      if (mDataMembers->mGenericBody != nullptr)
      {
         thickness = mDataMembers->mGenericBody->GetSkinWidth();
      }
      return thickness;
   }

   /////////////////////////////////////////////////////////////////////////////
   void PhysicsObject::SetSkinThickness(Real st)
   {
      mDataMembers->mSkinThickness = st;
      if (mDataMembers->mGenericBody != nullptr)
      {
         mDataMembers->mGenericBody->SetSkinWidth(st);
      }
   }

   //////////////////////////////////////////////////////
   BodyWrapper* PhysicsObject::GetBodyWrapper()
   {
      return mDataMembers->mGenericBody.get();
   }

   //////////////////////////////////////////////////////
   GenericBodyWrapper* PhysicsObject::GetGenericBodyWrapper()
   {
      return mDataMembers->mGenericBody.get();
   }

   /////////////////////////////////////////////////////////////////////////////
   void PhysicsObject::SetUserData(osg::Referenced* data)
   {
      mDataMembers->mUserData = data;
   }

   /////////////////////////////////////////////////////////////////////////////
   const osg::Referenced* PhysicsObject::GetUserData() const
   {
      return mDataMembers->mUserData.get();
   }

   /////////////////////////////////////////////////////////////////////////////
   osg::Referenced* PhysicsObject::GetUserData()
   {
      return mDataMembers->mUserData.get();
   }

   /////////////////////////////////////////////////////////////////////////////
   CollisionGroup PhysicsObject::GetCollisionGroup() const
   {
      CollisionGroup g = mDataMembers->mCollisionGroup;
      if (mDataMembers->mGenericBody != nullptr)
      {
         g = mDataMembers->mGenericBody->GetGroup();
      }
      return g;
   }

   /////////////////////////////////////////////////////////////////////////////
   void PhysicsObject::SetCollisionGroup(CollisionGroup group)
   {
      mDataMembers->mCollisionGroup = group;
      if (mDataMembers->mGenericBody != nullptr)
      {
         mDataMembers->mGenericBody->SetGroup(group);
      }
   }

   /////////////////////////////////////////////////////////////////////////////
   const std::string& PhysicsObject::GetName() const
   {
      return mDataMembers->mName;
   }

   /////////////////////////////////////////////////////////////////////////////
   void PhysicsObject::SetName(const std::string& s)
   {
      mDataMembers->mName = s;
   }

   /////////////////////////////////////////////////////////////////////////////
   void PhysicsObject::AddForce(const VectorType& forceToAdd)
   {
      if (mDataMembers->mGenericBody.valid())
      {
         mDataMembers->mGenericBody->AddForce(forceToAdd);
      }
   }

   /////////////////////////////////////////////////////////////////////////////
   void PhysicsObject::AddLocalForce(const VectorType& forceToAdd)
   {
      if (mDataMembers->mGenericBody.valid())
      {
         mDataMembers->mGenericBody->AddForce(TransformToWorldSpace(forceToAdd));
      }
   }

   /////////////////////////////////////////////////////////////////////////////
   void PhysicsObject::AddLocalTorque( const VectorType& torqueToAdd )
   {
      if (mDataMembers->mGenericBody.valid())
      {
         mDataMembers->mGenericBody->AddTorque(TransformToWorldSpace(torqueToAdd));
      }
   }

   /////////////////////////////////////////////////////////////////////////////
   osg::Vec3 PhysicsObject::TransformToWorldSpace( const osg::Vec3& localSpaceVector )
   {
      VectorType vec = localSpaceVector;
      TransformType localToWorld;
      GetTransform(localToWorld);

      osg::Matrix rot;
      localToWorld.GetRotation(rot);

      return osg::Matrix::transform3x3(vec, rot);
   }

   /////////////////////////////////////////////////////////////////////////////
   void PhysicsObject::AddForceAtPosition(const VectorType& position, const VectorType& force)
   {
      if (mDataMembers->mGenericBody.valid())
      {
         mDataMembers->mGenericBody->AddForceAtPosition(position, force);
      }
   }

   /////////////////////////////////////////////////////////////////////////////
   void PhysicsObject::AddLocalForceAtPosition(const VectorType& position, const VectorType& force)
   {
      if (mDataMembers->mGenericBody.valid())
      {
         dtCore::Transform xform;
         GetTransform(xform);

         MatrixType m;
         xform.Get(m);
         VectorType worldPos = m.preMult(position);

         mDataMembers->mGenericBody->AddForceAtPosition(worldPos, TransformToWorldSpace(force));
      }
   }

   /////////////////////////////////////////////////////////////////////////////
   void PhysicsObject::ApplyImpulse(const VectorType& impulse)
   {
      if (mDataMembers->mGenericBody.valid())
      {
         mDataMembers->mGenericBody->ApplyImpulse(impulse);
      }
   }

   /////////////////////////////////////////////////////////////////////////////
   void PhysicsObject::ApplyLocalImpulse(const VectorType& impulse)
   {
      if (mDataMembers->mGenericBody.valid())
      {
         mDataMembers->mGenericBody->ApplyImpulse(TransformToWorldSpace(impulse));
      }
   }

   /////////////////////////////////////////////////////////////////////////////
   void PhysicsObject::ApplyAngularImpulse(const VectorType& angularImpulse)
   {
      if (mDataMembers->mGenericBody.valid())
      {
         mDataMembers->mGenericBody->ApplyAngularImpulse(angularImpulse);
      }
   }

   /////////////////////////////////////////////////////////////////////////////
   void PhysicsObject::ApplyImpulseAtPosition(const VectorType& position, const VectorType& impulse)
   {
      if (mDataMembers->mGenericBody.valid())
      {
         mDataMembers->mGenericBody->ApplyImpulseAtPosition(position, impulse);
      }
   }

   /////////////////////////////////////////////////////////////////////////////
   void PhysicsObject::ApplyLocalImpulseAtPosition(const VectorType& position, const VectorType& impulse)
   {
      if (mDataMembers->mGenericBody.valid())
      {
         dtCore::Transform xform;
         GetTransform(xform);

         MatrixType m;
         xform.Get(m);
         VectorType worldPos = m.preMult(position);

         mDataMembers->mGenericBody->ApplyImpulseAtPosition(worldPos, TransformToWorldSpace(impulse));
      }
   }

   /////////////////////////////////////////////////////////////////////////////
   void PhysicsObject::AddTorque(const VectorType& torque)
   {
      if (mDataMembers->mGenericBody.valid())
      {
         mDataMembers->mGenericBody->AddTorque(torque);
      }
   }

   /////////////////////////////////////////////////////////////////////////////
   void PhysicsObject::SetLinearVelocity(const VectorType& velocity)
   {
      if (mDataMembers->mGenericBody.valid())
      {
         mDataMembers->mGenericBody->SetLinearVelocity(velocity);
      }
   }

   /////////////////////////////////////////////////////////////////////////////
   void PhysicsObject::SetAngularVelocity(const VectorType& velocity_rad)
   {
      if (mDataMembers->mGenericBody.valid())
      {
         mDataMembers->mGenericBody->SetAngularVelocity(velocity_rad);
      }
   }

   /////////////////////////////////////////////////////////////////////////////
   VectorType PhysicsObject::GetLinearVelocity() const
   {
      if (mDataMembers->mGenericBody.valid())
      {
         return mDataMembers->mGenericBody->GetLinearVelocity();
      }
      return VectorType();
   }

   /////////////////////////////////////////////////////////////////////////////
   VectorType PhysicsObject::GetAngularVelocity() const
   {
      if (mDataMembers->mGenericBody.valid())
      {
         return mDataMembers->mGenericBody->GetAngularVelocity();
      }
      return VectorType();
   }

   /////////////////////////////////////////////////////////////////////////////
   VectorType PhysicsObject::GetLinearVelocityAtLocalPoint(const VectorType& relPos) const
   {
      return GetLinearVelocity() + (GetAngularVelocity() ^ relPos);
   }

   /////////////////////////////////////////////////////////////////////////////
   void PhysicsObject::AddGeometry(Geometry& geom)
   {
      if (mDataMembers->mGenericBody.valid())
      {
         mDataMembers->mGeometries.push_back(&geom);
         mDataMembers->mGenericBody->ConnectGeometry(geom);
      }
   }

   /////////////////////////////////////////////////////////////////////////////
   Geometry* PhysicsObject::GetGeometry(unsigned idx)
   {
      if (mDataMembers->mGenericBody.valid() && mDataMembers->mGeometries.size() > idx)
      {
         return mDataMembers->mGeometries[idx];
      }
      return nullptr;
   }

   /////////////////////////////////////////////////////////////////////////////
   void PhysicsObject::RemoveGeometry(Geometry& geom)
   {
      if (mDataMembers->mGenericBody.valid())
      {
         mDataMembers->mGeometries.erase(std::remove(mDataMembers->mGeometries.begin(), mDataMembers->mGeometries.end(), dtCore::RefPtr<Geometry>(&geom)),
               mDataMembers->mGeometries.end());
         mDataMembers->mGenericBody->RemoveGeometry(geom);
      }
   }

   /////////////////////////////////////////////////////////////////////////////
   unsigned PhysicsObject::GetNumGeometries() const
   {
      if (mDataMembers->mGenericBody.valid())
      {
         return mDataMembers->mGeometries.size();
      }
      return 0;
   }

   /////////////////////////////////////////////////////////////////////////////
   void PhysicsObject::SetGravityEnabled(bool enable)
   {
      if (mDataMembers->mGenericBody.valid())
      {
         mDataMembers->mGenericBody->SetGravityEnabled(enable);
      }
   }

   /////////////////////////////////////////////////////////////////////////////
   bool PhysicsObject::IsGravityEnabled() const
   {
      bool result = true;
      if (mDataMembers->mGenericBody.valid())
      {
         result = mDataMembers->mGenericBody->IsGravityEnabled();
      }
      return result;
   }

   /////////////////////////////////////////////////////////////////////////////
   void PhysicsObject::SetCollisionResponseEnabled(bool enabled)
   {
      mDataMembers->mCollisionResponseEnabled = enabled;
      if (mDataMembers->mGenericBody.valid())
      {
         return mDataMembers->mGenericBody->GetPalGenericBody().SetCollisionResponseEnabled(enabled);
      }
   }

   /////////////////////////////////////////////////////////////////////////////
   bool PhysicsObject::IsCollisionResponseEnabled() const
   {
      bool result = mDataMembers->mCollisionResponseEnabled;
      if (mDataMembers->mGenericBody.valid())
      {
         result = mDataMembers->mGenericBody->GetPalGenericBody().IsCollisionResponseEnabled();
      }
      return result;
   }

   /////////////////////////////////////////////////////////////////////////////
   void PhysicsObject::SetExtents(const VectorType& v)
   {
      mDataMembers->mExtents = v;
   }

   /////////////////////////////////////////////////////////////////////////////
   VectorType PhysicsObject::GetExtents() const
   {
      return mDataMembers->mExtents;
   }

   /////////////////////////////////////////////////////////////////////////////
   void PhysicsObject::SetTranslation(const VectorType& v)
   {
      TransformType xform;
      GetTransform(xform);
      xform.SetTranslation(v);
      SetTransform(xform);
   }

   /////////////////////////////////////////////////////////////////////////////
   VectorType PhysicsObject::GetTranslation() const
   {
      TransformType xform;
      GetTransform(xform);
      VectorType v;
      xform.GetTranslation(v);
      return v;
   }

   /////////////////////////////////////////////////////////////////////////////
   void PhysicsObject::SetRotation(const VectorType& v)
   {
      TransformType xform;
      GetTransform(xform);
      xform.SetRotation(v);
      SetTransform(xform);
   }

   /////////////////////////////////////////////////////////////////////////////
   VectorType PhysicsObject::GetRotation() const
   {
      TransformType xform;
      GetTransform(xform);
      VectorType v;
      xform.GetRotation(v);
      return v;
   }

   /////////////////////////////////////////////////////////////////////////////
   void PhysicsObject::SetMechanicsType(MechanicsType& m)
   {
      mDataMembers->mMechanicsType = &m;
      if(mDataMembers->mGenericBody.valid())
      {
         mDataMembers->mGenericBody->SetMechanicsType(m);
      }
   }

   /////////////////////////////////////////////////////////////////////////////
   MechanicsType& PhysicsObject::GetMechanicsType() const
   {
      if (mDataMembers->mGenericBody.valid())
      {
         return mDataMembers->mGenericBody->GetMechanicsType();
      }
      return *mDataMembers->mMechanicsType;
   }

   /////////////////////////////////////////////////////////////////////////////
   void PhysicsObject::SetPrimitiveType(PrimitiveType& p)
   {
      mDataMembers->mPrimitiveType = &p;
   }

   /////////////////////////////////////////////////////////////////////////////
   PrimitiveType& PhysicsObject::GetPrimitiveType() const
   {
      return *mDataMembers->mPrimitiveType;
   }

   /////////////////////////////////////////////////////////////////////////////
   Real PhysicsObject::GetMass() const
   {
      if (mDataMembers->mGenericBody.valid())
      {
         return mDataMembers->mGenericBody->GetMass();
      }
      return mDataMembers->mMassOfObject;
   }

   /////////////////////////////////////////////////////////////////////////////
   void PhysicsObject::SetMass(Real m)
   {
      mDataMembers->mMassOfObject = m;
      if(mDataMembers->mGenericBody.valid())
      {
         mDataMembers->mGenericBody->SetMass(m);
      }
   }

   /////////////////////////////////////////////////////////////////////////////
   void PhysicsObject::SetMassAndScaleMomentOfInertia(Real newMass)
   {
      Real oldMass = GetMass();
      Real ratio = newMass/oldMass;
      VectorType moi = GetMomentOfInertia();
      SetMomentOfInertia(
            VectorType(
                  moi.x() > 0.0 ? moi.x() * ratio: -1.0f,
                        moi.y() > 0.0 ? moi.y() * ratio: -1.0f,
                              moi.z() > 0.0 ? moi.z() * ratio: -1.0f
            )
      );
      SetMass(newMass);
   }

   /////////////////////////////////////////////////////////////////////////////
   void PhysicsObject::SetTransform(const TransformType& t)
   {
      mDataMembers->mTransform = t;

      if(mDataMembers->mGenericBody.valid())
      {
         mDataMembers->mGenericBody->SetTransform(t);
      }
   }

   /////////////////////////////////////////////////////////////////////////////
   void PhysicsObject::GetTransform(TransformType& xform, bool interpolated) const
   {
      if (mDataMembers->mGenericBody.valid())
      {
         mDataMembers->mGenericBody->GetTransform(xform, interpolated);
      }
      else
      {
         xform = mDataMembers->mTransform;
      }
   }

   /////////////////////////////////////////////////////////////////////////////
   void PhysicsObject::SetTransformAsVisual(const TransformType& xform)
   {
      if (mDataMembers->mVisualToBodyIsIdentity)
      {
         SetTransform(xform);
      }
      else
      {
         MatrixType m;
         xform.Get(m);
         m.preMult(mDataMembers->mVisualToBodyTransform);
         dtCore::Transform xformBody;
         xformBody.Set(m);
         SetTransform(xformBody);
      }
   }

   /////////////////////////////////////////////////////////////////////////////
   void PhysicsObject::GetTransformAsVisual(TransformType& xform) const
   {
      GetTransform(xform, true);
      if (!mDataMembers->mVisualToBodyIsIdentity)
      {
         osg::Matrix m;
         xform.Get(m);
         m.preMult(MatrixType::inverse(mDataMembers->mVisualToBodyTransform));
         xform.Set(m);
      }
   }

   /////////////////////////////////////////////////////////////////////////////
   void PhysicsObject::SetVisualToBodyTransform(const TransformType& xform)
   {
      xform.Get(mDataMembers->mVisualToBodyTransform);
      mDataMembers->mVisualToBodyIsIdentity = xform.IsIdentity();
   }

   /////////////////////////////////////////////////////////////////////////////
   void PhysicsObject::GetVisualToBodyTransform(TransformType& xform) const
   {
      xform.Set(mDataMembers->mVisualToBodyTransform);
   }

   /////////////////////////////////////////////////////////////////////////////
   void PhysicsObject::SetOriginOffset(const VectorType& v)
   {
      mDataMembers->mOriginOffset = v;
   }

   /////////////////////////////////////////////////////////////////////////////
   VectorType PhysicsObject::GetOriginOffset() const
   {
      return mDataMembers->mOriginOffset;
   }

   /////////////////////////////////////////////////////////////////////////////
   void PhysicsObject::GetOriginOffsetInWorldSpace(TransformType& offsetWorld) const
   {
      MatrixType offsetM;
      offsetM.makeIdentity();
      offsetM.setTrans(GetOriginOffset());

      MatrixType m;
      mDataMembers->mTransform.Get(m);
      offsetM = offsetM * m;
      offsetWorld.Set(offsetM);
   }

   /////////////////////////////////////////////////////////////////////////////
   void PhysicsObject::SetMomentOfInertia(const VectorType& v)
   {
      mDataMembers->mMomentOfInertia = v;
      if (mDataMembers->mGenericBody.valid())
      {
         // Only assign the inertia on the internal body if the value is > 0 so that the body can calculate it on it's own
         // by default.
         bool validInertia = true;
         for (unsigned i = 0; i < 3; ++i)
         {
            if (v[i] <= 0.0f)
            {
               validInertia = false;
            }
         }
         if (validInertia)
         {
            mDataMembers->mGenericBody->SetInertia(v);
         }
      }
   }

   /////////////////////////////////////////////////////////////////////////////
   VectorType PhysicsObject::GetMomentOfInertia() const
   {
      VectorType result = mDataMembers->mMomentOfInertia;
      if (mDataMembers->mGenericBody.valid())
      {
         result = mDataMembers->mGenericBody->GetInertia();
      }
      return result;
   }

   /////////////////////////////////////////////////////////////////////////////
   void PhysicsObject::SetActive(bool active)
   {
      if(mDataMembers->mGenericBody.valid())
      {
         mDataMembers->mGenericBody->SetActive(active);
      }
   }

   /////////////////////////////////////////////////////////////////////////////
   bool PhysicsObject::IsActive() const
   {
      if(mDataMembers->mGenericBody.valid())
      {
         return mDataMembers->mGenericBody->IsActive();
      }
      return false;
   }

   /////////////////////////////////////////////////////////////////////////////
   Real PhysicsObject::GetActivationLinearVelocityThreshold() const
   {
      if (mDataMembers->mActivationSettings != nullptr)
      {
         return Real(mDataMembers->mActivationSettings->GetActivationLinearVelocityThreshold());
      }
      return mDataMembers->mActivationLinearVelocityThreshold;
   }

   /////////////////////////////////////////////////////////////////////////////
   void PhysicsObject::SetActivationLinearVelocityThreshold(Real threshold)
   {
      if (mDataMembers->mActivationSettings != nullptr)
      {
         mDataMembers->mActivationSettings->SetActivationLinearVelocityThreshold(Float(threshold));
      }
      mDataMembers->mActivationLinearVelocityThreshold = threshold;
   }

   /////////////////////////////////////////////////////////////////////////////
   Real PhysicsObject::GetActivationAngularVelocityThreshold() const
   {
      if (mDataMembers->mActivationSettings != nullptr)
      {
         return Real(mDataMembers->mActivationSettings->GetActivationAngularVelocityThreshold());
      }
      return mDataMembers->mActivationAngularVelocityThreshold;
   }

   /////////////////////////////////////////////////////////////////////////////
   void PhysicsObject::SetActivationAngularVelocityThreshold(Real threshold)
   {
      if (mDataMembers->mActivationSettings != nullptr)
      {
         mDataMembers->mActivationSettings->SetActivationAngularVelocityThreshold(Float(threshold));
      }
      mDataMembers->mActivationAngularVelocityThreshold = threshold;
   }

   /////////////////////////////////////////////////////////////////////////////
   Real PhysicsObject::GetActivationTimeThreshold() const
   {
      if (mDataMembers->mActivationSettings != nullptr)
      {
         return Real(mDataMembers->mActivationSettings->GetActivationTimeThreshold());
      }
      return mDataMembers->mActivationTimeThreshold;
   }

   /////////////////////////////////////////////////////////////////////////////
   void PhysicsObject::SetActivationTimeThreshold(Real threshold)
   {
      if (mDataMembers->mActivationSettings != nullptr)
      {
         mDataMembers->mActivationSettings->SetActivationTimeThreshold(Float(threshold));
      }
      mDataMembers->mActivationTimeThreshold = threshold;

   }

   /////////////////////////////////////////////////////////////////////////////
   Real PhysicsObject::GetLinearDamping() const
   {
      Real result = mDataMembers->mLinearDamping;
      if (mDataMembers->mGenericBody.valid())
      {
         result = Real(mDataMembers->mGenericBody->GetPalGenericBody().GetLinearDamping());
      }
      return result;
   }

   /////////////////////////////////////////////////////////////////////////////
   void PhysicsObject::SetLinearDamping(Real damping)
   {
      dtUtil::Clamp(damping, Real(0.0), Real(1.0));
      mDataMembers->mLinearDamping = damping;
      if (mDataMembers->mGenericBody.valid())
      {
         mDataMembers->mGenericBody->GetPalGenericBody().SetLinearDamping(Float(damping));
      }
   }

   /////////////////////////////////////////////////////////////////////////////
   Real PhysicsObject::GetAngularDamping() const
   {
      Real result = mDataMembers->mAngularDamping;
      if (mDataMembers->mGenericBody.valid())
      {
         result = Real(mDataMembers->mGenericBody->GetPalGenericBody().GetAngularDamping());
      }
      return result;
   }

   /////////////////////////////////////////////////////////////////////////////
   void PhysicsObject::SetAngularDamping(Real damping)
   {
      dtUtil::Clamp(damping, Real(0.0), Real(1.0));
      mDataMembers->mAngularDamping = damping;
      if (mDataMembers->mGenericBody.valid())
      {
         mDataMembers->mGenericBody->GetPalGenericBody().SetAngularDamping(Float(damping));
      }
   }

   /////////////////////////////////////////////////////////////////////////////
   const dtCore::ResourceDescriptor& PhysicsObject::GetMeshResource() const
   {
      return mDataMembers->mMeshResource;
   }

   /////////////////////////////////////////////////////////////////////////////
   void PhysicsObject::SetMeshResource(const dtCore::ResourceDescriptor& meshRes)
   {
      mDataMembers->mMeshResource = meshRes;
   }

   /////////////////////////////////////////////////////////////////////////////
   const VectorType& PhysicsObject::GetMeshScale() const
   {
      return mDataMembers->mMeshScale;
   }

   /////////////////////////////////////////////////////////////////////////////
   void PhysicsObject::SetMeshScale(const VectorType& meshScale)
   {
      mDataMembers->mMeshScale = meshScale;
   }

   /////////////////////////////////////////////////////////////////////////////
   void PhysicsObject::CalculateOriginAndExtentsForNode(PrimitiveType& type, const osg::BoundingBox& bb,
         VectorType& center, VectorType& extents)
   {
      if (type == PrimitiveType::SPHERE)
      {
         float radiusX = (bb.xMax() - bb.xMin()) / 2.0f;
         float radiusY = (bb.yMax() - bb.yMin()) / 2.0f;
         float radiusZ = (bb.zMax() - bb.zMin()) / 2.0f;

         // Taking the radius of the box always gives a sphere that is very large.
         // This may not completely encase the object, but the thought it that if
         // one is using a sphere, the object probably fits pretty well in a sphere, so this
         // code makes that assumption.
         float largestAxis = dtUtil::Max(dtUtil::Max(radiusX, radiusY), radiusZ);
         extents[0] = largestAxis;
         extents[1] = 0.0f;
         extents[2] = 0.0f;
         center = bb.center();
      }
      else if (type == PrimitiveType::BOX)
      {
         extents[0] = bb.xMax() - bb.xMin();
         extents[1] = bb.yMax() - bb.yMin();
         extents[2] = bb.zMax() - bb.zMin();
         center = bb.center();
      }
      else if (type == PrimitiveType::CYLINDER)
      {
         float radiusX = (bb.xMax() - bb.xMin()) / 2.0f;
         float radiusY = (bb.yMax() - bb.yMin()) / 2.0f;

         // Taking the radius of the box always gives a radius that is very large.
         // This may not completely encompass the object, but the thought it that if
         // one is using a cylinder, the object probably fits pretty well in it, so this
         // code makes that assumption.
         float largestAxis = dtUtil::Max(radiusX, radiusY);

         float height = bb.zMax() - bb.zMin();

         extents[0] = height;
         extents[1] = largestAxis;
         extents[2] = 0.0f;
         center = bb.center();
      }
      else if (type == PrimitiveType::CAPSULE)
      {
         float radiusX = (bb.xMax() - bb.xMin()) / 2.0f;
         float radiusY = (bb.yMax() - bb.yMin()) / 2.0f;

         // Taking the radius of the box always gives a radius that is very large.
         // This may not completely encompass the object, but the thought it that if
         // one is using a capsule, the object probably fits pretty well in it, so this
         // code makes that assumption.
         float largestAxis = dtUtil::Max(radiusX, radiusY);

         float height = bb.zMax() - bb.zMin();

         extents[0] = height;
         extents[1] = largestAxis;
         extents[2] = 0.0f;
         center = bb.center();
      }
      else
      {
         // Triangle mesh types use a visitor to get all of the vertices
         // and the center is part of that and the extents are unimportant.
         center.set(0.0f, 0.0f, 0.0f);
         extents.set(0.0f, 0.0f, 0.0f);
      }
   }

   ///////////////////////////////////////////////////////////////////////////////////////////
   palRevoluteLink* PhysicsObject::CreateRevoluteJoint ( dtPhysics::PhysicsObject& parent,
         dtPhysics::PhysicsObject& child,
         const VectorType&         pivotAnchor,
         const VectorType&         pivotAxis,
         bool                      disableCollisionBetweenBodies)
   {
      if (parent.GetBodyWrapper() == nullptr && child.GetBodyWrapper() == nullptr)
         return nullptr;

      palBodyBase* bodyBase1( &(parent.GetBodyWrapper()->GetPalBodyBase()) );
      palBodyBase* bodyBase2( &(child.GetBodyWrapper()->GetPalBodyBase()) );
      palRevoluteLink* joint = dynamic_cast<palRevoluteLink*>(dtPhysics::PhysicsWorld::GetInstance().GetPalFactory()->CreateLink(PAL_LINK_REVOLUTE, bodyBase1, bodyBase2,
            palVector3(pivotAnchor[0], pivotAnchor[1], pivotAnchor[2]),
            palVector3(pivotAxis[0], pivotAxis[1], pivotAxis[2]),
            disableCollisionBetweenBodies));

      return joint;
   }


   ///////////////////////////////////////////////////////////////////////////////////////////
   palGenericLink* PhysicsObject::Create6DOFJoint (dtPhysics::PhysicsObject& body1,
         dtPhysics::PhysicsObject& body2,
         const dtCore::Transform& frameA,
         const dtCore::Transform& frameB,
         bool disableCollisionBetweenBodies)
   {

      if (body1.GetBodyWrapper() == nullptr && body2.GetBodyWrapper() == nullptr)
         return nullptr;

      palMatrix4x4 palFrameA, palFrameB;

      // convert to correct matrix
      TransformToPalMatrix(palFrameA, frameA);
      TransformToPalMatrix(palFrameB, frameB);

      palBodyBase* bodyBase1( &(body1.GetBodyWrapper()->GetPalBodyBase()) );
      palBodyBase* bodyBase2( &(body2.GetBodyWrapper()->GetPalBodyBase()) );


      // lower is high than upper.  That means free movement.
      palVector3 lower ( FLT_EPSILON, FLT_EPSILON, FLT_EPSILON ), upper ( 0.0, 0.0, 0.0 );
      palGenericLink* pivot = dynamic_cast<palGenericLink*>(dtPhysics::PhysicsWorld::GetInstance().GetPalFactory()->CreateLink(PAL_LINK_GENERIC, bodyBase1, bodyBase2,
            palFrameA, palFrameB,
            disableCollisionBetweenBodies));

      return pivot;
   }

   palRigidLink* PhysicsObject::CreateFixedJoint(dtPhysics::PhysicsObject& parent, dtPhysics::PhysicsObject& child, bool disableCollisionBetweenBodies)
   {
      if (parent.GetBodyWrapper() == nullptr && child.GetBodyWrapper() == nullptr)
         return nullptr;

      palBodyBase* bodyBase1( &(parent.GetBodyWrapper()->GetPalBodyBase()) );
      palBodyBase* bodyBase2( &(child.GetBodyWrapper()->GetPalBodyBase()) );
      palRigidLink* joint = dtPhysics::PhysicsWorld::GetInstance().GetPalFactory()->CreateRigidLink(bodyBase1, bodyBase2, disableCollisionBetweenBodies);

      return joint;

   }

   palMotor* PhysicsObject::CreateMotor(palLink& joint, const MotorDesc& desc)
   {
      palMotor* result = dtPhysics::PhysicsWorld::GetInstance().GetPalFactory()->CreateMotor(&joint, desc.GetAxis().GetIndex() + (desc.GetIsAngular() ? 3 : 0));
      if (result != nullptr)
      {
         result->Update(desc.GetTargetVelocity(), desc.GetMaxForce());
      }
      return result;
   }

   palLink* PhysicsObject::CreateJoint(PhysicsObject& one, PhysicsObject& two, const JointDesc& desc)
   {
      if (one.GetBodyWrapper() == nullptr && two.GetBodyWrapper() == nullptr)
         return nullptr;

      int palType = desc.GetJointType().GetPalLinkType();
      palBodyBase* bodyBase1( &(one.GetBodyWrapper()->GetPalBodyBase()) );
      palBodyBase* bodyBase2( &(two.GetBodyWrapper()->GetPalBodyBase()) );
      palMatrix4x4 palFrameA, palFrameB;
      TransformType frameOne, frameTwo;
      desc.GetBody1Frame(frameOne);
      desc.GetBody2Frame(frameTwo);
      // convert to correct matrix
      TransformToPalMatrix(palFrameA, frameOne);
      TransformToPalMatrix(palFrameB, frameTwo);

      palLink* link = dtPhysics::PhysicsWorld::GetInstance().GetPalFactory()->CreateLink(palLinkType(palType), bodyBase1, bodyBase2,
            palFrameA, palFrameB,
            desc.GetDisableCollisionBetweenBodies());

      if (link->SupportsParameters())
      {
         for (unsigned i = 0; i < 3; ++i)
         {
            link->SetParam(PAL_LINK_PARAM_DOF_MIN, desc.GetLinearLimitMinimums()[i], i);
            link->SetParam(PAL_LINK_PARAM_DOF_MAX, desc.GetLinearLimitMaximums()[i], i);
         }
         for (unsigned i = 0; i < 3; ++i)
         {
            link->SetParam(PAL_LINK_PARAM_DOF_MIN, osg::DegreesToRadians(desc.GetAngularLimitMinimums()[i]), i+3);
            link->SetParam(PAL_LINK_PARAM_DOF_MAX, osg::DegreesToRadians(desc.GetAngularLimitMaximums()[i]), i+3);
         }

         link->SetParam(PAL_LINK_PARAM_BREAK_IMPULSE, desc.GetBreakingThreshold());
      }

      return link;
   }


} // namespace dtPhysics

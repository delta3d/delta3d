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
 * Bradley Anderegg
 * Allen Danklefsen
 */

//Must include this before pal.
#include <dtCore/enginepropertytypes.h>
#include <dtCore/propertymacros.h>

#include <dtPhysics/physicsobject.h>
#include <dtPhysics/physicsmaterialactor.h>
#include <dtPhysics/palphysicsworld.h>
#include <dtPhysics/bodywrapper.h>
#include <dtPhysics/geometry.h>
#include <dtPhysics/physicsreaderwriter.h>
#include <dtGame/gameactor.h>
#include <dtCore/project.h>
#include <dtUtil/mathdefines.h>

#include <osg/ComputeBoundsVisitor>

#include <pal/palBodyBase.h>
#include <pal/palActivation.h>
#include <pal/palBodies.h>

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
      ,  mVisualToBodyIsIdentity(true)
      ,  mMaterial(NULL)
      ,  mActivationLinearVelocityThreshold(Real(-1.0))
      ,  mActivationAngularVelocityThreshold(Real(-1.0))
      ,  mActivationTimeThreshold(Real(-1.0))
      ,  mLinearDamping(Real(0.01))
      ,  mAngularDamping(Real(0.01))
      ,  mActivationSettings(NULL)
      ,  mMeshScale(Real(1.0), Real(1.0), Real(1.0))
      ,  mPropertyNamePrefixEnabled(false)
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
      bool mVisualToBodyIsIdentity;

      Material* mMaterial;

      Real mActivationLinearVelocityThreshold, mActivationAngularVelocityThreshold, mActivationTimeThreshold;
      Real mLinearDamping, mAngularDamping;

      dtCore::RefPtr<GenericBodyWrapper> mGenericBody;
      palActivationSettings* mActivationSettings;

      dtCore::ResourceDescriptor mMeshResource;
      VectorType mMeshScale;

      bool mPropertyNamePrefixEnabled; // Temporary flag for migrating old property names to newer cleaner ones.

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

         if (geom == NULL)
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
         if (mActivationSettings != NULL)
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

   /////////////////////////////////////////////////////////////////////////////
   PhysicsObject::~PhysicsObject()
   {
      CleanUp();
      delete mDataMembers;
      mDataMembers = NULL;
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
   Material* PhysicsObject::GetMaterial() const
   {
      return mDataMembers->mMaterial;
   }

   /////////////////////////////////////////////////////////////////////////////
   bool PhysicsObject::SetMaterialByIndex(dtPhysics::MaterialIndex index)
   {
      bool success = false;

      if (PhysicsWorld::IsInitialized())
      {
         dtPhysics::PhysicsMaterials& materials = PhysicsWorld::GetInstance().GetMaterials();

         Material* mat = materials.GetMaterialByIndex(index);
         if (mat != NULL)
         {
            SetMaterial(mat);
            success = true;
         }
      }

      return success;
   }

   /////////////////////////////////////////////////////////////////////////////
   void PhysicsObject::SetMaterial(Material* mat)
   {
      if (mDataMembers->mGenericBody.valid())
      {
         mDataMembers->mGenericBody->SetMaterial(mat);
      }
      mDataMembers->mMaterial = mat;
   }

   /////////////////////////////////////////////////////////////////////////////
   std::string PhysicsObject::CreatePropertyName(const std::string& baseName) const
   {
      if (mDataMembers->mPropertyNamePrefixEnabled)
      {
         std::string generatedName;
         generatedName = GetName(); // Object Name
         generatedName.append(": ");
         generatedName.append(baseName); // Property Name
         return generatedName;
      }

      return baseName;
   }

   /////////////////////////////////////////////////////////////////////////////
   void PhysicsObject::BuildPropertyMap()
   {
      // Don't make this static since it's generated.
      const dtUtil::RefString GROUP("PhysicsObject-" + GetName());

      typedef dtCore::PropertyRegHelper<PhysicsObject> PropRegType;
      PropRegType propRegHelper(*this, this, GROUP);

      dtUtil::RefString generatedName = CreatePropertyName("Collision Group");
      DT_REGISTER_PROPERTY_WITH_NAME(CollisionGroup, generatedName, "The numeric collision group for this object."
               "Groups can be configured to collide or not collide with each other."
               , PropRegType, propRegHelper);

      generatedName = CreatePropertyName("Mechanics Type");
      DT_REGISTER_PROPERTY_WITH_NAME(MechanicsType, generatedName,
               "Sets which collision type this actor will use", PropRegType, propRegHelper);

      generatedName = CreatePropertyName("Primitive Type");
      DT_REGISTER_PROPERTY_WITH_NAME(PrimitiveType, generatedName,
               "Sets which primitive type this actor will use", PropRegType, propRegHelper);

      generatedName = CreatePropertyName("Translation");
      DT_REGISTER_PROPERTY_WITH_NAME(Translation, generatedName,
               "Initial Translation", PropRegType, propRegHelper);

      generatedName = CreatePropertyName("Rotation");
      DT_REGISTER_PROPERTY_WITH_NAME(Rotation, generatedName,
               "Initial Rotation HPR", PropRegType, propRegHelper);

      generatedName = CreatePropertyName("Origin Offset");
      DT_REGISTER_PROPERTY_WITH_NAME(OriginOffset, generatedName,
               "Offsets the origin of the collision geometry relative to the center of mass."
               , PropRegType, propRegHelper);

      generatedName = CreatePropertyName("Dimensions");
      DT_REGISTER_PROPERTY_WITH_NAME(Extents, generatedName,
               "Used for determining extents of the collision volume"
               , PropRegType, propRegHelper);

      generatedName = CreatePropertyName("Mass");
      DT_REGISTER_PROPERTY_WITH_NAME(Mass, generatedName,
               "Total Mass of the object"
               , PropRegType, propRegHelper);

      generatedName = CreatePropertyName("Skin Thickness");
      DT_REGISTER_PROPERTY_WITH_NAME(SkinThickness, generatedName,
               "How far things may penetrate this physics object. Improves stability. Don't make this 0."
               , PropRegType, propRegHelper);

      generatedName = CreatePropertyName("Notify Collisions");
      DT_REGISTER_PROPERTY_WITH_NAME(NotifyCollisions, generatedName,
               "Set to true to enable collisions notifications for this object."
               , PropRegType, propRegHelper);

      generatedName = CreatePropertyName("Moment Of Inertia");
      DT_REGISTER_PROPERTY_WITH_NAME(MomentOfInertia, generatedName,
               "Sets the moment of inerta tensor 3x3 matrix diagonals.  If any value of this is 0 or negative, "
               "it will be computed from the geometry."
               , PropRegType, propRegHelper);

      generatedName = CreatePropertyName("ActivationLinearVelocityThreshold");
      DT_REGISTER_PROPERTY_WITH_NAME(ActivationLinearVelocityThreshold, generatedName,
               "the linear velocity threshold under which the object will auto-deactivate. -1 means engine default."
               , PropRegType, propRegHelper);

      generatedName = CreatePropertyName("ActivationAngularVelocityThreshold");
      DT_REGISTER_PROPERTY_WITH_NAME(ActivationAngularVelocityThreshold, generatedName,
               "the angular velocity threshold under which the object will auto-deactivate. -1 means engine default."
               , PropRegType, propRegHelper);

      generatedName = CreatePropertyName("ActivationTimeThreshold");
      DT_REGISTER_PROPERTY_WITH_NAME(ActivationTimeThreshold, generatedName,
               "The minimum time the object must be under all the activation thresholds before the object will auto-deactivate. -1 means engine default."
               , PropRegType, propRegHelper);

      generatedName = CreatePropertyName("LinearDamping");
      DT_REGISTER_PROPERTY_WITH_NAME(LinearDamping, generatedName,
               "Artifical linear body damping. 0 means off, 1 means pretty much don't move."
               , PropRegType, propRegHelper);

      generatedName = CreatePropertyName("AngularDamping");
      DT_REGISTER_PROPERTY_WITH_NAME(AngularDamping, generatedName,
               "Artifical angular body damping. 0 means off, 1 means pretty much don't move."
               , PropRegType, propRegHelper);

      generatedName = CreatePropertyName("PhysicsMesh");
      DT_REGISTER_RESOURCE_PROPERTY_WITH_NAME(dtCore::DataType::STATIC_MESH, MeshResource, generatedName, generatedName,
               "Geometry file to load for the physics to use.  It can be either a renderable mesh or a compiled physics mesh.",
                PropRegType, propRegHelper);

      generatedName = CreatePropertyName("PhysicsMeshScale");
      DT_REGISTER_PROPERTY_WITH_NAME(MeshScale, generatedName,
               "If a physics mesh is set, it can be scaled with this property.",
                PropRegType, propRegHelper);

   }

   void PhysicsObject::CalculateBoundsAndOrigin(const osg::Node* nodeToLoad, bool calcDimensions, bool adjustOriginOffsetForGeometry)
   {
      VectorType center, newDimensions;
      osg::BoundingBox bBox;

      if (nodeToLoad != NULL)
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
         if (nodeToLoad != NULL)
         {
            bool polytope = GetPrimitiveType() == PrimitiveType::CONVEX_HULL;
            VertexData::GetOrCreateCachedDataForNode(data, nodeToLoad, polytope && !cachingKey.empty() ? cachingKey + POLYTOPE_SUFFIX : cachingKey, polytope);
         }
         else
         {
            GetVertexDataForResource(data, cachingKey);
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
               vertDataOut = NULL;
               throw dtUtil::Exception("Unable to load triangle data from existing file resource: "
                     + GetMeshResource().GetResourceIdentifier(), __FILE__, __LINE__);
            }
         }
      }
   }

   /////////////////////////////////////////////////////////////////////////////
   bool PhysicsObject::CreateFromGeometry(dtPhysics::Geometry& geometry)
   {
      dtCore::Transform transform;
      GetTransform(transform);

      mDataMembers->mGeometries.clear();

      mDataMembers->mGenericBody = BaseBodyWrapper::CreateGenericBody(transform, *mDataMembers->mMechanicsType, mDataMembers->mCollisionGroup, mDataMembers->mMassOfObject);

      if (mDataMembers->mGenericBody.valid())
      {
         mDataMembers->mGenericBody->Init();

         AddGeometry(geometry);

         //Reset this because it has to be done after geometry is added.
         SetMomentOfInertia(mDataMembers->mMomentOfInertia);
      }
      else
      {
         LOGN_ERROR("physicsobject.cpp", "Created a GenericBodyWrapper, but dynamic casting it to one failed.  Bailing out.");
         mDataMembers->mGenericBody = NULL;
         return false;
      }

      // Set the default material if nothing is set.
      if (GetMaterial() == NULL)
      {
         SetMaterial(PhysicsWorld::GetInstance().GetMaterials().GetMaterial(PhysicsMaterials::DEFAULT_MATERIAL_NAME));
      }
      SetNotifyCollisions(GetNotifyCollisions());

      CreateWithBody(*mDataMembers->mGenericBody);

      mDataMembers->ApplyActivationSettings();
      SetLinearDamping(mDataMembers->mLinearDamping);
      SetAngularDamping(mDataMembers->mAngularDamping);

      return true;
   }

   /////////////////////////////////////////////////////////////////////////////
   bool PhysicsObject::CreateInternal(VertexData* data)
   {
      TransformType xform;
      GetTransform(xform);
      mDataMembers->mGeometries.clear();

      mDataMembers->mGenericBody = BaseBodyWrapper::CreateGenericBody(xform, *mDataMembers->mMechanicsType, mDataMembers->mCollisionGroup, mDataMembers->mMassOfObject);

      if (mDataMembers->mGenericBody.valid())
      {
         mDataMembers->mGenericBody->Init();
         dtCore::Transform geometryWorld;
         GetOriginOffsetInWorldSpace(geometryWorld);
         if (GetPrimitiveType().IsSimpleShape())
         {
            mDataMembers->CreateSimpleGeometry(GetPrimitiveType(), GetExtents(), geometryWorld, GetMass());
         }
         else if (data != NULL)
         {
            // Scale the data in place, which is actually the cached one.
            // Each user will just rescale the data to what they need, but
            // chances are good that the same scale will end up being used over and over.
            data->Scale(mDataMembers->mMeshScale);

            mDataMembers->CreateComplexGeometry(GetPrimitiveType(), geometryWorld, *data, GetMass());
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
         mDataMembers->mGenericBody = NULL;
         return false;
      }


      // Set the default material if nothing is set.
      if (GetMaterial() == NULL)
      {
         SetMaterial(PhysicsWorld::GetInstance().GetMaterials().GetMaterial(PhysicsMaterials::DEFAULT_MATERIAL_NAME));
      }
      SetSkinThickness(mDataMembers->mSkinThickness);
      SetNotifyCollisions(GetNotifyCollisions());

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
      mDataMembers->mActivationSettings = NULL;
      // These are ref ptrs, so they should get cleaned up
      mDataMembers->mGenericBody = NULL;
   }

   /////////////////////////////////////////////////////////////////////////////
   Real PhysicsObject::GetSkinThickness() const
   {
      Real thickness = mDataMembers->mSkinThickness;
      if (mDataMembers->mGenericBody != NULL)
      {
         thickness = mDataMembers->mGenericBody->GetSkinWidth();
      }
      return thickness;
   }

   /////////////////////////////////////////////////////////////////////////////
   void PhysicsObject::SetSkinThickness(Real st)
   {
      mDataMembers->mSkinThickness = st;
      if (mDataMembers->mGenericBody != NULL)
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
      if (mDataMembers->mGenericBody != NULL)
      {
         g = mDataMembers->mGenericBody->GetGroup();
      }
      return g;
   }

   /////////////////////////////////////////////////////////////////////////////
   void PhysicsObject::SetCollisionGroup(CollisionGroup group)
   {
      mDataMembers->mCollisionGroup = group;
      if (mDataMembers->mGenericBody != NULL)
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
      return NULL;
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
      if (mDataMembers->mGenericBody.valid())
      {
         return mDataMembers->mGenericBody->GetPalGenericBody().SetCollisionResponseEnabled(enabled);
      }
   }

   /////////////////////////////////////////////////////////////////////////////
   bool PhysicsObject::IsCollisionResponseEnabled() const
   {
      bool result = true;
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
      if (mDataMembers->mActivationSettings != NULL)
      {
         return Real(mDataMembers->mActivationSettings->GetActivationLinearVelocityThreshold());
      }
      return mDataMembers->mActivationLinearVelocityThreshold;
   }

   /////////////////////////////////////////////////////////////////////////////
   void PhysicsObject::SetActivationLinearVelocityThreshold(Real threshold)
   {
      if (mDataMembers->mActivationSettings != NULL)
      {
         mDataMembers->mActivationSettings->SetActivationLinearVelocityThreshold(Float(threshold));
      }
      mDataMembers->mActivationLinearVelocityThreshold = threshold;
   }

   /////////////////////////////////////////////////////////////////////////////
   Real PhysicsObject::GetActivationAngularVelocityThreshold() const
   {
      if (mDataMembers->mActivationSettings != NULL)
      {
         return Real(mDataMembers->mActivationSettings->GetActivationAngularVelocityThreshold());
      }
      return mDataMembers->mActivationAngularVelocityThreshold;
   }

   /////////////////////////////////////////////////////////////////////////////
   void PhysicsObject::SetActivationAngularVelocityThreshold(Real threshold)
   {
      if (mDataMembers->mActivationSettings != NULL)
      {
         mDataMembers->mActivationSettings->SetActivationAngularVelocityThreshold(Float(threshold));
      }
      mDataMembers->mActivationAngularVelocityThreshold = threshold;
   }

   /////////////////////////////////////////////////////////////////////////////
   Real PhysicsObject::GetActivationTimeThreshold() const
   {
      if (mDataMembers->mActivationSettings != NULL)
      {
         return Real(mDataMembers->mActivationSettings->GetActivationTimeThreshold());
      }
      return mDataMembers->mActivationTimeThreshold;
   }

   /////////////////////////////////////////////////////////////////////////////
   void PhysicsObject::SetActivationTimeThreshold(Real threshold)
   {
      if (mDataMembers->mActivationSettings != NULL)
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

   /////////////////////////////////////////////////////////////////////////////
   void PhysicsObject::SetPropertyNamePrefixEnabled(bool enablePrefix)
   {
      mDataMembers->mPropertyNamePrefixEnabled = enablePrefix;
   }

   /////////////////////////////////////////////////////////////////////////////
   bool PhysicsObject::IsPropertyNamePrefixEnabled() const
   {
      return mDataMembers->mPropertyNamePrefixEnabled;
   }


} // namespace dtPhysics

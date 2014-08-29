/* -*-c++-*-
 * dtPhysics
 * Copyright 2007-2008, Alion Science and Technology
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
 */

#include <dtPhysics/bodywrapper.h>
#include <dtPhysics/palutil.h>
#include <dtPhysics/trianglerecorder.h>
#include <dtPhysics/physicsmaterialactor.h>
#include <dtPhysics/geometry.h>
#include <pal/palFactory.h>
#include <pal/palBodies.h>
#include <pal/palStatic.h>
#include <dtUtil/log.h>
#include <dtUtil/exception.h>

namespace dtPhysics
{
   /////////////////////////////////////////////////////////////////////////////
   class BaseBodyWrapperImpl
   {
      public:
         ///////////////////////////////////////////////////////////////////////////
         BaseBodyWrapperImpl() : mBody(NULL) {}
         ~BaseBodyWrapperImpl()
         {
            SafeUninitialize();
         }

         ///////////////////////////////////////////////////////////////////////////
         void SafeUninitialize()
         {
            // do some unintialization
            delete mBody;

            // reset back to NULL
            mBody = NULL;
         }

         palBodyBase* mBody;

   };

   /////////////////////////////////////////////////////////////////////////////
   class BodyWrapperImpl
   {
      public:
         ///////////////////////////////////////////////////////////////////////////
         BodyWrapperImpl() : mDynBody(NULL){}
         ~BodyWrapperImpl()
         {
            SafeUninitialize();
         }

         ///////////////////////////////////////////////////////////////////////////
         void SafeUninitialize()
         {
            mDynBody = NULL;
         }

         palBody* mDynBody;
   };

   /////////////////////////////////////////////////////////////////////////////
   class GenericBodyWrapperImpl
   {
      public:
         ///////////////////////////////////////////////////////////////////////////
         GenericBodyWrapperImpl() : mGenericBody(NULL) {}
         ~GenericBodyWrapperImpl()
         {
            SafeUninitialize();
         }

         ///////////////////////////////////////////////////////////////////////////
         void SafeUninitialize()
         {
            mGenericBody = NULL;
         }

         palGenericBody* mGenericBody;
   };

   ///////////////////////////////////////////////////////////////////////////
   BaseBodyWrapper::BaseBodyWrapper(palBodyBase& body)
   {
      mImpl = new BaseBodyWrapperImpl();
      mImpl->mBody = &body;
   }

   ///////////////////////////////////////////////////////////////////////////
   BaseBodyWrapper::~BaseBodyWrapper()
   {
      delete mImpl;
      mImpl = NULL;
   }

   ///////////////////////////////////////////////////////////////////////////
   palBodyBase& BaseBodyWrapper::GetPalBodyBase()
   {
      return *mImpl->mBody;
   }

   ///////////////////////////////////////////////////////////////////////////
   void BaseBodyWrapper::SetGroup(CollisionGroup group)
   {
      mImpl->mBody->SetGroup(palGroup(group));
   }

   ///////////////////////////////////////////////////////////////////////////
   CollisionGroup BaseBodyWrapper::GetGroup() const
   {
      CollisionGroup g = 0;
      g = mImpl->mBody->GetGroup();
      return g;
   }

   ///////////////////////////////////////////////////////////////////////////
   palBody& BodyWrapper::GetPalBody()
   {
      return *mImpl->mDynBody;
   }

   ///////////////////////////////////////////////////////////////////////////
   BodyWrapper::BodyWrapper(palBody& body)
   : BaseBodyWrapper(body)
   {
      mImpl = new BodyWrapperImpl();
      mImpl->mDynBody = &body;
   }

   ///////////////////////////////////////////////////////////////////////////
   BodyWrapper::~BodyWrapper()
   {
      delete mImpl;
   }

   ///////////////////////////////////////////////////////////////////////////
   dtCore::RefPtr<BaseBodyWrapper> BaseBodyWrapper::CreateBox(const TransformType& worldPos,
                           const VectorType& dimensions, Real mass)
   {
      // cast so we can call our specific initialization function
      palBox* ourBox = palFactory::GetInstance()->CreateBox();

      if (ourBox == NULL)
      {
         return NULL;
      }

      dtCore::RefPtr<BodyWrapper> bodyWrapper = new BodyWrapper(*ourBox);

      palMatrix4x4 palOrigin;
      TransformToPalMatrix(palOrigin, worldPos);

      ourBox->Init(0.0, 0.0, 0.0,
                   dimensions[0], dimensions[1], dimensions[2], mass);
      ourBox->SetPosition(palOrigin);

      return bodyWrapper.get();
   }

   ///////////////////////////////////////////////////////////////////////////
   dtCore::RefPtr<BaseBodyWrapper> BaseBodyWrapper::CreateStaticBox(const TransformType& worldPos,
                           const VectorType& dimensions)
   {
      // cast so we can call our specific initialization function
      static const std::string palStaticBoxName("palStaticBox");
      palStaticBox* ourBox = dynamic_cast<palStaticBox*>(palFactory::GetInstance()->CreateObject(palStaticBoxName));

      if (ourBox == NULL)
      {
         return NULL;
      }

      dtCore::RefPtr<BaseBodyWrapper> bodyWrapper = new BaseBodyWrapper(*ourBox);

      palMatrix4x4 palOrigin;
      TransformToPalMatrix(palOrigin, worldPos);

      ourBox->Init(palOrigin,
                   dimensions[0], dimensions[1], dimensions[2]);

      return bodyWrapper;
   }

   ///////////////////////////////////////////////////////////////////////////
   dtCore::RefPtr<BaseBodyWrapper> BaseBodyWrapper::CreateSphere(const TransformType& worldPos,
                             const VectorType& dimensions, Real mass)
   {
      // cast so we can call our specific initialization function
      palSphere* ourSphere = palFactory::GetInstance()->CreateSphere();

      if (ourSphere == NULL)
      {
         return NULL;
      }

      dtCore::RefPtr<BodyWrapper> bodyWrapper = new BodyWrapper(*ourSphere);

      palMatrix4x4 palOrigin;
      TransformToPalMatrix(palOrigin, worldPos);

      // init
      ourSphere->Init(0.0, 0.0, 0.0,
                      dimensions[0],
                      mass);

      ourSphere->SetPosition(palOrigin);

      return bodyWrapper.get();
   }

   ///////////////////////////////////////////////////////////////////////////
   dtCore::RefPtr<BaseBodyWrapper> BaseBodyWrapper::CreateCylinder(const TransformType& worldPos,
                               const VectorType& dimensions, Real mass)
   {
      // cast so we can call our specific initialization function
      palCapsule* ourCylinder = palFactory::GetInstance()->CreateCapsule();

      if (ourCylinder == NULL)
      {
         return NULL;
      }

      dtCore::RefPtr<BodyWrapper> bodyWrapper = new BodyWrapper(*ourCylinder);

      palMatrix4x4 palOrigin;
      TransformToPalMatrix(palOrigin, worldPos);

      // init
      ourCylinder->Init(0.0, 0.0, 0.0,
        dimensions[1], dimensions[0], mass);

      ourCylinder->SetPosition(palOrigin);

      return bodyWrapper.get();
   }



   ///////////////////////////////////////////////////////////////////////////
   dtCore::RefPtr<BaseBodyWrapper> BaseBodyWrapper::CreateConvexHull(const TransformType& worldPos,
                                 const osg::Node* mesh, Real mass)
   {
      TriangleRecorder recorder;
      recorder.Record(*mesh);
      if (recorder.mData.mVertices->empty() || recorder.mData.mFaces->empty())
      {
         throw dtUtil::Exception("Unable to create convex mesh, no vertex data was found when traversing the osg Node.", __FILE__, __LINE__);
      }

      // cast so we can call our specific initialization function
      palConvex* ourBody = palFactory::GetInstance()->CreateConvex();

      if (ourBody == NULL)
      {
         return NULL;
      }

      dtCore::RefPtr<BodyWrapper> bodyWrapper = new BodyWrapper(*ourBody);

      palMatrix4x4 palOrigin;
      TransformToPalMatrix(palOrigin, worldPos);

      ourBody->Init(0.0, 0.0, 0.0,
               (Real*)(&recorder.mData.mVertices->front()), recorder.mData.mVertices->size(), mass);

      ourBody->SetPosition(palOrigin);

      return bodyWrapper.get();
   }

   ///////////////////////////////////////////////////////////////////////////
   dtCore::RefPtr<BaseBodyWrapper> BaseBodyWrapper::CreateTriangleMesh(const TransformType& worldPos,
            const osg::Node* mesh, Real mass)
   {
      // cast so we can call our specific initialization function
      palCompoundBody* ourBody = palFactory::GetInstance()->CreateCompoundBody();

      if (ourBody == NULL)
      {
         return NULL;
      }

      dtCore::RefPtr<BodyWrapper> bodyWrapper = new BodyWrapper(*ourBody);

      palMatrix4x4 palOrigin;
      TransformToPalMatrix(palOrigin, worldPos);

      //palConcaveGeometry* pgeom = dynamic_cast<palConcaveGeometry*>(ourBody->AddGeometry("palConvexGeometry"));

//      pgeom->Init()
//
//      ourBody->Init(0.0, 0.0, 0.0);

      ourBody->SetPosition(palOrigin);

      return bodyWrapper.get();
   }

   ///////////////////////////////////////////////////////////////////////////
   dtCore::RefPtr<BaseBodyWrapper> BaseBodyWrapper::CreateHeightfield(const TransformType& worldPos,
                                  const osg::Node* mesh)
   {
      return BaseBodyWrapper::CreateTerrainMesh(worldPos, mesh);
   }

   ///////////////////////////////////////////////////////////////////////////
   dtCore::RefPtr<BaseBodyWrapper> BaseBodyWrapper::CreateTerrainMesh(const TransformType& worldPos,
                                  const osg::Node* mesh)
   {
      TriangleRecorder recorder;
      recorder.Record(*mesh);
      if (recorder.mData.mVertices->empty() || recorder.mData.mFaces->empty())
      {
         throw dtUtil::Exception("Unable to create terrain mesh, no vertex data was found when traversing the osg Node.", __FILE__, __LINE__);
      }

      // cast so we can call our specific initialization function
      palTerrainMesh* ourTerrainMesh = palFactory::GetInstance()->CreateTerrainMesh();

      if (ourTerrainMesh == NULL)
      {
         return NULL;
      }

      dtCore::RefPtr<BaseBodyWrapper> bodyWrapper = new BaseBodyWrapper(*ourTerrainMesh);

      osg::Vec3d trans;
      worldPos.GetTranslation(trans);

      ourTerrainMesh->Init(trans.x(), trans.y(), trans.z(),
               (Real*)(&recorder.mData.mVertices->front()), recorder.mData.mVertices->size(),
               reinterpret_cast<int*>(&recorder.mData.mFaces->front()), recorder.mData.mFaces->size());

      return bodyWrapper;
   }

   ////////////////////////////////////////////////////////////////
   dtCore::RefPtr<BaseBodyWrapper> BaseBodyWrapper::CreateGenericBody(const TransformType& worldPos,
            MechanicsType& mechType, CollisionGroup collisionGroup, Real mass)
   {
      // cast so we can call our specific initialization function
      palGenericBody* body = dynamic_cast<palGenericBody*>(palFactory::GetInstance()->CreateObject("palGenericBody"));

      if (body == NULL)
      {
         return NULL;
      }

      dtCore::RefPtr<GenericBodyWrapper> bodyWrapper = new GenericBodyWrapper(*body);


      bodyWrapper->SetMechanicsType(mechType);
      bodyWrapper->SetGroup(collisionGroup);
      bodyWrapper->SetMass(mass);
      bodyWrapper->SetTransform(worldPos);

      return bodyWrapper;
   }

   ///////////////////////////////////////////////////////////////////////////
   void BaseBodyWrapper::GetTransform(TransformType& xform) const
   {
      // set initial matrix
      palMatrix4x4 palmatrix;

      // this sets orientation and matrix
      palmatrix = mImpl->mBody->GetLocationMatrix();

      // convert to correct matrix
      PalMatrixToTransform(xform, palmatrix);
   }

   ///////////////////////////////////////////////////////////////////////////
   void BaseBodyWrapper::SetMaterial(Material* material)
   {
      // do lookup here set it on mBody
      if (material != NULL)
      {
         mImpl->mBody->SetMaterial(material);
      }
   }

   ///////////////////////////////////////////////////////////////////////////
   void BodyWrapper::AddForce(const VectorType& forceToAdd)
   {
      mImpl->mDynBody->ApplyForce(forceToAdd[0], forceToAdd[1], forceToAdd[2]);
   }

   ///////////////////////////////////////////////////////////////////////////
   void BodyWrapper::AddForceAtPosition(const VectorType& position, const VectorType& forceAmount)
   {
      mImpl->mDynBody->ApplyForceAtPosition(position[0], position[1], position[2],
               forceAmount[0], forceAmount[1], forceAmount[2]);
   }

   ///////////////////////////////////////////////////////////////////////////
//   void BodyWrapper::SetTorque(const VectorType& torqueToSet)
//   {
//   }

   ///////////////////////////////////////////////////////////////////////////
   void BodyWrapper::AddTorque(const VectorType& torqueToAdd)
   {
      mImpl->mDynBody->ApplyTorque(torqueToAdd[0], torqueToAdd[1], torqueToAdd[2]);
   }

   ///////////////////////////////////////////////////////////////////////////
   void BodyWrapper::ApplyImpulse(const VectorType& impulseAmount)
   {
      mImpl->mDynBody->ApplyImpulse(impulseAmount[0], impulseAmount[1], impulseAmount[2]);
   }

   ///////////////////////////////////////////////////////////////////////////
   void BodyWrapper::ApplyAngularImpulse(const VectorType& angularImpulse)
   {
      mImpl->mDynBody->ApplyAngularImpulse(angularImpulse[0], angularImpulse[1], angularImpulse[2]);
   }

   ///////////////////////////////////////////////////////////////////////////
   void BodyWrapper::ApplyImpulseAtPosition(const VectorType& position, const VectorType& impulseAmount)
   {
      mImpl->mDynBody->ApplyImpulseAtPosition(position[0], position[1], position[2],
                                    impulseAmount[0], impulseAmount[1], impulseAmount[2]);
   }

   ///////////////////////////////////////////////////////////////////////////
   void BodyWrapper::SetLinearVelocity(const VectorType& velocity)
   {
      palVector3 vel;
      vel._vec[0] = velocity[0];
      vel._vec[1] = velocity[1];
      vel._vec[2] = velocity[2];

      mImpl->mDynBody->SetLinearVelocity(vel);
   }

   ///////////////////////////////////////////////////////////////////////////
   void BodyWrapper::SetAngularVelocity(const VectorType& velocity_rad)
   {
      if (mImpl->mDynBody == NULL)
         return;

      palVector3 vel;
      vel._vec[0] = velocity_rad[0];
      vel._vec[1] = velocity_rad[1];
      vel._vec[2] = velocity_rad[2];

      mImpl->mDynBody->SetAngularVelocity(vel);
   }

   ///////////////////////////////////////////////////////////////////////////
   VectorType BodyWrapper::GetLinearVelocity() const
   {
      palVector3 vel;
      VectorType velocity;

      mImpl->mDynBody->GetLinearVelocity(vel);

      velocity[0] = vel._vec[0];
      velocity[1] = vel._vec[1];
      velocity[2] = vel._vec[2];
      return velocity;
   }

   ///////////////////////////////////////////////////////////////////////////
   VectorType BodyWrapper::GetAngularVelocity() const
   {
      palVector3 vel;
      VectorType velocity;

      mImpl->mDynBody->GetAngularVelocity(vel);

      velocity[0] = vel._vec[0];
      velocity[1] = vel._vec[1];
      velocity[2] = vel._vec[2];
      return velocity;
   }

//   VectorType BodyWrapper::GetForce() const
//   {
//      return mImpl->mDynBody->GetForce();
//   }
//
//   VectorType BodyWrapper::GetTorque() const
//   {
//      return mImpl->mDynBody->GetTorque();
//   }

   ///////////////////////////////////////////////////////////////////////////
   bool BodyWrapper::IsActive()
   {
      return mImpl->mDynBody->IsActive();
   }

   ///////////////////////////////////////////////////////////////////////////
   void BodyWrapper::SetActive(bool active)
   {
      mImpl->mDynBody->SetActive(active);
   }

   ///////////////////////////////////////////////////////////////////////////
   void BodyWrapper::ResetForces()
   {
      SetAngularVelocity(VectorType(0,0,0));
      SetLinearVelocity(VectorType(0,0,0));
   }

   ///////////////////////////////////////////////////////////////////////////
   void BodyWrapper::SetTransform(const TransformType& xform)
   {
      // set initial matrix
      palMatrix4x4 palmatrix;

      // convert to correct matrix
      TransformToPalMatrix(palmatrix, xform);

      // this sets orientation and matrix
      mImpl->mDynBody->SetPosition(palmatrix);
   }

   ///////////////////////////////////////////////////////////////////////////
   Real BodyWrapper::GetMass()
   {
      //Pal should really not make this public.
      return mImpl->mDynBody->m_fMass;
   }

   ///////////////////////////////////////////////////////////////////////////
   GenericBodyWrapper::GenericBodyWrapper(palGenericBody& body)
   : BodyWrapper(body)
   , mImpl(new GenericBodyWrapperImpl)
   {
      mImpl->mGenericBody = &body;
   }

   ///////////////////////////////////////////////////////////////////////////
   GenericBodyWrapper::~GenericBodyWrapper(){}

   ///////////////////////////////////////////////////////////////////////////
   void GenericBodyWrapper::Init()
   {
      // The creation function sets the transform, so we pass that down to the init function to make
      // the user not need to do so.
      mImpl->mGenericBody->Init(mImpl->mGenericBody->GetLocationMatrix());
   }

   ///////////////////////////////////////////////////////////////////////////
   MechanicsType& GenericBodyWrapper::GetMechanicsType() const
   {
      palDynamicsType dt = mImpl->mGenericBody->GetDynamicsType();
      MechanicsType* mt = &MechanicsType::DYNAMIC;
      switch (dt)
      {
         case PALBODY_DYNAMIC:
         {
            mt = &MechanicsType::DYNAMIC;
            break;
         }
         case PALBODY_STATIC:
         {
            mt = &MechanicsType::STATIC;
            break;
         }
         case PALBODY_KINEMATIC:
         {
            mt = &MechanicsType::KINEMATIC;
            break;
         }
      }
      return *mt;
   }

   ///////////////////////////////////////////////////////////////////////////
   void GenericBodyWrapper::SetMechanicsType(MechanicsType& mechType)
   {
      mImpl->mGenericBody->SetDynamicsType(palDynamicsType(mechType.GetID()));
   }

   ///////////////////////////////////////////////////////////////////////////
   void GenericBodyWrapper::SetMass(Real newMass)
   {
      mImpl->mGenericBody->SetMass(newMass);
   }

   ///////////////////////////////////////////////////////////////////////////
   void GenericBodyWrapper::SetInertia(const VectorType& ixxiyyizz)
   {
      mImpl->mGenericBody->SetInertia(ixxiyyizz.x(), ixxiyyizz.y(), ixxiyyizz.z());
   }

   ///////////////////////////////////////////////////////////////////////////
   VectorType GenericBodyWrapper::GetInertia() const
   {
      VectorType inertia;
      mImpl->mGenericBody->GetInertia(inertia.x(), inertia.y(), inertia.z());
      return inertia;
   }

   ///////////////////////////////////////////////////////////////////////////
   void GenericBodyWrapper::ConnectGeometry(Geometry& toAdd)
   {
      mImpl->mGenericBody->ConnectGeometry(&toAdd.GetPalGeometry());
   }

   ///////////////////////////////////////////////////////////////////////////
   void GenericBodyWrapper::RemoveGeometry(Geometry& toRemove)
   {
      mImpl->mGenericBody->RemoveGeometry(&toRemove.GetPalGeometry());
   }

   ///////////////////////////////////////////////////////////////////////////
   void GenericBodyWrapper::SetGravityEnabled(bool enable)
   {
      mImpl->mGenericBody->SetGravityEnabled(enable);
   }

   ///////////////////////////////////////////////////////////////////////////
   bool GenericBodyWrapper::IsGravityEnabled() const
   {
      return mImpl->mGenericBody->IsGravityEnabled();
   }

   ///////////////////////////////////////////////////////////////////////////
   bool GenericBodyWrapper::SetSkinWidth(Real skinWidth)
   {
      return mImpl->mGenericBody->SetSkinWidth(skinWidth);
   }

   ///////////////////////////////////////////////////////////////////////////
   Real GenericBodyWrapper::GetSkinWidth() const
   {
      return mImpl->mGenericBody->GetSkinWidth();
   }

   ///////////////////////////////////////////////////////////////////////////
   palGenericBody& GenericBodyWrapper::GetPalGenericBody()
   {
      return *mImpl->mGenericBody;
   }
}

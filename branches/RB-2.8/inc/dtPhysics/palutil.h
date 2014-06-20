/*
 * palutil.h
 *
 *  Created on: Jul 11, 2008
 *      Author: david
 */

#ifndef PALUTIL_H_
#define PALUTIL_H_

#include <dtUtil/warningdisable.h>
#include <dtUtil/mswin.h>
#include <pal/palMath.h>
#include <pal/palCollision.h>
#include <pal/pal.h>
#include <dtPhysics/physicstypes.h>
#include <dtPhysics/collisioncontact.h>
#include <dtPhysics/raycast.h>
#include <dtPhysics/physicsmaterials.h>

namespace dtPhysics
{

   ///////////////////////////////////////////////////////////////////////////
   template <class DtMatrixType>
   inline void TransformToPalMatrix(palMatrix4x4& fillInMatrix, const DtMatrixType& xform)
   {
      for (unsigned i = 0; i < 4; ++i)
      {
         for (unsigned j = 0; j < 4; ++j)
         {
            fillInMatrix._mat[4*i + j] = xform(i, j);
         }

      }
   }

   ///////////////////////////////////////////////////////////////////////////
   template <class DtMatrixType>
   inline void PalMatrixToTransform(DtMatrixType& xform, const palMatrix4x4& palMatrix)
   {
      for (unsigned i = 0; i < 4; ++i)
      {
         for (unsigned j = 0; j < 4; ++j)
         {
             xform(i, j) = palMatrix._mat[4*i + j];
         }

      }
   }

   ///////////////////////////////////////////////////////////////////////////
   template <typename palVecType, typename dtVecType>
   inline void PalVecToVectorType(dtVecType& dtVec, const palVecType& palVec)
   {
      for (size_t i = 0; i < dtVecType::num_components; ++i)
      {
         dtVec[i] = palVec[i];
      }
   }

   ///////////////////////////////////////////////////////////////////////////
   template <typename palVecType, typename dtVecType>
   inline void VectorTypeToPalVec(palVecType& palVec, const dtVecType& dtVec)
   {
      for (size_t i = 0; i < dtVecType::num_components; ++i)
      {
         palVec[i] = dtVec[i];
      }
   }

   ///////////////////////////////////////////////////////////////////////////
   inline void MatDefToPalMatDesc(palMaterialDesc& desc, const MaterialDef& def)
   {
      desc.m_fStatic = def.GetStaticFriction();
      desc.m_fKinetic = def.GetKineticFriction();
      desc.m_fRestitution = def.GetRestitution();
      VectorTypeToPalVec(desc.m_vStaticAnisotropic, def.GetStaticAnisotropicFriction());
      VectorTypeToPalVec(desc.m_vKineticAnisotropic, def.GetKineticAnisotropicFriction());
      VectorTypeToPalVec(desc.m_vDirAnisotropy, def.GetDirOfAnisotropy());
      desc.m_bEnableAnisotropicFriction = def.GetEnableAnisotropicFriction();
      desc.m_bDisableStrongFriction = def.GetDisableStrongFriction();
   }

   ///////////////////////////////////////////////////////////////////////////
   inline void PalMatDescToMatDef(MaterialDef& def, const palMaterialDesc& desc)
   {
      def.SetStaticFriction(desc.m_fStatic);
      def.SetKineticFriction(desc.m_fKinetic);
      def.SetRestitution(desc.m_fRestitution);
      osg::Vec3 tmp;
      PalVecToVectorType(tmp, desc.m_vStaticAnisotropic);
      def.SetStaticAnisotropicFriction(tmp);
      PalVecToVectorType(tmp, desc.m_vKineticAnisotropic);
      def.SetKineticAnisotropicFriction(tmp);
      PalVecToVectorType(tmp, desc.m_vDirAnisotropy);
      def.SetDirOfAnisotropy(tmp);
      def.SetEnableAnisotropicFriction(desc.m_bEnableAnisotropicFriction);
      def.SetDisableStrongFriction(desc.m_bDisableStrongFriction);
   }

   ///////////////////////////////////////////////////////////////////////////
   inline void PalRayHitToRayCastReport(RayCast::Report& report, const palRayHit& rayHit)
   {
      report.mHasHitObject = rayHit.m_bHit;
      if (rayHit.m_bHit)
      {
         report.mDistance = rayHit.m_fDistance;
         if (rayHit.m_bHitNormal)
         {
            PalVecToVectorType(report.mHitNormal, rayHit.m_vHitNormal);
         }

         if (rayHit.m_bHitPosition)
         {
            PalVecToVectorType(report.mHitPos, rayHit.m_vHitPosition);
         }

         if (rayHit.m_pBody != NULL)
         {
            report.mHitObject = (PhysicsObject*)(rayHit.m_pBody->GetUserData());
         }
      }
   }

   ///////////////////////////////////////////////////////////////////////////
   class ContactConverter
   {
   public:
      ContactConverter(std::vector<CollisionContact>& contacts)
      : mContacts(contacts)
      {}
      void operator () (palContactPoint& contactPoint)
      {
         // Creating
         CollisionContact c;

         // PhysicsObject always sets the user data to the physics object, so if it's set
         // it should be the physics object.  Unfortunately, there is no way to force that to be the case.
         // It's also a void* so there's no way to know if it's actually the right type.
         if (contactPoint.m_pBody1 != NULL)
         {
            PhysicsObject* po1 = reinterpret_cast<PhysicsObject*>(contactPoint.m_pBody1->GetUserData());
            c.mObject1 = po1;
         }
         if (contactPoint.m_pBody2 != NULL)
         {
            PhysicsObject* po2 = reinterpret_cast<PhysicsObject*>(contactPoint.m_pBody2->GetUserData());
            c.mObject2 = po2;
         }
         c.mDistance = contactPoint.m_fDistance;
         c.mImpulse = contactPoint.m_fImpulse;
         PalVecToVectorType(c.mNormal, contactPoint.m_vContactNormal);
         PalVecToVectorType(c.mPosition, contactPoint.m_vContactPosition);
         mContacts.push_back(c);
         PalVecToVectorType(c.mImpulseLateral1, contactPoint.m_vImpulseLateral1);
         PalVecToVectorType(c.mImpulseLateral2, contactPoint.m_vImpulseLateral2);
      }
      std::vector<CollisionContact>& mContacts;
   };

}

#endif /* PALUTIL_H_ */

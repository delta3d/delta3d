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

#ifndef COLLISIONPRIMITIVE_H_
#define COLLISIONPRIMITIVE_H_

#include <dtPhysics/physicsexport.h>
#include <dtPhysics/physicstypes.h>
#include <dtPhysics/mechanicstype.h>
#include <dtCore/refptr.h>

class palBodyBase;
class palBody;
class palGenericBody;

namespace osg
{
   class Node;
}

namespace dtPhysics
{
   class MaterialActor;
   class MechanicsType;

   class Geometry;
   class BaseBodyWrapperImpl;
   class GenericBodyWrapper;

   class DT_PHYSICS_EXPORT BaseBodyWrapper: public osg::Referenced
   {
   public:
      BaseBodyWrapper(palBodyBase& body);

      /// Creates a generic body.  You must call Init manually after adding geometry.
      static dtCore::RefPtr<GenericBodyWrapper> CreateGenericBody(const TransformType& worldPos,
               MechanicsType& mechType, CollisionGroup collisionGroup, Real mass);

      /// Changes the collision group for the body.
      void SetGroup(CollisionGroup group);

      /// @return the collision group for the body.
      CollisionGroup GetGroup() const;

      void SetMaterial(Material* material);
      Material* GetMaterial();

      palBodyBase& GetPalBodyBase();

      /// @see dtPhysics::PhysicsObject::GetTransform
      void GetTransform(TransformType& xform, bool interpolated) const;

   protected:
      ~BaseBodyWrapper();
   private:
      // can move to other file if need be
      BaseBodyWrapperImpl* mImpl;
   };

   class BodyWrapperImpl;

   class DT_PHYSICS_EXPORT BodyWrapper: public BaseBodyWrapper
   {
   public:
      BodyWrapper(palBody& body);
      //////////////////////////////////////////////////////
      // manipulation functions
      //void SetForce(const VectorType& forceToSet);
      void AddForce(const VectorType& forceToAdd);
      void AddForceAtPosition(const VectorType& position, const VectorType& forceAmount);
      void ApplyImpulse(const VectorType& impulseAmount);
      void ApplyAngularImpulse(const VectorType& angularImpulse);
      void ApplyImpulseAtPosition(const VectorType& position, const VectorType& impulseAmount);
      //         void SetTorque(const VectorType& torqueToSet);
      void AddTorque(const VectorType& torqueToAdd);
      void SetLinearVelocity(const VectorType& velocity);
      void SetAngularVelocity(const VectorType& velocity_rad);
//      VectorType GetForce() const;
//      VectorType GetTorque() const;
      VectorType GetLinearVelocity() const;
      VectorType GetAngularVelocity() const;

      // reset all forces on the physics object
      void ResetForces();

      bool IsActive();
      void SetActive(bool active);

      void SetTransform(const TransformType& worldPos);

      Real GetMass();

      palBody& GetPalBody();
   protected:
      ~BodyWrapper();
   private:
      BodyWrapperImpl* mImpl;
   };

   class GenericBodyWrapperImpl;

   class DT_PHYSICS_EXPORT GenericBodyWrapper: public BodyWrapper
   {
   public:
      GenericBodyWrapper(palGenericBody& genBody);

      /// The generic body needs geometry attached, so it has to be initialized after the static constructor.
      void Init();

      MechanicsType& GetMechanicsType() const;
      void SetMechanicsType(MechanicsType& mechType);

      void SetMass(Real newMass);

      void SetInertia(const VectorType& ixxiyyizz);
      VectorType GetInertia() const;

      void ConnectGeometry(Geometry& toAdd);
      void RemoveGeometry(Geometry& toRemove);

      void SetGravityEnabled(bool enable);
      bool IsGravityEnabled() const;

      /**
       * @param skinWidth the skin width to set.
       * @return true if skin width is supported in the underlying engine
       */
      bool SetSkinWidth(Real skinWidth);

      /**
       * @return the skin width or < 0 if not supported.
       */
      Real GetSkinWidth() const;

      palGenericBody& GetPalGenericBody();
   protected:
      ~GenericBodyWrapper();

   private:
      GenericBodyWrapperImpl* mImpl;
   };


}
#endif /* COLLISIONPRIMITIVE_H_ */

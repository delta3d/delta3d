/* -*-c++-*-
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2007-2008, Alion Science and Technology
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
 * @author Allen Danklefsen
 * @author David Guthrie
 */
#include <prefix/unittestprefix.h>


#include "basedtphysicstestfixture.h"

#include <dtCore/project.h>
#include <dtCore/datatype.h>
#include <dtCore/map.h>
#include <dtCore/gameeventmanager.h>
#include <dtCore/gameevent.h>
#include <dtCore/actortype.h>

#include <dtGame/message.h>
#include <dtGame/basemessages.h>

#include <dtCore/system.h>
#include <dtCore/scene.h>

#include <dtUtil/log.h>
#include <dtUtil/macros.h>
#include <dtUtil/fileutils.h>
#include <dtUtil/mathdefines.h>
#include <dtUtil/datapathutils.h>

#include <dtABC/application.h>

#include <string>
#include <cmath>

#include <dtPhysics/physicscomponent.h>
#include <dtPhysics/physicsactcomp.h>
#include <dtPhysics/physicsinterface.h>
#include <dtPhysics/physicsmaterialactor.h>
#include <dtPhysics/physicsmaterials.h>
#include <dtPhysics/physicsobject.h>
#include <dtPhysics/physicstypes.h>
#include <dtPhysics/raycast.h>
#include <dtPhysics/physicsactorregistry.h>
#include <dtPhysics/bodywrapper.h>
#include <dtPhysics/physicsreaderwriter.h>
#include <dtPhysics/geometry.h>
#include <dtPhysics/palutil.h>

#include <osg/Geode>
#include <osg/Shape>
#include <osg/ShapeDrawable>
#include <osg/ComputeBoundsVisitor>

#include <pal/pal.h>
#include <pal/palFactory.h>
#include <pal/palActivation.h>

#include <osgDB/ReadFile>
namespace dtPhysics
{
   class dtPhysicsTests : public BaseDTPhysicsTestFixture
   {
      CPPUNIT_TEST_SUITE(dtPhysicsTests);
      CPPUNIT_TEST(testPrimitiveType);
      CPPUNIT_TEST(testMaterialActor);
      CPPUNIT_TEST(testAutoCreate);
      CPPUNIT_TEST(testGeometryMarginPerEngine);
      CPPUNIT_TEST(testObjectArray);
      CPPUNIT_TEST(testObjectPropertyNames);
      CPPUNIT_TEST(testObjectPerEngine);
      CPPUNIT_TEST(testObjectConvexPerEngine);
      CPPUNIT_TEST(testObjectCollisionPerEngine);
      CPPUNIT_TEST(testObjectOffsetPerEngine);
      CPPUNIT_TEST(testObjectVisualTransformPerEngine);
      CPPUNIT_TEST(testObjectDynamicsPerEngine);
      CPPUNIT_TEST(testObjectKinematicPerEngine);
      CPPUNIT_TEST(testObjectVelocityAtPointPerEngine);
      CPPUNIT_TEST(testObjectMomentOfInertiaPerEngine);
      CPPUNIT_TEST(testObjectActivationDefaultsPerEngine);
      CPPUNIT_TEST(testObjectActivationInitializeAndChangePerEngine);
      CPPUNIT_TEST(testObjectDampingAccessorsPerEngine);
      CPPUNIT_TEST(testObjectShapeCalc);
      CPPUNIT_TEST(testObjectShapeCalcOriginOffsetPerEngine);
      CPPUNIT_TEST(testObjectDeleteWithConstraintsPerEngine);
      CPPUNIT_TEST(testPhysicsObjectMaterialActorPerEngine);
      CPPUNIT_TEST(testActCompPerEngine);
      CPPUNIT_TEST(testConvexHullCachingPerEngine);
      CPPUNIT_TEST(testComponentPerEngine);
      CPPUNIT_TEST(testCallbacksPerEngine);
      CPPUNIT_TEST(testPhysicsReaderWriter);
      CPPUNIT_TEST_SUITE_END();

   public:
      dtPhysicsTests() {}
      ~dtPhysicsTests() {}

      void GetRequiredLibraries(NameVector& names) override
      {
         static const std::string DTPHYSICS_REGISTRY = "dtPhysics";
         names.push_back(DTPHYSICS_REGISTRY);
      }

      void testPrimitiveType();

      void testGeometryMarginPerEngine();
      void testObjectArray();
      void testObjectOldPropertyNameRemoval();
      void testObjectPropertyNames();
      void testObjectPerEngine();
      void testObjectCollisionPerEngine();
      void testObjectOffsetPerEngine();
      void testObjectVisualTransformPerEngine();
      void testObjectConvexPerEngine();
      void testObjectDynamicsPerEngine();
      void testObjectKinematicPerEngine();
      void testObjectVelocityAtPointPerEngine();
      void testObjectMomentOfInertiaPerEngine();
      void testObjectActivationDefaultsPerEngine();
      void testObjectActivationInitializeAndChangePerEngine();
      void testObjectDampingAccessorsPerEngine();
      void testPhysicsObjectMaterialActorPerEngine();
      void testObjectShapeCalc();
      void testObjectShapeCalcOriginOffsetPerEngine();
      void testObjectDeleteWithConstraintsPerEngine();
      void testActCompPerEngine();
      void testConvexHullCachingPerEngine();
      void testComponentPerEngine();
      void testCallbacksPerEngine();
      void testPhysicsReaderWriter();

      // used so we have a place to test actors
      // not called multiple times like the others.
      void testMaterialActor();
      void testAutoCreate();

   private:
      //Sub Tests
      void testComponent(const std::string& engine);
      void testGeometryMargin(const std::string& engine);
      void testPhysicsObject(const std::string& engine);
      void testPhysicsObjectCollision(const std::string& engine);
      void testPhysicsObjectWithOffset(const std::string& engine);
      void testPhysicsObjectMaterialActor(const std::string& engine);

      void testPhysicsObjectWithMultipleShapes();

      void testPhysicsObjectWithOffsetAndShape(dtPhysics::PrimitiveType& shapeType);
      void testPhysicsObjectVisualTransform(const std::string& engine);
      void testPhysicsObjectConvex(const std::string& engine);
      void testPhysicsObjectDynamics(const std::string& engine);
      void testPhysicsObjectKinematic(const std::string& engine);

      void testPhysicsObjectVelocityAtPoint(const std::string& engine);
      void testPhysicsObjectShapeCalcOriginOffset(const std::string& engine);
      void testPhysicsObjectMomentOfInertia(const std::string& engine);
      void testPhysicsObjectActivationDefaults(const std::string& engine);
      void testPhysicsObjectActivationInitializeAndChange(const std::string& engine);
      void testPhysicsObjectDampingAccessors(const std::string& engine);
      void testPhysicsObjectDeleteWithConstraints(const std::string& engine);

      void testPhysicsActComp(const std::string& engine);
      void testConvexHullCaching(const std::string& engine);
      void testPhysicsWorld(const std::string& engine);
      void testCallbacks(const std::string& engine);
      void testMass(dtPhysics::PhysicsActComp& actorComp);

   };

   CPPUNIT_TEST_SUITE_REGISTRATION(dtPhysicsTests);

   class CallbackTester
   {
      public:
         CallbackTester()
         {
            Reset();
         }

         void Reset()
         {
            mCalledPre = false;
            mCalledPost = false;
            mCalledActionUpdate = false;
         }

         bool HasCalledPre() const { return mCalledPre; }
         bool HasCalledPost() const { return mCalledPost; }
         bool HasCalledAction() const { return mCalledActionUpdate; }

         void UpdateCallbackPre()
         {
            mCalledPre = true;
         }

         void UpdateCallbackPost()
         {
            mCalledPost = true;
         }

         void ActionUpdate(float)
         {
            mCalledActionUpdate = true;
         }

      private:
         bool mCalledPre, mCalledPost, mCalledActionUpdate;
   };

   /////////////////////////////////////////////////////////
   void dtPhysicsTests::testPrimitiveType()
   {
      const std::vector<PrimitiveType*>& typeVec = PrimitiveType::EnumerateType();
      for (size_t i = 0; i < typeVec.size(); ++i)
      {
         PrimitiveType& primType = *typeVec[i];
         if (primType == PrimitiveType::HEIGHTFIELD ||
                  primType == PrimitiveType::TERRAIN_MESH)
         {
            CPPUNIT_ASSERT_MESSAGE("The heightfield and terrain mesh should be flagged as terrain primitive types",
                     primType.IsTerrainType());
         }
         else
         {
            CPPUNIT_ASSERT_MESSAGE("All other types should not be flagged as terrain types",
                     !primType.IsTerrainType());
         }
      }
   }

   /////////////////////////////////////////////////////////
   void dtPhysicsTests::testGeometryMarginPerEngine()
   {
      std::for_each(GetPhysicsEngineList().begin(), GetPhysicsEngineList().end(),
               dtUtil::MakeFunctor(&dtPhysicsTests::testGeometryMargin, this));
   }

   /////////////////////////////////////////////////////////
   // HELPER PRED
   struct TestPropertyNameCollector
   {
      typedef dtCore::RefPtr<dtCore::ActorProperty> PropPtr;
      typedef dtUtil::Functor<void, TYPELIST_1(PropPtr)> VoidPropPtrFunc;

      typedef std::set<dtUtil::RefString> StrSet;
      StrSet mNames;

      void operator() (PropPtr prop)
      {
         mNames.insert(prop->GetName());
      }

      VoidPropPtrFunc GetFunc()
      {
         return dtUtil::MakeFunctor(&TestPropertyNameCollector::operator(), this);
      }
   };

   /////////////////////////////////////////////////////////
   void dtPhysicsTests::testObjectPropertyNames()
   {
      dtCore::PropertyContainer::PropertyVector props;

      std::string objName("testObject");
      std::string objNamePrefix(objName + ": ");
      dtCore::RefPtr<PhysicsObject> po = PhysicsObject::CreateNew(objName);

      TestPropertyNameCollector pred;
      CPPUNIT_ASSERT(pred.mNames.empty());

      // Test that the old property naming functionality can still work.
      dtCore::RefPtr<PhysicsActComp> pac = new PhysicsActComp();
      pac->AddPhysicsObject(*po);
      pac->Init(*PhysicsActComp::TYPE);

      unsigned numProps = po->GetNumProperties();
      CPPUNIT_ASSERT(!po->GetObjectType().DefaultsEmpty());
      // Just pick a number equal to the number when this was coded.
      CPPUNIT_ASSERT(numProps >= 19U);

      po->ForEachProperty(pred.GetFunc());
      po->GetPropertyList(props);

      CPPUNIT_ASSERT( ! props.empty());
      CPPUNIT_ASSERT( ! pred.mNames.empty());
      CPPUNIT_ASSERT(pred.mNames.size() == props.size());

      typedef std::set<dtUtil::RefString> StrSet;
      dtUtil::RefString str;
      int numPropNamesFound = 0;
      StrSet::iterator curIter = pred.mNames.begin();
      StrSet::iterator endIter = pred.mNames.end();
      for (; curIter != endIter; ++curIter)
      {
         str = *curIter;
         CPPUNIT_ASSERT(str->find(objName) == std::string::npos);
         CPPUNIT_ASSERT(str->find(objNamePrefix) == std::string::npos);
         CPPUNIT_ASSERT(po->GetProperty(str) != nullptr);
         ++numPropNamesFound;

         CPPUNIT_ASSERT(pac->GetDeprecatedProperty(objNamePrefix + str) != nullptr);
         CPPUNIT_ASSERT(po->GetObjectType().DefaultExists(str));
      }

      CPPUNIT_ASSERT_EQUAL(numPropNamesFound, (int)props.size());
   }

   /////////////////////////////////////////////////////////
   void dtPhysicsTests::testObjectPerEngine()
   {
      std::for_each(GetPhysicsEngineList().begin(), GetPhysicsEngineList().end(),
               dtUtil::MakeFunctor(&dtPhysicsTests::testPhysicsObject, this));
   }

   /////////////////////////////////////////////////////////
   void dtPhysicsTests::testObjectCollisionPerEngine()
   {
      std::for_each(GetPhysicsEngineList().begin(), GetPhysicsEngineList().end(),
               dtUtil::MakeFunctor(&dtPhysicsTests::testPhysicsObjectCollision, this));
   }

   /////////////////////////////////////////////////////////
   void dtPhysicsTests::testObjectOffsetPerEngine()
   {
      std::for_each(GetPhysicsEngineList().begin(), GetPhysicsEngineList().end(),
               dtUtil::MakeFunctor(&dtPhysicsTests::testPhysicsObjectWithOffset, this));
   }

   /////////////////////////////////////////////////////////
   void dtPhysicsTests::testObjectVisualTransformPerEngine()
   {
      std::for_each(GetPhysicsEngineList().begin(), GetPhysicsEngineList().end(),
               dtUtil::MakeFunctor(&dtPhysicsTests::testPhysicsObjectVisualTransform, this));
   }

   /////////////////////////////////////////////////////////
   void dtPhysicsTests::testObjectConvexPerEngine()
   {
      std::for_each(GetPhysicsEngineList().begin(), GetPhysicsEngineList().end(),
               dtUtil::MakeFunctor(&dtPhysicsTests::testPhysicsObjectConvex, this));
   }

   /////////////////////////////////////////////////////////
   void dtPhysicsTests::testObjectDynamicsPerEngine()
   {
      std::for_each(GetPhysicsEngineList().begin(), GetPhysicsEngineList().end(),
               dtUtil::MakeFunctor(&dtPhysicsTests::testPhysicsObjectDynamics, this));
   }

   /////////////////////////////////////////////////////////
   void dtPhysicsTests::testObjectKinematicPerEngine()
   {
      std::for_each(GetPhysicsEngineList().begin(), GetPhysicsEngineList().end(),
               dtUtil::MakeFunctor(&dtPhysicsTests::testPhysicsObjectKinematic, this));
   }


   /////////////////////////////////////////////////////////
   void dtPhysicsTests::testObjectVelocityAtPointPerEngine()
   {
      std::for_each(GetPhysicsEngineList().begin(), GetPhysicsEngineList().end(),
               dtUtil::MakeFunctor(&dtPhysicsTests::testPhysicsObjectVelocityAtPoint, this));
   }

   /////////////////////////////////////////////////////////
   void dtPhysicsTests::testObjectMomentOfInertiaPerEngine()
   {
      std::for_each(GetPhysicsEngineList().begin(), GetPhysicsEngineList().end(),
               dtUtil::MakeFunctor(&dtPhysicsTests::testPhysicsObjectMomentOfInertia, this));
   }

   /////////////////////////////////////////////////////////
   void dtPhysicsTests::testObjectActivationDefaultsPerEngine()
   {
      std::for_each(GetPhysicsEngineList().begin(), GetPhysicsEngineList().end(),
               dtUtil::MakeFunctor(&dtPhysicsTests::testPhysicsObjectActivationDefaults, this));
   }

   /////////////////////////////////////////////////////////
   void dtPhysicsTests::testObjectDampingAccessorsPerEngine()
   {
      std::for_each(GetPhysicsEngineList().begin(), GetPhysicsEngineList().end(),
               dtUtil::MakeFunctor(&dtPhysicsTests::testPhysicsObjectDampingAccessors, this));
   }

   /////////////////////////////////////////////////////////
   void dtPhysicsTests::testPhysicsObjectMaterialActorPerEngine()
   {
      std::for_each(GetPhysicsEngineList().begin(), GetPhysicsEngineList().end(),
               dtUtil::MakeFunctor(&dtPhysicsTests::testPhysicsObjectMaterialActor, this));
   }

   /////////////////////////////////////////////////////////
   void dtPhysicsTests::testObjectShapeCalc()
   {
      try
      {
         //std::string path = dtCore::Project::GetInstance().GetResourcePath(dtCore::ResourceDescriptor("StaticMeshes:physics_crate.ive"));
         std::string path = "../examples/data/StaticMeshes/physics_crate.ive";
         path = dtUtil::FindFileInPathList(path);
         dtCore::RefPtr<osgDB::ReaderWriter::Options> options = new osgDB::ReaderWriter::Options;

         options->setObjectCacheHint(osgDB::ReaderWriter::Options::CACHE_ALL);

         dtCore::RefPtr<osg::Node> box = osgDB::readNodeFile(path, options.get());

         CPPUNIT_ASSERT(box.valid());

         VectorType center, extents;
         osg::ComputeBoundsVisitor bb;

         const_cast<osg::Node&>(*box).accept(bb);
         PhysicsObject::CalculateOriginAndExtentsForNode(PrimitiveType::BOX, bb.getBoundingBox(), center, extents);

         CPPUNIT_ASSERT(dtUtil::Equivalent(center, VectorType(0.0f, 0.0f, 0.0f), 0.01f));
         CPPUNIT_ASSERT_EQUAL(VectorType(1.0f, 1.0f, 1.0f), extents);
         center.set(0.0f, 0.0f, 0.0f);
         extents.set(0.0f, 0.0f, 0.0f);

         osg::Vec3 offsetCenter(1.7, 3.3, -4.6);
         dtCore::RefPtr<dtCore::Transformable> xformable = new dtCore::Transformable;
         xformable->GetOSGNode()->asGroup()->addChild(box);
         dtCore::Transform xform;
         xform.SetTranslation(offsetCenter);
         xformable->SetTransform(xform);

         bb.reset();
         const_cast<osg::Node&>(*xformable->GetOSGNode()).accept(bb);
         PhysicsObject::CalculateOriginAndExtentsForNode(PrimitiveType::BOX, bb.getBoundingBox(), center, extents);
         CPPUNIT_ASSERT_EQUAL(offsetCenter, center);
         CPPUNIT_ASSERT_EQUAL(VectorType(1.0f, 1.0f, 1.0f), extents);

         bb.reset();
         const_cast<osg::Node&>(*xformable->GetOSGNode()).accept(bb);
         PhysicsObject::CalculateOriginAndExtentsForNode(PrimitiveType::SPHERE, bb.getBoundingBox(), center, extents);
         CPPUNIT_ASSERT_EQUAL(offsetCenter, center);
         CPPUNIT_ASSERT_EQUAL(VectorType(0.5f, 0.0f, 0.0f), extents);

         bb.reset();
         const_cast<osg::Node&>(*xformable->GetOSGNode()).accept(bb);
         PhysicsObject::CalculateOriginAndExtentsForNode(PrimitiveType::CYLINDER, bb.getBoundingBox(), center, extents);
         CPPUNIT_ASSERT_EQUAL(offsetCenter, center);
         CPPUNIT_ASSERT_EQUAL(VectorType(1.0f, 0.5f, 0.0f), extents);

         bb.reset();
         const_cast<osg::Node&>(*xformable->GetOSGNode()).accept(bb);
         PhysicsObject::CalculateOriginAndExtentsForNode(PrimitiveType::CAPSULE, bb.getBoundingBox(), center, extents);
         CPPUNIT_ASSERT_EQUAL(offsetCenter, center);
         CPPUNIT_ASSERT_EQUAL(VectorType(1.0f, 0.5f, 0.0f), extents);

         VectorType zeroVec(0.0f, 0.0f, 0.0f);
         PhysicsObject::CalculateOriginAndExtentsForNode(PrimitiveType::CONVEX_HULL, bb.getBoundingBox(), center, extents);
         CPPUNIT_ASSERT_EQUAL(zeroVec, center);
         CPPUNIT_ASSERT_EQUAL(zeroVec, extents);

         PhysicsObject::CalculateOriginAndExtentsForNode(PrimitiveType::TRIANGLE_MESH, bb.getBoundingBox(), center, extents);
         CPPUNIT_ASSERT_EQUAL(zeroVec, center);
         CPPUNIT_ASSERT_EQUAL(zeroVec, extents);

         PhysicsObject::CalculateOriginAndExtentsForNode(PrimitiveType::TERRAIN_MESH, bb.getBoundingBox(), center, extents);
         CPPUNIT_ASSERT_EQUAL(zeroVec, center);
         CPPUNIT_ASSERT_EQUAL(zeroVec, extents);

      }
      catch (const dtUtil::Exception& ex)
      {
         CPPUNIT_FAIL(ex.ToString());
      }

   }
   /////////////////////////////////////////////////////////
   void dtPhysicsTests::testObjectShapeCalcOriginOffsetPerEngine()
   {
      std::for_each(GetPhysicsEngineList().begin(), GetPhysicsEngineList().end(),
               dtUtil::MakeFunctor(&dtPhysicsTests::testPhysicsObjectShapeCalcOriginOffset, this));
   }

   /////////////////////////////////////////////////////////
   void dtPhysicsTests::testObjectActivationInitializeAndChangePerEngine()
   {
      std::for_each(GetPhysicsEngineList().begin(), GetPhysicsEngineList().end(),
               dtUtil::MakeFunctor(&dtPhysicsTests::testPhysicsObjectActivationInitializeAndChange, this));
   }

   /////////////////////////////////////////////////////////
   void dtPhysicsTests::testObjectDeleteWithConstraintsPerEngine()
   {
      std::for_each(GetPhysicsEngineList().begin(), GetPhysicsEngineList().end(),
               dtUtil::MakeFunctor(&dtPhysicsTests::testPhysicsObjectDeleteWithConstraints, this));
   }


   /////////////////////////////////////////////////////////
   void dtPhysicsTests::testActCompPerEngine()
   {
      std::for_each(GetPhysicsEngineList().begin(), GetPhysicsEngineList().end(),
               dtUtil::MakeFunctor(&dtPhysicsTests::testPhysicsActComp, this));
   }

   /////////////////////////////////////////////////////////
   void dtPhysicsTests::testConvexHullCachingPerEngine()
   {
      std::for_each(GetPhysicsEngineList().begin(), GetPhysicsEngineList().end(),
               dtUtil::MakeFunctor(&dtPhysicsTests::testConvexHullCaching, this));
   }

   /////////////////////////////////////////////////////////
   void dtPhysicsTests::testComponentPerEngine()
   {
      std::for_each(GetPhysicsEngineList().begin(), GetPhysicsEngineList().end(),
               dtUtil::MakeFunctor(&dtPhysicsTests::testComponent, this));
   }

   /////////////////////////////////////////////////////////
   void dtPhysicsTests::testCallbacksPerEngine()
   {
      std::for_each(GetPhysicsEngineList().begin(), GetPhysicsEngineList().end(),
               dtUtil::MakeFunctor(&dtPhysicsTests::testCallbacks, this));
   }

   /////////////////////////////////////////////////////////
   void dtPhysicsTests::testGeometryMargin(const std::string& engine)
   {
      ChangeEngine(engine);
      TransformType xform;
      dtCore::RefPtr<Geometry> geom = Geometry::CreateBoxGeometry(xform, VectorType(3.3, 3.4, 2.6), 12.0f);

      Real margin = geom->GetMargin();
      if (geom->SetMargin(0.033))
      {
         CPPUNIT_ASSERT_DOUBLES_EQUAL(Real(0.033), geom->GetMargin(), Real(0.001));
      }
      else
      {
         CPPUNIT_ASSERT_DOUBLES_EQUAL(Real(-1.0), margin, Real(0.01));
         CPPUNIT_ASSERT_DOUBLES_EQUAL(Real(-1.0), geom->GetMargin(), Real(0.01));
      }
   }

   /////////////////////////////////////////////////////////
   void dtPhysicsTests::testPhysicsObjectConvex(const std::string& engine)
   {
      ChangeEngine(engine);
      // test constructor name
      dtCore::RefPtr<PhysicsObject> physicsObject = PhysicsObject::CreateNew("mypalFriend");
      physicsObject->SetMass(20.0);
      physicsObject->SetPrimitiveType(PrimitiveType::CONVEX_HULL);
      physicsObject->SetMeshResource(dtCore::ResourceDescriptor("StaticMeshes:chairs:sunchair:SunChair_PHYSICS.osg"));
      physicsObject->SetMeshScale(VectorType(20.0f, 20.0f, 20.0f));
      physicsObject->SetOriginOffset(VectorType(0.0f, 0.0f, 0.0f));
      VectorType position(0.0f, 0.0f, 100.0f);
      physicsObject->SetTranslation(position);
      CPPUNIT_ASSERT(physicsObject->Create(nullptr, true));

      CPPUNIT_ASSERT_MESSAGE("The auto compute origin offset should not occur on a mesh.", dtUtil::Equivalent(VectorType(0.0f, 0.0f, 0.0f), VectorType(0.0f, 0.0f, 0.0f), 0.001f));
      dtPhysics::RayCast ray;
      dtPhysics::RayCast::Report report;

      ray.SetOrigin(VectorType(0.0f, 0.0f, 0.0f));
      // The ray is set to hit the chair off the origin by 15 units to make sure the scale by 20 worked.  The 1.5 just makes
      // sure it overshoots to go through the target.
      ray.SetDirection((position + VectorType(0.0f, 15.0f, 0.0f)) * 1.5f);

      std::string shapeMsg("The Convex Hull should have been hit with the ray because the chair is scaled up.");

      shapeMsg +=" on engine "+ PhysicsWorld::GetInstance().GetEngineName();

      PhysicsWorld::GetInstance().TraceRay(ray, report);

      CPPUNIT_ASSERT_MESSAGE(shapeMsg, report.mHasHitObject);

   }

   void dtPhysicsTests::testPhysicsObjectDynamics(const std::string& engine)
   {
      ChangeEngine(engine);
      // test constructor name
      dtCore::RefPtr<PhysicsObject> physicsObject = PhysicsObject::CreateNew("mypalFriend");
      physicsObject->SetMass(20.0);
      physicsObject->SetPrimitiveType(dtPhysics::PrimitiveType::CYLINDER);
      physicsObject->SetExtents(VectorType(3.0, 4.0, 4.0));

      VectorType force(70.0, 7.0, 9.0);
      VectorType pos(1.0, 2.0, 3.0);

      // These just make sure it doesn't crash.  TODO make a test to really test this works
      physicsObject->AddForce(force);
      physicsObject->AddForceAtPosition(pos, force);
      physicsObject->ApplyImpulse(force);
      physicsObject->ApplyImpulseAtPosition(pos, force);
      physicsObject->ApplyAngularImpulse(force);
      physicsObject->AddTorque(force);

      physicsObject->SetLinearVelocity(force);
      CPPUNIT_ASSERT_EQUAL_MESSAGE("If the body has not been created, the linear velocity should always be 0.",
               VectorType(), physicsObject->GetLinearVelocity());

      physicsObject->SetAngularVelocity(force);
      CPPUNIT_ASSERT_EQUAL_MESSAGE("If the body has not been created, the angular velocity should always be 0.",
               VectorType(), physicsObject->GetAngularVelocity());

      physicsObject->Create();

      // These just make sure it doesn't crash.  TODO make a test to really test this works
      physicsObject->AddForce(force);
      physicsObject->AddForceAtPosition(pos, force);
      physicsObject->ApplyImpulse(force);
      physicsObject->ApplyImpulseAtPosition(pos, force);
      physicsObject->ApplyAngularImpulse(force);
      physicsObject->AddTorque(force);

      physicsObject->SetLinearVelocity(force);
      CPPUNIT_ASSERT_EQUAL(force, physicsObject->GetLinearVelocity());

      physicsObject->SetAngularVelocity(force);
      CPPUNIT_ASSERT_EQUAL(force, physicsObject->GetAngularVelocity());
   }

   /////////////////////////////////////////////////////////
   void dtPhysicsTests::testPhysicsObjectCollision(const std::string& engine)
   {
      ChangeEngine(engine);

      dtCore::RefPtr<PhysicsObject> physicsObject[2];
      physicsObject[0] = PhysicsObject::CreateNew("mypalFriend");
      physicsObject[1] = PhysicsObject::CreateNew("mypallypalFriend");

      for (unsigned i = 0; i < 2; ++i)
      {
         physicsObject[i]->SetPrimitiveType(PrimitiveType::SPHERE);
         physicsObject[i]->SetExtents(VectorType(1.0, 1.0, 1.0));
         // Identity;
         TransformType xform;
         physicsObject[i]->SetTransform(xform);

         // Set different collision groups.
         physicsObject[i]->SetCollisionGroup(i);
         physicsObject[i]->SetNotifyCollisions(false);
         physicsObject[i]->Create();
      }

      dtPhysics::PhysicsWorld::GetInstance().UpdateStep(0.01666666f);


      std::vector<CollisionContact> ccVec;
      dtPhysics::PhysicsWorld::GetInstance().GetContacts(*physicsObject[0], *physicsObject[1], ccVec);

      CPPUNIT_ASSERT_MESSAGE(engine, ccVec.empty());

      for (unsigned i = 0; i < 2; ++i)
      {
         TransformType xform;
         physicsObject[i]->SetTransform(xform);
         physicsObject[i]->SetNotifyCollisions(true);
      }

      dtPhysics::PhysicsWorld::GetInstance().UpdateStep(0.01666666f);

      ccVec.clear();
      dtPhysics::PhysicsWorld::GetInstance().GetContacts(*physicsObject[0], *physicsObject[1], ccVec);

      CPPUNIT_ASSERT_MESSAGE(engine, !ccVec.empty());

      for (unsigned i = 0; i < 2; ++i)
      {
         TransformType xform;
         physicsObject[i]->SetTransform(xform);
         physicsObject[i]->SetNotifyCollisions(false);
      }

      dtPhysics::PhysicsWorld::GetInstance().NotifyCollision(*physicsObject[0], *physicsObject[1], true);

      dtPhysics::PhysicsWorld::GetInstance().UpdateStep(0.01666666f);

      ccVec.clear();
      dtPhysics::PhysicsWorld::GetInstance().GetContacts(*physicsObject[0], *physicsObject[1], ccVec);

      CPPUNIT_ASSERT_MESSAGE(engine, !ccVec.empty());

      std::vector<CollisionContact>::const_iterator i, iend;
      i = ccVec.begin();
      iend = ccVec.end();
      for (; i != iend; ++i)
      {
         const CollisionContact& cc = *i;

         CPPUNIT_ASSERT_MESSAGE(engine, cc.mObject1 == physicsObject[0] || cc.mObject1 == physicsObject[1]);
         CPPUNIT_ASSERT_MESSAGE(engine, cc.mObject2 == physicsObject[0] || cc.mObject2 == physicsObject[1]);

         // Use 1.1 for some fuzz room.
         CPPUNIT_ASSERT_MESSAGE(engine, cc.mPosition.length() <= (physicsObject[0]->GetExtents().length()) * 1.1f);
      }

      dtPhysics::PhysicsWorld::GetInstance().ClearContacts();
      ccVec.clear();
      dtPhysics::PhysicsWorld::GetInstance().GetContacts(*physicsObject[0], *physicsObject[1], ccVec);
      CPPUNIT_ASSERT_MESSAGE(engine, ccVec.empty());

   }

   /////////////////////////////////////////////////////////
   void dtPhysicsTests::testPhysicsObjectVisualTransform(const std::string& engine)
   {
      ChangeEngine(engine);

      TransformType xformOffset;
      xformOffset.SetTranslation(3.0, 3.0, 3.0);
      xformOffset.SetRotation(90.0, 0.0, 0.0);

      TransformType xformVisual;
      xformVisual.SetTranslation(90.0, 100.0, 34.0);
      xformVisual.SetRotation(45.0, 45.0, 0.0);

      dtCore::RefPtr<PhysicsObject> physicsObject = PhysicsObject::CreateNew("mypalFriend");
      physicsObject->SetPrimitiveType(PrimitiveType::SPHERE);
      physicsObject->SetExtents(VectorType(1.0, 1.0, 1.0));
      physicsObject->SetVisualToBodyTransform(xformOffset);

      physicsObject->Create();

      physicsObject->SetTransformAsVisual(xformVisual);
      TransformType xformVisualRetrieved;
      TransformType xformBody;
      physicsObject->GetTransformAsVisual(xformVisualRetrieved);
      physicsObject->GetTransform(xformBody);

      dtCore::Transform xformBodyExpected;
      xformBodyExpected.SetRow(0, osg::Vec4(-0.5, 0.499, std::sqrt(2.0)/2.0, 0.0));
      xformBodyExpected.SetRow(1, osg::Vec4(-std::sqrt(2.0)/2.0, -std::sqrt(2.0)/2.0, -0.0, 0.0));
      xformBodyExpected.SetRow(2, osg::Vec4(0.5, -0.499, std::sqrt(2.0)/2.0, 0.0));
      xformBodyExpected.SetRow(3, osg::Vec4(92.121, 102.121, 38.242, 1.0));

      CPPUNIT_ASSERT(xformVisualRetrieved.EpsilonEquals(xformVisual, 0.01));
      CPPUNIT_ASSERT(xformBody.EpsilonEquals(xformBodyExpected, 0.01));
      //CPPUNIT_ASSERT()
   }

   /////////////////////////////////////////////////////////
   void dtPhysicsTests::testPhysicsObject(const std::string& engine)
   {
      ChangeEngine(engine);

      // test constructor name
      dtCore::RefPtr<PhysicsObject> physicsObject = PhysicsObject::CreateNew("mypalFriend");
      CPPUNIT_ASSERT_MESSAGE("name: should be mypalFriend", physicsObject->GetName() == "mypalFriend");

      // test name of object
      physicsObject->SetName("myPalFriend2");
      CPPUNIT_ASSERT_MESSAGE("name: should be myPalFriend2", physicsObject->GetName() == "myPalFriend2");

      // test transform
   /*   TransformType transformz;
      transformz.SetScale(osg::Vec3(2,2,2));
      physicsObject->SetTransform(transformz);
      CPPUNIT_ASSERT_MESSAGE("transformz object doesnt match up", physicsObject->GetTransform().GetScale() == transformz.GetScale());
   */
      // test physics actorComp
      dtCore::RefPtr<PhysicsActComp> actorComp = new PhysicsActComp();
      actorComp->SetName("booga");
      CPPUNIT_ASSERT_MESSAGE("UserData should default to nullptr on a physics object", physicsObject->GetUserData() == nullptr);
      physicsObject->SetUserData(actorComp.get());
      CPPUNIT_ASSERT_MESSAGE("Tried setting a actorComp as the user data on the object", physicsObject->GetUserData() == actorComp.get());
      physicsObject->SetUserData(nullptr);
      actorComp->AddPhysicsObject(*physicsObject);
      CPPUNIT_ASSERT_MESSAGE("The user data should be the actorComp after adding it as a child of the actorComp",
               physicsObject->GetUserData() == actorComp.get());
      actorComp->RemovePhysicsObject(*physicsObject);
      CPPUNIT_ASSERT_MESSAGE("UserData should be nullptr on a physics object after removing it from a actorComp by passing in the object",
               physicsObject->GetUserData() == nullptr);

      // we already  know this call works by the time we get here, no need to check again.
      actorComp->AddPhysicsObject(*physicsObject);
      actorComp->RemovePhysicsObject(physicsObject->GetName());
      CPPUNIT_ASSERT_MESSAGE("UserData should be nullptr on a physics object after removing it from a actorComp by name",
               physicsObject->GetUserData() == nullptr);

      dtCore::RefPtr<dtPhysics::MaterialActor> mat;
      mGM->CreateActor(*dtPhysics::PhysicsActorRegistry::PHYSICS_MATERIAL_ACTOR_TYPE, mat);
      mat->SetName("Steel");
      CPPUNIT_ASSERT(mat != nullptr);

      PhysicsMaterials& materials = PhysicsWorld::GetInstance().GetMaterials();
      Material* uniqueMaterial = materials.NewMaterial(mat->GetName(), mat->GetMaterialDef());
      CPPUNIT_ASSERT(uniqueMaterial == materials.GetMaterial(mat->GetName()));

      // Pre set this to make sure it gets set propertly on create from properties.
      physicsObject->SetCollisionGroup(5);

      CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("Mass should default to 1!", 1.0f, physicsObject->GetMass(), 1e-3f);
      physicsObject->SetMass(12.0f);

      physicsObject->SetOriginOffset(VectorType(4.0f, 4.5f, 4.25f));

      CPPUNIT_ASSERT_EQUAL_MESSAGE("Extents should all default to -1", VectorType(-1.0f, -1.0f, -1.0f), physicsObject->GetExtents());
      physicsObject->SetPrimitiveType(PrimitiveType::SPHERE);
      physicsObject->SetExtents(osg::Vec3(1.0f, 0.0f, 0.0f));

      CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("Skin thickness should default to 0.02!", 0.02f, physicsObject->GetSkinThickness(), 1e-3f);
      physicsObject->SetSkinThickness(1.2f);
      CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("Skin thickness should have changed!", 1.2f, physicsObject->GetSkinThickness(), 1e-3f);

      CPPUNIT_ASSERT(physicsObject->GetCollisionResponseEnabled());
      physicsObject->SetCollisionResponseEnabled(false);
      CPPUNIT_ASSERT(!physicsObject->GetCollisionResponseEnabled());
      ///////// CREATE //////////////
      CPPUNIT_ASSERT_MESSAGE("Should be able to call CreateFromPrimitive on a physics object",
               physicsObject->Create());

      CPPUNIT_ASSERT(!physicsObject->GetCollisionResponseEnabled());

      CPPUNIT_ASSERT_MESSAGE("The Material should default to, of all things, the default one.",
               physicsObject->GetMaterial() == materials.GetMaterial(PhysicsMaterials::DEFAULT_MATERIAL_NAME));

      physicsObject->SetMaterial(uniqueMaterial);
      CPPUNIT_ASSERT(physicsObject->GetMaterial() == uniqueMaterial);

      
      // Test assignment of a material by index.
      Material* newMat = materials.NewMaterial("AnotherMaterial", mat->GetMaterialDef());

      MaterialIndex newMatIndex = newMat->GetId();

      CPPUNIT_ASSERT(physicsObject->GetMaterial() != newMat);
      CPPUNIT_ASSERT(physicsObject->SetMaterialByIndex(newMatIndex));
      CPPUNIT_ASSERT(physicsObject->GetMaterial() == newMat);

      // Ensure assigning a bad index does not change the current material assignment.
      CPPUNIT_ASSERT( ! physicsObject->SetMaterialByIndex(12345));
      CPPUNIT_ASSERT(physicsObject->GetMaterial() == newMat);


      BaseBodyWrapper* baseBodyWrapper = physicsObject->GetBodyWrapper();

      CPPUNIT_ASSERT(baseBodyWrapper != nullptr);

      // test mass
      CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("Mass should have been created as 12!", 12.0f, physicsObject->GetMass(), 1e-3f);
      physicsObject->SetMass(10.0f);
      CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("Mass should have changed.", 10.0f, physicsObject->GetMass(), 1e-3f);

      if (engine != dtPhysics::PhysicsWorld::ODE_ENGINE)
      {
         CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("Skin thickness should be set right after create.", 1.2f, physicsObject->GetSkinThickness(), 1e-3f);
      }
      else
      {
          CPPUNIT_ASSERT_MESSAGE("Skin thickness is not supported by ode, so it should return less than 0.  "
                      "If this has changed, this test in invalid",
                      physicsObject->GetSkinThickness() < 0.0f);
      }

      // test extents
      VectorType extents(1.0f,2.0f,3.0f);
      physicsObject->SetExtents(extents);
      CPPUNIT_ASSERT_EQUAL_MESSAGE("Extents should equal what was assigned", extents, physicsObject->GetExtents());

      // test origin offset
      CPPUNIT_ASSERT_MESSAGE("Origin offsets don't match up on create", physicsObject->GetOriginOffset() == VectorType(4.0f,4.5f,4.25f));
      physicsObject->SetOriginOffset(VectorType(3.0f,3.0f,3.0f));
      CPPUNIT_ASSERT_MESSAGE("Origin offsets don't match up", physicsObject->GetOriginOffset() == VectorType(3.0f, 3.0f, 3.0f));

      // test mechanics enum
      physicsObject->SetMechanicsType(MechanicsType::KINEMATIC);
      CPPUNIT_ASSERT_MESSAGE("Mechanics enum doesn't match up", physicsObject->GetMechanicsType() == MechanicsType::KINEMATIC);

      physicsObject->SetMechanicsType(MechanicsType::DYNAMIC);
      CPPUNIT_ASSERT_MESSAGE("Mechanics enum doesn't match up", physicsObject->GetMechanicsType() == MechanicsType::DYNAMIC);

      // test primitive type
      physicsObject->SetPrimitiveType(PrimitiveType::CYLINDER);
      CPPUNIT_ASSERT_MESSAGE("Primitive type doesn't match up", physicsObject->GetPrimitiveType() == PrimitiveType::CYLINDER);

      CPPUNIT_ASSERT_EQUAL_MESSAGE("Collision group doesn't match up on create", CollisionGroup(5), physicsObject->GetCollisionGroup());
      CPPUNIT_ASSERT_EQUAL_MESSAGE("Collision group doesn't match up on create", CollisionGroup(5), baseBodyWrapper->GetGroup());
      // test collision group
      physicsObject->SetCollisionGroup(6);
      CPPUNIT_ASSERT_EQUAL_MESSAGE("Collision group doesn't match up", CollisionGroup(6), physicsObject->GetCollisionGroup());
      CPPUNIT_ASSERT_EQUAL_MESSAGE("Collision group doesn't match up", CollisionGroup(6), baseBodyWrapper->GetGroup());

      // test active
      physicsObject->SetActive(true);

      CPPUNIT_ASSERT_MESSAGE("Set active / is active flag doesn't work", physicsObject->IsActive());

      // physx doesn't seem to report it's asleep when you tell it to be asleep.
      if (engine != dtPhysics::PhysicsWorld::PHYSX_ENGINE) {
         // test active
         physicsObject->SetActive(false);

         CPPUNIT_ASSERT_MESSAGE("Set active / is active flag doesn't work", !physicsObject->IsActive());
      }

      CPPUNIT_ASSERT_MESSAGE("The physics object should be set to not get collisions notifications by default.",
               !physicsObject->GetNotifyCollisions());

      physicsObject->SetNotifyCollisions(true);
      CPPUNIT_ASSERT_MESSAGE("The physics object should be set to get collisions notifications by default.",
               physicsObject->GetNotifyCollisions());


      for (unsigned i = 0; i < 20; ++i)
      {
         CPPUNIT_ASSERT_MESSAGE("Should be able to call Create multiple times to regenerate the object",
                  physicsObject->Create());
         CPPUNIT_ASSERT_EQUAL(1U, physicsObject->GetNumGeometries());
      }

      // TODO: Test create from primitive, and create from properties functionality
      CPPUNIT_ASSERT(physicsObject->GetGenericBodyWrapper() != nullptr);

      CPPUNIT_ASSERT(physicsObject->IsGravityEnabled());
      CPPUNIT_ASSERT(physicsObject->GetGenericBodyWrapper()->IsGravityEnabled());

      physicsObject->SetGravityEnabled(false);

      CPPUNIT_ASSERT(!physicsObject->IsGravityEnabled());
      CPPUNIT_ASSERT(!physicsObject->GetGenericBodyWrapper()->IsGravityEnabled());

      physicsObject->SetCollisionResponseEnabled(true);
      CPPUNIT_ASSERT(physicsObject->IsCollisionResponseEnabled());
      CPPUNIT_ASSERT(physicsObject->GetGenericBodyWrapper()->GetPalGenericBody().IsCollisionResponseEnabled());

      physicsObject->SetCollisionResponseEnabled(false);

      CPPUNIT_ASSERT(!physicsObject->IsCollisionResponseEnabled());
      CPPUNIT_ASSERT(!physicsObject->GetGenericBodyWrapper()->GetPalGenericBody().IsCollisionResponseEnabled());
   }

   void dtPhysicsTests::testPhysicsObjectWithOffset(const std::string& engine)
   {
      ChangeEngine(engine);
      try
      {
         testPhysicsObjectWithMultipleShapes();

         testPhysicsObjectWithOffsetAndShape(PrimitiveType::BOX);
         testPhysicsObjectWithOffsetAndShape(PrimitiveType::SPHERE);
         testPhysicsObjectWithOffsetAndShape(PrimitiveType::CYLINDER);
         testPhysicsObjectWithOffsetAndShape(PrimitiveType::CAPSULE);
         testPhysicsObjectWithOffsetAndShape(PrimitiveType::CONVEX_HULL);
         testPhysicsObjectWithOffsetAndShape(PrimitiveType::TRIANGLE_MESH);
      }
      catch(dtUtil::Exception& ex)
      {
         CPPUNIT_FAIL(ex.ToString());
      }
   }

   void dtPhysicsTests::testPhysicsObjectKinematic(const std::string& engine)
   {
      ChangeEngine(engine);
      dtCore::RefPtr<PhysicsObject> po = PhysicsObject::CreateNew("jojo");

      po->SetOriginOffset(VectorType(40.0,40.0,40.0));

      po->SetMechanicsType(dtPhysics::MechanicsType::KINEMATIC);
      po->SetPrimitiveType(dtPhysics::PrimitiveType::CONVEX_HULL);
      po->SetExtents(VectorType(3.0f, 3.0f, 3.0f));
      dtCore::Transform xform;
      Real yPos = 10.f;
      xform.SetTranslation(0.0f, yPos, 0.0f);
      xform.SetRotation(90.0, 0.0, 0.0);
      po->SetTransform(xform);

      dtCore::RefPtr<osg::Geode> geode = new osg::Geode();
      dtCore::RefPtr<osg::Sphere> sphere = new osg::Sphere(osg::Vec3(0.0f, 0.0f, 0.0f), 3.0f);
      dtCore::RefPtr<osg::ShapeDrawable> shapeDraw = new osg::ShapeDrawable(sphere.get());
      geode->addDrawable(shapeDraw.get());
      po->Create(geode);

      dtCore::Transform xformActual;

      for(unsigned i = 0; i < 3; ++i)
      {
         po->GetTransform(xformActual);

         CPPUNIT_ASSERT_DOUBLES_EQUAL(yPos, xformActual.GetTranslation().y(), 0.01f);

         yPos += 2.0f;
         dtPhysics::PhysicsWorld::GetInstance().UpdateStep(0.016666f);

         xform.SetTranslation(0.0f, yPos, 0.0f);
         po->SetTransform(xform);
      }

   }

   void dtPhysicsTests::testPhysicsObjectMaterialActor(const std::string& engine)
   {
      mGM->DeleteAllActors(true);
      ChangeEngine(engine);

      dtCore::RefPtr<PhysicsObject> po = PhysicsObject::CreateNew("jojo");
      dtCore::RefPtr<dtPhysics::MaterialActor> mat;
      mGM->CreateActor(*dtPhysics::PhysicsActorRegistry::PHYSICS_MATERIAL_ACTOR_TYPE, mat);
      if (mGM->GetComponentByName(dtPhysics::PhysicsComponent::DEFAULT_NAME) == nullptr)
      {
         mGM->AddComponent(*new dtPhysics::PhysicsComponent(dtPhysics::PhysicsWorld::GetInstance(), false));
      }

      // the po has to be added to an actor, so the material actor is as good as any.
      mat->AddComponent(*new dtPhysics::PhysicsActComp);
      mat->GetComponent<dtPhysics::PhysicsActComp>()->AddPhysicsObject(*po, true);

      mat->SetName("boola");
      mat->GetMaterialDef().SetRestitution(0.125);
      mat->GetMaterialDef().SetStaticFriction(0.25);
      mGM->AddActor(*mat);

      dtCore::RefPtr<dtPhysics::MaterialActor> matAutre;
      mGM->CreateActor(*dtPhysics::PhysicsActorRegistry::PHYSICS_MATERIAL_ACTOR_TYPE, matAutre);
      matAutre->SetName("niais");
      matAutre->GetMaterialDef().SetRestitution(0.343);
      matAutre->GetMaterialDef().SetStaticFriction(0.242);
      mGM->AddActor(*matAutre);

      PhysicsMaterials& materials = dtPhysics::PhysicsWorld::GetInstance().GetMaterials();

      po->SetMaterialId(mat->GetId());
      CPPUNIT_ASSERT(po->GetMaterial() == nullptr);
      po->Create();
      CPPUNIT_ASSERT(po->GetMaterial() != nullptr);
      CPPUNIT_ASSERT_EQUAL(po->GetMaterial()->GetName(), std::string("boola"));
      po->SetMaterialId(dtCore::UniqueId(false));
      CPPUNIT_ASSERT(po->GetMaterial() != nullptr);
      CPPUNIT_ASSERT(po->GetMaterial() == materials.GetMaterial(PhysicsMaterials::DEFAULT_MATERIAL_NAME));

      po->SetMaterialId(matAutre->GetId());
      CPPUNIT_ASSERT(po->GetMaterial() != nullptr);
      CPPUNIT_ASSERT_EQUAL(po->GetMaterial()->GetName(), std::string("niais"));

      // Delete the material actor so we can see if it tries to look up the actor again.
      mGM->DeleteActor(*matAutre);
      dtCore::System::GetInstance().Step(0.01666f);

      // Setting the ID again after setting the first time should not trigger a re-lookup of the actor.
      po->SetMaterialId(matAutre->GetId());
      CPPUNIT_ASSERT(po->GetMaterial() != nullptr);
      CPPUNIT_ASSERT_EQUAL(po->GetMaterial()->GetName(), std::string("niais"));

      // set to the first Id to make it switch.
      po->SetMaterialId(mat->GetId());
      // Setting the material to deleted one should result in the object having the default material.
      po->SetMaterialId(matAutre->GetId());
      CPPUNIT_ASSERT(po->GetMaterial() != nullptr);
      CPPUNIT_ASSERT(po->GetMaterial() == materials.GetMaterial(PhysicsMaterials::DEFAULT_MATERIAL_NAME));
   }

   void dtPhysicsTests::testPhysicsObjectWithMultipleShapes()
   {
      dtCore::RefPtr<PhysicsObject> po = PhysicsObject::CreateNew("jojo");

      po->SetOriginOffset(VectorType(40.0,40.0,40.0));

      VectorType extents(3.0f, 3.0f, 3.0f);
      dtCore::Transform xform;
      VectorType pos(0.0f, 10.0f, 0.0f);
      xform.SetTranslation(pos);
      po->SetTransform(xform);
      po->SetMass(50.0f);
      po->SetMechanicsType(dtPhysics::MechanicsType::KINEMATIC);
      std::vector<dtPhysics::GeometryPtr> geoms;

      geoms.push_back(dtPhysics::Geometry::CreateBoxGeometry(xform, extents, 50.0f/4.0f));

      pos.y() += 10.0f;
      xform.SetTranslation(pos);
      geoms.push_back(dtPhysics::Geometry::CreateSphereGeometry(xform, extents.x(), 50.0f/4.0f));

      pos.y() += 10.0f;
      xform.SetTranslation(pos);
      geoms.push_back(dtPhysics::Geometry::CreateCylinderGeometry(xform, extents.x(), extents.y(), 50.0f/4.0f));

      pos.y() += 10.0f;
      xform.SetTranslation(pos);
      geoms.push_back(dtPhysics::Geometry::CreateCapsuleGeometry(xform, extents.x(), extents.y(), 50.0f/4.0f));

      po->CreateFromGeometry(geoms);

      dtPhysics::PhysicsWorld::GetInstance().UpdateStep(0.0166667f);
      dtPhysics::PhysicsWorld::GetInstance().UpdateStep(0.0166667f);

      dtPhysics::RayCast ray;
      dtPhysics::RayCast::Report report;

      ray.SetOrigin(VectorType(0.0f, 0.0f, 50.0f));
      //Shoot at the additional objects first
      ray.SetDirection(VectorType(0.0, 10.0, -50.0));

         PhysicsWorld::GetInstance().TraceRay(ray, report);
      CPPUNIT_ASSERT_MESSAGE("The box geometry should have been hit by the ray"
                , report.mHasHitObject);

      // aim high to test the height and orientation of the capsule .
      ray.SetDirection(VectorType(0.0, 20.0, -50.0));
      PhysicsWorld::GetInstance().TraceRay(ray, report);
      CPPUNIT_ASSERT_MESSAGE("The sphere geometry should have been hit by the ray"
                , report.mHasHitObject);

      // aim high to test the size of the box.
      ray.SetDirection(VectorType(0.0, 30.0, -50.0));
      PhysicsWorld::GetInstance().TraceRay(ray, report);
      CPPUNIT_ASSERT_MESSAGE("The capsule geometry should have been hit by the ray"
                , report.mHasHitObject);

      ray.SetDirection(VectorType(0.0, 40.0, -50.0));
      PhysicsWorld::GetInstance().TraceRay(ray, report);
      CPPUNIT_ASSERT_MESSAGE("The cylinder geometry should have been hit by the ray"
                , report.mHasHitObject);

   }

   void dtPhysicsTests::testPhysicsObjectWithOffsetAndShape(dtPhysics::PrimitiveType& shapeType)
   {
      dtCore::RefPtr<PhysicsObject> po = PhysicsObject::CreateNew("jojo");

      po->SetOriginOffset(VectorType(40.0,40.0,40.0));

      po->SetPrimitiveType(shapeType);
      po->SetExtents(VectorType(3.0f, 3.0f, 3.0f));
      dtCore::Transform xform;
      xform.SetTranslation(0.0f, 10.0f, 0.0f);
      xform.SetRotation(90.0, 0.0, 0.0);
      po->SetTransform(xform);
      po->SetMechanicsType(dtPhysics::MechanicsType::KINEMATIC);
      if (shapeType == PrimitiveType::CONVEX_HULL || shapeType == PrimitiveType::TRIANGLE_MESH)
      {
         dtCore::RefPtr<osg::Geode> geode = new osg::Geode();
         dtCore::RefPtr<osg::Sphere> sphere = new osg::Sphere(osg::Vec3(0.0f, 0.0f, 0.0f), 3.0f);
         dtCore::RefPtr<osg::ShapeDrawable> shapeDraw = new osg::ShapeDrawable(sphere.get());
         geode->addDrawable(shapeDraw.get());
         po->Create(geode);
      }
      else
      {
         po->Create();
      }

      CPPUNIT_ASSERT_EQUAL(1U, po->GetNumGeometries());

      xform.MakeIdentity();
      dtPhysics::VectorType spherePosWorld(-25.0f, 50.0f, 33.0f);
      xform.SetTranslation(spherePosWorld);
      dtCore::RefPtr<dtPhysics::Geometry> sphereGeom = dtPhysics::Geometry::CreateSphereGeometry(xform, 4.0f, 1.0f);
      po->AddGeometry(*sphereGeom);

      CPPUNIT_ASSERT_EQUAL(2U, po->GetNumGeometries());

      dtPhysics::VectorType capPosWorld(-35.0f, 45.0f, 33.0f);
      xform.SetTranslation(capPosWorld);
      dtCore::RefPtr<dtPhysics::Geometry> capGeom = dtPhysics::Geometry::CreateCapsuleGeometry(xform, 4.0f, 1.0f, 1.0f);
      po->AddGeometry(*capGeom);

      CPPUNIT_ASSERT_EQUAL(3U, po->GetNumGeometries());

      dtPhysics::VectorType boxPosWorld(-45.0f, 40.0f, 33.0f);
      xform.SetTranslation(boxPosWorld);
      dtCore::RefPtr<dtPhysics::Geometry> boxGeom = dtPhysics::Geometry::CreateBoxGeometry(xform, osg::Vec3(1.5f, 1.5f, 4.5f), 1.0f);
      po->AddGeometry(*boxGeom);

      CPPUNIT_ASSERT_EQUAL(4U, po->GetNumGeometries());

      dtPhysics::PhysicsWorld::GetInstance().UpdateStep(0.0166667f);
      dtPhysics::PhysicsWorld::GetInstance().UpdateStep(0.0166667f);

      dtPhysics::RayCast ray;
      dtPhysics::RayCast::Report report;

      ray.SetOrigin(VectorType(0.0f, 0.0f, 0.0f));
      //Shoot at the additional objects first
      ray.SetDirection(spherePosWorld * 1.5f);

      std::string shapeMsg(" should have been hit when using a first shape");

      shapeMsg += shapeType.GetName() + " on engine "+ PhysicsWorld::GetInstance().GetEngineName();

         PhysicsWorld::GetInstance().TraceRay(ray, report);
      CPPUNIT_ASSERT_MESSAGE(std::string("The additional geometry sphere")  + shapeMsg
                , report.mHasHitObject);

      // aim high to test the height and orientation of the capsule .
      ray.SetDirection(VectorType(capPosWorld.x(), capPosWorld.y(), capPosWorld.z() + 2.5f) * 1.5f);
      PhysicsWorld::GetInstance().TraceRay(ray, report);
      CPPUNIT_ASSERT_MESSAGE(std::string("The additional geometry capsule")  + shapeMsg
                , report.mHasHitObject);

      // aim high to test the size of the box.
      ray.SetDirection(VectorType(boxPosWorld.x(), boxPosWorld.y(), boxPosWorld.z() + 1.3f) * 1.5f);
      PhysicsWorld::GetInstance().TraceRay(ray, report);
      CPPUNIT_ASSERT_MESSAGE(std::string("The additional geometry box")  + shapeMsg
                , report.mHasHitObject);

      ray.SetDirection(VectorType(-40.0, 50.0, 40.0) * 1.5f);
      PhysicsWorld::GetInstance().TraceRay(ray, report);
      if (!report.mHasHitObject)
      {
         ray.SetDirection(VectorType(0.0, 10.0, 0.0));
         dtPhysics::RayCast::Report report;
         PhysicsWorld::GetInstance().TraceRay(ray, report);

         if (report.mHasHitObject)
         {
            CPPUNIT_FAIL("geometry offset didn't work.  The geometry is at the origin of the body");
         }
         else
         {
            CPPUNIT_FAIL(std::string("geometry offset didn't work with ") + shapeType.GetName() + " on engine "
                     + PhysicsWorld::GetInstance().GetEngineName()
                     + " raycast couldn't find the geometry at the origin or at the offset position.");
         }
      }
   }


   ////////////////////////////////////////////////////////////////////////////////
   void dtPhysicsTests::testPhysicsObjectShapeCalcOriginOffset(const std::string& engine)
   {
      ChangeEngine(engine);
      try
      {
         //std::string path = dtCore::Project::GetInstance().GetResourcePath(dtCore::ResourceDescriptor("StaticMeshes:physics_crate.ive"));
         std::string path = "../examples/data/StaticMeshes/physics_crate.ive";
         path = dtUtil::FindFileInPathList(path);
         dtCore::RefPtr<osgDB::ReaderWriter::Options> options = new osgDB::ReaderWriter::Options;

         options->setObjectCacheHint(osgDB::ReaderWriter::Options::CACHE_ALL);

         dtCore::RefPtr<osg::Node> box = osgDB::readNodeFile(path, options.get());

         CPPUNIT_ASSERT(box.valid());

         osg::Vec3 offsetCenter(1.7, 3.3, -4.6);
         dtCore::RefPtr<dtCore::Transformable> xformable = new dtCore::Transformable;
         xformable->GetOSGNode()->asGroup()->addChild(box);
         dtCore::Transform xform;
         xform.SetTranslation(offsetCenter);
         xformable->SetTransform(xform);

         dtCore::RefPtr<PhysicsObject> po = PhysicsObject::CreateNew("Brad");
         VectorType originOffset(3.7f, 4.1f, 3.3f);
         po->SetOriginOffset(originOffset);
         po->SetPrimitiveType(PrimitiveType::BOX);

         po->Create(xformable->GetOSGNode(), true);

         CPPUNIT_ASSERT_EQUAL_MESSAGE("The extents should have been calculated from the unit box",
                  /*dtUtil::Equivalent(*/VectorType(1.0f, 1.0f, 1.0f), po->GetExtents());//, 0.01f));
         CPPUNIT_ASSERT_MESSAGE("the origin offset should equal the origin offset plus the center offset of the box.",
                  dtUtil::Equivalent(originOffset + offsetCenter, po->GetOriginOffset(), 0.01f));
      }
      catch (const dtUtil::Exception& ex)
      {
         CPPUNIT_FAIL(ex.ToString());
      }

   }


   ////////////////////////////////////////////////////////////////////////////////
   void dtPhysicsTests::testPhysicsObjectVelocityAtPoint(const std::string& engine)
   {
      ChangeEngine(engine);
      dtCore::RefPtr<PhysicsObject> po = PhysicsObject::CreateNew("jojo");
      po->SetPrimitiveType(PrimitiveType::SPHERE);
      po->SetExtents(VectorType(1.0f, 0.0f, 0.0f));
      po->SetMass(30.0f);
      po->Create();

      po->SetAngularVelocity(VectorType(0.0f, 20.0f, 0.0f));
      po->SetLinearVelocity(VectorType(30.0f, 25.0f, 19.7f));
      VectorType vec = po->GetLinearVelocityAtLocalPoint(VectorType(0.0, 0.0, 2.0f));
      VectorType expected(70.0, 25.0f, 19.7f);
      CPPUNIT_ASSERT_DOUBLES_EQUAL(expected[0], vec[0], FLT_EPSILON * 100.0f);
      CPPUNIT_ASSERT_DOUBLES_EQUAL(expected[1], vec[1], FLT_EPSILON * 100.0f);
      CPPUNIT_ASSERT_DOUBLES_EQUAL(expected[2], vec[2], FLT_EPSILON * 100.0f);
   }

   void dtPhysicsTests::testPhysicsObjectMomentOfInertia(const std::string& engine)
   {
      ChangeEngine(engine);
      dtCore::RefPtr<PhysicsObject> po = PhysicsObject::CreateNew("jojo");
      po->SetPrimitiveType(PrimitiveType::SPHERE);
      po->SetExtents(VectorType(1.0f, 0.0f, 0.0f));
      po->SetMass(45.0f);

      VectorType tensorPreInit = po->GetMomentOfInertia();
      CPPUNIT_ASSERT(tensorPreInit.x() < 0.0f && tensorPreInit.y() < 0.0f && tensorPreInit.z() < 0.0f);
      po->SetMassAndScaleMomentOfInertia(64.0f);

      tensorPreInit = po->GetMomentOfInertia();
      CPPUNIT_ASSERT(tensorPreInit.x() < 0.0f && tensorPreInit.y() < 0.0f && tensorPreInit.z() < 0.0f);

      po->SetMomentOfInertia(VectorType(2.0f, 4.0f, 6.0f));
      po->SetMassAndScaleMomentOfInertia(32.0f);
      tensorPreInit = po->GetMomentOfInertia();

      CPPUNIT_ASSERT_DOUBLES_EQUAL(tensorPreInit.x(), 1.0f, 0.01);
      CPPUNIT_ASSERT_DOUBLES_EQUAL(tensorPreInit.y(), 2.0f, 0.01);
      CPPUNIT_ASSERT_DOUBLES_EQUAL(tensorPreInit.z(), 3.0f, 0.01);

      po->Create();

      VectorType tensor = po->GetMomentOfInertia();
      po->SetMomentOfInertia(VectorType(0.0f, 0.0f, 0.0f));
      VectorType tensor2 = po->GetMomentOfInertia();
      CPPUNIT_ASSERT_EQUAL_MESSAGE("The moment of inertia should not have changed.", tensor, tensor2);
      VectorType newInertia(3.0f, 200.0f, 5000.0f);
      po->SetMomentOfInertia(newInertia);
      VectorType tensor3 = po->GetMomentOfInertia();
      CPPUNIT_ASSERT_EQUAL_MESSAGE("The moment of inertia should have changed to the new value.", newInertia, tensor3);
      po->SetMassAndScaleMomentOfInertia(16.0f);
      newInertia /= 2.0f;
      tensor3 = po->GetMomentOfInertia();
      CPPUNIT_ASSERT_EQUAL_MESSAGE("The moment of inertia should have scaled by half.", newInertia, tensor3);
   }

   void dtPhysicsTests::testPhysicsObjectActivationDefaults(const std::string& engine)
   {
      ChangeEngine(engine);
      dtCore::RefPtr<PhysicsObject> po = PhysicsObject::CreateNew("jojo");
      po->SetPrimitiveType(PrimitiveType::SPHERE);
      po->SetExtents(VectorType(1.0f, 0.0f, 0.0f));
      po->SetMass(30.0f);

      CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("All activation setting should default to -1, engine default",
               po->GetActivationLinearVelocityThreshold(), Real(-1.0), Real(0.1));
      CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("All activation setting should default to -1, engine default",
               po->GetActivationAngularVelocityThreshold(), Real(-1.0), Real(0.1));
      CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("All activation setting should default to -1, engine default",
               po->GetActivationTimeThreshold(), Real(-1.0), Real(0.1));

      po->Create();

      palActivationSettings* activationImpl = dynamic_cast<palActivationSettings*>(&po->GetBodyWrapper()->GetPalBodyBase());
      // No activation support, so return
      if (activationImpl == nullptr)
      {
         CPPUNIT_ASSERT_MESSAGE("ODE and Bullet plugins are known to have activation interface support, but the dynamic cast failed",
                  engine != PhysicsWorld::BULLET_ENGINE && engine != PhysicsWorld::ODE_ENGINE);
         return;
      }

      if (activationImpl->GetSupportedActivationSettings()[palActivationSettings::LINEAR_VELOCITY_THRESHOLD])
      {
         CPPUNIT_ASSERT_MESSAGE("Activation setting should change to real values after init",
                  po->GetActivationLinearVelocityThreshold() >= Real(0.0));
      }

      if (activationImpl->GetSupportedActivationSettings()[palActivationSettings::ANGULAR_VELOCITY_THRESHOLD])
      {
         CPPUNIT_ASSERT_MESSAGE("Activation setting should change to real values after init",
                  po->GetActivationAngularVelocityThreshold() >= Real(0.0));
      }

      if (activationImpl->GetSupportedActivationSettings()[palActivationSettings::TIME_THRESHOLD])
      {
         CPPUNIT_ASSERT_MESSAGE("Activation setting should change to real values after init",
                  po->GetActivationTimeThreshold() >= Real(0.0));
      }

      CPPUNIT_ASSERT_EQUAL(activationImpl->GetActivationLinearVelocityThreshold(), po->GetActivationLinearVelocityThreshold());
      CPPUNIT_ASSERT_EQUAL(activationImpl->GetActivationAngularVelocityThreshold(), po->GetActivationAngularVelocityThreshold());
      CPPUNIT_ASSERT_EQUAL(activationImpl->GetActivationTimeThreshold(), po->GetActivationTimeThreshold());
   }

   void dtPhysicsTests::testPhysicsObjectDampingAccessors(const std::string& engine)
   {
      ChangeEngine(engine);
      dtCore::RefPtr<PhysicsObject> po = PhysicsObject::CreateNew("jojo");
      po->SetPrimitiveType(PrimitiveType::SPHERE);
      po->SetExtents(VectorType(1.0f, 0.0f, 0.0f));
      po->SetMass(30.0f);

      CPPUNIT_ASSERT_DOUBLES_EQUAL(Real(0.0), po->GetLinearDamping(), Real(0.01));
      CPPUNIT_ASSERT_DOUBLES_EQUAL(Real(0.0), po->GetAngularDamping(), Real(0.01));

      po->SetLinearDamping(Real(70.0));
      po->SetAngularDamping(Real(72.0));

      CPPUNIT_ASSERT_DOUBLES_EQUAL(Real(1.0), po->GetLinearDamping(), Real(0.01));
      CPPUNIT_ASSERT_DOUBLES_EQUAL(Real(1.0), po->GetAngularDamping(), Real(0.01));

      po->SetLinearDamping(Real(0.1));
      po->SetAngularDamping(Real(0.2));

      CPPUNIT_ASSERT_DOUBLES_EQUAL(Real(0.1), po->GetLinearDamping(), Real(0.01));
      CPPUNIT_ASSERT_DOUBLES_EQUAL(Real(0.2), po->GetAngularDamping(), Real(0.01));

      po->Create();

      CPPUNIT_ASSERT_DOUBLES_EQUAL(Real(0.1), po->GetLinearDamping(), Real(0.01));
      CPPUNIT_ASSERT_DOUBLES_EQUAL(Real(0.2), po->GetAngularDamping(), Real(0.01));

      CPPUNIT_ASSERT(po->GetGenericBodyWrapper() != nullptr);

      CPPUNIT_ASSERT_DOUBLES_EQUAL(Real(0.1), po->GetGenericBodyWrapper()->GetPalGenericBody().GetLinearDamping(), Real(0.01));
      CPPUNIT_ASSERT_DOUBLES_EQUAL(Real(0.2), po->GetGenericBodyWrapper()->GetPalGenericBody().GetAngularDamping(), Real(0.01));

      po->SetLinearDamping(Real(0.3));
      po->SetAngularDamping(Real(0.4));

      CPPUNIT_ASSERT_DOUBLES_EQUAL(Real(0.3), po->GetLinearDamping(), Real(0.01));
      CPPUNIT_ASSERT_DOUBLES_EQUAL(Real(0.4), po->GetAngularDamping(), Real(0.01));
      CPPUNIT_ASSERT_DOUBLES_EQUAL(Real(0.3), po->GetGenericBodyWrapper()->GetPalGenericBody().GetLinearDamping(), Real(0.01));
      CPPUNIT_ASSERT_DOUBLES_EQUAL(Real(0.4), po->GetGenericBodyWrapper()->GetPalGenericBody().GetAngularDamping(), Real(0.01));
   }

   void dtPhysicsTests::testPhysicsObjectDeleteWithConstraints(const std::string& engine)
   {
      ChangeEngine(engine);
      dtCore::RefPtr<PhysicsObject> poA = PhysicsObject::CreateNew("jojo");
      poA->SetMass(30.0f);
      poA->Create();

      dtCore::RefPtr<PhysicsObject> poB = PhysicsObject::CreateNew("mojo");
      poB->SetMass(50.0f);
      poB->Create();

      palLink* rigidLinkBefore = dtPhysics::PhysicsWorld::GetInstance().GetPalFactory()->CreateRigidLink(&poA->GetBodyWrapper()->GetPalBodyBase(),
            &poB->GetBodyWrapper()->GetPalBodyBase());
      palLink* rigidLinkAfter = dtPhysics::PhysicsWorld::GetInstance().GetPalFactory()->CreateRigidLink(&poA->GetBodyWrapper()->GetPalBodyBase(),
            &poB->GetBodyWrapper()->GetPalBodyBase());

      CPPUNIT_ASSERT(rigidLinkBefore != nullptr);
      CPPUNIT_ASSERT(rigidLinkAfter != nullptr);

      // this test just verifies you can delete the link BEFORE deleting the bodies.
      delete rigidLinkBefore;

      poA = nullptr;
      poB = nullptr;
      // this test just verifies you can delete the link AFTER deleting the bodies.
      delete rigidLinkAfter;
   }

   void dtPhysicsTests::testPhysicsObjectActivationInitializeAndChange(const std::string& engine)
   {
      ChangeEngine(engine);
      dtCore::RefPtr<PhysicsObject> po = PhysicsObject::CreateNew("jojo");
      po->SetMass(30.0f);

      CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("All activation settings should default to -1, engine default",
               po->GetActivationLinearVelocityThreshold(), Real(-1.0), Real(0.1));
      CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("All activation settings should default to -1, engine default",
               po->GetActivationAngularVelocityThreshold(), Real(-1.0), Real(0.1));
      CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("All activation settings should default to -1, engine default",
               po->GetActivationTimeThreshold(), Real(-1.0), Real(0.1));

      po->SetActivationLinearVelocityThreshold(Real(3.0f));
      po->SetActivationAngularVelocityThreshold(Real(4.0f));
      po->SetActivationTimeThreshold(Real(5.0f));

      CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("All activation settings should have changed",
               po->GetActivationLinearVelocityThreshold(), Real(3.0), Real(0.01));
      CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("All activation settings should have changed",
               po->GetActivationAngularVelocityThreshold(), Real(4.0), Real(0.1));
      CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("All activation settings should have changed",
               po->GetActivationTimeThreshold(), Real(5.0), Real(0.1));

      po->SetCollisionResponseEnabled(false);
      po->SetNotifyCollisions(true);

      // Create from geometry this time to make sure it, too, sets activation correctly.
      dtCore::RefPtr<Geometry> geom = Geometry::CreateBoxGeometry(TransformType(), VectorType(1.0, 1.0, 1.0), 1.0f);
      po->CreateFromGeometry(*geom);

      CPPUNIT_ASSERT(!po->GetCollisionResponseEnabled());
      CPPUNIT_ASSERT(po->GetNotifyCollisions());

      palActivationSettings* activationImpl = dynamic_cast<palActivationSettings*>(&po->GetBodyWrapper()->GetPalBodyBase());

      if (activationImpl != nullptr)
      {
         if (activationImpl->GetSupportedActivationSettings()[palActivationSettings::LINEAR_VELOCITY_THRESHOLD])
         {
            CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("Activation setting should keep configured value after init, even if activation is not supported",
                     Real(3.0), po->GetActivationLinearVelocityThreshold(), Real(0.01));
         }

         if (activationImpl->GetSupportedActivationSettings()[palActivationSettings::ANGULAR_VELOCITY_THRESHOLD])
         {
            CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("Activation setting should keep configured value after init, even if activation is not supported",
                     Real(4.0), po->GetActivationAngularVelocityThreshold(), Real(0.01));
         }

         if (activationImpl->GetSupportedActivationSettings()[palActivationSettings::TIME_THRESHOLD])
         {
            CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("Activation setting should keep configured value after init, even if activation is not supported",
                     Real(5.0), po->GetActivationTimeThreshold(), Real(0.01));
         }

         CPPUNIT_ASSERT_EQUAL(activationImpl->GetActivationLinearVelocityThreshold(), po->GetActivationLinearVelocityThreshold());
         CPPUNIT_ASSERT_EQUAL(activationImpl->GetActivationAngularVelocityThreshold(), po->GetActivationAngularVelocityThreshold());
         CPPUNIT_ASSERT_EQUAL(activationImpl->GetActivationTimeThreshold(), po->GetActivationTimeThreshold());
      }
   }

   struct TestFunctor
   {
      TestFunctor(unsigned& count, bool& foundNull)
      : mCount(count)
      , mFoundNull(foundNull)
      {
      }

      void operator()(dtCore::RefPtr<PhysicsObject>& po)
      {
         mCount++;
         mFoundNull = mFoundNull || !po.valid();
      }

      unsigned& mCount;
      bool& mFoundNull;
   };

   /////////////////////////////////////////////////////////
   void dtPhysicsTests::testPhysicsActComp(const std::string& engine)
   {
      ChangeEngine(engine);

      std::string name1("mypalFriend1");
      std::string name2("mypalFriend2");
      std::string name3("mypalFriend3");

      // make object
      dtCore::RefPtr<PhysicsActComp> tehVoodoo = new PhysicsActComp();

      // test name
      tehVoodoo->SetName("tehVoodooActorComp");
      CPPUNIT_ASSERT_MESSAGE("name: should be tehVoodooActorComp", tehVoodoo->GetName() == std::string("tehVoodooActorComp"));

      // test adding and removing of objects
      dtCore::RefPtr<PhysicsObject> physicsObject1 = PhysicsObject::CreateNew(name1);
      dtCore::RefPtr<PhysicsObject> physicsObject2 = PhysicsObject::CreateNew(name2);
      dtCore::RefPtr<PhysicsObject> physicsObject3 = PhysicsObject::CreateNew(name3);

      CPPUNIT_ASSERT(tehVoodoo->GetMainPhysicsObject() == nullptr);
      // adding
      tehVoodoo->AddPhysicsObject(*physicsObject2);
      CPPUNIT_ASSERT(tehVoodoo->GetMainPhysicsObject() == physicsObject2);
      tehVoodoo->AddPhysicsObject(*physicsObject3, false);
      CPPUNIT_ASSERT(tehVoodoo->GetMainPhysicsObject() == physicsObject2);
      tehVoodoo->AddPhysicsObject(*physicsObject1, true);
      CPPUNIT_ASSERT(tehVoodoo->GetMainPhysicsObject() == physicsObject1);

      std::vector<PhysicsObject*> toFill;

      tehVoodoo->GetAllPhysicsObjects(toFill);
      CPPUNIT_ASSERT_EQUAL(size_t(3U), toFill.size());
      CPPUNIT_ASSERT(toFill[0] == physicsObject1);
      CPPUNIT_ASSERT(toFill[1] == physicsObject2);
      CPPUNIT_ASSERT(toFill[2] == physicsObject3);

      TransformType xformEx, xformActual, xformVtoBody;
      VectorType v3(Real(3.1), Real(7.7), Real(91.23));
      xformEx.SetTranslation(v3);

      tehVoodoo->SetMultiBodyTransform(xformEx);
      tehVoodoo->GetMultiBodyTransform(xformActual);
      CPPUNIT_ASSERT_MESSAGE("SetMultiBodyTransform should return what was set.",
            xformActual.EpsilonEquals(xformEx, 0.001f));

      toFill[0]->GetTransform(xformActual);
      CPPUNIT_ASSERT_MESSAGE("By default, SetMultiBodyTransform should just set the main body.",
            xformActual.EpsilonEquals(xformEx, 0.001f));
      toFill[1]->GetTransform(xformActual);
      CPPUNIT_ASSERT_MESSAGE("By default, SetMultiBodyTransform should just set the main body.",
            !xformActual.EpsilonEquals(xformEx, 0.001f));
      toFill[2]->GetTransform(xformActual);
      CPPUNIT_ASSERT_MESSAGE("By default, SetMultiBodyTransform should just set the main body.",
            !xformActual.EpsilonEquals(xformEx, 0.001f));

      // Set a non-zero visual to body transform
      // so that GetTransform and GetTransformAsVisual won't do the same thing.
      xformVtoBody.SetRotation(31.3f, 93.2f, 11.3f);
      for (unsigned i = 0; i < 3; ++i)
      {
         toFill[i]->SetVisualToBodyTransform(xformVtoBody);
      }

      tehVoodoo->SetMultiBodyTransformAsVisual(xformEx);
      tehVoodoo->GetMultiBodyTransformAsVisual(xformActual);
      CPPUNIT_ASSERT_MESSAGE("SetMultiBodyTransformAsVisual should return what was set.",
            xformActual.EpsilonEquals(xformEx, 0.001f));

      toFill[0]->GetTransformAsVisual(xformActual);
      CPPUNIT_ASSERT_MESSAGE("By default, SetMultiBodyTransformAsVisual should on all bodies.",
            xformActual.EpsilonEquals(xformEx, 0.001f));
      toFill[1]->GetTransformAsVisual(xformActual);
      CPPUNIT_ASSERT_MESSAGE("By default, SetMultiBodyTransformAsVisual should on all bodies.",
            xformActual.EpsilonEquals(xformEx, 0.001f));
      toFill[2]->GetTransformAsVisual(xformActual);
      CPPUNIT_ASSERT_MESSAGE("By default, SetMultiBodyTransformAsVisual should on all bodies.",
            xformActual.EpsilonEquals(xformEx, 0.001f));

      unsigned count = 0;
      bool foundNull = false;
      TestFunctor getAllFunc(count, foundNull);
      // I want to force it to pass the functor by reference
      tehVoodoo->ForEachPhysicsObject(getAllFunc);
      CPPUNIT_ASSERT_EQUAL(3U, getAllFunc.mCount);
      CPPUNIT_ASSERT(!getAllFunc.mFoundNull);

      physicsObject1->Create();
      physicsObject2->Create();
      physicsObject3->Create();

      tehVoodoo->SetAllActive(true);
      CPPUNIT_ASSERT_MESSAGE("physics object1 should be active", tehVoodoo->GetPhysicsObject(name1)->IsActive());
      CPPUNIT_ASSERT_MESSAGE("physics object1 should be active", tehVoodoo->GetPhysicsObject(name2)->IsActive());
      CPPUNIT_ASSERT_MESSAGE("physics object1 should be active", tehVoodoo->GetPhysicsObject(name3)->IsActive());
      tehVoodoo->SetAllActive(false);
      CPPUNIT_ASSERT_MESSAGE("physics object1 should not active", !tehVoodoo->GetPhysicsObject(name1)->IsActive());
      CPPUNIT_ASSERT_MESSAGE("physics object1 should not active", !tehVoodoo->GetPhysicsObject(name2)->IsActive());
      CPPUNIT_ASSERT_MESSAGE("physics object1 should not active", !tehVoodoo->GetPhysicsObject(name3)->IsActive());

      // see if they exist
      CPPUNIT_ASSERT_MESSAGE("physics object1 should be on actorComp", tehVoodoo->GetPhysicsObject(name1) != nullptr);
      CPPUNIT_ASSERT_MESSAGE("physics object2 should be on actorComp", tehVoodoo->GetPhysicsObject(name2) != nullptr);
      CPPUNIT_ASSERT_MESSAGE("physics object3 should be on actorComp", tehVoodoo->GetPhysicsObject(name3) != nullptr);

      // remove one see if u can't get to it, and can get to others
      tehVoodoo->RemovePhysicsObject("mypalFriend2");
      CPPUNIT_ASSERT_MESSAGE("physics object2 shouldn't be on actorComp", tehVoodoo->GetPhysicsObject(name2) == nullptr);
      CPPUNIT_ASSERT_MESSAGE("physics object1 should be on actorComp", tehVoodoo->GetPhysicsObject(name1) != nullptr);
      CPPUNIT_ASSERT_MESSAGE("physics object3 should be on actorComp", tehVoodoo->GetPhysicsObject(name3) != nullptr);

      // clear all, none should on be on the actorComp
      tehVoodoo->ClearAllPhysicsObjects();
      CPPUNIT_ASSERT_MESSAGE("physics object2 shouldn't be on actorComp", tehVoodoo->GetPhysicsObject(name2) == nullptr);
      CPPUNIT_ASSERT_MESSAGE("physics object1 shouldn't be on actorComp", tehVoodoo->GetPhysicsObject(name1) == nullptr);
      CPPUNIT_ASSERT_MESSAGE("physics object3 shouldn't be on actorComp", tehVoodoo->GetPhysicsObject(name3) == nullptr);

      tehVoodoo->GetAllPhysicsObjects(toFill);
      CPPUNIT_ASSERT_EQUAL_MESSAGE("GetAllPhysicsObjects Should not clear the vector", size_t(3U), toFill.size());
      toFill.clear();
      tehVoodoo->GetAllPhysicsObjects(toFill);
      CPPUNIT_ASSERT_EQUAL_MESSAGE("GetAllPhysicsObjects should add nothing to the passed in vector because there are no objects",
               size_t(0U), toFill.size());

      testMass(*tehVoodoo);
   }

   /////////////////////////////////////////////////////////
   void dtPhysicsTests::testConvexHullCaching(const std::string& engine)
   {
      dtPhysics::VertexData::ClearAllCachedData();
      std::string cachingString("jojo");
      ChangeEngine(engine);
      CPPUNIT_ASSERT(dtPhysics::VertexData::FindCachedData(cachingString) == nullptr);
      try
      {
         //std::string path = dtCore::Project::GetInstance().GetResourcePath(dtCore::ResourceDescriptor("StaticMeshes:physics_crate.ive"));
         std::string path = "../examples/data/StaticMeshes/physics_crate.ive";
         path = dtUtil::FindFileInPathList(path);
         dtCore::RefPtr<osgDB::ReaderWriter::Options> options = new osgDB::ReaderWriter::Options;

         options->setObjectCacheHint(osgDB::ReaderWriter::Options::CACHE_ALL);

         dtCore::RefPtr<osg::Node> box = osgDB::readNodeFile(path, options.get());

         CPPUNIT_ASSERT(box.valid());

         osg::Vec3 offsetCenter(1.7, 3.3, -4.6);
         dtCore::RefPtr<dtCore::Transformable> xformable = new dtCore::Transformable;
         xformable->GetOSGNode()->asGroup()->addChild(box);
         dtCore::Transform xform;
         xform.SetTranslation(offsetCenter);
         xformable->SetTransform(xform);

         dtCore::RefPtr<PhysicsObject> po = PhysicsObject::CreateNew("Brad");
         VectorType originOffset(3.7f, 4.1f, 3.3f);
         po->SetOriginOffset(originOffset);
         po->SetPrimitiveType(PrimitiveType::CONVEX_HULL);

         dtCore::Timer_t timerBegin = dtCore::Timer::Instance()->Tick();
         po->Create(xformable->GetOSGNode(), true, cachingString);
         dtCore::Timer_t timerEnd = dtCore::Timer::Instance()->Tick();

         po->CleanUp();

         double elapsedFirst = dtCore::Timer::Instance()->DeltaMil(timerBegin, timerEnd);

         CPPUNIT_ASSERT_MESSAGE("A cached convex hull should exist with name " + cachingString + "_Polytope",
                  dtPhysics::VertexData::FindCachedData(cachingString + "_Polytope") != nullptr);

         // Creating it a second time should use the cache.
         timerBegin = dtCore::Timer::Instance()->Tick();
         po->Create(xformable->GetOSGNode(), true, cachingString);
         timerEnd = dtCore::Timer::Instance()->Tick();

         double elapsedSecond = dtCore::Timer::Instance()->DeltaMil(timerBegin, timerEnd);

         std::ostringstream ss;
         ss << " Times:  cached [" << elapsedSecond << "] milliseconds  vs uncached [" << elapsedFirst << "] milliseconds";
         CPPUNIT_ASSERT_MESSAGE("The elapsed time for creating a cached hull should be significantly less that the first time"
               + ss.str(),
                  (elapsedSecond * 1.25f) < elapsedFirst);

         dtPhysics::VertexData::ClearCachedData(cachingString);
         CPPUNIT_ASSERT_MESSAGE("After cleaning the cached data, it should be gone",
                  dtPhysics::VertexData::FindCachedData(cachingString) == nullptr);
      }
      catch (const dtUtil::Exception& ex)
      {
         CPPUNIT_FAIL(ex.ToString());
      }
   }

   /////////////////////////////////////////////////////////
   void dtPhysicsTests::testMass(dtPhysics::PhysicsActComp& actorComp)
   {
      CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("Mass should default to 0", actorComp.GetMass(),
               0.0f, 1e-3f);
      float newVal = 73.236f;
      actorComp.SetMass(newVal);
      CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("Mass should have changed", actorComp.GetMass(),
               newVal, 1e-3f);

   }

   /////////////////////////////////////////////////////////
   void dtPhysicsTests::testObjectArray()
   {
      try
      {
         dtCore::RefPtr<PhysicsActComp> actCompPtr = new PhysicsActComp();
         PhysicsActComp& actorComp = *actCompPtr;

         std::string name1("TestObjectA");
         std::string name2("TestObjectB");
         std::string name3("TestObjectC");

         CPPUNIT_ASSERT(actorComp.GetPhysicsObjectCount() == 0);

         actorComp.InsertNewPhysicsObject(0);
         actorComp.InsertNewPhysicsObject(0);

         CPPUNIT_ASSERT(actorComp.GetPhysicsObjectCount() == 2);
         CPPUNIT_ASSERT(actorComp.GetPhysicsObjectByIndex(0) != nullptr);
         CPPUNIT_ASSERT(actorComp.GetPhysicsObjectByIndex(1) != nullptr);
         CPPUNIT_ASSERT(actorComp.GetPhysicsObjectByIndex(2) == nullptr);

         dtCore::RefPtr<PhysicsObject> po1 = actorComp.GetPhysicsObjectByIndex(0);
         dtCore::RefPtr<PhysicsObject> po3 = actorComp.GetPhysicsObjectByIndex(1);
         po1->SetName(name1);
         po3->SetName(name3);

         actorComp.InsertNewPhysicsObject(1);
         CPPUNIT_ASSERT(actorComp.GetPhysicsObjectCount() == 3);
         CPPUNIT_ASSERT(actorComp.GetPhysicsObjectByIndex(0) != nullptr);
         CPPUNIT_ASSERT(actorComp.GetPhysicsObjectByIndex(1) != nullptr);
         CPPUNIT_ASSERT(actorComp.GetPhysicsObjectByIndex(2) != nullptr);
         CPPUNIT_ASSERT(actorComp.GetPhysicsObjectByIndex(3) == nullptr);

         // Insert a new object between the first and last.
         dtCore::RefPtr<PhysicsObject> po2 = actorComp.GetPhysicsObjectByIndex(1);
         po2->SetName(name2);

         CPPUNIT_ASSERT(actorComp.GetPhysicsObjectByIndex(0)->GetName() == name1);
         CPPUNIT_ASSERT(actorComp.GetPhysicsObjectByIndex(1)->GetName() == name2);
         CPPUNIT_ASSERT(actorComp.GetPhysicsObjectByIndex(2)->GetName() == name3);
         CPPUNIT_ASSERT(actorComp.GetPhysicsObject(name1) == po1.get());
         CPPUNIT_ASSERT(actorComp.GetPhysicsObject(name2) == po2.get());
         CPPUNIT_ASSERT(actorComp.GetPhysicsObject(name3) == po3.get());


         // Test removal
         actorComp.RemovePhysicsObjectByIndex(1);
         CPPUNIT_ASSERT(actorComp.GetPhysicsObjectCount() == 2);
         CPPUNIT_ASSERT(actorComp.GetPhysicsObjectByIndex(0) != nullptr);
         CPPUNIT_ASSERT(actorComp.GetPhysicsObjectByIndex(1) != nullptr);
         CPPUNIT_ASSERT(actorComp.GetPhysicsObjectByIndex(2) == nullptr);
         CPPUNIT_ASSERT(actorComp.GetPhysicsObjectByIndex(0)->GetName() == name1);
         CPPUNIT_ASSERT(actorComp.GetPhysicsObjectByIndex(1)->GetName() == name3);

         actorComp.RemovePhysicsObjectByIndex(0);
         CPPUNIT_ASSERT(actorComp.GetPhysicsObjectCount() == 1);
         CPPUNIT_ASSERT(actorComp.GetPhysicsObjectByIndex(0) != nullptr);
         CPPUNIT_ASSERT(actorComp.GetPhysicsObjectByIndex(1) == nullptr);
         CPPUNIT_ASSERT(actorComp.GetPhysicsObjectByIndex(0)->GetName() == name3);

         // --- Test failed removal
         actorComp.RemovePhysicsObjectByIndex(2);
         CPPUNIT_ASSERT(actorComp.GetPhysicsObjectCount() == 1);
         CPPUNIT_ASSERT(actorComp.GetPhysicsObjectByIndex(0) != nullptr);
         CPPUNIT_ASSERT(actorComp.GetPhysicsObjectByIndex(1) == nullptr);
         CPPUNIT_ASSERT(actorComp.GetPhysicsObjectByIndex(0)->GetName() == name3);

         actorComp.RemovePhysicsObjectByIndex(0);
         CPPUNIT_ASSERT(actorComp.GetPhysicsObjectCount() == 0);
         CPPUNIT_ASSERT(actorComp.GetPhysicsObjectByIndex(0) == nullptr);
         CPPUNIT_ASSERT(actorComp.GetPhysicsObjectByIndex(1) == nullptr);
      }
      catch(dtUtil::Exception& ex)
      {
         LOG_ERROR("Exception hit: " + ex.ToString());
      }
      catch(...)
      {
         LOG_ERROR("Unknown exception encountered.");
      }
   }

   /////////////////////////////////////////////////////////
   void dtPhysicsTests::testComponent(const std::string& engine)
   {
      ChangeEngine(engine);

      // test the stepping values
      mPhysicsComp->GetPhysicsWorld().SetStepTime(123);
      CPPUNIT_ASSERT_MESSAGE("World step doesn't match up!", mPhysicsComp->GetPhysicsWorld().GetStepTime() == 123);

      // trace ray not implemented.

      // Get actorComp for proxy not done.

      // do test for process message

      // make a couple of actorComps
      dtCore::RefPtr<PhysicsActComp> tehVoodoo1 = new PhysicsActComp();
      dtCore::RefPtr<PhysicsActComp> tehVoodoo2 = new PhysicsActComp();
      dtCore::RefPtr<PhysicsActComp> tehVoodoo3 = new PhysicsActComp();

      // fill in their names
      tehVoodoo1->SetName("tehVoodooActorComp1");
      tehVoodoo2->SetName("tehVoodooActorComp2");
      tehVoodoo3->SetName("tehVoodooActorComp3");

      // register a couple of actorComps
      mPhysicsComp->RegisterActorComp(*tehVoodoo1.get());
      mPhysicsComp->RegisterActorComp(*tehVoodoo2.get());
      mPhysicsComp->RegisterActorComp(*tehVoodoo3.get());

      // find the actorComps
      CPPUNIT_ASSERT_MESSAGE("ActorComp should have been registered", mPhysicsComp->IsActorCompRegistered(*tehVoodoo1.get()) != false);
      CPPUNIT_ASSERT_MESSAGE("ActorComp should have been registered", mPhysicsComp->IsActorCompRegistered(*tehVoodoo2.get()) != false);
      CPPUNIT_ASSERT_MESSAGE("ActorComp should have been registered", mPhysicsComp->IsActorCompRegistered(*tehVoodoo3.get()) != false);

      // remove a actorComp
      mPhysicsComp->UnregisterActorComp(*tehVoodoo2.get());

      // find or not find actorCompsd
      PhysicsActComp* actorComp1 = mPhysicsComp->GetActorComp("tehVoodooActorComp1");
      PhysicsActComp* actorComp2 = mPhysicsComp->GetActorComp("tehVoodooActorComp2");
      PhysicsActComp* actorComp3 = mPhysicsComp->GetActorComp("tehVoodooActorComp3");
      CPPUNIT_ASSERT_MESSAGE("ActorComp should have been registered", actorComp1 != nullptr);
      CPPUNIT_ASSERT_MESSAGE("ActorComp should have not been registered", actorComp2 == nullptr);
      CPPUNIT_ASSERT_MESSAGE("ActorComp should have been registered", actorComp3 != nullptr);

      // make materials

      // register materials with gm

      // find materials through physicscomponent (finds through gm)

      // clear all actorComps
      mPhysicsComp->ClearAll();

      // find or not find actorComps
      actorComp1 = mPhysicsComp->GetActorComp("tehVoodooActorComp1");
      actorComp2 = mPhysicsComp->GetActorComp("tehVoodooActorComp2");
      actorComp3 = mPhysicsComp->GetActorComp("tehVoodooActorComp3");
      CPPUNIT_ASSERT_MESSAGE("ActorComp should not have been registered", actorComp1 == nullptr);
      CPPUNIT_ASSERT_MESSAGE("ActorComp should not have been registered", actorComp2 == nullptr);
      CPPUNIT_ASSERT_MESSAGE("ActorComp should not have been registered", actorComp3 == nullptr);
   }

   /////////////////////////////////////////////////////////
   void dtPhysicsTests::testCallbacks(const std::string& engine)
   {
      ChangeEngine(engine);

      // make a couple of actorComps
      dtCore::RefPtr<PhysicsActComp> tehVoodoo1 = new PhysicsActComp();
      dtCore::RefPtr<PhysicsActComp> tehVoodoo2 = new PhysicsActComp();

      // Need to create a physics object because, if not, the physics may not step.
      dtCore::RefPtr<PhysicsObject> poA = PhysicsObject::CreateNew("jojo");
      poA->SetMass(30.0f);
      poA->Create();

      // fill in their names
      tehVoodoo1->SetName("tehVoodooActorComp1");
      tehVoodoo2->SetName("tehVoodooActorComp2");

      CallbackTester cb1;
      CallbackTester cb2;
      tehVoodoo1->SetPrePhysicsCallback(dtUtil::MakeFunctor(&CallbackTester::UpdateCallbackPre, &cb1));
      tehVoodoo1->SetPostPhysicsCallback(dtUtil::MakeFunctor(&CallbackTester::UpdateCallbackPost, &cb1));

      tehVoodoo1->SetActionUpdateCallback(dtUtil::MakeFunctor(&CallbackTester::ActionUpdate, &cb1));

      tehVoodoo2->SetPrePhysicsCallback(dtUtil::MakeFunctor(&CallbackTester::UpdateCallbackPre, &cb2));
      tehVoodoo2->SetPostPhysicsCallback(dtUtil::MakeFunctor(&CallbackTester::UpdateCallbackPost, &cb2));

      tehVoodoo2->SetActionUpdateCallback(dtUtil::MakeFunctor(&CallbackTester::ActionUpdate, &cb2));

      // register a couple of actorComps
      mPhysicsComp->RegisterActorComp(*tehVoodoo1.get());
      mPhysicsComp->RegisterActorComp(*tehVoodoo2.get());

      int steps = dtPhysics::PhysicsWorld::GetInstance().GetNumStepsSinceStartup();
      // Quick check to make sure the physics didn't step.
      mPhysicsComp->SetSteppingEnabled(false);
      dtCore::System::GetInstance().Step(0.1667);
      int stepsNext = dtPhysics::PhysicsWorld::GetInstance().GetNumStepsSinceStartup();
      CPPUNIT_ASSERT_EQUAL(steps, stepsNext);
      CPPUNIT_ASSERT(!cb1.HasCalledPre());
      CPPUNIT_ASSERT(!cb1.HasCalledPost());
      CPPUNIT_ASSERT(!cb1.HasCalledAction());
      CPPUNIT_ASSERT(!cb2.HasCalledPre());
      CPPUNIT_ASSERT(!cb2.HasCalledPost());
      CPPUNIT_ASSERT(!cb2.HasCalledAction());

      mPhysicsComp->SetSteppingEnabled(true);
      dtCore::System::GetInstance().Step(0.1667);
      stepsNext = dtPhysics::PhysicsWorld::GetInstance().GetNumStepsSinceStartup();
      CPPUNIT_ASSERT(steps < stepsNext);
      CPPUNIT_ASSERT(cb1.HasCalledPre());
      CPPUNIT_ASSERT(cb1.HasCalledPost());
      CPPUNIT_ASSERT(cb1.HasCalledAction());
      CPPUNIT_ASSERT(cb2.HasCalledPre());
      CPPUNIT_ASSERT(cb2.HasCalledPost());
      CPPUNIT_ASSERT(cb2.HasCalledAction());

      /// All flags set, but still shouldn't call collision because none happened.

      cb1.Reset();
      cb2.Reset();
      tehVoodoo1->SetPrePhysicsCallback(PhysicsActComp::UpdateCallback());
      tehVoodoo1->SetPostPhysicsCallback(PhysicsActComp::UpdateCallback());
      tehVoodoo1->SetActionUpdateCallback(PhysicsActComp::ActionUpdateCallback());

      tehVoodoo2->SetPrePhysicsCallback(PhysicsActComp::UpdateCallback());
      tehVoodoo2->SetPostPhysicsCallback(PhysicsActComp::UpdateCallback());
      tehVoodoo2->SetActionUpdateCallback(PhysicsActComp::ActionUpdateCallback());

      dtCore::System::GetInstance().Step(0.1667);

      CPPUNIT_ASSERT(!cb1.HasCalledPre());
      CPPUNIT_ASSERT(!cb1.HasCalledPost());
      CPPUNIT_ASSERT(!cb2.HasCalledPre());
      CPPUNIT_ASSERT(!cb2.HasCalledPost());

      // clear all actorComps
      mPhysicsComp->ClearAll();

   }

   /////////////////////////////////////////////////////////
   void dtPhysicsTests::testMaterialActor()
   {
      ChangeEngine(GetPhysicsEngineList()[0]);
      // create proxy
      dtCore::RefPtr<dtPhysics::MaterialActor> mat;
      mGM->CreateActor(*dtPhysics::PhysicsActorRegistry::PHYSICS_MATERIAL_ACTOR_TYPE, mat);
      CPPUNIT_ASSERT_MESSAGE("Materials should be created on the client and server.", mat->GetInitialOwnership() == dtGame::GameActorProxy::Ownership::CLIENT_AND_SERVER_LOCAL);

      // test to see if valid
      CPPUNIT_ASSERT(mat.valid());

      mat->SetName("Melange");
      mat->AddAlias("Spice");
      mat->AddAlias("Control");

      CPPUNIT_ASSERT_EQUAL_MESSAGE("The Default is wrong", mat->GetMaterialDef().GetRestitution(), 0.2f);
      CPPUNIT_ASSERT_EQUAL_MESSAGE("The Default is wrong", mat->GetMaterialDef().GetStaticFriction(), 0.5f);
      CPPUNIT_ASSERT_EQUAL_MESSAGE("The Default is wrong", mat->GetMaterialDef().GetKineticFriction(), 0.5f);
      CPPUNIT_ASSERT_MESSAGE("The Default is wrong", !mat->GetMaterialDef().GetDisableStrongFriction());
      CPPUNIT_ASSERT_MESSAGE("The Default is wrong", !mat->GetMaterialDef().GetEnableAnisotropicFriction());
      CPPUNIT_ASSERT_EQUAL_MESSAGE("The Default is wrong", mat->GetMaterialDef().GetStaticAnisotropicFriction(), osg::Vec3(1.0f, 1.0f, 1.0f));
      CPPUNIT_ASSERT_EQUAL_MESSAGE("The Default is wrong", mat->GetMaterialDef().GetKineticAnisotropicFriction(), osg::Vec3(1.0f, 1.0f, 1.0f));
      CPPUNIT_ASSERT_EQUAL_MESSAGE("The Default is wrong", mat->GetMaterialDef().GetDirOfAnisotropy(), osg::Vec3(1.0f, 0.0f, 0.0f));

      // check properties on the material.
      mat->GetMaterialDef().SetRestitution(0.3f);
      mat->GetMaterialDef().SetStaticFriction(71.0f);
      mat->GetMaterialDef().SetKineticFriction(9.0f);
      mat->GetMaterialDef().SetDisableStrongFriction(true);
      mat->GetMaterialDef().SetEnableAnisotropicFriction(true);
      mat->GetMaterialDef().SetStaticAnisotropicFriction(osg::Vec3(1.0, 1.1, 0.3));
      mat->GetMaterialDef().SetKineticAnisotropicFriction(osg::Vec3(0.9, 1.2, 0.4));
      mat->GetMaterialDef().SetDirOfAnisotropy(osg::Vec3(0.707, 0.0, 0.707));

      // test to see if they were set correctly.
      CPPUNIT_ASSERT_EQUAL_MESSAGE("Failed to set property", mat->GetMaterialDef().GetRestitution(), 0.3f);
      CPPUNIT_ASSERT_EQUAL_MESSAGE("Failed to set property", mat->GetMaterialDef().GetStaticFriction(), 71.0f);
      CPPUNIT_ASSERT_EQUAL_MESSAGE("Failed to set property", mat->GetMaterialDef().GetKineticFriction(), 9.0f);
      CPPUNIT_ASSERT_MESSAGE("Failed to set property", mat->GetMaterialDef().GetDisableStrongFriction());
      CPPUNIT_ASSERT_MESSAGE("Failed to set property", mat->GetMaterialDef().GetEnableAnisotropicFriction());
      CPPUNIT_ASSERT_EQUAL_MESSAGE("Failed to set property", mat->GetMaterialDef().GetStaticAnisotropicFriction(), osg::Vec3(1.0, 1.1, 0.3));
      CPPUNIT_ASSERT_EQUAL_MESSAGE("Failed to set property", mat->GetMaterialDef().GetKineticAnisotropicFriction(), osg::Vec3(0.9, 1.2, 0.4));
      CPPUNIT_ASSERT_EQUAL_MESSAGE("Failed to set property", mat->GetMaterialDef().GetDirOfAnisotropy(), osg::Vec3(0.707, 0.0, 0.707));

      PhysicsMaterials& materials = dtPhysics::PhysicsWorld::GetInstance().GetMaterials();

      CPPUNIT_ASSERT(materials.GetMaterial("Melange") == nullptr);
      mGM->AddActor(*mat);
      dtPhysics::Material* materialM = materials.GetMaterial("Melange");
      CPPUNIT_ASSERT(materialM != nullptr);
      CPPUNIT_ASSERT(materials.GetMaterial("Spice") == materialM);
      CPPUNIT_ASSERT(materials.GetMaterial("Control") == materialM);
      // test to see if they were set correctly.
      CPPUNIT_ASSERT_EQUAL_MESSAGE("Failed to set property", mat->GetMaterialDef().GetRestitution(), materialM->m_fRestitution);
      CPPUNIT_ASSERT_EQUAL_MESSAGE("Failed to set property", mat->GetMaterialDef().GetStaticFriction(), materialM->m_fStatic);
      CPPUNIT_ASSERT_EQUAL_MESSAGE("Failed to set property", mat->GetMaterialDef().GetKineticFriction(), materialM->m_fKinetic);
      CPPUNIT_ASSERT_EQUAL_MESSAGE("Failed to set property", mat->GetMaterialDef().GetDisableStrongFriction(), materialM->m_bDisableStrongFriction);
      CPPUNIT_ASSERT_EQUAL_MESSAGE("Failed to set property", mat->GetMaterialDef().GetEnableAnisotropicFriction(), materialM->m_bEnableAnisotropicFriction);
      dtPhysics::VectorType testVec;
      dtPhysics::PalVecToVectorType(testVec, materialM->m_vStaticAnisotropic);
      CPPUNIT_ASSERT_EQUAL_MESSAGE("Failed to set property", mat->GetMaterialDef().GetStaticAnisotropicFriction(), testVec);
      dtPhysics::PalVecToVectorType(testVec, materialM->m_vKineticAnisotropic);
      CPPUNIT_ASSERT_EQUAL_MESSAGE("Failed to set property", mat->GetMaterialDef().GetKineticAnisotropicFriction(), testVec);
      dtPhysics::PalVecToVectorType(testVec, materialM->m_vDirAnisotropy);
      CPPUNIT_ASSERT_EQUAL_MESSAGE("Failed to set property", mat->GetMaterialDef().GetDirOfAnisotropy(), testVec);

      mGM->DeleteActor(*mat);
      dtCore::System::GetInstance().Step(0.01667f);
      dtCore::System::GetInstance().Step(0.01667f);
      CPPUNIT_ASSERT(materials.GetMaterial("Spice") == nullptr);
      CPPUNIT_ASSERT(materials.GetMaterial("Control") == nullptr);
   }

   void dtPhysicsTests::testAutoCreate()
   {
      dtCore::RefPtr<PhysicsActComp> une = new PhysicsActComp();
      dtCore::RefPtr<PhysicsActComp> deux = new PhysicsActComp();

      // fill in their names
      une->SetName("unus");
      deux->SetName("duo");

      CPPUNIT_ASSERT(!une->GetAutoCreateOnEnteringWorld());
      CPPUNIT_ASSERT(!deux->GetAutoCreateOnEnteringWorld());
      une->SetAutoCreateOnEnteringWorld(true);
   }

   /////////////////////////////////////////////////////////
   void dtPhysicsTests::testPhysicsReaderWriter()
   {
      dtCore::RefPtr<dtPhysics::VertexData> data = new dtPhysics::VertexData;

      CPPUNIT_ASSERT(data->GetMaterialCount() == 0);
      CPPUNIT_ASSERT(data->GetMaterialIndex("") == dtPhysics::VertexData::INVALID_INDEX);
      
      unsigned i = 0;
      for(; i < 100; ++i)
      {
         data->mIndices.push_back(i);
      }

      for(;i < 200; ++i)
      {
         data->mMaterialFlags.push_back(i);
      }

      for(;i < 300; ++i)
      {
         data->mVertices.push_back(osg::Vec3(0.0f, 0.0f, i));
      }

      const std::string MAT_NAME_A("MatA");
      const std::string MAT_NAME_B("MatB");
      const std::string MAT_NAME_C("MatC");
      data->SetMaterialName(1, MAT_NAME_A);
      CPPUNIT_ASSERT(data->GetMaterialCount() == 1);
      data->SetMaterialName(3, MAT_NAME_B);
      CPPUNIT_ASSERT(data->GetMaterialCount() == 2);
      data->SetMaterialName(5, MAT_NAME_C);
      CPPUNIT_ASSERT(data->GetMaterialCount() == 3);


      std::string filename("temp_dtPhysicsTests.phys");
      dtPhysics::PhysicsReaderWriter::SaveTriangleDataFile(*data, filename);

      data->mIndices.clear();
      data->mMaterialFlags.clear();
      data->mVertices.clear();
      CPPUNIT_ASSERT(data->ClearMaterialTable() == 3);
      CPPUNIT_ASSERT(data->GetMaterialCount() == 0);

      dtPhysics::PhysicsReaderWriter::LoadTriangleDataFile(*data, filename);

      for(i = 0; i < 100; ++i)
      {         
         CPPUNIT_ASSERT_EQUAL(i, data->mIndices.at(i));
      }

      for(;i < 200; ++i)
      {
         CPPUNIT_ASSERT_EQUAL(i, data->mMaterialFlags.at(i - 100));
      }

      for(;i < 300; ++i)
      {
         CPPUNIT_ASSERT_EQUAL(osg::Vec3(0.0f, 0.0f, i), data->mVertices.at(i - 200));
      }

      CPPUNIT_ASSERT(data->GetMaterialIndex(MAT_NAME_A) == 1);
      CPPUNIT_ASSERT(data->GetMaterialIndex(MAT_NAME_B) == 3);
      CPPUNIT_ASSERT(data->GetMaterialIndex(MAT_NAME_C) == 5);
      CPPUNIT_ASSERT(data->GetMaterialCount() == 3);
   }
}

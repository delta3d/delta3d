#include "dt.h"
#include "dtabc.h"
#include "soarx/dtsoarx.h"

#include <osg/CullSettings>
#include <osg/PolygonMode>
#include <osg/StateAttribute>
#include <osg/StateSet>

//#include <osgUtil/InsertImpostorsVisitor>

// included from PrintVisitor
#include <osg/NodeVisitor>
#include "sketchVisitor.h"

#include <iostream>

using namespace dtCore;
using namespace dtABC;
using namespace dtSOARX;
using namespace std;

//To be used with Imposter code?
//typedef osg::ref_ptr<osg::Node>	NodePtr;
//typedef std::list<NodePtr>		NodeContainer;
//typedef NodeContainer::iterator	NodeIterator;
//NodeContainer					nodes;


//////////////////////////////////////////////////////////////////////////////
// class ExtentsVisitor
//
// description: visit all nodes and compute bounding box extents
// NOTE: Not used in current implementation - saved for future use
class ExtentsVisitor : public osg::NodeVisitor
{
public:

	ExtentsVisitor():NodeVisitor(NodeVisitor::TRAVERSE_ALL_CHILDREN) {}
	// constructor
	~ExtentsVisitor() {}
	// destructor

	virtual void apply(osg::Geode &node)
	{
		osg::BoundingBox bb;
		// update bounding box
		for (unsigned int i = 0; i < node.getNumDrawables(); ++i) {
			// expand overall bounding box
			bb.expandBy(node.getDrawable(i)->getBound());
		}

		osg::BoundingBox xbb;
		// transform corners by current matrix
		for (unsigned int i = 0; i < 8; ++i) {
			osg::Vec3 xv = bb.corner(i) * m_TransformMatrix;
			xbb.expandBy(xv);
		}

		// update overall bounding box size
		m_BoundingBox.expandBy(xbb);

		// continue traversing the graph
		traverse(node);
	}
	// handle geode drawable extents to expand the box

	virtual void apply(osg::MatrixTransform &node)
	{
		m_TransformMatrix *= node.getMatrix();

		// continue traversing the graph
		traverse(node);
	}
	// handle transform to expand bounding box

	osg::BoundingBox &GetBound() { return m_BoundingBox; }
	// return bounding box

protected:

	osg::BoundingBox m_BoundingBox;
	// bound box
	osg::Matrix m_TransformMatrix;
	// current transform matrix
};


//////////////////////////////////////////////////////////////////////////////
// class PrintVisitor
//
// description: visit all nodes and print resulting scene graph
//
// taken from dead code within OSG CullVisitor
// NOTE: Not used in current implementation - saved for future use
class PrintVisitor : public osg::NodeVisitor
{

public:

	PrintVisitor():NodeVisitor(NodeVisitor::TRAVERSE_ALL_CHILDREN)
	{
		_indent = 0;
		_step = 4;
	}

	inline void moveIn() { _indent += _step; }
	inline void moveOut() { _indent -= _step; }
	inline void writeIndent()
	{
		for(int i=0;i<_indent;++i) std::cout << " ";
	}

	virtual void apply(osg::Node& node)
	{
		moveIn();
		writeIndent(); std::cout << node.className() << " name=" << node.getName() << std::endl;
		traverse(node);
		moveOut();
	}

	virtual void apply(osg::Geode& node)         { apply((osg::Node&)node); }
	virtual void apply(osg::Billboard& node)     { apply((osg::Geode&)node); }
	virtual void apply(osg::LightSource& node)   { apply((osg::Node&)node); }
	virtual void apply(osg::Group& node)         { apply((osg::Node&)node); }
	virtual void apply(osg::Transform& node)     { apply((osg::Group&)node); }
	virtual void apply(osg::Switch& node)        { apply((osg::Group&)node); }
	virtual void apply(osg::LOD& node)           { apply((osg::Group&)node); }
	virtual void apply(osg::Impostor& node)      { apply((osg::LOD&)node); }

protected:

	int _indent;
	int _step;
};


/**
 * The terrain test application.
 */
class TestTerrainApplication : public Application
{
   DECLARE_MANAGEMENT_LAYER(TestTerrainApplication)

   public:

      /**
       * Constructor.
       */
      TestTerrainApplication()
         : Application("config.xml"),
           mWalkMode(false),
		   mTreeOn(true),
           mWireframe(false),
           mTimeOfDay(-1.0f)
      {
         SetDataFilePathList("../../data/;./data/;" + GetDeltaDataPathList());

		 weather = new dtABC::Weather();
		 mEnvironment = weather->GetEnvironment();
		 mEnvironment->GetOSGNode()->setName("MyEnvironment");
		 GetScene()->GetSceneNode()->setName("MyScene");
		 GetScene()->AddDrawable(mEnvironment.get());
		 weather->SetTheme(Weather::THEME_FAIR);

         mSOARXTerrain = new SOARXTerrain();
         mSOARXTerrain->LoadConfiguration("data/soarxterrain.xml");
		 mSOARXTerrain->GetOSGNode()->setName("MySOARXTerrain");
		 mEnvironment->AddDrawable(mSOARXTerrain.get());

         GetScene()->GetSceneHandler()->GetSceneView()->setComputeNearFarMode(
            osg::CullSettings::DO_NOT_COMPUTE_NEAR_FAR
         );
		 GetScene()->GetSceneHandler()->GetSceneView()->setCullingMode(
			 osg::CullSettings::ENABLE_ALL_CULLING
		 );
		 GetScene()->GetSceneHandler()->GetSceneView()->setImpostorsActive(FALSE);
		 GetScene()->GetSceneHandler()->GetSceneView()->setSmallFeatureCullingPixelSize(18);

         GetCamera()->GetLens()->setPerspective(
			 60.0f, 60.0f, 1.0f, 50000.0f
         );

         Transform transform(0, 0, 2000.0);

         GetCamera()->SetTransform(&transform);
		 GetScene()->SetNextStatisticsType();
      }

      /**
       * Pre-frame callback.
       *
       * @param deltaFrameTime the amount of time elapsed since the last frame
       */
      virtual void PreFrame(const double deltaFrameTime)
      {
         if(GetKeyboard()->GetKeyState(Producer::Key_minus))
         {
            mSOARXTerrain->SetThreshold(
               clamp((float)(mSOARXTerrain->GetThreshold() - deltaFrameTime*5.0), 1.0f, 10.0f)
            );
         }

         if(GetKeyboard()->GetKeyState(Producer::Key_equal))
         {
            mSOARXTerrain->SetThreshold(
               clamp((float)(mSOARXTerrain->GetThreshold() + deltaFrameTime*5.0), 1.0f, 10.0f)
            );
         }

         if(GetKeyboard()->GetKeyState(Producer::Key_bracketleft))
         {
            mSOARXTerrain->SetDetailMultiplier(
               clamp((float)(mSOARXTerrain->GetDetailMultiplier() - deltaFrameTime*5.0), 1.0f, 20.0f)
            );
         }

         if(GetKeyboard()->GetKeyState(Producer::Key_bracketright))
         {
            mSOARXTerrain->SetDetailMultiplier(
               clamp((float)(mSOARXTerrain->GetDetailMultiplier() + deltaFrameTime*5.0), 1.0f, 20.0f)
            );
         }

         if(GetKeyboard()->GetKeyState(Producer::Key_semicolon))
         {
            int year, month, day, hour, minute, second;

            mEnvironment->GetDateTime(&year, &month, &day, &hour, &minute, &second);

            if(mTimeOfDay < 0.0f)
            {
               mTimeOfDay = hour*100.0f+minute*100.0f/60.0f;
            }

            mTimeOfDay -= (float)(deltaFrameTime*100);

            if(mTimeOfDay < 0.0f)
            {
               mTimeOfDay += 2400.0f;
            }

            hour = (int)mTimeOfDay/100;

            minute = (int)(fmod(mTimeOfDay, 100.0f)*60/100);

            mEnvironment->SetDateTime(
               year, month, day, hour, minute, second
            );
         }

         if(GetKeyboard()->GetKeyState(Producer::Key_apostrophe))
         {
            int year, month, day, hour, minute, second;

            mEnvironment->GetDateTime(&year, &month, &day, &hour, &minute, &second);

            if(mTimeOfDay < 0.0f)
            {
               mTimeOfDay = hour*100.0f+minute*100.0f/60.0f;
            }

            mTimeOfDay += (float)(deltaFrameTime*100);

            if(mTimeOfDay > 2400.0f)
            {
               mTimeOfDay -= 2400.0f;
            }

            hour = (int)mTimeOfDay/100;

            minute = (int)(fmod(mTimeOfDay, 100.0f)*60/100);

            mEnvironment->SetDateTime(
               year, month, day, hour, minute, second
            );
         }

         Transform transform;

         GetCamera()->GetTransform(&transform);

         sgVec3 xyz, hpr;

         transform.Get(xyz, hpr);

         if(GetKeyboard()->GetKeyState(Producer::Key_Left))
         {
            hpr[0] += (float)(deltaFrameTime*90.0);
         }

         if(GetKeyboard()->GetKeyState(Producer::Key_Right))
         {
            hpr[0] -= (float)(deltaFrameTime*90.0);
         }

         float speed = mWalkMode ? 500.0 : 10000.0;

         if(GetKeyboard()->GetKeyState(Producer::Key_Up))
         {
            if(GetKeyboard()->GetKeyState(Producer::Key_Shift_R) ||
               GetKeyboard()->GetKeyState(Producer::Key_Shift_L))
            {
               xyz[2] += (float)(deltaFrameTime*speed/5);
            }
            else
            {
               sgVec3 vec = { 0, deltaFrameTime*speed, 0 };

               sgMat4 mat;

               sgMakeRotMat4(mat, hpr[0], 0, 0);

               sgXformVec3(vec, mat);

               sgAddVec3(xyz, vec);
            }
         }

         if(GetKeyboard()->GetKeyState(Producer::Key_Down))
         {
            if(GetKeyboard()->GetKeyState(Producer::Key_Shift_R) ||
               GetKeyboard()->GetKeyState(Producer::Key_Shift_L))
            {
               xyz[2] -= (float)(deltaFrameTime*speed/5);
            }
            else
            {
               sgVec3 vec = { 0, -deltaFrameTime*speed, 0 };

               sgMat4 mat;

               sgMakeRotMat4(mat, hpr[0], 0, 0);

               sgXformVec3(vec, mat);

               sgAddVec3(xyz, vec);
            }
         }

         if(mWalkMode)
         {
            // Clamp to ground
			osg::Vec2f position = mSOARXTerrain->GetLongLat(xyz[0],xyz[1]);
            xyz[2] = mSOARXTerrain->GetHeight(xyz[0], xyz[1]) + 10.0f;
         }

         transform.Set(xyz, hpr);

         GetCamera()->SetTransform(&transform);
		 sgVec3 fogcolor = {0.0f, 0.0f, 0.0f};
		 mEnvironment->GetModFogColor(fogcolor);
		 glFogfv(GL_FOG_COLOR, fogcolor);  // to make sure the terrain's fog color is matched to the environment
      }

      /**
       * Place an object on the terrain.
       *
       * @param x the x coordinate at which to place the object
       * @param y the y coordinate at which to place the object
       * @param name the name of the object to place
       */

	  void PlaceObject(float x, float y, float deltaz, string name, osg::Group* ObjGroup)
      {
		 osg::ref_ptr<dtCore::Object> obj = new dtCore::Object(name);

		 obj->LoadFile(name,TRUE);

		 dtCore::Transform transform;

		 transform.SetTranslation(x, y, mSOARXTerrain->GetHeight(x, y)+deltaz);

//		 obj->GetOSGNode()->setDataVariance(osg::Object::STATIC);
		 obj->SetTransform(&transform);

		 obj->GetOSGNode()->setName("placed object "+name);
		 ObjGroup->addChild(obj->GetOSGNode());

//		 cout << ObjGroup->getName() << " ";

         mObjects.push_back(obj);
      }

	  void VegePlacement(int LCCtype, char* vegename, string modelname, osg::Group* VegeGroup)
	  {
		  Notify(WARN, "Placing LCCtype %i '%s'....",LCCtype, vegename);

		  const float originx = -16697.8 + 100;
		  const float originy = -64564.9;
//		  const int probabilitylimit = 170;  //  170/256=66%
		  const int probabilitylimit = 128;  //  128/256=50%
		  int vegecount = 0;

		  char myname[20];

		  srand(static_cast<unsigned>(time(NULL)));
		  float random_x, random_y;

		  const int MAX_VEGE_PER_GROUP = 100;
		  const int MAX_VEGE_PER_CELL = 100000;
		  const int LOOKS_PER_PIXEL = 1;
		  const int MAX_VEGE_GROUPS_PER_CELL = 4096;

		  osg::Group* LeafGroup[MAX_VEGE_GROUPS_PER_CELL];
		  for (int vg = 0; vg < MAX_VEGE_GROUPS_PER_CELL; vg++)
		  {
			  sprintf(myname, "LeafGroup[%i]",vg);
			  LeafGroup[vg] = new osg::Group();
			  LeafGroup[vg]->setName(myname);
			  LeafGroup[vg]->setCullingActive(true);
			  LeafGroup[vg]->setDataVariance(osg::Object::STATIC);
		  }

		  vg = 0;

		  for (int y = 0; y < 1024; y++)
		  {
			  for (int my = 0; my < LOOKS_PER_PIXEL; my++)
			  {
				for (int x = 0; x < 1024; x++)
				{
					if (mSOARXTerrain->GetVegetation(LCCtype, x,y,probabilitylimit) && (vegecount < MAX_VEGE_PER_CELL))
//					if (mSOARXTerrain->GetVegetation(LCCtype, x,y,255.0f*(float)rand()/(RAND_MAX+1.0f)) && (vegecount < MAX_VEGE_PER_CELL))
					{
						vg = (int)(x/16)+((y/16)*16);

						for (int mx = 0; mx <LOOKS_PER_PIXEL; mx++)
						{
							random_x = (float)rand()/(RAND_MAX+1.0f)-0.5f;
							random_y = (float)rand()/(RAND_MAX+1.0f)-0.5f;

							PlaceObject(originx+(x+random_x*0.75f)*108.7+108.7/LOOKS_PER_PIXEL*mx,
										originy+(y+random_y*0.75f)*108.7+108.7/LOOKS_PER_PIXEL*my,
										40, modelname, LeafGroup[vg]);
							vegecount++;
						}
					}
				}
			  }
		  }

		  //stats counters
		  int groupcount = 0;
		  int maxchildren = 0;
		  int minchildren = 999;
		  int vegechild = 0;

		  osg::Group* QuadGroup2[4];
		  for (int i = 0; i < 4; i++)
		  {
			  sprintf(myname, "QuadGroup2[%i]",i);
			  QuadGroup2[i] = new osg::Group;
			  QuadGroup2[i]->setName(myname);
			  VegeGroup->addChild(QuadGroup2[i]);
		  }
		  osg::Group* QuadGroup4[16];
		  for (i = 0; i < 16; i++)
		  {
			  sprintf(myname, "QuadGroup4[%i]",i);
			  QuadGroup4[i] = new osg::Group;
			  QuadGroup4[i]->setName(myname);
			  QuadGroup2[i/4]->addChild(QuadGroup4[i]);
		  }
		  osg::Group* QuadGroup8[64];
		  for (i = 0; i < 64; i++)
		  {
			  sprintf(myname, "QuadGroup8[%i]",i);
			  QuadGroup8[i] = new osg::Group;
			  QuadGroup8[i]->setName(myname);
			  QuadGroup4[i/4]->addChild(QuadGroup8[i]);
		  }
		  osg::Group* QuadGroup16[256];
		  for (i = 0; i < 256; i++)
		  {
			  sprintf(myname, "QuadGroup16[%i]",i);
			  QuadGroup16[i] = new osg::Group;
			  QuadGroup16[i]->setName(myname);
			  QuadGroup8[i/4]->addChild(QuadGroup16[i]);
		  }
		  osg::Group* QuadGroup32[1024];
		  for (i = 0; i < 1024; i++)
		  {
			  sprintf(myname, "QuadGroup32[%i]",i);
			  QuadGroup32[i] = new osg::Group;
			  QuadGroup32[i]->setName(myname);
			  QuadGroup16[i/4]->addChild(QuadGroup32[i]);
		  }

//  To be used with Imposter code
//		  // calculate bounding box
//		  osg::BoundingBox bbox;
//		  for (NodeIterator node = nodes.begin(); node != nodes.end(); ++node)
//			  bbox.expandBy((*node)->getBound());

		  //only add in groups that are non-empty
		  for (vg = 0; vg < MAX_VEGE_GROUPS_PER_CELL; vg++)
		  {
			  vegechild = LeafGroup[vg]->getNumChildren();

			  if (vegechild != 0)
				 {
					 if (vegechild > maxchildren)	maxchildren = vegechild;
					 if (vegechild < minchildren)	minchildren = vegechild;

//					 Notify(WARN, "%i, %i", vg, vegechild);
//					 Notify(WARN, "Adding VegeGround #%i:%i to Scene; max = %i, min = %i", groupcount++, vg, maxchildren, minchildren);

//						osg::Impostor * impostor = new osg::Impostor();
//						impostor->setImpostorThreshold(5000.0f);
//						impostor->addChild(LeafGroup[vg]);
//						impostor->setRange(0, 0.0f, 100000.0f);
//						impostor->setCenter(LeafGroup[vg]->getBound().center());
//						QuadGroup32[vg/4]->addChild(impostor);
						QuadGroup32[vg/4]->addChild(LeafGroup[vg]);
				}
			}
				 Notify(WARN, "%s count = %i", vegename, vegecount);
				 Notify(WARN, "mObjects.size() = %i", mObjects.size());
				 Notify(WARN, "Placement Done!");
		}
      /**
       * Post-frame callback.
       *
       * @param deltaFrameTime the amount of time elapsed since the last frame
       */
      virtual void PostFrame(const double deltaFrameTime)
      {
         // Place objects after terrain initialized

         if ((mObjects.size() == 0) && (mSOARXTerrain->GetLCCStatus()))
         {
			 mRootVegeGroup = new osg::Switch();
			 mRootVegeGroup.get()->setName("mRootVegeGroup");
			 mRootVegeGroup->setDataVariance(osg::Object::STATIC);
			 mSOARXTerrain->GetOSGNode()->asGroup()->addChild(mRootVegeGroup.get());

			osg::Group* TreeGroup = new osg::Group();
			TreeGroup->setName("TreeGroup");
			mRootVegeGroup.get()->addChild(TreeGroup);
			VegePlacement(42, "tree", "cone2.3ds", TreeGroup);

			osg::Group* ShrubGroup = new osg::Group();
			ShrubGroup->setName("ShrubGroup");
			mRootVegeGroup.get()->addChild(ShrubGroup);
			VegePlacement(51, "shrub", "yellow.3ds", ShrubGroup);

			osgUtil::Optimizer optimzer;
			optimzer.optimize(mRootVegeGroup.get(),
				osgUtil::Optimizer::SHARE_DUPLICATE_STATE |
				osgUtil::Optimizer::MERGE_GEOMETRY |
				osgUtil::Optimizer::REMOVE_REDUNDANT_NODES |
				osgUtil::Optimizer::FLATTEN_STATIC_TRANSFORMS |
				osgUtil::Optimizer::CHECK_GEOMETRY);
			Notify(WARN, "Optimization Done!");

			mSOARXTerrain->GetOSGNode()->setCullingActive(true);
			mEnvironment->GetOSGNode()->setCullingActive(true);
			ShrubGroup->setCullingActive(true);
			TreeGroup->setCullingActive(true);

//			PrintVisitor pv;
//			mEnvironment->GetOSGNode()->accept(pv);

//			sketchVisitor sv;
//			mEnvironment->GetOSGNode()->accept(sv);

//			ExtentsVisitor ev;
//			mRootVegeGroup->accept(ev);
//			osg::BoundingBox extents = ev.GetBound();

		 }
	  }


      /**
       * KeyboardListener override
       * Called when a key is pressed.
       *
       * @param keyboard the source of the event
       * @param key the key pressed
       * @param character the corresponding character
       */
      virtual void KeyPressed(dtCore::Keyboard* keyboard,
                              Producer::KeyboardKey key,
                              Producer::KeyCharacter character)
      {
         Application::KeyPressed(keyboard, key, character);

         osg::StateSet* ss;
         osg::PolygonMode* pm;

         switch(key)
         {
            case Producer::Key_W:
               mWalkMode = !mWalkMode;
               break;

			case Producer::Key_T:
				mTreeOn = !mTreeOn;
				if (mTreeOn)
				{
					mRootVegeGroup.get()->setAllChildrenOn();
					Notify(WARN, "#childen with culling disabled = %i ", mEnvironment->GetOSGNode()->getNumChildrenWithCullingDisabled());
				}
				else
				{
					mRootVegeGroup.get()->setAllChildrenOff();
					Notify(WARN, "#childen with culling disabled = %i ", mEnvironment->GetOSGNode()->getNumChildrenWithCullingDisabled());
				}
				break;


			case Producer::Key_F1: weather->SetBasicVisibilityType(Weather::VIS_UNLIMITED);    break;
			case Producer::Key_F2: weather->SetBasicVisibilityType(Weather::VIS_FAR);    break;
			case Producer::Key_F3: weather->SetBasicVisibilityType(Weather::VIS_MODERATE); break;
			case Producer::Key_F4: weather->SetBasicVisibilityType(Weather::VIS_LIMITED);  break;

			case Producer::Key_F5: mEnvironment->SetVisibility(50000.0f); break;
			case Producer::Key_F6: mEnvironment->SetVisibility(40000.0f); break;
			case Producer::Key_F7: mEnvironment->SetVisibility(30000.0f); break;
			case Producer::Key_F8: mEnvironment->SetVisibility(20000.0f); break;
			case Producer::Key_F9: mEnvironment->SetVisibility(10000.0f); break;


            case Producer::Key_space:

               mWireframe = !mWireframe;

               ss = mSOARXTerrain->GetOSGNode()->getOrCreateStateSet();

               pm = (osg::PolygonMode*)ss->getAttribute(osg::StateAttribute::POLYGONMODE);

               if(pm == NULL)
               {
                  pm = new osg::PolygonMode;

                  ss->setAttributeAndModes(pm);
               }

               pm->setMode(
                  osg::PolygonMode::FRONT_AND_BACK,
                  mWireframe ? osg::PolygonMode::LINE : osg::PolygonMode::FILL
               );

               break;

            case Producer::Key_Return:
               GetScene()->SetNextStatisticsType();
               break;
         }
      }


   private:

      /**
       * The camera motion model.
       */
      osg::ref_ptr<UFOMotionModel> mCameraMotionModel;

      /**
       * The environment.
       */
      osg::ref_ptr<Environment> mEnvironment;

	  /**
	  * The weather.
	  */
	  dtABC::Weather *weather;

      /**
       * The SOARX terrain object.
       */
      osg::ref_ptr<SOARXTerrain> mSOARXTerrain;

      /**
       * Objects placed in the scene (i.e. trees, shrubs).
       */
	  std::vector< osg::ref_ptr<dtCore::Object> > mObjects;

      /**
       * The navigation mode (walk or magic carpet).
       */
      bool mWalkMode;

	  /**
	  * The vegetation on/off toggle.
	  */
	  bool mTreeOn;

	  /**
	  * The top node of the vegetation scene graph.
	  */
	  osg::ref_ptr<osg::Switch> mRootVegeGroup;

      /**
       * The time of day, in minutes, from 0000 to 2359.
       */
      float mTimeOfDay;

      /**
       * Wireframe flag.
       */
      bool mWireframe;
};

IMPLEMENT_MANAGEMENT_LAYER(TestTerrainApplication)

int main( int argc, char **argv )
{
   SetDataFilePathList("..;../../data/;../../../data/;./data/;" + GetDeltaDataPathList());

   TestTerrainApplication* testTerrainApp =
      new TestTerrainApplication;

   SetNotifyLevel(NOTICE,WARN);

   testTerrainApp->Config();
   testTerrainApp->Run();

   return 0;
}
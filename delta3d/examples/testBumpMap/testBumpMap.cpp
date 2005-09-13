#include "dtCore/dt.h"
#include "dtABC/dtabc.h"
#include "gui_fl/guimgr.h"
#include <dtCore/infiniteterrain.h>

#include <osg/Group>
#include <osg/PositionAttitudeTransform>
#include <osg/Drawable>
#include <osg/Geode>
#include <osg/Geometry>
#include <osg/Image>
#include <osg/Texture2D>
#include <osg/PrimitiveSet>
#include <osg/Program>
#include <osgDB/ReadFile>   
#include <osg/Uniform>

#include <osg/Vec3>
#include <osg/Vec4>

#include "CubeVertices.h"


using namespace dtABC;
using namespace dtCore;

class TestBumpMapApp : public Application
{

	DECLARE_MANAGEMENT_LAYER( TestBumpMapApp )

public:
	TestBumpMapApp( std::string configFilename = "config.xml" )
		: Application( configFilename )
	{

      AddSender(System::Instance());

		CreateGeometry();
		EnableShaders();

      GetCamera()->SetPerspective(90.0f, 70.0f, 1.0f, 5000.0f);

      GetScene()->GetSceneNode()->addChild(mGeode.get());

		Transform xform(0.0f, -300.0f, 0.0f, 0.0f, 0.0f, 0.0f);
		GetCamera()->SetTransform(&xform);

		mMotionModel = new FlyMotionModel( GetKeyboard(), GetMouse() );
		mMotionModel->SetTarget(GetCamera());

	}


	void CreateGeometry()
	{
		mGeode = new osg::Geode;

		mGeometry = new osg::Geometry;

		//set our geometry variables

		//the actual verts
		mGeometry->setVertexArray(mData.mVerts.get());
      mGeometry->setNormalArray(mData.mNormals.get());

		//verts 2 and 3
		mGeometry->setTexCoordArray(0, mData.mVerts2.get());
		mGeometry->setTexCoordArray(1, mData.mVerts3.get());

		//tex coords 1-3
      
		mGeometry->setTexCoordArray(2, mData.mTexCoords.get());
		mGeometry->setTexCoordArray(3, mData.mTexCoords2.get());
		mGeometry->setTexCoordArray(4, mData.mTexCoords3.get());

		//create indices
		RefPtr<osg::IntArray> indices = 
			new osg::IntArray(36);

		for(int i = 0; i < 36; ++i)
		{
		  (*indices)[i] = i;
		}

		mGeometry->setVertexIndices(indices.get());

		mGeometry->setNormalIndices(indices.get());

		mGeometry->setTexCoordIndices(0, indices.get());
		mGeometry->setTexCoordIndices(1, indices.get());
		mGeometry->setTexCoordIndices(2, indices.get());
		mGeometry->setTexCoordIndices(3, indices.get());
		mGeometry->setTexCoordIndices(4, indices.get());

		mGeometry->addPrimitiveSet( new osg::DrawArrays(osg::PrimitiveSet::TRIANGLES, 0, 36) );

		mGeometry->setNormalBinding(osg::Geometry::BIND_PER_VERTEX);

		mGeode->addDrawable(mGeometry.get());
      mGeode->setName("BumpMapCube");

	}

	void EnableShaders()
	{
		//create state set
		osg::StateSet* ss = mGeode->getOrCreateStateSet();
      //ss->setMode(osg::StateAttribute::CULLFACE, GL_FRONT);
		
		//set up textures
		RefPtr<osg::Image> img1 = osgDB::readImageFile( std::string(GetDeltaRootPath()+ "/data/Textures/Tile4.bmp"));
		RefPtr<osg::Image> img2 = osgDB::readImageFile( std::string(GetDeltaRootPath()+ "/data/Textures/Tile4Bump.bmp"));
     

		RefPtr<osg::Texture2D> tex1 = new osg::Texture2D(img1.get());
		RefPtr<osg::Texture2D> tex2 = new osg::Texture2D(img2.get());

		tex1->setWrap(osg::Texture::WRAP_S, osg::Texture::REPEAT);
		tex1->setWrap(osg::Texture::WRAP_T, osg::Texture::REPEAT);

		tex2->setWrap(osg::Texture::WRAP_S, osg::Texture::REPEAT);
		tex2->setWrap(osg::Texture::WRAP_T, osg::Texture::REPEAT);

      ss->setTextureAttributeAndModes(0,tex1.get(),osg::StateAttribute::ON);
      ss->setTextureAttributeAndModes(1,tex2.get(),osg::StateAttribute::ON);

		ss->setTextureMode(0, GL_TEXTURE_2D, GL_TRUE);
		ss->setTextureMode(1, GL_TEXTURE_2D, GL_TRUE);


		//load the shader file
		mProg = new osg::Program;

		RefPtr<osg::Shader> bumpMapVert = new osg::Shader( osg::Shader::VERTEX );
		RefPtr<osg::Shader> bumpMapFrag = new osg::Shader( osg::Shader::FRAGMENT );

		mProg->addShader( bumpMapVert.get() );
		mProg->addShader( bumpMapFrag.get() );

		bool vertLoaded = bumpMapVert->loadShaderSourceFromFile( GetDeltaRootPath()+ "/data/shaders/BumpMap.vert");
	
		bool fragLoaded = bumpMapFrag->loadShaderSourceFromFile( GetDeltaRootPath()+ "/data/shaders/BumpMap.frag");

		mLightPos = new osg::Uniform(osg::Uniform::FLOAT_VEC4, "lightPos");
		mLightPos->set(osg::Vec4(0.0f, 0.0f, 0.0f, 0.0f));
		ss->addUniform(mLightPos.get());

      mEyePos = new osg::Uniform(osg::Uniform::FLOAT_VEC4, "eyePosition");
      mEyePos->set(osg::Vec4(0.0f, 0.0f, 0.0f, 0.0f));
      ss->addUniform(mEyePos.get());


      RefPtr<osg::Uniform> unTex1 = new osg::Uniform(osg::Uniform::SAMPLER_2D, "texMap");
      unTex1->set(0);
      ss->addUniform(unTex1.get());


      RefPtr<osg::Uniform> unTex2 = new osg::Uniform(osg::Uniform::SAMPLER_2D, "normalMap");
      unTex2->set(1);
      ss->addUniform(unTex2.get());

		ss->setAttributeAndModes( mProg.get(), osg::StateAttribute::ON );
	}


   void OnMessage( MessageData *data )
   {
      if (data->message == "preframe")
      {
         double deltaFrameTime = *reinterpret_cast<double*>(data->userData);
         Update(deltaFrameTime);
      }
   }

   void Update( const double deltaFrameTime )
   {
      static float lightRadius = 900.0f;
      static float lightHeight = 0.0f;
      static float radPerSec   = osg::DegreesToRadians(30.0f);
      static float totalTime = 0.0f;

      totalTime += deltaFrameTime;

      osg::StateSet* ss = mGeode->getOrCreateStateSet();

      osg::Vec2 lp;
      
      lp[0] = lightRadius * cosf(radPerSec * totalTime);
      lp[1] = lightRadius * sinf(radPerSec * totalTime);

      osg::Vec3 ep;
      Transform trans;
      GetCamera()->GetTransform(&trans);
      trans.GetTranslation(ep);

      ss->getUniform( "lightPos" )->set( osg::Vec4(lp[0], lp[1], lightHeight, 0.0f));
      ss->getUniform( "eyePosition" )->set(osg::Vec4(ep[0], ep[1], ep[2], 0.0f));

   }

	~TestBumpMapApp()
	{
      GetScene()->GetSceneNode()->removeChild(mGeode.get());
	   RemoveSender( System::Instance() );
	}

private:

	RefPtr<FlyMotionModel>	                  mMotionModel;
   RefPtr<osg::Geometry>                     mGeometry;
	RefPtr<osg::Geode>		                  mGeode;

   RefPtr<osg::Program>                      mProg; 
	RefPtr<osg::Uniform>                      mLightPos; 
   RefPtr<osg::Uniform>                      mEyePos;
	CubeVertices                              mData;


};

IMPLEMENT_MANAGEMENT_LAYER( TestBumpMapApp )

int main(int argc, char* argv[])
{
   
	SetDataFilePathList( GetDeltaRootPath() + "/examples/testShaders/;" +
		                  GetDeltaDataPathList()  );

	RefPtr<TestBumpMapApp> app = new TestBumpMapApp( "config.xml" );
	app->Config();
	app->Run();

   
	return 0;
}

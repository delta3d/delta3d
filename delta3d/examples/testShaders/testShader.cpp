#include "dtCore/dt.h"
#include "dtABC/dtabc.h"
#include "gui_fl/guimgr.h"

#include <osg/Drawable>
#include <osg/Geode>
#include <osg/Geometry>
#include <osg/Image>
#include <osg/Texture2D>
#include <osg/PrimitiveSet>
#include <osg/Program>

#include <osg/Vec3>
#include <osg/Vec4>

#include "CubeVertices.h"

using namespace dtABC;
using namespace dtCore;

class TestShadersApp : public Application
{

	DECLARE_MANAGEMENT_LAYER( TestShadersApp )

public:
	TestShadersApp( std::string configFilename = "config.xml" )
		: Application( configFilename )
	{

		CreateGeometry();
		EnableShaders();

		Transform xform(0.0f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f);
		GetCamera()->SetTransform(&xform);

		mMotionModel = new FlyMotionModel( GetKeyboard(), GetMouse() );
		mMotionModel->SetTarget(GetCamera());
	}


	void CreateGeometry()
	{
		RefPtr<osg::Geode> geode = new osg::Geode;

		RefPtr<osg::Geometry> geom = new osg::Geometry;

		//set our geometry variables

		//the actual verts
		geom->setVertexArray(mData.mVerts.get());

		//verts 2 and 3
		geom->setTexCoordArray(0, mData.mVerts2.get());
		geom->setTexCoordArray(1, mData.mVerts3.get());

		//tex coords 1-3
		geom->setTexCoordArray(2, mData.mTexCoords.get());
		geom->setTexCoordArray(3, mData.mTexCoords2.get());
		geom->setTexCoordArray(4, mData.mTexCoords3.get());

		//create indices
		RefPtr<osg::IntArray> indices = 
			new osg::IntArray(36);

		for(int i = 0; i < 36; ++i)
		{
		  (*indices)[i] = i;
		}


		geom->setVertexIndices(indices.get());

		geom->setNormalIndices(indices.get());

		geom->setTexCoordIndices(0, indices.get());
		geom->setTexCoordIndices(1, indices.get());
		geom->setTexCoordIndices(2, indices.get());
		geom->setTexCoordIndices(3, indices.get());
		geom->setTexCoordIndices(4, indices.get());

		geom->addPrimitiveSet( new osg::DrawArrays(osg::PrimitiveSet::TRIANGLES, 0, 36) );

		geom->setNormalBinding(osg::Geometry::BIND_PER_VERTEX);

		geode->addDrawable(geom.get());

		GetScene()->GetSceneNode()->addChild(geode.get());
	}

	void EnableShaders()
	{
		//create state set
		osg::StateSet* ss = GetScene()->GetSceneNode()->getOrCreateStateSet();
		
		//set up textures
		RefPtr<osg::Image> img1 = new osg::Image();
		RefPtr<osg::Image> img2 = new osg::Image();

		img1->setFileName(std::string(GetDeltaRootPath()+ "/data/shaders/Tile4.bmp"));
		img2->setFileName(std::string(GetDeltaRootPath()+ "/data/shaders/Tile4Bump.bmp"));

		RefPtr<osg::Texture2D> tex1 = new osg::Texture2D(img1.get());
		RefPtr<osg::Texture2D> tex2 = new osg::Texture2D(img2.get());

		tex1->setWrap(osg::Texture::WRAP_S, osg::Texture::REPEAT);
		tex1->setWrap(osg::Texture::WRAP_T, osg::Texture::REPEAT);

		tex2->setWrap(osg::Texture::WRAP_S, osg::Texture::REPEAT);
		tex2->setWrap(osg::Texture::WRAP_T, osg::Texture::REPEAT);

		ss->setTextureAttribute(0, tex1.get());
		ss->setTextureAttribute(1, tex2.get());

		ss->setTextureMode(0, GL_TEXTURE_2D, GL_TRUE);
		ss->setTextureMode(1, GL_TEXTURE_2D, GL_TRUE);


		//load the shader file
		RefPtr<osg::Program> prog = new osg::Program;
		ss->setAttributeAndModes( prog.get(), osg::StateAttribute::ON );

		RefPtr<osg::Program> shaderProgram = new osg::Program;

		RefPtr<osg::Shader> bumpMapVert = new osg::Shader( osg::Shader::VERTEX );
		RefPtr<osg::Shader> bumpMapFrag = new osg::Shader( osg::Shader::FRAGMENT );

		shaderProgram->addShader( bumpMapVert.get() );
		shaderProgram->addShader( bumpMapFrag.get() );

		bumpMapVert->loadShaderSourceFromFile( GetDeltaRootPath()+ "/data/shaders/BumpMap.vert");
	
		bumpMapFrag->loadShaderSourceFromFile( GetDeltaRootPath()+ "/data/shaders/BumpMap.frag");

		mLightPos = new osg::Uniform(osg::Uniform::FLOAT_VEC3, "lightPos");
		mLightPos->set(osg::Vec3(0.0f, 0.0f, 0.0f));
		ss->addUniform(mLightPos.get());

		ss->setAttributeAndModes( prog.get(), osg::StateAttribute::ON );
	}


	~TestShadersApp()
	{
	
	}

protected:
	void KeyPressed(dtCore::Keyboard* keyboard, Producer::KeyboardKey key, Producer::KeyCharacter character)
	{
		/*switch(key) 
		{

			default:

				break;
		}*/
	}

private:

	RefPtr<FlyMotionModel>	mMotionModel;
	RefPtr<osg::Geode>		mGeometry;

	RefPtr<osg::Uniform> mLightPos; 
	CubeVertices  mData;


};

IMPLEMENT_MANAGEMENT_LAYER( TestShadersApp )

int main(int argc, char* argv[])
{
	SetDataFilePathList( GetDeltaRootPath() + "/examples/testShaders/;" +
		GetDeltaDataPathList()  );

	RefPtr<TestShadersApp> app = new TestShadersApp( "config.xml" );
	app->Config();
	app->Run();

	return 0;
}

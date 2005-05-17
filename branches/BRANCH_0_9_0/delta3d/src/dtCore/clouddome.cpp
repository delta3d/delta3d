
#include "dtCore/clouddome.h"
#include "dtCore/system.h"
#include "dtCore/notify.h"
#include "dtCore/scene.h"

#include <osg/Vec3>
#include <osg/BlendFunc>

#include <osgDB/ReadFile>
#include <osgDB/FileUtils>

using namespace dtCore;

IMPLEMENT_MANAGEMENT_LAYER(CloudDome)

// Constructor without volume texture file
CloudDome::CloudDome( int   octaves,
                      int   frequency,
                      float amp,
                      float persistence,
                      float cutoff,
                      float exponent,
                      float radius,
                      int   segments)
   : EnvEffect("CloudDome"), 
     mDome(0),
     mImage_3D(0),
     mTex3D(0),
     mFileName(""),
     mFogColor(0),
     mOctaves(octaves),
     mFrequency(frequency),
     mPersistence(persistence),
     mAmplitude(amp),
     mXform(0),
     ctime(0),
     mEnable(true),
     shaders_enabled(true),
     mRadius(radius),
     mSegments(segments),
     mWidth(0),
     mHeight(0),
     mSlices(0),
     Cloud_Prog(0),
     Cloud_Vert(0),
     Cloud_Frag(0),
     mScale(1/mRadius*1000),
     mExponent(exponent),
     mCutoff(cutoff),
     mSpeedX(0.1f),
     mSpeedY(0.05f),
     mBias(1.0f),
     mCloudColor(new osg::Vec3(1.0f, 1.0f, 1.0f)),
     mOffset(new osg::Vec3(.01f, .01f, 0.f))
{
    RegisterInstance(this);
    mNode = new osg::Group();
    mNode->setName( this->GetName() );
    Create();
    AddSender(System::GetSystem());
}

// Constructor with filename of volume texture
CloudDome::CloudDome( float radius,
                      int   segments,
                      std::string filename )
   : EnvEffect("CloudDome"),
     mDome(0),
     mImage_3D(0),
     mTex3D(0),
     mFileName(filename),
     mFogColor(0),
     mOctaves(6),
     mFrequency(2),
     mPersistence(.5),
     mAmplitude(.7),
     mXform(0),
     ctime(0),
     mEnable(true),
     shaders_enabled(true),
     mRadius(radius),
     mSegments(segments),
     mWidth(0),
     mHeight(0),
     mSlices(0),
     Cloud_Prog(0),
     Cloud_Vert(0),
     Cloud_Frag(0),
     mScale(1/mRadius*1000),
     mExponent(5.f),
     mCutoff(.5f),
     mSpeedX(0.1f),
     mSpeedY(0.05f),
     mBias(1.0f),
     mCloudColor(new osg::Vec3(1.0f, 1.0f, 1.0f)),
     mOffset(new osg::Vec3(.01f, .01f, 0.f))
{
    RegisterInstance(this);

    mNode = new osg::Group();
    mNode->setName( this->GetName() );

    Create();
    AddSender(System::GetSystem());
}

CloudDome::~CloudDome()
{
    DeregisterInstance(this);
    Notify(DEBUG_INFO, "CloudDome: deleting %s", this->GetName().c_str());
    RemoveSender(System::GetSystem());
}

void CloudDome::loadShaderSource( osg::Shader* obj, std::string fileName)
{
    std::string fqFileName = osgDB::findDataFile(fileName);
    if( fqFileName.length() != 0 )
    {
       obj->loadShaderSourceFromFile( fqFileName.c_str() );
    }
    else
    {
		   Notify(WARN,  "File '%s' not found.", fileName.c_str() );
    }
}


osg::Geode *CloudDome::createDome( float radius, int segs)
{
    int i, j;
    float lev[] = { 1.0, 5.0, 10.0, 15.0, 30.0, 45.0 , 60.0, 75, 90.0  };
    float cc[][4] =
    {
        { 1.0, 0.0, 0.0,  0.0 },  // has bright colors for debugging when 
        { 0.0, 1.0, 0.0,  0.3 },  // shaders are disabled
        { 0.0, 0.0, 1.0,  0.7 },
        { 1.0, 0.0, 0.0,  1.0 },
        { 0.0, 1.0, 0.0,  1.0 },
        { 0.0, 0.0, 1.0,  1.0 },
        { 1.0, 0.0, 0.0,  1.0 },
        { 0.0, 1.0, 0.0,  1.0 },
        { 0.0, 0.0, 1.0,  1.0 }
    };
	
	--segs; // We want segments to span from  0 to segs-1

    float x, y, z;
    float alpha, theta;
    int nlev = sizeof( lev )/sizeof(float);

    osg::Geometry *geom = new osg::Geometry;

    osg::Vec3Array& coords = *(new osg::Vec3Array(segs*nlev));
    osg::Vec4Array& colors = *(new osg::Vec4Array(segs*nlev));
    osg::Vec2Array& tcoords = *(new osg::Vec2Array(segs*nlev));


    int ci = 0;

    for( i = 0; i < nlev; ++i )
    {
        for( j = 0; j < segs; ++j, ++ci)
        {
            alpha = osg::DegreesToRadians(lev[i]);
            theta = osg::DegreesToRadians((float)(j*360/(segs-1)));

            x = radius * cosf( alpha ) * cosf( theta );
            y = radius * cosf( alpha ) * -sinf( theta );
            z = radius * sinf( alpha );

            coords[ci][0] = x;
            coords[ci][1] = y;
            coords[ci][2] = z;

            colors[ci][0] = cc[i][0];
            colors[ci][1] = cc[i][1];
            colors[ci][2] = cc[i][2];
            colors[ci][3] = cc[i][3];

            tcoords[ci][0] = (float)j/(segs);
            tcoords[ci][1] = (float)i/(float)(nlev-1);

        }
    }

    for( i = 0; i < nlev-1; ++i )
    {
        osg::DrawElementsUShort* drawElements = new osg::DrawElementsUShort(osg::PrimitiveSet::TRIANGLE_STRIP);

        for( j = 0; j < segs; ++j )
        {
            drawElements->push_back((i+1)*segs+j);
            drawElements->push_back((i+0)*segs+j);
        }

        geom->addPrimitiveSet(drawElements);
    }

    geom->setVertexArray( &coords );
    geom->setTexCoordArray( 0, &tcoords );

    geom->setColorArray( &colors );
    geom->setColorBinding( osg::Geometry::BIND_PER_VERTEX );

    osg::Geode *geode = new osg::Geode;
    geode = new osg::Geode;
    geode ->addDrawable( geom );
    geode->setName("CloudDome");

    return geode;
}

void CloudDome::Create()
{
    mXform = new MoveEarthySkyWithEyePointTransform();

    mDome = createDome(mRadius, mSegments);
    osg::StateSet *domeStateSet = mDome->getOrCreateStateSet();

    if (mFileName != "")
	{
		mImage_3D = osgDB::readImageFile(mFileName);
		if (!mImage_3D.valid())
		{
			dtCore::Notify(WARN, "CloudDome can't load image file."  );
			dtCore::Notify(WARN, "Creating new 128^3 3d texture...");

			NoiseGenerator noise3d(mOctaves, mFrequency, mAmplitude, mPersistence, 128, 128, 128);
			mImage_3D = noise3d.makeNoiseTexture(GL_ALPHA);
		}
	}
	else
	{
		dtCore::Notify(ALWAYS, "Creating 128^3 3d texture...");	
		NoiseGenerator noise3d(mOctaves, mFrequency, mAmplitude, mPersistence, 128, 128, 128);
		mImage_3D = noise3d.makeNoiseTexture(GL_ALPHA);
	}

    mTex3D = new osg::Texture3D;
    mTex3D->setImage(mImage_3D.get()); 

    mTex3D->setFilter(osg::Texture3D::MIN_FILTER, osg::Texture3D::LINEAR);
    mTex3D->setFilter(osg::Texture3D::MAG_FILTER, osg::Texture3D::LINEAR);
    mTex3D->setWrap(osg::Texture3D::WRAP_S, osg::Texture3D::REPEAT);
    mTex3D->setWrap(osg::Texture3D::WRAP_T, osg::Texture3D::REPEAT);
    mTex3D->setWrap(osg::Texture3D::WRAP_R, osg::Texture3D::REPEAT);

    domeStateSet->setTextureAttributeAndModes(0, mTex3D.get(), osg::StateAttribute::ON);

    // Blending Function
    // This operates on the fragments AFTER the frag-shader to get alpha blending
    // with the skydome background
    osg::BlendFunc *trans = new osg::BlendFunc();
    trans->setFunction(osg::BlendFunc::SRC_ALPHA ,osg::BlendFunc::ONE_MINUS_SRC_ALPHA);
    domeStateSet->setAttributeAndModes(trans);
    
    osg::Program* progObj = new osg::Program;
    _progList.push_back( progObj );
    domeStateSet->setAttributeAndModes(progObj, osg::StateAttribute::ON);

    // Set the program objects
    Cloud_Prog = new osg::Program;
    _progList.push_back( Cloud_Prog );
    Cloud_Vert = new osg::Shader( osg::Shader::VERTEX );
    Cloud_Frag = new osg::Shader( osg::Shader::FRAGMENT );
    Cloud_Prog->addShader( Cloud_Frag );
    Cloud_Prog->addShader( Cloud_Vert );
  
    loadShaderSource( Cloud_Vert, "cloud1.vert" );
    loadShaderSource( Cloud_Frag, "cloud1.frag" );

    osg::Uniform* uniform; 

    uniform = new osg::Uniform( "Scale", mScale / 1000 );
    domeStateSet->addUniform( uniform );   

    uniform = new osg::Uniform( "Cutoff", mCutoff );
    domeStateSet->addUniform( uniform ); 

    uniform = new osg::Uniform( "Exponent", mExponent );
    domeStateSet->addUniform( uniform ); 

    uniform = new osg::Uniform( "Bias", mBias );
    domeStateSet->addUniform( uniform ); 

    //vec3
    uniform = new osg::Uniform( "Offset", *mOffset);
    domeStateSet->addUniform( uniform ); 

    uniform= new osg::Uniform( "CloudColor", *mCloudColor );
    domeStateSet->addUniform( uniform ); 

    //sampler_3d
    uniform = new osg::Uniform( osg::Uniform::SAMPLER_3D, "Noise" );
    uniform->set( 0 );  // TODO: Change texture files at runtime
    domeStateSet->addUniform( uniform );

    // TODO: Add the fog uniform parameter

    domeStateSet->setAttributeAndModes( Cloud_Prog, osg::StateAttribute::ON);

    mXform->addChild(mDome.get());
    dynamic_cast<osg::Group*>(mNode.get())->addChild(mXform.get());
}

void CloudDome::Repaint(sgVec4 sky_color, sgVec4 fog_color, 
                        double sun_angle, double sunAzimuth,
                        double vis)
{

}

void CloudDome::OnMessage(MessageData *data)
{
    if (data->message == "preframe")
    {
        double *deltaFrameTime = (double*)data->userData;
        Update(*deltaFrameTime);
    }
}

void CloudDome::Update(const double deltaFrameTime)
{
    ctime += deltaFrameTime;
    if(ctime > INT_MAX)  
        ctime = 0;        

    mOffset->set( ctime * mSpeedX/10, ctime * mSpeedY/20, 0);

    osg::StateSet* domeStateSet = mDome->getOrCreateStateSet();

    if(mEnable)
    {
        if(!shaders_enabled)
        {
            for( unsigned int i = 0; i < _progList.size(); i++ )
            {
                //_progList[i]->enable(true);
               domeStateSet->setAttributeAndModes(_progList[i], osg::StateAttribute::ON);
            }
            shaders_enabled = true;
        }

        // Set Uniform variables
        domeStateSet->getUniform( "Offset" )->set( *mOffset );
        domeStateSet->getUniform( "Scale" )->set( mScale/1000 );
        domeStateSet->getUniform( "Exponent" )->set( mExponent/1000 );
        domeStateSet->getUniform( "Cutoff" )->set( mCutoff );
        domeStateSet->getUniform( "Bias" )->set( mBias );
        domeStateSet->getUniform( "CloudColor" )->set( *mCloudColor );

    }
    else
    {
        for( unsigned int i = 0; i < _progList.size(); i++ )
        {
            //_progList[i]->enable(false);
            domeStateSet->setAttributeAndModes(_progList[i], osg::StateAttribute::OFF);
        }
        shaders_enabled = false;
    }
}

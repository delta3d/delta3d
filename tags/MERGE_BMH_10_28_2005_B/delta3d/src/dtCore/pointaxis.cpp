#include <cassert>

#include <osg/Geode>
#include <osg/Geometry>
#include <osg/Switch>
#include <osg/MatrixTransform>
#include <osgText/Text>

#include "dtCore/pointaxis.h"
#include "dtCore/scene.h"


using namespace   dtCore;
IMPLEMENT_MANAGEMENT_LAYER(PointAxis)



/// static member variables
const float                PointAxis::DEF_AXIS_SIZE(1.0f);
const float                PointAxis::DEF_CHAR_SIZE(0.3f);
const PointAxis::AXISCOLOR PointAxis::DEF_COLOR[NUMAXES]  =
                           {
                              RED,
                              GREEN,
                              BLUE
                           };

const char*                PointAxis::DEF_LABEL_XYZ[NUMAXES]  =
                           {
                              "X",
                              "Y",
                              "Z"
                           };

const char*                PointAxis::DEF_LABEL_HPR[NUMAXES]  =
                           {
                              "P",
                              "R",
                              "H"
                           };

const osg::Vec4            PointAxis::COLOR[BASECOLORS] =
                           {
                              osg::Vec4(0.0f, 0.0f, 0.0f, 1.0f),
                              osg::Vec4(1.0f, 1.0f, 1.0f, 1.0f),
                              osg::Vec4(1.0f, 0.0f, 0.0f, 1.0f),
                              osg::Vec4(1.0f, 1.0f, 0.0f, 1.0f),
                              osg::Vec4(0.0f, 1.0f, 0.0f, 1.0f),
                              osg::Vec4(0.0f, 1.0f, 1.0f, 1.0f),
                              osg::Vec4(0.0f, 0.0f, 1.0f, 1.0f),
                              osg::Vec4(1.0f, 0.0f, 1.0f, 1.0f),
                           };



/**
 * default constructor
 */
PointAxis::PointAxis()
:  Transformable(),
   mMainSwitch(NULL),
   mLabelSwitch(NULL)
{
   RegisterInstance( this );

   memset( mPoint, 0L, sizeof(mPoint) );
   for( int ii(0L); ii < NUMAXES; ii++ )
   {
      mLabel[ii]        = DEF_LABEL_XYZ[ii];
      mCLabel[ii]       = DEF_LABEL_XYZ[ii];
      mColorID[ii]      = DEF_COLOR[ii];
      mLColorID[ii]     = DEF_COLOR[ii];
      mColor[ii]        = COLOR[DEF_COLOR[ii]];
      mLColor[ii]       = COLOR[DEF_COLOR[ii]];
      mPoint[ii][ii]    = DEF_AXIS_SIZE;
   }

   ctor();
   SetName( "PointAxis" );

   // Default collision category = 8
   SetCollisionCategoryBits( UNSIGNED_BIT(8) );
}



/**
 * destructor
 */
PointAxis::~PointAxis()
{
   DeregisterInstance( this );
}



/**
 * enable rendering axes, labels and label types
 *
 * @param f : flags of the items to enable
 */
void
PointAxis::Enable( int f )
{
   if( f & AXES )
   {
      mMainSwitch->setValue( 1L, true );
   }

   if( f & LABELS )
   {
      mMainSwitch->setValue( 0L, true );
   }

   if( f & LABEL_X )
   {
      mLabelSwitch->setValue( X, true );
   }

   if( f & LABEL_Y )
   {
      mLabelSwitch->setValue( Y, true );
   }

   if( f & LABEL_Z )
   {
      mLabelSwitch->setValue( Z, true );
   }

   if( f & XYZ_LABELS )
   {
      osg::Geode* g(NULL);

      g  = static_cast<osg::Geode*>(mLabelSwitch->getChild( X ));
      assert( g );

      mLabel[X]   = DEF_LABEL_XYZ[X];
      LabelSetup( g, mLabel[X].c_str(), mPoint[X], mLColor[X], GetLength( X ) );


      g  = static_cast<osg::Geode*>(mLabelSwitch->getChild( Y ));
      assert( g );

      mLabel[Y]   = DEF_LABEL_XYZ[Y];
      LabelSetup( g, mLabel[Y].c_str(), mPoint[Y], mLColor[Y], GetLength( Y ) );


      g  = static_cast<osg::Geode*>(mLabelSwitch->getChild( Z ));
      assert( g );

      mLabel[Z]   = DEF_LABEL_XYZ[Z];
      LabelSetup( g, mLabel[Z].c_str(), mPoint[Z], mLColor[Z], GetLength( Z ) );
   }

   if( f & HPR_LABELS )
   {
      osg::Geode* g(NULL);

      g  = static_cast<osg::Geode*>(mLabelSwitch->getChild( X ));
      assert( g );

      mLabel[X]   = DEF_LABEL_HPR[X];
      LabelSetup( g, mLabel[X].c_str(), mPoint[X], mLColor[X], GetLength( X ) );


      g  = static_cast<osg::Geode*>(mLabelSwitch->getChild( Y ));
      assert( g );

      mLabel[Y]   = DEF_LABEL_HPR[Y];
      LabelSetup( g, mLabel[Y].c_str(), mPoint[Y], mLColor[Y], GetLength( Y ) );


      g  = static_cast<osg::Geode*>(mLabelSwitch->getChild( Z ));
      assert( g );

      mLabel[Z]   = DEF_LABEL_HPR[Z];
      LabelSetup( g, mLabel[Z].c_str(), mPoint[Z], mLColor[Z], GetLength( Z ) );
   }

   if( f & CUSTOM_LABELS )
   {
      osg::Geode* g(NULL);

      g  = static_cast<osg::Geode*>(mLabelSwitch->getChild( X ));
      assert( g );

      mLabel[X]   = mCLabel[X];
      LabelSetup( g, mLabel[X].c_str(), mPoint[X], mLColor[X], GetLength( X ) );


      g  = static_cast<osg::Geode*>(mLabelSwitch->getChild( Y ));
      assert( g );

      mLabel[Y]   = mCLabel[Y];
      LabelSetup( g, mLabel[Y].c_str(), mPoint[Y], mLColor[Y], GetLength( Y ) );


      g  = static_cast<osg::Geode*>(mLabelSwitch->getChild( Z ));
      assert( g );

      mLabel[Z]   = mCLabel[Z];
      LabelSetup( g, mLabel[Z].c_str(), mPoint[Z], mLColor[Z], GetLength( Z ) );
   }
}



/**
 * disable rendering axes, labels and label types
 *
 * @param f : flags of the items to disable
 */
void
PointAxis::Disable( int f )
{
   if( f & AXES )
   {
      mMainSwitch->setValue( 1L, false );
   }

   if( f & LABELS )
   {
      mMainSwitch->setValue( 0L, false );
   }

   if( f & LABEL_X )
   {
      mLabelSwitch->setValue( X, false );
   }

   if( f & LABEL_Y )
   {
      mLabelSwitch->setValue( Y, false );
   }

   if( f & LABEL_Z )
   {
      mLabelSwitch->setValue( Z, false );
   }
}



/**
 * get the label of a specific axis
 *
 * @param a : id of the axis
 *
 * @return  pointer to the label
 */
const char*
PointAxis::GetLabel( AXIS a )   const
{
   if( a >= NUMAXES )
      return   NULL;

   return   mLabel[a].c_str();
}



/**
 * set a custom label for a specific axis
 *
 * @param a : id of the axis
 * @param l : pointer to the label
 */
void
PointAxis::SetLabel( AXIS a, const char* l )
{
   if( a >= NUMAXES )
      return;

   mCLabel[a]   = l;
   Enable( CUSTOM_LABELS );
}



/**
 * get the length of a specific axis
 *
 * @param a : id of the axis
 *
 * @return  the axis length
 */
float
PointAxis::GetLength( AXIS a )  const
{
   if( a >= NUMAXES )
      return   0.0f;

   return   mPoint[a][a];
}



/**
 * set the length of a specific axis
 *
 * @param a : id of the axis
 * @param l : the new length
 */
void
PointAxis::SetLength( AXIS a, float l )
{
   if( a >= NUMAXES )
      return;

   mPoint[a][a]   = l;
   AxesSetup();

   osg::Geode* g  = static_cast<osg::Geode*>(mLabelSwitch->getChild( a ));
   assert( g );

   LabelSetup( g, mLabel[a].c_str(), mPoint[a], mLColor[a], l );
}



/**
 * get the color-id of a specific axis
 *
 * @param a : id of the axis
 *
 * @return  the color-id
 */
PointAxis::AXISCOLOR
PointAxis::GetColor( AXIS a )   const
{
   if( a >= NUMAXES )
      return   NUMAXISCOLORS;

   return   mColorID[a];
}



/**
 * set the color of a specific axis by
 * using a predefined color id
 *
 * @param a : id of the axis
 * @param c : color-id to use
 */
void
PointAxis::SetColor( AXIS a, AXISCOLOR c )
{
   if( ( a >= NUMAXES ) || ( int(c) >= int(BASECOLORS) ) )
      return;

   mColorID[a] = c;
   mColor[a]   = COLOR[mColorID[a]];

   AxesSetup();
}



/**
 * get the color value of a specific axis
 *
 * @param a : id of the axis
 * @param c : the color value array
 */
void
PointAxis::GetColor( AXIS a, osg::Vec4& c )  const
{
   if( a >= NUMAXES )
      return;

   c  = mColor[a];
}



/**
 * set the color of a specific axis by
 * using a custom color value
 *
 * @param a : id of the axis
 * @param c : color value array
 */
void
PointAxis::SetColor( AXIS a, const osg::Vec4 c )
{
   if( a >= NUMAXES )
      return;

   mColorID[a] = AXISCOLOR(a + BASECOLORS);
   mColor[a]   = c;

   AxesSetup();
}



/**
 * get the color-id of a specific label
 *
 * @param a : id of the axis
 *
 * @return  the color-id
 */
PointAxis::AXISCOLOR
PointAxis::GetLabelColor( AXIS a )   const
{
   if( a >= NUMAXES )
      return   NUMAXISCOLORS;

   return   mLColorID[a];
}



/**
 * set the color of a specific label by
 * using a predefined color id
 *
 * @param a : id of the axis
 * @param c : color-id to use
 */
void
PointAxis::SetLabelColor( AXIS a, AXISCOLOR c )
{
   if( ( a >= NUMAXES ) || ( int(c) >= int(BASECOLORS) ) )
      return;

   mLColorID[a] = c;
   mLColor[a]   = COLOR[mLColorID[a]];

   osg::Geode* g  = static_cast<osg::Geode*>(mLabelSwitch->getChild( a ));
   assert( g );

   LabelSetup( g, mLabel[a].c_str(), mPoint[a], mLColor[a], GetLength( a ) );
}



/**
 * get the color value of a specific label
 *
 * @param a : id of the axis
 * @param c : the color value array
 */
void
PointAxis::GetLabelColor( AXIS a, osg::Vec4& c )   const
{
   if( a >= NUMAXES )
      return;

   c  = mLColor[a];
}



/**
 * set the color of a specific label by
 * using a custom color value
 *
 * @param a : id of the axis
 * @param c : color value array
 */
void
PointAxis::SetLabelColor( AXIS a, const osg::Vec4 c )
{
   if( a >= NUMAXES )
      return;

   mLColorID[a] = AXISCOLOR(a + BASECOLORS);
   mLColor[a]   = c;

   osg::Geode* g  = static_cast<osg::Geode*>(mLabelSwitch->getChild( a ));
   assert( g );

   LabelSetup( g, mLabel[a].c_str(), mPoint[a], mLColor[a], GetLength( a ) );
}



/// private member functions
/**
 * create the geometry of the PointAxis
 */
void
PointAxis::ctor( void )
{
   osg::Geode*    geode(NULL);
   osg::StateSet* ss(NULL);

   mMainSwitch = new osg::Switch;
   assert( mMainSwitch.get() );

   GetMatrixNode()->addChild( mMainSwitch.get() );


   mLabelSwitch = new osg::Switch;
   assert( mLabelSwitch.get() );

   mMainSwitch->addChild( mLabelSwitch.get(), true );


   // create the axes
   AxesSetup();

   // create the X-label
   geode = new osg::Geode;
   assert( geode );

   ss = geode->getOrCreateStateSet();
   assert( ss );

   ss->setRenderBinDetails( 20, "RenderBin" );
   ss->setMode( GL_LIGHTING, osg::StateAttribute::OFF );
   ss->setMode( GL_DEPTH_TEST, osg::StateAttribute::OFF );

   LabelSetup( geode, mLabel[X].c_str(), mPoint[X], mLColor[X], GetLength( X ) );

   mLabelSwitch->addChild( geode, false );


   // create the Y-label
   geode = new osg::Geode;
   assert( geode );

   ss = geode->getOrCreateStateSet();
   assert( ss );

   ss->setRenderBinDetails( 20, "RenderBin" );
   ss->setMode( GL_LIGHTING, osg::StateAttribute::OFF );
   ss->setMode( GL_DEPTH_TEST, osg::StateAttribute::OFF );

   LabelSetup( geode, mLabel[Y].c_str(), mPoint[Y], mLColor[Y], GetLength( Y ) );

   mLabelSwitch->addChild( geode, false );


   // create the Z-label
   geode = new osg::Geode;
   assert( geode );

   ss = geode->getOrCreateStateSet();
   assert( ss );

   ss->setRenderBinDetails( 20, "RenderBin" );
   ss->setMode( GL_LIGHTING, osg::StateAttribute::OFF );
   ss->setMode( GL_DEPTH_TEST, osg::StateAttribute::OFF );

   LabelSetup( geode, mLabel[Z].c_str(), mPoint[Z], mLColor[Z], GetLength( Z ) );

   mLabelSwitch->addChild( geode, false );
}



/**
 * build the axes using data stored in the member functions
 */
void
PointAxis::AxesSetup( void )
{
   assert( mMainSwitch.get() );

   osg::Geode*    geode(NULL);
   osg::Geometry* geome(NULL);

   if( mMainSwitch->getNumChildren() < 2L )
   {
      geode = new osg::Geode;
      assert( geode );

      geome = new osg::Geometry;
      assert( geome );

      geode->addDrawable( geome );
      mMainSwitch->addChild( geode, true );

      osg::StateSet* ss = geode->getOrCreateStateSet();
      assert( ss );

      ss->setRenderBinDetails( 20L, "RenderBin" );
      ss->setMode( GL_LIGHTING, osg::StateAttribute::OFF );
      ss->setMode( GL_DEPTH_TEST, osg::StateAttribute::OFF );
   }
   else
   {
      geode = static_cast<osg::Geode*>(mMainSwitch->getChild( 1L ));
      assert( geode );

      geome = static_cast<osg::Geometry*>(geode->getDrawable( 0L ));
      assert( geome );
   }


   osg::Vec3   vertices[6] =
               {
                  mPoint[NUMAXES],  // origin
                  mPoint[X],        // X-point
                  mPoint[NUMAXES],  // origin
                  mPoint[Y],        // Y-point
                  mPoint[NUMAXES],  // origin
                  mPoint[Z],        // Y-point
               };


   geome->setVertexArray( new osg::Vec3Array(6, vertices) );
   geome->setColorArray( new osg::Vec4Array(3, mColor) );
   geome->setColorBinding( osg::Geometry::BIND_PER_PRIMITIVE );
   geome->addPrimitiveSet( new osg::DrawArrays(osg::PrimitiveSet::LINES, 0, 6) );
}



/**
 * build the label for a single axis
 *
 * @param g : geode to attach geometry to
 * @param l : label text pointer
 * @param p : position of the label
 * @param c : color of the label
 * @param s : size of the text-character
 */
void
PointAxis::LabelSetup( osg::Geode* g, const char* l, osg::Vec3 p, osg::Vec4 c, float s /*= 1.0f*/ )
{
   assert( g );

   osgText::Text* txt(NULL);

   if( g->getNumDrawables() )
   {
      txt   = static_cast<osgText::Text*>(g->getDrawable( 0L ));
      assert( txt );
   }
   else
   {
      txt   = new osgText::Text;
      assert( txt );

      g->addDrawable( txt );
   }

   txt->setText( l );
   txt->setCharacterSize( s * DEF_CHAR_SIZE );
   txt->setAutoRotateToScreen( true );
   txt->setColor( c );
   txt->setPosition( p );
}


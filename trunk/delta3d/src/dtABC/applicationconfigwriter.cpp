#include <dtABC/applicationconfigwriter.h>
#include <dtABC/applicationconfigschema.h>
#include <dtABC/applicationconfigdata.h>

#include <dtABC/application.h>
#include <dtCore/keyboard.h>
#include <dtCore/mouse.h>
#include <dtCore/camera.h>
#include <dtCore/scene.h>
#include <dtCore/generickeyboardlistener.h>

#include <dtUtil/stringutils.h>
#include <dtUtil/xerceswriter.h>

#include <xercesc/dom/DOMDocument.hpp>
#include <xercesc/dom/DOMElement.hpp>
#include <xercesc/util/XMLString.hpp>

using namespace dtABC;
XERCES_CPP_NAMESPACE_USE

// --- App config code's implementation --- //
void ApplicationConfigWriter::operator ()(const std::string& filename)
{
   // initialize the xerces xml system.
   dtCore::RefPtr<dtUtil::XercesWriter> writer = new dtUtil::XercesWriter();

   // create instances of the models, using the xerces system,
   // so it needs to be initilized with the writer first.
   DefaultModel def;
   SchemaModel sch;

   // specify the name of the top node.
   writer->CreateDocument( "Application" );
   XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument* doc = writer->GetDocument();
   DOMElement* app = doc->getDocumentElement();

   DOMElement* windo = doc->createElement(sch.WINDOW);
   windo->setAttribute( sch.NAME, def.WINDOW_NAME );
   windo->setAttribute( sch.X , def.WINDOW_X );
   windo->setAttribute( sch.Y , def.WINDOW_Y );
   windo->setAttribute( sch.WIDTH , def.WINDOW_WIDTH );
   windo->setAttribute( sch.HEIGHT , def.WINDOW_HEIGHT );
   windo->setAttribute( sch.PIXELDEPTH , def.PIXEL_DEPTH );
   windo->setAttribute( sch.REFRESHRATE, def.REFRESH );
   windo->setAttribute( sch.SHOWCURSOR, def.SHOW_CURSOR );
   windo->setAttribute( sch.FULLSCREEN , def.FULL_SCREEN );
   windo->setAttribute( sch.CHANGEDISPLAYRESOLUTION, def.CHANGE_RESOLUTION );
   app->appendChild( windo );

   DOMElement* scene = doc->createElement(sch.SCENE);
   scene->setAttribute( sch.NAME , def.SCENE_NAME );
   app->appendChild( scene );

   DOMElement* camera = doc->createElement(sch.CAMERA);
   camera->setAttribute( sch.NAME , def.CAMERA_NAME );
   camera->setAttribute( sch.WINDOW , def.WINDOW_NAME );
   camera->setAttribute( sch.SCENE , def.SCENE_NAME );
   app->appendChild( camera );

   writer->WriteFile( filename );
}

ApplicationConfigWriter::SchemaModel::SchemaModel()
{
   WINDOW = XMLString::transcode( ApplicationConfigSchema::WINDOW.c_str() );
   NAME = XMLString::transcode( ApplicationConfigSchema::NAME.c_str() );
   SCENE = XMLString::transcode( ApplicationConfigSchema::SCENE.c_str() );
   CAMERA = XMLString::transcode( ApplicationConfigSchema::CAMERA.c_str() );

   X = XMLString::transcode( ApplicationConfigSchema::X.c_str() );
   Y = XMLString::transcode( ApplicationConfigSchema::Y.c_str() );
   WIDTH = XMLString::transcode( ApplicationConfigSchema::WIDTH.c_str() );
   HEIGHT = XMLString::transcode( ApplicationConfigSchema::HEIGHT.c_str() );

   PIXELDEPTH = XMLString::transcode( ApplicationConfigSchema::PIXELDEPTH.c_str() );
   REFRESHRATE = XMLString::transcode( ApplicationConfigSchema::REFRESHRATE.c_str() );
   SHOWCURSOR = XMLString::transcode( ApplicationConfigSchema::SHOWCURSOR.c_str() );
   FULLSCREEN = XMLString::transcode( ApplicationConfigSchema::FULLSCREEN.c_str() );
   CHANGEDISPLAYRESOLUTION = XMLString::transcode( ApplicationConfigSchema::CHANGEDISPLAYRESOLUTION.c_str() );

   WINDOWINSTANCE = XMLString::transcode( ApplicationConfigSchema::WINDOWINSTANCE.c_str() );
   SCENEINSTANCE = XMLString::transcode( ApplicationConfigSchema::SCENEINSTANCE.c_str() );
}

ApplicationConfigWriter::SchemaModel::~SchemaModel()
{
   XMLString::release( &WINDOW );
   XMLString::release( &NAME );
   XMLString::release( &SCENE );
   XMLString::release( &CAMERA );

   XMLString::release( &X );
   XMLString::release( &Y );
   XMLString::release( &WIDTH );
   XMLString::release( &HEIGHT );

   XMLString::release( &PIXELDEPTH );
   XMLString::release( &REFRESHRATE );
   XMLString::release( &SHOWCURSOR );
   XMLString::release( &FULLSCREEN );
   XMLString::release( &CHANGEDISPLAYRESOLUTION );

   XMLString::release( &WINDOWINSTANCE );
   XMLString::release( &SCENEINSTANCE );
}

ApplicationConfigWriter::DefaultModel::DefaultModel()
{
   ApplicationConfigData data = dtABC::Application::GetDefaultConfigData();
   WINDOW_X = XMLString::transcode( dtUtil::ToString(data.WINDOW_X).c_str() );
   WINDOW_Y = XMLString::transcode( dtUtil::ToString(data.WINDOW_Y).c_str() );
   WINDOW_WIDTH = XMLString::transcode( dtUtil::ToString(data.RESOLUTION.width).c_str() );
   WINDOW_HEIGHT = XMLString::transcode( dtUtil::ToString(data.RESOLUTION.height).c_str() );
   REFRESH = XMLString::transcode( dtUtil::ToString(data.RESOLUTION.refresh).c_str() );
   PIXEL_DEPTH = XMLString::transcode( dtUtil::ToString(data.RESOLUTION.bitDepth).c_str() );
   SHOW_CURSOR = XMLString::transcode( dtUtil::ToString(data.SHOW_CURSOR).c_str() );
   FULL_SCREEN = XMLString::transcode( dtUtil::ToString(data.FULL_SCREEN).c_str() );
   CHANGE_RESOLUTION = XMLString::transcode( dtUtil::ToString(data.CHANGE_RESOLUTION).c_str() );

   SCENE_NAME = XMLString::transcode( data.SCENE_NAME.c_str() );
   WINDOW_NAME = XMLString::transcode( data.WINDOW_NAME.c_str() );
   CAMERA_NAME = XMLString::transcode( data.CAMERA_NAME.c_str() );

   ///\todo scene_instance window_instance
}

ApplicationConfigWriter::DefaultModel::~DefaultModel()
{
   XMLString::release( &WINDOW_NAME );
   XMLString::release( &WINDOW_X );
   XMLString::release( &WINDOW_Y );
   XMLString::release( &WINDOW_WIDTH );
   XMLString::release( &WINDOW_HEIGHT );

   XMLString::release( &REFRESH );
   XMLString::release( &PIXEL_DEPTH );
   XMLString::release( &SHOW_CURSOR );
   XMLString::release( &FULL_SCREEN );

   XMLString::release( &SCENE_NAME );
   XMLString::release( &CAMERA_NAME );

   ///\todo scene_instance window_instance
}

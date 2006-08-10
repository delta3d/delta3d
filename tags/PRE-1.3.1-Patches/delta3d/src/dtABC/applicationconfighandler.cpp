#include <dtABC/applicationconfighandler.h>

#include <dtUtil/stringutils.h>
#include <dtUtil/log.h>
#include <dtUtil/xercesutils.h>

#include <xercesc/sax2/XMLReaderFactory.hpp>

// includes necessary to use dtABC::Application
#include <dtABC/application.h>
#include <dtCore/keyboard.h>
#include <dtCore/generickeyboardlistener.h>
#include <dtCore/mouse.h>

#include <dtCore/camera.h>
#include <dtCore/scene.h>

using namespace dtABC;
XERCES_CPP_NAMESPACE_USE

ApplicationConfigHandler::ApplicationConfigHandler()
{
}

ApplicationConfigHandler::~ApplicationConfigHandler()
{
}

void ApplicationConfigHandler::characters(const XMLCh* const chars, const unsigned int length) {}
void ApplicationConfigHandler::endDocument() {}
void ApplicationConfigHandler::endElement(const XMLCh* const uri,const XMLCh* const localname,const XMLCh* const qname) {}
void ApplicationConfigHandler::ignorableWhitespace(const XMLCh* const chars, const unsigned int length) {}
void ApplicationConfigHandler::processingInstruction(const XMLCh* const target, const XMLCh* const data) {}
void ApplicationConfigHandler::setDocumentLocator(const XERCES_CPP_NAMESPACE_QUALIFIER Locator* const locator) {}
void ApplicationConfigHandler::startDocument() {}
void ApplicationConfigHandler::startPrefixMapping(const	XMLCh* const prefix,const XMLCh* const uri) {}
void ApplicationConfigHandler::endPrefixMapping(const XMLCh* const prefix) {}
void ApplicationConfigHandler::skippedEntity(const XMLCh* const name) {}

///\todo use ApplicationConfigSchema for attribute and node name searches.
///\todo optimize out string and data copies by just using the default applicatioinconfigdata struct
void ApplicationConfigHandler::startElement(const XMLCh* const uri,
                                            const XMLCh* const localname,
                                            const XMLCh* const qname,
                                            const XERCES_CPP_NAMESPACE_QUALIFIER Attributes& attrs)
{
   char* elementname = XMLString::transcode( localname );
   std::string ename( elementname );
   XMLString::release( &elementname );

   if( ename == "Window" )
   {
      // set up some defaults
      std::string name("DefaultApp"),
                  xstring("100"), ystring("100"),
                  wstring("640"), hstring("480"),
                  showstring("1"), fullstring("0"),
                  changeres("0"), bitdepth("24"), refresh("60");

      // search for specific named attributes, append the list of searched strings
      dtUtil::AttributeSearch windowattrs;

      // perform the search
      typedef dtUtil::AttributeSearch::ResultMap RMap;
      RMap results = windowattrs( attrs );

      // if the named attributes were found, use them, else use defaults
      RMap::iterator iter = results.find("Name");
      if( iter != results.end() )
         name = iter->second;

      iter = results.find("X");
      if( iter != results.end() )
         xstring = iter->second;

      iter = results.find("Y");
      if( iter != results.end() )
         ystring = iter->second;

      iter = results.find("Width");
      if( iter != results.end() )
         wstring = iter->second;

      iter = results.find("Height");
      if( iter != results.end() )
         hstring = iter->second;

      iter = results.find("ShowCursor");
      if( iter != results.end() )
         showstring = iter->second;

      iter = results.find("FullScreen");
      if( iter != results.end() )
         fullstring = iter->second;

      iter = results.find("ChangeDisplayResolution");
      if( iter != results.end() )
         changeres = iter->second;

      iter = results.find("PixelDepth");
      if( iter != results.end() )
         bitdepth = iter->second;

      iter = results.find("RefreshRate");
      if( iter != results.end() )
         refresh = iter->second;

      int winX = dtUtil::ToType<int>(xstring.c_str());
      int winY = dtUtil::ToType<int>(ystring.c_str());
      int width = dtUtil::ToType<int>(wstring.c_str());
      int height = dtUtil::ToType<int>(hstring.c_str());
      bool showCursor = dtUtil::ToType<bool>(showstring.c_str());
      bool fullScreen = dtUtil::ToType<bool>(fullstring.c_str());
      bool resolution_changed = dtUtil::ToType<bool>(changeres.c_str());
      int depth = dtUtil::ToType<int>(bitdepth.c_str());
      int rate = dtUtil::ToType<int>(refresh.c_str());

      mConfigData.WINDOW_NAME = name;
      mConfigData.RESOLUTION.width = width;
      mConfigData.RESOLUTION.height = height;
      mConfigData.RESOLUTION.bitDepth = depth;
      mConfigData.RESOLUTION.refresh = rate;
      mConfigData.WINDOW_X = winX;
      mConfigData.WINDOW_Y = winY;
      mConfigData.SHOW_CURSOR = showCursor;
      mConfigData.FULL_SCREEN = fullScreen;
      mConfigData.CHANGE_RESOLUTION = resolution_changed;
   }

   if( ename == "Scene" )
   {
      dtUtil::AttributeSearch sceneattrs;
      dtUtil::AttributeSearch::ResultMap results = sceneattrs( attrs );

      dtUtil::AttributeSearch::ResultMap::iterator iter = results.find("Name");
      if( iter != results.end() )
      {
         mConfigData.SCENE_NAME = iter->second;
      }
   }

   if( ename == "Camera" )
   {
      // push some keys
      dtUtil::AttributeSearch camattrs;

      // do the attribute search, catch the results
      dtUtil::AttributeSearch::ResultMap results = camattrs( attrs );

      dtUtil::AttributeSearch::ResultMap::iterator iter = results.find("Name");
      if( iter != results.end() )
      {
         mConfigData.CAMERA_NAME = iter->second;
      }

      iter = results.find("WindowInstance");
      if( iter != results.end() )
      {
         mConfigData.WINDOW_INSTANCE = iter->second;
      }
      else
      {
         LOG_ERROR("No DeltaWin instance speficied for Camera, " + mConfigData.CAMERA_NAME)
      }

      iter = results.find("SceneInstance");
      if( iter != results.end() )
      {
         mConfigData.SCENE_INSTANCE = iter->second;
      }
      else
      {
         LOG_ERROR("No Scene Instance specified for Camera, " + mConfigData.CAMERA_NAME)
      }
   }
}

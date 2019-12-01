/* -*-c++-*- 
 * Delta3D Open Source Game and Simulation Engine 
 * Copyright (C) 2004-2005 MOVES Institute 
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
 * John K. Grant
 * David Guthrie
 */
#include <dtABC/applicationconfigwriter.h>
#include <dtABC/applicationconfigschema.h>
#include <dtABC/applicationconfigdata.h>

#include <osgViewer/CompositeViewer>

#include <dtUtil/stringutils.h>
#include <dtUtil/xerceswriter.h>
#include <dtUtil/xercesutils.h>

#include <xercesc/dom/DOMDocument.hpp>
#include <xercesc/dom/DOMElement.hpp>
#include <xercesc/dom/DOMText.hpp>
#include <xercesc/util/XMLString.hpp>

XERCES_CPP_NAMESPACE_USE

namespace dtABC
{

   // --- App config code's implementation --- //
   void ApplicationConfigWriter::operator ()(const std::string& filename, const ApplicationConfigData& data)
   {
      // initialize the xerces xml system.
      dtCore::RefPtr<dtUtil::XercesWriter> writer = new dtUtil::XercesWriter();

      SchemaModel sch;

      // specify the name of the top node.
      writer->CreateDocument( "Application" );
      XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument* doc = writer->GetDocument();
      DOMElement* app = doc->getDocumentElement();

      DOMElement* windo = doc->createElement(sch.WINDOW);
      windo->setAttribute( sch.NAME, dtUtil::StringToXMLConverter(data.WINDOW_NAME).ToXmlString() );
      
      std::ostringstream ss;
      ss << data.WINDOW_X;

      windo->setAttribute( sch.X, dtUtil::StringToXMLConverter(ss.str()).ToXmlString() );
      
      ss.str("");
      ss << data.WINDOW_Y;
      windo->setAttribute( sch.Y, dtUtil::StringToXMLConverter(ss.str()).ToXmlString() );

      ss.str("");
      ss << data.RESOLUTION.width;
      windo->setAttribute( sch.WIDTH, dtUtil::StringToXMLConverter(ss.str()).ToXmlString() );

      ss.str("");
      ss << data.RESOLUTION.height;
      windo->setAttribute( sch.HEIGHT, dtUtil::StringToXMLConverter(ss.str()).ToXmlString() );

      ss.str("");
      ss << data.RESOLUTION.bitDepth;
      windo->setAttribute( sch.PIXELDEPTH, dtUtil::StringToXMLConverter(ss.str()).ToXmlString() );

      ss.str("");
      ss << data.RESOLUTION.refresh;
      windo->setAttribute( sch.REFRESHRATE, dtUtil::StringToXMLConverter(ss.str()).ToXmlString() );
      
      windo->setAttribute( sch.SHOWCURSOR, 
         dtUtil::StringToXMLConverter(
            data.SHOW_CURSOR ? "1" : "0"
         ).ToXmlString()
      );
      
      windo->setAttribute( sch.FULLSCREEN, 
         dtUtil::StringToXMLConverter(
            data.FULL_SCREEN ? "1" : "0"
         ).ToXmlString()
      );

      windo->setAttribute( sch.REALIZE_UPON_CREATE,
         dtUtil::StringToXMLConverter(
            data.REALIZE_UPON_CREATE ? "1" : "0"
         ).ToXmlString()
      );
      
      
      windo->setAttribute( sch.CHANGEDISPLAYRESOLUTION,
         dtUtil::StringToXMLConverter(
            data.CHANGE_RESOLUTION ? "1" : "0"
         ).ToXmlString()
      );

      windo->setAttribute( sch.VSYNC,
         dtUtil::StringToXMLConverter(
            data.VSYNC ? "1" : "0"
         ).ToXmlString()
      );

      windo->setAttribute(sch.HIDE_WINDOWS_CONSOLE,
         dtUtil::StringToXMLConverter(
         data.HIDE_WINDOWS_CONSOLE ? "1" : "0"
         ).ToXmlString()
         );

      ss.str("");
      ss << data.MULTI_SAMPLE;
      windo->setAttribute( sch.MULTI_SAMPLE, dtUtil::StringToXMLConverter(ss.str()).ToXmlString() );

      app->appendChild( windo );

      DOMElement* scene = doc->createElement(sch.SCENE);
      scene->setAttribute( sch.NAME , dtUtil::StringToXMLConverter(data.SCENE_NAME).ToXmlString() );
      app->appendChild( scene );

      DOMElement* camera = doc->createElement(sch.CAMERA);
      camera->setAttribute( sch.NAME , dtUtil::StringToXMLConverter(data.CAMERA_NAME).ToXmlString());
      camera->setAttribute( sch.WINDOWINSTANCE , dtUtil::StringToXMLConverter(data.WINDOW_INSTANCE).ToXmlString() );
      app->appendChild( camera );

      DOMElement* view = doc->createElement(sch.VIEW);
      view->setAttribute( sch.NAME , dtUtil::StringToXMLConverter(data.VIEW_NAME).ToXmlString());
      view->setAttribute( sch.CAMERAINSTANCE , dtUtil::StringToXMLConverter(data.CAMERA_INSTANCE).ToXmlString() );
      view->setAttribute( sch.SCENEINSTANCE , dtUtil::StringToXMLConverter(data.SCENE_INSTANCE).ToXmlString() );
      app->appendChild( view );
      
      DOMElement* globalLogLevel = doc->createElement(sch.GLOBAL_LOG_LEVEL);
      globalLogLevel->setAttribute(sch.LOG_LEVEL, dtUtil::StringToXMLConverter(data.GLOBAL_LOG_LEVEL).ToXmlString());
      app->appendChild(globalLogLevel);

      for (std::map<std::string, std::string>::const_iterator i = data.LOG_LEVELS.begin();
         i != data.LOG_LEVELS.end(); ++i)
      {
         DOMElement* log = doc->createElement(sch.LOG);

         if (i->first != dtUtil::Log::GetInstance().GetName())
            log->setAttribute( sch.NAME , dtUtil::StringToXMLConverter(i->first).ToXmlString() );

         log->setAttribute( sch.LOG_LEVEL , dtUtil::StringToXMLConverter(i->second).ToXmlString());
         app->appendChild( log );
      }

      for (std::vector<std::string>::const_iterator i = data.LIBRARY_PATHS.begin();
         i != data.LIBRARY_PATHS.end(); ++i)
      {
         DOMElement* log = doc->createElement(sch.LIBRARY_PATH);
         DOMText* path = doc->createTextNode(dtUtil::StringToXMLConverter(*i).ToXmlString());
         log->appendChild( path );
         app->appendChild( log );
      }

      DOMElement* properties = doc->createElement(sch.APP_PROPERTIES);
      app->appendChild( properties );

      for (std::map<std::string, std::string>::const_iterator i = data.mProperties.begin();
         i != data.mProperties.end(); ++i)
      {
         DOMElement* property = doc->createElement(sch.APP_PROPERTY);

         property->setAttribute( sch.NAME , dtUtil::StringToXMLConverter(i->first).ToXmlString() );

         DOMText* propValue = doc->createTextNode(dtUtil::StringToXMLConverter(i->second).ToXmlString());
         
         property->appendChild(propValue);
         properties->appendChild( property );
      }

      writer->WriteFile( filename );
   }

   ApplicationConfigWriter::SchemaModel::SchemaModel()
   {
      WINDOW = XMLString::transcode( ApplicationConfigSchema::WINDOW.c_str() );
      NAME = XMLString::transcode( ApplicationConfigSchema::NAME.c_str() );
      SCENE = XMLString::transcode( ApplicationConfigSchema::SCENE.c_str() );
      CAMERA = XMLString::transcode( ApplicationConfigSchema::CAMERA.c_str() );
      VIEW = XMLString::transcode( ApplicationConfigSchema::VIEW.c_str() );
      LOG = XMLString::transcode( ApplicationConfigSchema::LOG.c_str() );
      LIBRARY_PATH = XMLString::transcode( ApplicationConfigSchema::LIBRARY_PATH.c_str() );

      X = XMLString::transcode( ApplicationConfigSchema::X.c_str() );
      Y = XMLString::transcode( ApplicationConfigSchema::Y.c_str() );
      WIDTH = XMLString::transcode( ApplicationConfigSchema::WIDTH.c_str() );
      HEIGHT = XMLString::transcode( ApplicationConfigSchema::HEIGHT.c_str() );

      PIXELDEPTH = XMLString::transcode( ApplicationConfigSchema::PIXELDEPTH.c_str() );
      REFRESHRATE = XMLString::transcode( ApplicationConfigSchema::REFRESHRATE.c_str() );
      SHOWCURSOR = XMLString::transcode( ApplicationConfigSchema::SHOWCURSOR.c_str() );
      FULLSCREEN = XMLString::transcode( ApplicationConfigSchema::FULLSCREEN.c_str() );
      REALIZE_UPON_CREATE = XMLString::transcode( ApplicationConfigSchema::REALIZE_UPON_CREATE.c_str() );
      CHANGEDISPLAYRESOLUTION = XMLString::transcode( ApplicationConfigSchema::CHANGEDISPLAYRESOLUTION.c_str() );
      VSYNC = XMLString::transcode(ApplicationConfigSchema::VSYNC.c_str());
      HIDE_WINDOWS_CONSOLE = XMLString::transcode(ApplicationConfigSchema::HIDE_WINDOWS_CONSOLE.c_str());
      MULTI_SAMPLE = XMLString::transcode( ApplicationConfigSchema::MULTI_SAMPLE.c_str() );

      WINDOWINSTANCE = XMLString::transcode( ApplicationConfigSchema::WINDOWINSTANCE.c_str() );
      SCENEINSTANCE = XMLString::transcode( ApplicationConfigSchema::SCENEINSTANCE.c_str() );
      CAMERAINSTANCE = XMLString::transcode( ApplicationConfigSchema::CAMERAINSTANCE.c_str() );

      LOG_LEVEL = XMLString::transcode( ApplicationConfigSchema::LOG_LEVEL.c_str() );
      GLOBAL_LOG_LEVEL = XMLString::transcode( ApplicationConfigSchema::GLOBAL_LOG_LEVEL.c_str() );

      APP_PROPERTIES = XMLString::transcode( ApplicationConfigSchema::APP_PROPERTIES.c_str() );
      APP_PROPERTY = XMLString::transcode( ApplicationConfigSchema::APP_PROPERTY.c_str() );
   }

   ApplicationConfigWriter::SchemaModel::~SchemaModel()
   {
      XMLString::release( &WINDOW );
      XMLString::release( &NAME );
      XMLString::release( &SCENE );
      XMLString::release( &CAMERA );
      XMLString::release( &VIEW );
      XMLString::release( &LOG );
      XMLString::release( &LIBRARY_PATH );

      XMLString::release( &X );
      XMLString::release( &Y );
      XMLString::release( &WIDTH );
      XMLString::release( &HEIGHT );

      XMLString::release( &PIXELDEPTH );
      XMLString::release( &REFRESHRATE );
      XMLString::release( &SHOWCURSOR );
      XMLString::release( &FULLSCREEN );
      XMLString::release( &REALIZE_UPON_CREATE );
      XMLString::release( &CHANGEDISPLAYRESOLUTION );

      XMLString::release( &WINDOWINSTANCE );
      XMLString::release( &SCENEINSTANCE );
      XMLString::release( &CAMERAINSTANCE );

      XMLString::release( &LOG_LEVEL );
      XMLString::release( &APP_PROPERTIES );
      XMLString::release( &APP_PROPERTY );
   }

}

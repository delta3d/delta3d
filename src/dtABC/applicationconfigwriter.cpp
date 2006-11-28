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

// These are the classes that need to be included to use dtABC::Application.
// we should only need the application.h file.
#include <dtABC/application.h>
#include <dtCore/keyboard.h>
#include <dtCore/mouse.h>
#include <dtCore/camera.h>
#include <dtCore/scene.h>
#include <dtCore/generickeyboardlistener.h>

#include <dtUtil/stringutils.h>
#include <dtUtil/xerceswriter.h>
#include <dtUtil/xercesutils.h>
#include <dtUtil/log.h>

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
      
      
      windo->setAttribute( sch.CHANGEDISPLAYRESOLUTION,
         dtUtil::StringToXMLConverter(
            data.CHANGE_RESOLUTION ? "1" : "0"
         ).ToXmlString()
      );
      app->appendChild( windo );

      DOMElement* scene = doc->createElement(sch.SCENE);
      scene->setAttribute( sch.NAME , dtUtil::StringToXMLConverter(data.SCENE_NAME).ToXmlString() );
      app->appendChild( scene );

      DOMElement* camera = doc->createElement(sch.CAMERA);
      camera->setAttribute( sch.NAME , dtUtil::StringToXMLConverter(data.CAMERA_NAME).ToXmlString());
      camera->setAttribute( sch.WINDOWINSTANCE , dtUtil::StringToXMLConverter(data.WINDOW_INSTANCE).ToXmlString() );
      camera->setAttribute( sch.SCENEINSTANCE , dtUtil::StringToXMLConverter(data.SCENE_INSTANCE).ToXmlString() );
      app->appendChild( camera );

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

      writer->WriteFile( filename );
   }

   ApplicationConfigWriter::SchemaModel::SchemaModel()
   {
      WINDOW = XMLString::transcode( ApplicationConfigSchema::WINDOW.c_str() );
      NAME = XMLString::transcode( ApplicationConfigSchema::NAME.c_str() );
      SCENE = XMLString::transcode( ApplicationConfigSchema::SCENE.c_str() );
      CAMERA = XMLString::transcode( ApplicationConfigSchema::CAMERA.c_str() );
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
      CHANGEDISPLAYRESOLUTION = XMLString::transcode( ApplicationConfigSchema::CHANGEDISPLAYRESOLUTION.c_str() );

      WINDOWINSTANCE = XMLString::transcode( ApplicationConfigSchema::WINDOWINSTANCE.c_str() );
      SCENEINSTANCE = XMLString::transcode( ApplicationConfigSchema::SCENEINSTANCE.c_str() );

      LOG_LEVEL = XMLString::transcode( ApplicationConfigSchema::LOG_LEVEL.c_str() );
   }

   ApplicationConfigWriter::SchemaModel::~SchemaModel()
   {
      XMLString::release( &WINDOW );
      XMLString::release( &NAME );
      XMLString::release( &SCENE );
      XMLString::release( &CAMERA );
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
      XMLString::release( &CHANGEDISPLAYRESOLUTION );

      XMLString::release( &WINDOWINSTANCE );
      XMLString::release( &SCENEINSTANCE );

      XMLString::release( &LOG_LEVEL);
   }

}

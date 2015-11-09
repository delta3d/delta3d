/* 
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
* Matthew "w00by" Stokes
*/
#ifndef DELTA_STATE_ATTRIB_COLLECTOR
#define DELTA_STATE_ATTRIB_COLLECTOR

#include <osg/Referenced>

#include <dtCore/refptr.h>
#include <dtUtil/export.h>

#include <map>
#include <string>

//Forward Declare the necessary osg classes that will be used by the NodeCollector class
/// @cond DOXYGEN_SHOULD_SKIP_THIS
namespace osg
{
   class Geode;
   class Material;
   class Texture;
   class Program;
   class Node;
}
/// @endcond

namespace dtUtil
{
   //Create the StateAttributeCollector Class for dtUtil.  It is used to gather osg State Attributes off of nodes.  The State Attributes that it 
   //gathers are: Material Objects, Program Objects, and Texture Objects.  It stores the different StateAttributes into corresponding maps which 
   //may than be retrieved by the user.  In the case of Geode nodes it retrieves the StateAttributes off of the Drawable objects which are 
   //associated with the Geode.
   class DT_UTIL_EXPORT StateAttributeCollector : public osg::Referenced
   {
   public:

      //Type Definitions for the three different geode node maps
      typedef std::map<std::string, dtCore::RefPtr<osg::Material> >  MaterialNodeMap;
      typedef std::map<std::string, dtCore::RefPtr<osg::Program> >   ProgramNodeMap;
      typedef std::map<std::string, dtCore::RefPtr<osg::Texture> >   TextureNodeMap;


      //Type Definition that is used to declare flags that allow the user to request searches for different types of state attributes.
      typedef unsigned StateAttribFlag;

      //StateAttribFlags that represent the three different kinds of state attributes that you can search for
      static const StateAttribFlag MaterialFlag;
      static const StateAttribFlag ProgramFlag;
      static const StateAttribFlag TextureFlag;


      //StateAttributeFlag that when defined will represent a Flag for a search for all every kind of state attribute
      static const StateAttribFlag AllAttributes;


      /**
      * Blank Constructor that is defined to do nothing.  
      * @note If this is used then you must call use the CollectAttributes function in order to generate any maps with this class.
      */
      StateAttributeCollector();

      /**
      * Constructor that when called will automatically generate the state attribute maps that you request
      * @param nodeToLoad The starting node who's state attributes you would like to explore.
      * @param mask The different types of state attributes you want to collect off of the loaded node.
      * @param nodeNameIgnored The name of a state attribute that you do not want to collect.
      */
      StateAttributeCollector(osg::Node* nodeToLoad, StateAttributeCollector::StateAttribFlag mask, const std::string & nodeNameIgnored = "");

      /**
      * Function that when called will automatically generate the state attribute maps that you request
      * @param nodeToLoad The starting node who's state attributes you would like to explore.
      * @param mask The different types of state attributes you want to collect off of the loaded node.
      * @param nodeNameIgnored The name of a state attribute that you do not want to collect.
      * @note This function was originally intended to be used in conjunction with the blank constructor or after a call of the ClearAllMaps function
      */
      void CollectStateAttributes(osg::Node* nodeToLoad, StateAttributeCollector::StateAttribFlag mask, const std::string & nodeNameIgnored = "");

      /**
      * Function that is defined to clear all the maps of their contents.
      */
      void ClearAll();

      /////////////////////////////////////////////////////////////////////////////////////////////
      //The following three methods are all used to return constant pointers to requested State Attributes//
      /////////////////////////////////////////////////////////////////////////////////////////////
     
      /**
      * Function that is used to request a CONST pointer to a Materials State Attribute
      * @param name A String that represents the name of the Materials State Attribute you are looking for
      * @return A CONST pointer to the Materials State Attribute you were looking for or NULL if the Materials State Attribute was not found
      */
      const osg::Material* GetMaterial(const std::string& name) const;

      /**
      * Function that is used to request a CONST pointer to a Program State Attribute
      * @param name A String that represents the name of the Program State Attribute you are looking for
      * @return A CONST pointer to the Program State Attribute you were looking for or NULL if the Program State Attribute was not found
      */
      const osg::Program* GetProgram(const std::string& name) const;

      /**
      * Function that is used to request a CONST pointer to a Texture State Attribute
      * @param name A String that represents the name of the Texture State Attribute you are looking for
      * @return A CONST pointer to the Texture State Attribute you were looking for or NULL if the Texture State Attribute was not found
      */
      const osg::Texture* GetTexture(const std::string& name) const;


      ////////////////////////////////////////////////////////////////////////////////////
      //The following three methods are all used to return pointers to requested state attributes//
      ////////////////////////////////////////////////////////////////////////////////////

      /**
      * Function that is used to request a pointer to a Materials State Attribute
      * @param name A String that represents the name of the Materials State Attribute you are looking for
      * @return A pointer to the Materials State Attribute you were looking for or NULL if the Materials State Attribute was not found
      */
      osg::Material* GetMaterial(const std::string& name);
     
      /**
      * Function that is used to request a pointer to a Program State Attribute
      * @param name A String that represents the name of the Program State Attribute you are looking for
      * @return A pointer to the Program State Attribute you were looking for or NULL if the Program State Attribute was not found
      */
      osg::Program* GetProgram( const std::string& name) ;

      /**
      * Function that is used to request a pointer to a Texture State Attribute
      * @param name A String that represents the name of the Texture State Attribute you are looking for
      * @return A pointer to the Texture State Attribute you were looking for or NULL if the Texture State Attribute was not found
      */
      osg::Texture* GetTexture( const std::string& name) ;

      /////////////////////////////////////////////////////////////////////////////////
      //The Following Four Methods are all used to add nodes to their respective maps//
      /////////////////////////////////////////////////////////////////////////////////

      /**
      * Function that is used to add a Material StateAttribute to the Material map
      * @param name A String that represents the name of the StateAttribute
      * @param materialObject The Material StateAttribute that you wish to add to the map
      */
      void AddMaterial(const std::string& name, osg::Material& materialObject);

      /**
      * Function that is used to add a Program StateAttribute to the Program map
      * @param name A String that represents the name of the StateAttribute
      * @param materialObject The Program StateAttribute that you wish to add to the map
      */
      void AddProgram(const std::string& name, osg::Program& programObject);

      /**
      * Function that is used to add a Texture StateAttribute to the Texture map
      * @param name A String that represents the name of the StateAttribute
      * @param materialObject The Texture StateAttribute that you wish to add to the map
      */
      void AddTexture(const std::string& name, osg::Texture& textureObject);



      //////////////////////////////////////////////////////////////////////////////////
      //The Following Methods are all used to return CONST maps populated with state attributes//
      //////////////////////////////////////////////////////////////////////////////////

      /**
       * Function that returns a CONST map populated with Material StateAttributes
       * @return A CONST map with the names of Material StateAttributes and osg Material StateAttributes
       */
      const StateAttributeCollector::MaterialNodeMap& GetMaterialMap() const;

      /**
      * Function that returns a CONST map populated with Program StateAttributes
      * @return A CONST map with the names of Program StateAttributes and osg Program StateAttributes
      */
      const StateAttributeCollector::ProgramNodeMap& GetProgramMap() const;

      /**
      * Function that returns a CONST map populated with Texture StateAttributes
      * @return A CONST map with the names of Texture StateAttributes and osg Texture StateAttributes
      */
      const StateAttributeCollector::TextureNodeMap& GetTextureMap() const;

      /////////////////////////////////////////////////////////////////////////////////////
      //The Following Methods are all used to return maps populated with state attributes//
      /////////////////////////////////////////////////////////////////////////////////////

      /**
      * Function that returns a map populated with Material StateAttributes
      * @return A map with the names of Material StateAttributes and osg Material StateAttributes
      */
      StateAttributeCollector::MaterialNodeMap& GetMaterialMap();

      /**
      * Function that returns a map populated with Program StateAttributes
      * @return A map with the names of Program StateAttributes and osg Program StateAttributes
      */
      StateAttributeCollector::ProgramNodeMap& GetProgramMap();

      /**
      * Function that returns a map populated with Texture StateAttributes
      * @return A map with the names of Texture StateAttributes and osg Texture StateAttributes
      */
      StateAttributeCollector::TextureNodeMap& GetTextureMap();

   protected:

      /**
      * Destructor 
      */
      virtual ~StateAttributeCollector();

   private:
      /**
      * The following three member variables each represent maps for the different StateAttribute Maps
      */
      StateAttributeCollector::MaterialNodeMap mMaterialNodeMap;
      StateAttributeCollector::ProgramNodeMap mProgramNodeMap;
      StateAttributeCollector::TextureNodeMap mTextureNodeMap;
   };
} // namespace

#endif  //DELTA_STATE_ATTRIB_COLLECTOR

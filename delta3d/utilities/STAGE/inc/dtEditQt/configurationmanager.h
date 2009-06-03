#ifndef _STAGE_CONFIGURATION_MANAGER_H__
#define _STAGE_CONFIGURATION_MANAGER_H__

#include <map>

#include <dtDAL/serializeable.h>

#include <osg/Vec2>

#include <xercesc/dom/DOMElement.hpp>


namespace dtEditQt
{

class ConfigurationManager : public dtDAL::XMLSerializeable 
{
public:
   enum SectionType
   {
      GENERAL,
      LAYOUT,
      MENU
   };

   ConfigurationManager();

   bool ReadXML(const std::string& fileName);
   void WriteXML(const std::string& fileName);

   std::string GetVariable(SectionType section, const std::string& name);
   void SetVariable(SectionType section, const std::string& name,
                    const std::string& value);
   void SetVariable(SectionType section, const std::string& name, int value);

private:
   //implementations of pure virtuals
   void ToXML(XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument& doc) const;
   bool FromXML(XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument& doc);

   XERCES_CPP_NAMESPACE_QUALIFIER DOMElement* 
      WriteSectionToXML(XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument& doc,
                         SectionType section) const;

   void SetDefaultConfigValues();

   std::map<std::string, std::string> mGeneralVariables;    // name/value pairs
   std::map<std::string, std::string> mLayoutVariables;     // name/value pairs
   std::map<std::string, std::string> mMenuVariables;       // name/value pairs
   //To add more sections to the config file, make some more maps here, add a 
   //value to the SectionType enum, and then add a few lines to the ToXML method,
   //and the ReadXML method.

   bool mLockViewDimensions;     //if true, user cannot change view dimensions
                                 //(e.g. if there is a single perspective view, user
                                 // cannot resize to reveal the other "hidden" views).

};

} //namespace dtEditQt

#endif //_STAGE_CONFIGURATION_MANAGER_H__

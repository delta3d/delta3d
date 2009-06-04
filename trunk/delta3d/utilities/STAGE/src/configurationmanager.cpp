#include <dtEditQt/configurationmanager.h>

#include <dtCore/refptr.h>
#include <dtUtil/log.h>
#include <dtUtil/xercesutils.h>
#include <dtUtil/xerceswriter.h>

#include <xercesc/dom/DOMDocument.hpp>
#include <xercesc/dom/DOMText.hpp>
#include <xercesc/parsers/XercesDOMParser.hpp>
#include <xercesc/util/XMLString.hpp>

namespace dtEditQt
{
////////////////////////////////////////////////////////////////////////////////
ConfigurationManager::ConfigurationManager()
   : mLockViewDimensions(false)
{
   xercesc::XMLPlatformUtils::Initialize();
}

////////////////////////////////////////////////////////////////////////////////
bool ConfigurationManager::ReadXML(const std::string& fileName)
{
   xercesc::XercesDOMParser parser;
   std::map<std::string, std::string> *currentMapPtr = NULL;

   parser.setValidationScheme(xercesc::XercesDOMParser::Val_Never);
   parser.setDoNamespaces(false);
   parser.setDoSchema(false);
   parser.setLoadExternalDTD(false);

   parser.parse(fileName.c_str());

   xercesc::DOMDocument* xmlDoc  = parser.getDocument();

   if (xmlDoc == NULL)
   {    
      dtUtil::Log::GetInstance().LogMessage(dtUtil::Log::LOG_ERROR, __FUNCTION__,
          __LINE__, "STAGE config file appears empty -- using default config values.");

      SetDefaultConfigValues();

      return false;
   }

   xercesc::DOMElement* root = xmlDoc->getDocumentElement();

   //parse all children
   xercesc::DOMNodeList* children = root->getChildNodes();
   for(XMLSize_t i = 0; i < children->getLength(); ++i)
   {
      xercesc::DOMNode* node = children->item(i);
      if(node == NULL)
      {
         continue;
      }
      if(node->getNodeType() != xercesc::DOMNode::ELEMENT_NODE)
      {
         continue;
      }

      dtUtil::XMLStringConverter sectionStrConv(node->getNodeName());
      std::string sectionStr = sectionStrConv.ToString();
      if(sectionStr == "General")
      {
         currentMapPtr = &mGeneralVariables;
      }
      else if(sectionStr == "Layout")
      {
         currentMapPtr = &mLayoutVariables;
      }
      else if(sectionStr == "Menu")
      {
         currentMapPtr = &mMenuVariables;
      }
      else
      {
         //Unrecognized group; try the next child of root
         continue;
      }

      //If we got here we are in a section and need to parse all variables in
      //that section
      xercesc::DOMNodeList* sectionChildren = node->getChildNodes();
      for(XMLSize_t j = 0; j < sectionChildren->getLength(); ++j)
      {
         xercesc::DOMNode* variableNode = sectionChildren->item(j);
         if(variableNode == NULL)
         {
            continue;
         }
         if (node->getNodeType() != xercesc::DOMNode::ELEMENT_NODE)
         {
            continue;
         }
         
         dtUtil::XMLStringConverter varStrConv(variableNode->getNodeName());
         //variableNode is supposed to have a text element
         xercesc::DOMNode* txtNode = variableNode->getFirstChild();
         if(txtNode != NULL)
         {
            //Map the variable name to the value
            (*currentMapPtr)[varStrConv.ToString()] = 
               XERCES_CPP_NAMESPACE_QUALIFIER XMLString::transcode(txtNode->getTextContent());
         }
      }
   }

   return true;  
}

////////////////////////////////////////////////////////////////////////////////
void ConfigurationManager::WriteXML(const std::string& fileName)
{
   dtCore::RefPtr<dtUtil::XercesWriter> writer = new dtUtil::XercesWriter();
   writer->CreateDocument("STAGEConfig");

   XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument* doc = writer->GetDocument();

   ToXML(*doc);

   writer->WriteFile(fileName);
   
}

////////////////////////////////////////////////////////////////////////////////
std::string ConfigurationManager::GetVariable(SectionType section, 
                                               const std::string& name)
{
   switch(section)
   {
      case GENERAL:
         return mGeneralVariables[name];
         break;
      case LAYOUT:
         return mLayoutVariables[name];
         break;
      case MENU:
         return mMenuVariables[name];
         break;
      default:
         dtUtil::Log::GetInstance().LogMessage(dtUtil::Log::LOG_ERROR,
                   __FUNCTION__, __LINE__, "Unrecognized SectionType");
         break;
   }

   return std::string("");
}

////////////////////////////////////////////////////////////////////////////////
void ConfigurationManager::SetVariable(SectionType section, 
                                       const std::string& name, const std::string& value)
{
   std::string errString;

   switch(section)
   {
      case GENERAL:
         mGeneralVariables[name] = value;
         break;
      case LAYOUT:
         mLayoutVariables[name] = value;
         break;
      case MENU:
         mMenuVariables[name] = value;
         break;
      default:
         errString = "Unrecognized SectionType, unable to set variable: ";
         errString += name;
         errString += " = ";
         errString += value;
         dtUtil::Log::GetInstance().LogMessage(dtUtil::Log::LOG_ERROR,
            __FUNCTION__, __LINE__, errString.c_str());
         break;
   }   
}

////////////////////////////////////////////////////////////////////////////////
void ConfigurationManager::SetVariable(SectionType section, 
                                       const std::string& name, int value)
{
   char val[1024];

   sprintf(val, "%d", value);

   SetVariable(section, name, val);
}

////////////////////////////////////////////////////////////////////////////////
void ConfigurationManager::ToXML(XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument& doc) const
{ 
   XERCES_CPP_NAMESPACE_QUALIFIER DOMElement* root = doc.getDocumentElement();   

   root->appendChild(WriteSectionToXML(doc, GENERAL));
   root->appendChild(WriteSectionToXML(doc, LAYOUT));
   root->appendChild(WriteSectionToXML(doc, MENU));
}


////////////////////////////////////////////////////////////////////////////////
bool ConfigurationManager::FromXML(XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument& doc)
{
   //Read each name/value pair
   //(Ended up just doing it in ReadXML)

   return false;
}

////////////////////////////////////////////////////////////////////////////////
XERCES_CPP_NAMESPACE_QUALIFIER DOMElement* 
   ConfigurationManager::WriteSectionToXML(XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument& doc, 
                                           SectionType section) const
{
   std::map<std::string, std::string>::const_iterator beginIt, endIt, it;
   XMLCh* groupName;

   switch(section)
   {
      case GENERAL:
         beginIt = mGeneralVariables.begin();
         endIt = mGeneralVariables.end();
         groupName = XERCES_CPP_NAMESPACE_QUALIFIER XMLString::transcode("General");      
         break;
      case LAYOUT:
         beginIt = mLayoutVariables.begin();
         endIt = mLayoutVariables.end();
         groupName = XERCES_CPP_NAMESPACE_QUALIFIER XMLString::transcode("Layout");
         break;
      case MENU:
         beginIt = mMenuVariables.begin();
         endIt = mMenuVariables.end();
         groupName = XERCES_CPP_NAMESPACE_QUALIFIER XMLString::transcode("Menu");
         break;
      default:
         dtUtil::Log::GetInstance().LogMessage(dtUtil::Log::LOG_ERROR,
            __FUNCTION__, __LINE__, "Unrecognized Section.");
         return NULL;
         break;
   }   
   
   XERCES_CPP_NAMESPACE_QUALIFIER DOMElement* groupElement = doc.createElement(groupName);   
   XERCES_CPP_NAMESPACE_QUALIFIER XMLString::release(&groupName);   

   for(it = beginIt; it != endIt; ++it)
   {
      XMLCh* name = XERCES_CPP_NAMESPACE_QUALIFIER XMLString::transcode(it->first.c_str());
      XMLCh* value = XERCES_CPP_NAMESPACE_QUALIFIER XMLString::transcode(it->second.c_str());

      XERCES_CPP_NAMESPACE_QUALIFIER DOMElement* element = doc.createElement(name);      
      XERCES_CPP_NAMESPACE_QUALIFIER DOMText* txt = doc.createTextNode(value);

      element->appendChild(txt);
      groupElement->appendChild(element);

      XERCES_CPP_NAMESPACE_QUALIFIER XMLString::release(&name);
      XERCES_CPP_NAMESPACE_QUALIFIER XMLString::release(&value);
   }

   return groupElement;
}

////////////////////////////////////////////////////////////////////////////////
void ConfigurationManager::SetDefaultConfigValues()
{
   mGeneralVariables["SaveConfigurationOnClose"] = "True";

   mLayoutVariables["ShowFrontView"] = "True";
   mLayoutVariables["ShowSideView"] = "True";
   mLayoutVariables["ShowTopView"] = "True";
   mLayoutVariables["ShowPerspView"] = "True";

   mLayoutVariables["ShowPropertyEditor"] = "True";
   mLayoutVariables["ShowActorTab"] = "True";
   mLayoutVariables["ShowResourceBrowser"] = "True";
}

} //namespace dtEditQt

/*
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2005, BMH Associates, Inc.
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
 * @author David Guthrie
 */

#include <iostream>
#include <stdio.h>
#include <string.h>

#include <xercesc/framework/MemBufInputSource.hpp>
#include <xercesc/dom/DOMNode.hpp>
#include <xercesc/dom/DOM.hpp>
#include <xercesc/framework/StdOutFormatTarget.hpp>
#include <xercesc/framework/LocalFileFormatTarget.hpp>
#include <xercesc/util/XMLUni.hpp>
#include <xercesc/util/OutOfMemoryException.hpp>
#include <xercesc/sax/ErrorHandler.hpp>
#include <xercesc/sax/SAXParseException.hpp>

#include <osgDB/FileNameUtils>
#include "dtDAL/rbodyresourcetypehandler.h"
#include <dtUtil/xercesutils.h>

#if defined (WIN32) || defined (_WIN32) || defined (__WIN32__)
#define snprintf _snprintf
#endif

XERCES_CPP_NAMESPACE_USE;

namespace dtUtil
{
   class FileUtils;
}

namespace dtDAL
{

   const std::string RBodyResourceTypeHandler::mResourceDirectoryExtension("rbody");
   const std::string RBodyResourceTypeHandler::mConfigFileHeader("# ReplicantBody v0.1");

   RBodyResourceTypeHandler::RBodyResourceTypeHandler():
      MATERIAL_ELEMENT(NULL), NUMMAPS_ATTRIBUTE(NULL), MAP_ELEMENT(NULL), 
      mParser(NULL), mDescription("Replicant Body Characters")
   {
      mFilters.insert(std::make_pair(mResourceDirectoryExtension, "Replicant Body Files"));

      mLogger = &dtUtil::Log::GetInstance("RBodyResourceTypeHandler.cpp");

      mXMLErrorHandler.mLogger = mLogger;
   }

   RBodyResourceTypeHandler::~RBodyResourceTypeHandler() {}

   bool RBodyResourceTypeHandler::HandlesFile(const std::string& path, dtUtil::FileType type) const
   {
      if (osgDB::getLowerCaseFileExtension(path) == mResourceDirectoryExtension )
      {
         if (type == dtUtil::REGULAR_FILE)
         {
            //It has the right extension, but can we parse the file?
            if (mRBodyConfig.Open(path, mConfigFileHeader))
            {
               mRBodyConfig.Close();
               return true;
            }

         }
         else if (type == dtUtil::DIRECTORY)
         {
            //the format is always the directory will match the internal file name,
            //e.g. marine.rbody/marine.rbody
            const std::string& filePath = path + dtUtil::FileUtils::PATH_SEPARATOR + osgDB::getSimpleFileName(path);

            //See if the file exists and if we can parse it?
            if (mRBodyConfig.Open(filePath, mConfigFileHeader))
            {
               mRBodyConfig.Close();
               return true;
            }
         }
      }
      return false;
   }

   ResourceDescriptor RBodyResourceTypeHandler::CreateResourceDescriptor(
      const std::string& category, const std::string& fileName) const
   {
      std::string displayString = GetResourceType().GetName();
      std::string resultString;
      if (!category.empty())
      {
         displayString += ResourceDescriptor::DESCRIPTOR_SEPARATOR +
            category + ResourceDescriptor::DESCRIPTOR_SEPARATOR + fileName;
      }
      else
      {
         displayString += ResourceDescriptor::DESCRIPTOR_SEPARATOR + fileName;
      }

      resultString = displayString + ResourceDescriptor::DESCRIPTOR_SEPARATOR + fileName;

      return ResourceDescriptor(displayString,resultString);
   }

   const std::string RBodyResourceTypeHandler::ImportResourceToPath(const std::string& newName,
                                                                    const std::string& srcPath, const std::string& destCategoryPath) const
   {

      //std::cout << "adding RBODY resource " << srcPath << " as " << newName << "." << std::endl;

      dtUtil::FileUtils& fileUtils = dtUtil::FileUtils::GetInstance();
      dtUtil::FileType ftype = fileUtils.GetFileInfo(srcPath).fileType;

      if (ftype != dtUtil::REGULAR_FILE)
      {
         EXCEPT(dtDAL::ExceptionEnum::ProjectFileNotFound,
                std::string("No such file:\"") + srcPath + "\".");
      }

      std::string extension = osgDB::getLowerCaseFileExtension(srcPath);
      std::string resourceFileName = newName + '.' + extension;

      const std::string& destDir = destCategoryPath + dtUtil::FileUtils::PATH_SEPARATOR + resourceFileName;
      //the format is the directory will match the internal file name,
      //e.g. marine.rbody/marine.rbody
      const std::string& destFile = destDir + dtUtil::FileUtils::PATH_SEPARATOR + resourceFileName;

      fileUtils.MakeDirectory(destDir);

      fileUtils.FileCopy(srcPath, destFile, true);

      if (mRBodyConfig.Open(srcPath, mConfigFileHeader))
      {
         try
         {
            const std::string& srcDir = osgDB::getFilePath(srcPath);

            std::string value;
            if (mRBodyConfig.Get("skeleton", value))
            {
               try
               {
                  fileUtils.FileCopy(srcDir + dtUtil::FileUtils::PATH_SEPARATOR + value, destDir, true);
               }
               catch (const dtUtil::Exception& ex)
               {
                  mLogger->LogMessage(dtUtil::Log::LOG_ERROR, __FUNCTION__, __LINE__,
                                      "Error \"%s\" copying associated rbody resource file %s to %s",
                                      ex.What().c_str(), (srcDir + dtUtil::FileUtils::PATH_SEPARATOR + value).c_str(), destDir.c_str());
                  throw ex;
               }
            }

            //create the parser and the parser constants
            //before handling the material section.
            //These items could, in theory, be created and held onto
            //for the entire life of this object, but since this object
            //will be cleaned up when the application is shutdown, the xercesc
            //platform utilities could have been cleaned up before this class, which will
            //cause these to crash.
            if (mParser != NULL)
            {
               mLogger->LogMessage(dtUtil::Log::LOG_WARNING, __FUNCTION__, __LINE__,
                                   "Parser member of RBodyResourceTypeHandler is not null, but it should be.  This could mean there is a possible memory leak.");
            }
            mParser = new XercesDOMParser;
            mParser->setValidationScheme(XercesDOMParser::Val_Never);
            mParser->setDoNamespaces(true);
            mParser->setDoSchema(false);
            mParser->setValidationSchemaFullChecking(false);
            mParser->setCreateEntityReferenceNodes(false);
            MATERIAL_ELEMENT = XMLString::transcode("MATERIAL");
            NUMMAPS_ATTRIBUTE = XMLString::transcode("NUMMAPS");
            MAP_ELEMENT = XMLString::transcode("MAP");
            try
            {
               CopyFilesForSections("Animation", "filename", srcDir, destDir);
               CopyFilesForSections("Mesh", "filename", srcDir, destDir);
               //The parser is really only needed for "Material" but if someone coded a messed up rbody
               //file and put an xrt file a mesh place, it COULD crash if the parser is not created.
               CopyFilesForSections("Material", "filename", srcDir, destDir);
            }
            catch (const dtUtil::Exception& ex)
            {
               delete mParser;
               mParser = NULL;
               XMLString::release(&MATERIAL_ELEMENT);
               XMLString::release(&NUMMAPS_ATTRIBUTE);
               XMLString::release(&MAP_ELEMENT);
               throw ex;
            }
            delete mParser;
            mParser = NULL;
            XMLString::release(&MATERIAL_ELEMENT);
            XMLString::release(&NUMMAPS_ATTRIBUTE);
            XMLString::release(&MAP_ELEMENT);
         }
         catch (const dtUtil::Exception& ex)
         {
            mRBodyConfig.Close();
            throw ex;
         }
         mRBodyConfig.Close();
      }

      return resourceFileName;
   }

   inline void RBodyResourceTypeHandler::CopyFilesForSections(
      const std::string& sectionName, const std::string& keyName, const std::string& srcDir, const std::string& destDir) const
   {
      std::string value;
      dtUtil::FileUtils& fileUtils = dtUtil::FileUtils::GetInstance();
      for (int i = 1; mRBodyConfig.Push(sectionName, i); ++i)
      {
         if (mRBodyConfig.Get(keyName, value))
         {
            try
            {
               fileUtils.FileCopy(srcDir + dtUtil::FileUtils::PATH_SEPARATOR + value, destDir, true);
            }
            catch (const dtUtil::Exception& ex)
            {
               mLogger->LogMessage(dtUtil::Log::LOG_ERROR, __FUNCTION__, __LINE__,
                                   "Error \"%s\" copying associated rbody resource file %s to %s",
                                   ex.What().c_str(), (srcDir + dtUtil::FileUtils::PATH_SEPARATOR + value).c_str(), destDir.c_str());
               throw ex;
            }
            //if it's a material file, it will probably have referenced values.
            if (osgDB::getLowerCaseFileExtension(value) == "xrf")
            {
               if (mParser != NULL)
                  ParseMaterialAndCopyReferenecedFiles(srcDir, value, destDir);
            }
         }
         mRBodyConfig.Pop();
      }
   }

   void RBodyResourceTypeHandler::RemoveResource(const std::string& resourcePath) const
   {
      dtUtil::FileUtils& fileUtils = dtUtil::FileUtils::GetInstance();
      if (fileUtils.FileExists(resourcePath))
      {
         const std::string& path = osgDB::getFilePath(resourcePath);
         if (HandlesFile(path, dtUtil::DIRECTORY))
            fileUtils.DirDelete(path, true);
      }
   }

   void RBodyResourceTypeHandler::ParseMaterialAndCopyReferenecedFiles(
      const std::string& srcDir, const std::string& docFileName, const std::string& destDir) const
   {
      const std::string& srcPath = srcDir + dtUtil::FileUtils::PATH_SEPARATOR + docFileName;

      dtUtil::FileUtils& fileUtils = dtUtil::FileUtils::GetInstance();
      size_t size = fileUtils.GetFileInfo(srcPath).size;

      char* xmlWrapperFmt = "<Hack>%s</Hack>";

      char* srcFileData = new char[size + 1];
      char* fullXML = new char[size + strlen(xmlWrapperFmt) + 1];

      FILE* srcFile = fopen(srcPath.c_str(), "rb");

      if (srcFile == NULL)
      {
         return;
      }

      fread(srcFileData, 1, size, srcFile);

      srcFileData[size] = '\0';

      if (snprintf(fullXML, size + strlen(xmlWrapperFmt) + 1, xmlWrapperFmt, srcFileData) == 0)
      {
         return;
      }

      //make a string out of it so that it will get deleted when
      //the method exits. The parser needs the data to hang around.
      std::string fullXMLString = fullXML;

      //for (std::string::iterator i = fullXMLString.begin(); i != fullXMLString.end(); ++i)
      //{
      //    if (*i == '\r')
      //    {
      //        i = fullXMLString.erase(i);
      //    }
      //}

      delete[] srcFileData;
      delete[] fullXML;

      DOMDocument* doc = NULL;
      std::string error;

      MemBufInputSource inputSource((const XMLByte*)fullXMLString.c_str(), fullXMLString.size(), docFileName.c_str(), false);

      try
      {
         mParser->setErrorHandler(&mXMLErrorHandler);
         mParser->parse(inputSource);

         doc = mParser->getDocument();
         if (doc == NULL)
         {
            mLogger->LogMessage(dtUtil::Log::LOG_INFO, __FUNCTION__, __LINE__,
                                "Xerces parser did not parse an document for \"%s\"", srcPath.c_str());
            return;
         }

         if (doc->getDocumentElement() == NULL)
         {
            mLogger->LogMessage(dtUtil::Log::LOG_INFO, __FUNCTION__, __LINE__,
                                "Xerces parser did not parse a document with any elements for file \"%s\"", srcPath.c_str());
            return;
         }

         DOMNode* materialNode = NULL;

         //search for the <MATERIAL> node.
         for (XMLSize_t i = 0; i < doc->getDocumentElement()->getChildNodes()->getLength() && materialNode == NULL; ++i)
         {
            DOMNode* node = doc->getDocumentElement()->getChildNodes()->item(i);
            if (node == NULL)
               continue;

            if (XMLString::compareString(node->getLocalName(), MATERIAL_ELEMENT) != 0)
               continue;
            else
               materialNode = node;
         }

         //if we found the node, search for the map files.
         if (materialNode != NULL)
         {
            //check to see how many material maps it expects me to find.
            DOMNode* numMapsAttr =
               materialNode->getAttributes()->getNamedItem(NUMMAPS_ATTRIBUTE);

            const XMLCh* value = NULL;
            if (numMapsAttr != NULL)
               value = numMapsAttr->getNodeValue();

            int numMaps = 0;

            if (XMLString::stringLen(value) > 0)
            {
               numMaps = XMLString::parseInt(value);
            }

            int foundMaps = 0;

            //Search through the elements for "MAP" elements.
            DOMNodeList* list = materialNode->getChildNodes();

            for (XMLSize_t i = 0; i < list->getLength(); ++i)
            {
               DOMNode* node = list->item(i);

               if (XMLString::compareString(node->getLocalName(), MAP_ELEMENT) != 0)
                  continue;

               //we found a map element, so increment.
               foundMaps++;

               char* fileNameChar = XMLString::transcode(node->getTextContent());

               if (fileNameChar == NULL)
                  continue;

               const std::string fileName(fileNameChar);
               XMLString::release(&fileNameChar);

               const std::string& materialPath = srcDir + dtUtil::FileUtils::PATH_SEPARATOR + fileName;
               dtUtil::FileUtils& fileUtils = dtUtil::FileUtils::GetInstance();

               //if we found a file in the MAP element, copy it.
               if (fileUtils.GetFileInfo(materialPath).fileType == dtUtil::REGULAR_FILE)
               {
                  try
                  {
                     fileUtils.FileCopy(materialPath, destDir, true);
                  }
                  catch (const dtUtil::Exception& ex)
                  {
                     mLogger->LogMessage(dtUtil::Log::LOG_ERROR, __FUNCTION__, __LINE__,
                                         "Error \"%s\" copying associated rbody resource file %s to %s",
                                         ex.What().c_str(), (srcPath).c_str(), destDir.c_str());
                                         
                     throw ex;
                  }
               }
               else
               {
                  if (mLogger->IsLevelEnabled(dtUtil::Log::LOG_WARNING))
                     mLogger->LogMessage(dtUtil::Log::LOG_WARNING, __FUNCTION__, __LINE__,
                                         "Material file \"%s\" referenced file %s, but it was not found. so it was not copied. The Replicant Body resource will probably not work.",
                                         srcPath.c_str(), materialPath.c_str());

               }
            }
            if (foundMaps != numMaps)
            {
               if (mLogger->IsLevelEnabled(dtUtil::Log::LOG_WARNING))
                  mLogger->LogMessage(dtUtil::Log::LOG_WARNING, __FUNCTION__, __LINE__,
                                      "Material file \"%s\" declared that it contained %d maps, but %d were actually found.",
                                      srcPath.c_str(), numMaps, foundMaps);
            }
         }

      }
      catch (const OutOfMemoryException&)
      {
         error = "OutOfMemoryException";
      }
      catch (const XMLException&)
      {
         error = "XMLException";
      }
      catch (const DOMException& e)
      {
         const unsigned int maxChars = 2047;
         XMLCh errText[maxChars + 1];

         if (DOMImplementation::loadDOMExceptionMsg(e.code, errText, maxChars)) {
            char* errTextChar = XMLString::transcode(errText);
            error = errTextChar;
            XMLString::release(&errTextChar);
         } else {
            error = "DOMException";
         }

      }
      catch (const dtUtil::Exception& ex)
      {
         mLogger->LogMessage(dtUtil::Log::LOG_ERROR, __FUNCTION__, __LINE__,
                             "Exception thrown attempting to parse and copy referenced file from material file \"%s\": %s",
                             srcPath.c_str(), ex.What().c_str());
         throw ex;
      }
      catch (...)
      {
         error = "Unknown error";
      }

      if (!error.empty())
      {
         EXCEPT(dtDAL::ExceptionEnum::ProjectResourceError,
                std::string("Error \"") + error + "\" occurred parsing material file " + srcPath);
      }
   }

   const std::map<std::string, std::string>& RBodyResourceTypeHandler::GetFileFilters() const
   {
      return mFilters;
   }


   void RBodyResourceTypeHandler::RBodyErrorHandler::error(const SAXParseException& exc)
   {
      mLogger->LogMessage(dtUtil::Log::LOG_ERROR, __FUNCTION__,  __LINE__,
                          "ERROR %d:%d - %s:%s - %s", exc.getLineNumber(),
                          exc.getColumnNumber(), dtUtil::XMLStringConverter(exc.getPublicId()).c_str(),
                          dtUtil::XMLStringConverter(exc.getSystemId()).c_str(),
                          dtUtil::XMLStringConverter(exc.getMessage()).c_str());
   }

   void RBodyResourceTypeHandler::RBodyErrorHandler::fatalError(const SAXParseException& exc)
   {
      mLogger->LogMessage(dtUtil::Log::LOG_ERROR, __FUNCTION__,  __LINE__,
                          "FATAL-ERROR %d:%d - %s:%s - %s", exc.getLineNumber(),
                          exc.getColumnNumber(), dtUtil::XMLStringConverter(exc.getPublicId()).c_str(),
                          dtUtil::XMLStringConverter(exc.getSystemId()).c_str(),
                          dtUtil::XMLStringConverter(exc.getMessage()).c_str());
   }

   void RBodyResourceTypeHandler::RBodyErrorHandler::warning(const SAXParseException& exc)
   {
      mLogger->LogMessage(dtUtil::Log::LOG_WARNING, __FUNCTION__,  __LINE__,
                          "WARNING %d:%d - %s:%s - %s", exc.getLineNumber(),
                          exc.getColumnNumber(), dtUtil::XMLStringConverter(exc.getPublicId()).c_str(),
                          dtUtil::XMLStringConverter(exc.getSystemId()).c_str(),
                          dtUtil::XMLStringConverter(exc.getMessage()).c_str());
   }

   void RBodyResourceTypeHandler::RBodyErrorHandler::resetErrors()
   {
   }

}

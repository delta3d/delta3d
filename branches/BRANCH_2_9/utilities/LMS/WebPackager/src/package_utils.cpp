/* -*-c++-*-
 * allTests - This source file (.h & .cpp) - Using 'The MIT License'
 * Copyright (C) 2006-2008, Alion Science and Technology Corporation
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 * This software was developed by Alion Science and Technology Corporation under
 * circumstances in which the U. S. Government may have rights in the software.
 *
 * @author Joseph Del Rocco
 */

/* @brief This file contains common helper functions.
 * @note It wraps the dtUtils::FileUtils functionality.
 */

// local
#include <package_utils.h>
// delta3d
#include <dtUtil/fileutils.h>
#include <osgDB/FileNameUtils>
#include <iostream>

#include <cstring>
#include <cstdlib>

//======================================
// OPTIONS CLASS FUNCTIONS
//======================================

Options::Options()
{
}

Options::Options(const Options& src)
{
   /*
   std::map<std::string,std::string>::const_iterator itr = src.mOptions.begin();
   while (itr != src.mOptions.end())
   {
      mOptions[itr->first] = itr->second;
      ++itr;
   }
   //*/
   mOptions = src.mOptions;
}

Options& Options::operator=(const Options& src)
{
   if (this != &src)
   {
      /*
      std::map<std::string,std::string>::const_iterator itr = src.mOptions.begin();
      while (itr != src.mOptions.end())
      {
         mOptions[itr->first] = itr->second;
         ++itr;
      }
      //*/
      mOptions = src.mOptions;
   }

   return (*this);
}

Options::~Options()
{

}

const std::string& Options::Get(const std::string& key )
{
   return mOptions[key];
}

std::string Options::Get(unsigned int idx)
{
   std::map<std::string,std::string>::iterator itr = mOptions.begin();
   unsigned int i = 0;

   while (itr != mOptions.end())
   {
      if (i == idx) { return itr->second; }
      ++i;
      ++itr;
   }

   return std::string();
}

void Options::Set(const std::string& key, const std::string& value)
{
   mOptions[key] = value;
}

bool Options::IsEmpty()
{
   return mOptions.empty();
}

unsigned int Options::GetCount()
{
   /*
   std::map<std::string,std::string>::iterator itr = mOptions.begin();
   unsigned int i = 0;

   while (itr != mOptions.end())
   {
      ++i;
      ++itr;
   }

   return i;
   //*/
   return mOptions.size();
}


//======================================
// FILE FUNCTIONS
//======================================

std::string GetFilePath(const std::string& fullfilename)
{
   //std::string final = fullfilename;
   //int separatorIdx  = -1, dotIdx = -1;

   //// check for file extension
   //int dotIdx = -1;
   //dotIdx = int(fullfilename.find_last_of(".", fullfilename.length()));
   //if (dotIdx == -1)
   //{
   //   CleanupFileString(final);
   //   return final;
   //}

   //// handle unix-style paths
   //separatorIdx = int(fullfilename.find_last_of("/", fullfilename.length()));
   //if (separatorIdx != -1 && separatorIdx < dotIdx)
   //{
   //   final = fullfilename.substr(0, separatorIdx);
   //   CleanupFileString(final);
   //   return final;
   //}
   //else if (separatorIdx != -1 && separatorIdx > dotIdx)
   //{
   //   CleanupFileString(final);
   //   return final;
   //}

   //// handle windows paths
   //separatorIdx = int(fullfilename.find_last_of( "\\", fullfilename.length()));
   //if (separatorIdx != -1 && separatorIdx < dotIdx)
   //{
   //   final = fullfilename.substr(0, separatorIdx);
   //   CleanupFileString(final);
   //   return final;
   //}
   //else if (separatorIdx != -1 && separatorIdx > dotIdx)
   //{
   //   CleanupFileString(final);
   //   return final;
   //}

   //// there is no path in this filename
   //return std::string();

   //***********new code
   //
   // check for file extension; if none then assume fullfilename is directory
   // path without a filename
   std::string path = fullfilename;
   int dotIdx = -1;

   dotIdx = int(fullfilename.find_last_of(".", fullfilename.length()));
   if (dotIdx == -1)
   {
      CleanupFileString(path);
      return path;
   }

   path = osgDB::getFilePath(fullfilename);
   CleanupFileString(path);
   return path;
}

std::string GetFileName(const std::string& fullfilename)
{
   //int separatorIdx = -1, dotIdx = -1;
   //std::string path = GetFilePath(fullfilename);

   //// check if no path, then remaining is the name
   //if (path.length() == 0)
   //{
   //   return fullfilename;
   //}

   //// no file extension means no filename
   //dotIdx = int(fullfilename.find_last_of(".", fullfilename.length()));
   //if (dotIdx == -1)
   //{
   //   return std::string();
   //}

   //// handle unix-style paths
   //separatorIdx = int(fullfilename.find_last_of("/", fullfilename.length()));
   //if (separatorIdx != -1 && separatorIdx < dotIdx)
   //{
   //   int namelength = int(fullfilename.length() - separatorIdx - 1);
   //   return fullfilename.substr(separatorIdx+1, namelength);
   //}
   //else if ( separatorIdx != -1 && separatorIdx > dotIdx )
   //{
   //   return std::string();
   //}

   //// handle windows paths
   //separatorIdx = int(fullfilename.find_last_of("\\", fullfilename.length()));
   //if (separatorIdx != -1 && separatorIdx < dotIdx)
   //{
   //   int namelength = int(fullfilename.length() - separatorIdx - 1);
   //   return fullfilename.substr(separatorIdx+1, namelength);
   //}
   //else if (separatorIdx != -1 && separatorIdx > dotIdx)
   //{
   //   return std::string();
   //}

   //// undefined..
   //return std::string();

   return osgDB::getSimpleFileName(fullfilename);
}

std::string GetFileNameNoExt(const std::string& fullfilename)
{
   //// first get the file name
   //std::string name = GetFileName(fullfilename);
   //if (name.length() != 0)
   //{
   //   // anything before the last '.' character is the extension
   //   int dotIdx = int(name.find_last_of(".", name.length()));
   //   if (dotIdx != -1)
   //   {
   //      return name.substr(0, dotIdx);
   //   }
   //}

   //// no extension found, so the file name is the name without extension
   //return name;

   return osgDB::getStrippedName(fullfilename);
}

std::string GetFileExtension(const std::string& fullfilename)
{
   // first get the file name
   //std::string name = GetFileName(fullfilename);
   //if (name.length() != 0)
   //{
   //   // anything after the last '.' character is the extension
   //   int dotIdx = int(name.find_last_of(".", name.length()));
   //   if (dotIdx != -1)
   //   {
   //      return name.substr(dotIdx+1, name.length()-(dotIdx+1));
   //   }
   //}

   //// no extension found
   //return std::string();

   return osgDB::getFileExtension(fullfilename);
}

void SetFileExtension(std::string& strFile, const std::string& extension)
{
   // get current extension
   //std::string ext = GetFileExtension(strFile);

   //// currently missing an extension
   //if (ext.empty())
   //{
   //   strFile += ".";
   //   strFile += extension;
   //}

   //// change the current extension to the one we want
   //else
   //{
   //   int idx = int(strFile.rfind(ext.c_str(), strFile.length()-1));
   //   strFile.replace(idx, ext.length(), extension.c_str(), extension.length());
   //}
   std::string fileNameNoExt = GetFileNameNoExt(strFile);
   strFile = fileNameNoExt + "." + extension;
}

void CleanupFileString(std::string& strFileOrDir)
{
   if (strFileOrDir.empty())
   {
      return;
   }

   // convert all separators to unix-style for conformity
   for (unsigned int i = 0; i < strFileOrDir.length(); ++i)
   {
      if (strFileOrDir[i] == '\\')
      {
         strFileOrDir[i] = '/';
      }
   }

   // get rid of trailing separators
   if (strFileOrDir[strFileOrDir.length()-1] == '/')
   {
      strFileOrDir = strFileOrDir.substr(0, strFileOrDir.length()-1);
   }
}

bool IsAbsolutePath(std::string strFileOrDir)
{
   // just in case, make sure we are using a valid path
   CleanupFileString(strFileOrDir);

   dtUtil::FileInfo info = dtUtil::FileUtils::GetInstance().GetFileInfo(strFileOrDir);
   if (info.fileType == dtUtil::FILE_NOT_FOUND)
   {
      return false;
   }

   // handle unix-style paths
   if (strFileOrDir.length() > 0 && strFileOrDir[0] == '/')
   {
      return true;
   }

   // handle windows-style paths
   else if (strFileOrDir.length() > 1 && strFileOrDir[1] == ':')
   {
      return true;
   }

   return false;
}

std::string RelativeToAbsolutePath(std::string relative, std::string parent)
{
   std::string absolute;

   // no parent specified, use current working directory
   if (parent.empty())
   {
      dtUtil::FileUtils& fileUtils = dtUtil::FileUtils::GetInstance();
      parent = fileUtils.CurrentDirectory();
   }

   // cleanup starting strings
   CleanupFileString(parent);
   CleanupFileString(relative);

   // handle empty relative path
   if (relative.empty())
   {
      return parent;
   }

   // handle wildcards
   if (relative.length() >= 2 && relative.compare(0, 2, "./") == 0)
   {
      relative = relative.substr(2, relative.length() - 2);
   }
   else if (relative.length() >= 3)
   {
      while (relative.length() >= 3 && relative.compare(0,3,"../") == 0)
      {
         relative = relative.substr(3, relative.length() - 3);
         int separatorIdx = int(parent.find_last_of("/", parent.length()));
         if (separatorIdx != -1)
         {
            parent = parent.substr(0, separatorIdx);
         }
      }
   }

   // simply append relative to parent
   absolute = parent + '/' + relative;
   return absolute;
}

void MakeDirectoryEX(std::string strDir)
{
   if (strDir.empty())
   {
      return;
   }

   // first make sure the directory string is clean
   CleanupFileString(strDir);

   // this generated string is used to check each subdirectory in the path
   std::string dir;

   // prepare tokenizer
   char* buffer = new char[strDir.length()+1];
   strcpy(buffer, strDir.c_str());
   char* tok = strtok(buffer, "/");

   // setup root directory
   if (strDir[0] == '/')
   {
      dir = "/";
      dir += tok;
   }
   else
   {
      dir = tok;
      dir += "/";
   }

   // traverse each subdirectory in path and create directories as needed
   dtUtil::FileUtils& fileUtils = dtUtil::FileUtils::GetInstance();
   while (tok)
   {
      // debug
      //std::cout << tok << std::endl;

      // create directory if necessary
      if (!fileUtils.DirExists(dir))
      {
         fileUtils.MakeDirectory(dir);
      }

      // get next subdirectory
      tok = strtok(NULL, "/");
      if (tok)
      {
         if (dir[dir.length()-1] != '/') { dir += '/'; }
         dir += tok;
      }
   }

   // cleanup
   delete [] buffer;
}

std::string ResolveEnvironmentVariables(std::string path)
{
   std::string startToken = "$(";
   std::string endToken = ")";

   // find first opening environment variable token
   size_t startPos = path.find(startToken, 0);
   if (startPos == std::string::npos)
   {
      // no environment variable starting with "$(" found; return inputString unchanged
      return path;
   }

   // find first closing environment variable token
   size_t endPos = path.find(endToken, startPos);
   if (endPos == std::string::npos)
   {
      // no ending parenthesis ")" found; return inputString unchanged
      std::cout << "Invalid environment variable: no closing parenthesis before end of string" << std::endl;
      return path;
   }

   // we need to check that the environment variable has a closing parenthesis before
   // the next directory token (slash/backslash); for example, a path like:
   //
   //    $(DELTA_ROOT/utility/$(SOME_OTHER_VAR)/bin
   // or
   //    $(DELTA_ROOT$(SOME_OTHER_VAR)/bin
   //
   // should throw an error because there is no closing parenthesis after $(DELTA_ROOT

   // check that closing parenthesis is found before next directory forward slash (linux-style)
   size_t nextSlash = path.find("/", startPos);
   if (nextSlash != std::string::npos && nextSlash < endPos)
   {
      std::cout << "Invalid environment variable: no closing parenthesis before forward slash" << std::endl;
      return path;
   }

   // check that closing parenthesis is found before next directory back slash (windows-style)
   size_t nextBackslash = path.find("\\", startPos);
   if (nextBackslash != std::string::npos && nextBackslash < endPos)
   {
      std::cout << "Invalid environment variable: no closing parenthesis before back slash" << std::endl;
      return path;
   }

   // check that closing parenthesis is found before next environment variable opening token
   size_t nextEnvVarToken = path.find(startToken, startPos + startToken.length());
   if (nextEnvVarToken != std::string::npos && nextEnvVarToken <= endPos)
   {
      std::cout << "Invalid environment variable: no closing parenthesis before start of next environment variable" << std::endl;
      return path;
   }

   // now replace the environment variable in the path with its value

   // extract first environment variable from string
   std::string envVar = path.substr(startPos + 2, endPos - (startPos + 2));

   // look up value for environment variable
   char* envValue = getenv(envVar.c_str());

   if (envValue != NULL)
   {
      path.replace(startPos, endPos - (startPos - 1), envValue);

      // recursively check for other environment variables
      path = ResolveEnvironmentVariables(path);
   }
   else
   {
      std::cout << "Invalid environment variable: '" << envVar.c_str() << "'" << std::endl;
   }

   return path;
}

std::string ToUpperCase(const std::string& str)
{
   std::string returnString = str;

   for (unsigned int i = 0; i < returnString.length(); ++i)
   {
      returnString[i] = toupper(returnString[i]);
   }

   return returnString;
}

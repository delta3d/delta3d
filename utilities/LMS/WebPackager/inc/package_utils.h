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
#ifndef PACKAGER_PACKAGE_UTILS
#define PACKAGER_PACKAGE_UTILS

// ansi
#include <string>
#include <map>


//======================================
// UTILITY CLASSES
//======================================

/**
 * @class Options This class wraps the functionality of a (string,string) tuple
 * where the second string value can be referenced by the first. Internally it
 * just uses a map<string,string>, but this class makes it easier to define and
 * use such functionality.
 */
class Options
{
   public:
      Options();
      Options( const Options &src );
      Options &operator=( const Options &src );
      ~Options();

      /**
       * Retrieves a specified option.
       * @param key The option whose value to return.
       * @return Returns empty string upon invalid option.
       */
      const std::string& Get( const std::string &key );

      /**
       * Retrieves an option by index.
       * @param idx The index of the option to get.
       * @return Returns empty string upon invalid index.
       */
      std::string Get( unsigned int idx );

      /**
       * Sets the value of a specified option.
       * @param key The option to set.
       * @param value The value to set it to.
       */
      void Set( const std::string &key, const std::string &value );

      /**
       * Check whether there are any options or not.
       * @return True if contains options.
       */
      bool IsEmpty();

      /**
       * Retrieves the number of options stored.
       * @return 0 if no options stored.
       */
      unsigned int GetCount();

   private:
      std::map<std::string, std::string> mOptions;
};


//======================================
// FILE FUNCTIONS
//======================================

/**
 * Returns the path part of a file string.
 * @param strFileOrDir A full filename path.
 * @return Empty string if no path found.
 */
std::string GetFilePath( const std::string &strFileOrDir );

/**
 * Returns the short filename part of a file string.
 * @param strFileOrDir A full filename path.
 * @return Empty string if no short name.
 */
std::string GetFileName( const std::string &strFileOrDir );

/**
 * Returns the short filename part of a file string, without extension.
 * @param strFileOrDir A full filename path.
 * @return Empty string if no short name.
 */
std::string GetFileNameNoExt( const std::string &strFileOrDir );

/**
 * Returns only the extension part of a file string.
 * @param strFileOrDir A full filename path.
 * @return Empty string if no extension found.
 * @note Does not include the '.' character.
 */
std::string GetFileExtension( const std::string &strFileOrDir );

/**
 * Ensure the the passed in string has the specified extension. If the passed in
 * string has a different extension, it is replaced. If the passed in string has
 * no extension, then it is appended.
 * @param strFile The string whose extension we are changing.
 * @param extension The desired extension.
 * @note The original string passed in is changed.
 * @note DO NOT include "." character when specifying extension.
 */
void SetFileExtension( std::string &strFile, const std::string &extension );

/**
 * Ensure that the passed in string is fit for use as a file or dir string.
 * In our case we want all separators to be the '/' character and we do not want
 * a separator at the end.
 * @param strFileOrDir The string to cleanup.
 * @note The original string passed in is changed.
 */
void CleanupFileString( std::string &strFileOrDir );

/**
 * Query whether a given string is an absolute path or not.
 * @param strFileOrDir The path to check.
 * @return True if absolute, False if relative.
 * @note This maybe a subjective determination, may need to add to this later.
 */
bool IsAbsolutePath( std::string strFileOrDir );

/**
 * Cleanly appends a relative path onto a parent path.
 * @param relative The path relative to the parent.
 * @param parent The parent path of the relative path.
 * @return Final absolute path.
 * @note If no parent path specified, uses the current working directory.
 */
std::string RelativeToAbsolutePath( std::string relative, std::string parent = "" );

/**
 * A more powerful version of the standard mkdir.  This function will check to
 * see if the directory exists first and only create if needed.  Also, it will
 * recursively create all subdirectories needed to create the final directory in
 * the passed in string.
 * @param strDir The directory to create.
 */
void MakeDirectoryEX( std::string strDir );

/**
 * Replaces the environment variables in a given path with their current values.
 * @param path The input path string to resolve environment variables in.
 * @return A string containing the input path with its environment variables resolved.
 */
std::string ResolveEnvironmentVariables( std::string path );

/**
 * Converts a string to upper case
 * @param str The input string to convert to upper case.
 * @return A string converted to upper case.
 */
extern std::string ToUpperCase( const std::string &str );

#endif


/*
* Delta3D Open Source Game and Simulation Engine
* Copyright (C) 2004-2006 MOVES Institute
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
* Jeff Houde 09/21/2009
*/

#ifndef DELTA_PACKAGER_UTIL
#define DELTA_PACKAGER_UTIL


#include <string>
#include <vector>

#include <dtUtil/export.h>

namespace dtUtil
{
   /**
   * The Packager is used to package multiple files into a
   * single .dtpkg file.  Using the packager will allow you
   * to view, import, and extract to and from the contents
   * of a package file.
   */
   class DT_UTIL_EXPORT Packager
   {
   public:

      struct PackTreeData
      {
         std::string name;
         std::string source;

         bool          isFromPack;
         fpos_t        seekPos;
         
         PackTreeData* parent;

         std::vector<PackTreeData> folders;
         std::vector<PackTreeData> files;
      };

      /**
      * Default constructor.
      */
      Packager();

      /**
      * Default deconstructor.
      */
      ~Packager();

      /**
      * Opens an existing package file.  You don't need this if you are
      * creating a new package.
      *
      * @param[in]  filename  The name of the file to open.
      *
      * @return     This will return false if the package was not found.
      */
      bool OpenPackage(const std::string& filename);

      /**
      * Packs the currently added files to a package.
      *
      * @param[in]  filename   The name of the package file (exclude the ext)
      * @param[in]  overwrite  True to overwrite any existing package files.
      *
      * @return     Returns false if the package was not saved.
      */
      bool PackPackage(const std::string& filename, bool overwrite = true);

      /**
      * Unpacks a package to a specified location.
      *
      * @param[in]  filename  The name of the pack file to unpack (don't include extension).
      * @param[in]  outDir    The output directory to unpack to.
      */
      bool UnpackPackage(const std::string& filename, const std::string& outDir);

      /**
      * Closest the currently openned package.
      *
      * @return  Fails if no package was opened.
      */
      bool ClosePackage();

      /**
      * Adds a specified file to the package.
      *
      * @param[in]  filepath  The path of the file to add.
      * @param[in]  outDir    The output directory when this file is unpacked.
      *                       (this is relative to the extraction directory)
      */
      bool AddFile(const std::string& filepath, const std::string& outDir);

      /**
      * Removes a file that is currently packaged.
      *
      * @param[in]  filepath  The path of the file to remove.
      *                       Note: this is not the path of the source file,
      *                       it is the path you used when adding the file
      *                       to this package.
      */
      bool RemoveFile(const std::string& filepath);

      /**
      * Retrieves the file structure tree of all files that
      * are currently bound to be packed into a package.
      */
      const PackTreeData& GetPackTree() {return mTree;}

      /**
      * This will parse the PackTreeData structure and
      * find the node to the given path.  If none exists, this
      * will return NULL.
      */
      PackTreeData* FindPackDataForPath(const std::string& path);

   private:

      /**
      * This will parse the PackTreeData structure and
      * find the node to the given path.  If none exists, this
      * will create one.
      */
      PackTreeData* CreatePackDataForPath(const std::string& path);

      /**
      * Recursive function that iterates the pack tree data
      * and packs a package.
      *
      * @param[in]  tree      The tree node to recurse through.
      * @param[in]  packFile  The package file to save to.
      * @param[in]  path      The current path.
      */
      bool PackPackage(PackTreeData* tree, FILE* packFile, const std::string& path);

      /**
      * Recursive function that iterates the package and unpacks it.
      *
      * @param[in]  file    The pack file to unpack.
      * @param[in]  outDir  The output directory to unpack all files to.
      */
      bool UnpackPackage(FILE* packFile, const std::string& outDir);

      /**
      * Packs the binary data of the file to the package.
      *
      * @param[in]  tree      The tree node of the file.
      * @param[in]  packFile  The package file to save to.
      */
      bool PackFile(PackTreeData* tree, FILE* packFile);

      /**
      * Unpacks a file from the package.
      *
      * @param[in]  file    The pack file to read from.
      * @param[in]  outDir  The output directory to place the file.
      */
      bool UnpackFile(FILE* packFile, const std::string& outDir);

      /**
      * Writes a string to the file.
      *
      * @param[in]  str   The string to write.
      * @param[in]  file  The file to write to.
      */
      bool WriteString(const std::string& str, FILE* file);

      /**
      * Reads a string from the file.
      *
      * @param[out]  str   The string that was read.
      * @param[in]   file  The file to read from.
      */
      bool ReadString(std::string& str, FILE* file);

      std::string    mPackagePath;
      FILE*          mPackage;
      PackTreeData   mTree;

      bool           mOpeningPackage;
   };
}

#endif // DELTA_PACAKGER_UTIL

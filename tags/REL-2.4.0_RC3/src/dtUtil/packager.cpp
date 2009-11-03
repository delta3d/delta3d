#include <dtUtil/packager.h>

#include <dtUtil/fileutils.h>

#include <osgDB/FileNameUtils>
#include <stdio.h>
#include <string.h>

namespace dtUtil
{
   ////////////////////////////////////////////////////////////////////////////////
   Packager::Packager()
   {
      mPackage = NULL;
      mOpeningPackage = false;

      mTree.name = "";
      mTree.source = "";
      mTree.isFromPack = false;
      mTree.parent = NULL;
   }

   ////////////////////////////////////////////////////////////////////////////////
   Packager::~Packager()
   {
      ClosePackage();
   }

   ////////////////////////////////////////////////////////////////////////////////
   bool Packager::OpenPackage(const std::string& filename)
   {
      ClosePackage();

      mPackagePath = filename;
      mPackage = fopen(std::string(osgDB::getFilePath(mPackagePath) + '\\' + osgDB::getStrippedName(mPackagePath) + ".dtpkg").c_str(), "rb");
      if (!mPackage)
      {
         return false;
      }

      // Now parse this package and set up our pack tree.
      mOpeningPackage = true;
      UnpackPackage(mPackage, "");
      mOpeningPackage = false;

      return true;
   }

   ////////////////////////////////////////////////////////////////////////////////
   bool Packager::PackPackage(const std::string& filename, bool overwrite)
   {
      std::string tempFile = osgDB::getFilePath(filename) + '\\' + osgDB::getStrippedName(filename) + ".tmp";
      std::string finalFile = osgDB::getFilePath(filename) + '\\' + osgDB::getStrippedName(filename) + ".dtpkg";

      // If we are not overwriting, then make sure the package doesn't already exist.
      if (!overwrite)
      {
         FILE* file = NULL;
         file = fopen(tempFile.c_str(), "rb");
         if (file)
         {
            fclose(file);
            return false;
         }
      }

      // Now create the file.
      FILE* file = NULL;
      file = fopen(tempFile.c_str(), "wb");
      if (!file)
      {
         return false;
      }

      PackPackage(&mTree, file, "");
      fclose(file);

      // Make sure we close the current package if we need to write over it.
      if (mPackagePath == filename)
      {
         ClosePackage();
      }

      // now rename the temp file to the actual file name.
      dtUtil::FileUtils::GetInstance().FileMove(tempFile, finalFile, true);

      if (mPackagePath == filename)
      {
         OpenPackage(mPackagePath);
      }

      return true;
   }

   ////////////////////////////////////////////////////////////////////////////////
   bool Packager::UnpackPackage(const std::string& filename, const std::string& outDir)
   {
      std::string finalFile = osgDB::getFilePath(filename) + '\\' + osgDB::getStrippedName(filename) + ".dtpkg";

      FILE* file = NULL;
      file = fopen(finalFile.c_str(), "rb");
      if (!file)
      {
         return false;
      }

      std::string path = outDir;
      if (path.length() > 0 &&
         path[path.length()-1] != '\\' &&
         path[path.length()-1] != '/')
      {
         path += '/';
      }

      UnpackPackage(file, path);
      fclose(file);

      return true;
   }

   ////////////////////////////////////////////////////////////////////////////////
   bool Packager::ClosePackage()
   {
      if (mPackage)
      {
         fclose(mPackage);

         mPackage = NULL;
         return true;
      }

      mTree.files.clear();
      mTree.folders.clear();

      return false;
   }

   ////////////////////////////////////////////////////////////////////////////////
   bool Packager::AddFile(const std::string& filepath, const std::string& outDir)
   {
      // First make sure the file exists.
      if (!dtUtil::FileUtils::GetInstance().FileExists(filepath))
      {
         return false;
      }

      PackTreeData* tree = CreatePackDataForPath(outDir);
      if (!tree) return false;

      // Parse out directory from the file name.
      int letter = (int)filepath.length() - 1;
      for (; letter > 0; letter--)
      {
         if (filepath[letter] == '\\' || filepath[letter] == '/')
         {
            break;
         }
      }

      std::string filename = "";
      if (letter >= 0 && letter < (int)filepath.length() - 1)
      {
         if (letter == 0) filename = filepath;
         else filename = &filepath[letter + 1];
      }

      // First check to see if this file already exists in this directory...
      for (int index = 0; index < (int)tree->files.size(); index++)
      {
         if (tree->files[index].name == filename)
         {
            return false;
         }
      }

      PackTreeData data;
      data.name = filename;
      data.source = filepath;
      data.isFromPack = false;
      data.parent = tree;

      tree->files.push_back(data);

      return true;
   }

   ////////////////////////////////////////////////////////////////////////////////
   bool Packager::RemoveFile(const std::string& filepath)
   {
      // Parse out directory from the file name.
      int letter = (int)filepath.length() - 1;
      for (; letter > 0; letter--)
      {
         if (filepath[letter] == '\\' || filepath[letter] == '/')
         {
            break;
         }
      }

      std::string filename = "";
      std::string outDir = filepath;
      if (letter >= 0 && letter < (int)filepath.length() - 1)
      {
         if (letter == 0) filename = filepath;
         else filename = &filepath[letter + 1];
         outDir.resize(letter);
      }

      // Search for the file to be removed.
      PackTreeData* tree = FindPackDataForPath(outDir);
      if (!tree) return false;

      while (tree)
      {
         for (int index = 0; index < (int)tree->files.size(); index++)
         {
            if (!filename.empty() && tree->files[index].name == filename)
            {
               tree->files.erase(tree->files.begin() + index);

               break;
            }
         }

         // If this tree has no more files or folders in it, remove it.
         if (tree->files.size() <= 0 && tree->folders.size() <= 0 && tree->parent)
         {
            for (int index = 0; index < (int)tree->parent->folders.size(); index++)
            {
               if (tree->parent->folders[index].name == tree->name)
               {
                  tree->parent->folders.erase(tree->parent->folders.begin() + index);
                  break;
               }
            }

            tree = tree->parent;
            filename = "";
            continue;
         }

         tree = NULL;
      }

      return true;
   }

   ////////////////////////////////////////////////////////////////////////////////
   Packager::PackTreeData* Packager::FindPackDataForPath(const std::string& path)
   {
      int len = (int)path.length();
      int start = 0;

      if (len == 0) return &mTree;

      PackTreeData* tree = &mTree;

      // Parse each folder from the path.
      for (int index = 0; index < len; index++)
      {
         if (index == len-1 || path[index+1] == '\\' || path[index+1] == '/')
         {
            std::string folder;
            for (int letter = start; letter < index + 1; letter++)
            {
               folder = folder + path[letter];
            }
            start = index + 2;

            // Skip empty names.
            if (folder.empty())
            {
               continue;
            }

            bool bFound = false;
            for (int index = 0; index < (int)tree->folders.size(); index++)
            {
               if (tree->folders[index].name == folder)
               {
                  bFound = true;
                  tree = &tree->folders[index];
                  break;
               }
            }

            // If this directory was not found, then create it.
            if (!bFound)
            {
               return NULL;
            }
         }
      }

      return tree;
   }

   ////////////////////////////////////////////////////////////////////////////////
   Packager::PackTreeData* Packager::CreatePackDataForPath(const std::string& path)
   {
      int len = (int)path.length();
      int start = 0;

      if (len == 0) return &mTree;

      PackTreeData* tree = &mTree;

      // Parse each folder from the path.
      for (int index = 0; index < len; index++)
      {
         if (index == len-1 || path[index+1] == '\\' || path[index+1] == '/')
         {
            std::string folder;
            for (int letter = start; letter < index + 1; letter++)
            {
               folder = folder + path[letter];
            }
            start = index + 2;

            // Skip empty names.
            if (folder.empty())
            {
               continue;
            }

            bool bFound = false;
            for (int index = 0; index < (int)tree->folders.size(); index++)
            {
               if (tree->folders[index].name == folder)
               {
                  bFound = true;
                  tree = &tree->folders[index];
                  break;
               }
            }

            // If this directory was not found, then create it.
            if (!bFound)
            {
               PackTreeData data;
               data.name = folder;
               data.source = "";
               data.isFromPack = false;
               data.parent = tree;
               tree->folders.push_back(data);

               tree = &tree->folders.back();
            }
         }
      }

      return tree;
   }

   ////////////////////////////////////////////////////////////////////////////////
   bool Packager::PackPackage(Packager::PackTreeData* tree, FILE* packFile, const std::string& path)
   {
      if (!tree) return false;

      WriteString(path, packFile);

      // First import files into the package.
      int fileCount = (int)tree->files.size();
      fwrite(&fileCount, sizeof(int), 1, packFile);

      for (int index = 0; index < fileCount; index++)
      {
         PackFile(&tree->files[index], packFile);
      }

      // Next iterate through sub folders.
      int folderCount = (int)tree->folders.size();
      fwrite(&folderCount, sizeof(int), 1, packFile);

      for (int index = 0; index < folderCount; index++)
      {
         std::string nextPath;
         if (!path.empty()) nextPath = path + '/';
         nextPath += tree->folders[index].name;
         PackPackage(&tree->folders[index], packFile, nextPath);
      }

      return true;
   }

   ////////////////////////////////////////////////////////////////////////////////
   bool Packager::UnpackPackage(FILE* packFile, const std::string& outDir)
   {
      // First read the path.
      std::string dir;
      ReadString(dir, packFile);
      std::string path = outDir + dir;

      // Remove any trailing slashes.
      if(path.length() > 0 &&
         (path[path.length()-1] == '\\' || path[path.length()-1] == '/'))
      {
         path.resize(path.length()-1);
      }

      if (!mOpeningPackage && !path.empty())
      {
         // Create the output directory.
         if (!dtUtil::FileUtils::GetInstance().DirExists(path))
         {
            dtUtil::FileUtils::GetInstance().MakeDirectory(path);
         }
      }

      // Now iterate through all the files.
      int fileCount = 0;
      fread(&fileCount, sizeof(int), 1, packFile);

      for (int index = 0; index < fileCount; index++)
      {
         UnpackFile(packFile, path);
      }

      // Now iterate through all sub folders.
      int folderCount = 0;
      fread(&folderCount, sizeof(int), 1, packFile);

      for (int index = 0; index < folderCount; index++)
      {
         UnpackPackage(packFile, outDir);
      }

      return true;
   }

   ////////////////////////////////////////////////////////////////////////////////
   bool Packager::PackFile(PackTreeData* tree, FILE* packFile)
   {
      if (!tree) return false;

      FILE* file = NULL;
      long bufferLength = 0;

      if (tree->isFromPack && mPackage)
      {
         file = mPackage;
         fsetpos(file, &tree->seekPos);
         fread(&bufferLength, sizeof(long), 1, file);
      }
      else
      {
         file = fopen(tree->source.c_str(), "rb");
         if (!file)
         {
            return false;
         }

         dtUtil::FileInfo fileInfo = dtUtil::FileUtils::GetInstance().GetFileInfo(tree->source);

         bufferLength = (long)fileInfo.size;
         if (bufferLength <= 0) fclose(file);
      }

      if (bufferLength > 0)
      {
         char* buffer = new char[bufferLength];
         if (!buffer)
         {
            fclose(file);
            return false;
         }

         memset(buffer, 0, bufferLength);
         fread(buffer, bufferLength, sizeof(char), file);
         if (!tree->isFromPack) fclose(file);

         // Write the header for this file.
         WriteString(tree->name, packFile);
         fwrite(&bufferLength, sizeof(long), 1, packFile);
         fwrite(buffer, sizeof(char), bufferLength, packFile);

         if (buffer) delete buffer;
         return true;
      }

      return false;
   }

   ////////////////////////////////////////////////////////////////////////////////
   bool Packager::UnpackFile(FILE* packFile, const std::string& outDir)
   {
      std::string filename;
      ReadString(filename, packFile);

      std::string path = outDir + '/' + filename;

      fpos_t seekPos;
      fgetpos(packFile, &seekPos);

      long bufferLength = 0;
      fread(&bufferLength, sizeof(long), 1, packFile);

      char* buffer = NULL;
      if (bufferLength > 0)
      {
         buffer = new char[bufferLength];
         if (!buffer)
         {
            return false;
         }

         memset(buffer, 0, bufferLength);
         fread(buffer, bufferLength, sizeof(char), packFile);
      }

      if (mOpeningPackage)
      {
         // Create the tree node.
         PackTreeData* tree = CreatePackDataForPath(outDir);
         if (!tree)
         {
            return false;
         }

         PackTreeData data;
         data.name = filename;
         data.source = "";
         data.isFromPack = true;
         data.seekPos = seekPos;
         data.parent = tree;

         tree->files.push_back(data);

         if (buffer) delete buffer;
         return true;
      }
      else
      {
         FILE* file = NULL;
         file = fopen(path.c_str(), "wb");
         if (!file)
         {
            return false;
         }

         if (buffer)
         {
            fwrite(buffer, sizeof(char), bufferLength, file);
            fclose(file);

            delete buffer;
            return true;
         }

         fclose(file);
      }
      return false;
   }

   ////////////////////////////////////////////////////////////////////////////////
   bool Packager::WriteString(const std::string& str, FILE* file)
   {
      if (!file) return false;

      size_t len = str.length();
      fwrite(&len, sizeof(size_t), 1, file);

      if (len <= 0) return true;
      fwrite(str.c_str(), sizeof(char), len, file);

      return true;
   }

   ////////////////////////////////////////////////////////////////////////////////
   bool Packager::ReadString(std::string& str, FILE* file)
   {
      if (!file) return false;

      size_t len;
      fread(&len, sizeof(size_t), 1, file);

      if (len > 0)
      {
         str.resize(len);
         fread(&str[0], sizeof(char), len, file);
      }

      return true;
   }
}

////////////////////////////////////////////////////////////////////////////////

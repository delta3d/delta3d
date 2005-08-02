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

#if defined(_WIN32) || defined(WIN32) || defined(__WIN32__)
#   include <windows.h>
#   include <direct.h>
#   include <io.h>
#   define stat64 _stati64
#   define mkdir(x,y) _mkdir((x))
extern "C" int errno;
//Linux and Mac OS X
#else
#   include <sys/param.h>
#   include <errno.h>
#endif

//this define take from fileutils in osg.
#if defined( __APPLE__ ) || defined(__CYGWIN__) || defined(__FreeBSD__)
#define stat64 stat
#endif

#include <osg/Notify>
#include <stack>

#include "dtDAL/fileutils.h"
#include "dtDAL/exception.h"

#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <iostream>


#include <osgDB/FileNameUtils>

#ifndef S_ISREG
#define S_ISREG(x) (((x) & S_IFMT) == S_IFREG)
#endif

#ifndef S_ISDIR
#define S_ISDIR(x) (((x) & S_IFMT) == S_IFDIR)
#endif


namespace dtDAL
{

    osg::ref_ptr<FileUtils> FileUtils::mInstance;

#if defined(_WIN32) || defined(WIN32) || defined(__WIN32__)
    const char FileUtils::PATH_SEPARATOR = '\\';
#else
    const char FileUtils::PATH_SEPARATOR = '/';
#endif
 
	//temporary copy of osgDB::makeDirectory because of some bugs in it.
    bool iMakeDirectory( const std::string &path )
	{
		if (path.empty())
		{
			osg::notify(osg::DEBUG_INFO) << "osgDB::makeDirectory():  cannot create an empty directory" << std::endl;
			return false;
		}

		struct stat64 stbuf;
		if( stat64( path.c_str(), &stbuf ) == 0 )
		{
			if( S_ISDIR(stbuf.st_mode))
				return true;
			else
			{
				osg::notify(osg::DEBUG_INFO) << "osgDB::makeDirectory():  "  <<
						path << " already exists and is not a  directory!" << std::endl;
				return false;
			}
		}

		std::string dir = path;
		std::stack<std::string> paths;
		while( true )
		{
			if( dir.empty() )
				break;

			if( stat64( dir.c_str(), &stbuf ) < 0 )
			{
				switch( errno )
				{
					case ENOENT:
					case ENOTDIR:
						paths.push( dir );
						break;

					default:
						osg::notify(osg::DEBUG_INFO) <<  "osgDB::makeDirectory(): "  << strerror(errno) << std::endl;
						return false;
				}
			}
			else
			{
				break;
			}
			dir = osgDB::getFilePath(std::string(dir));
		}

		while( !paths.empty() )
		{
			std::string dir = paths.top();

			if( mkdir( dir.c_str(), 0755 )< 0 )
			{
				osg::notify(osg::DEBUG_INFO) << "osgDB::makeDirectory():  "  << strerror(errno) << std::endl;
				return false;
			}
			paths.pop();
		}
		return true;
	}
	
	//-----------------------------------------------------------------------
    bool FileUtils::FileExists( const std::string& strFile ) const
    {
        return GetFileInfo(strFile).fileType != FILE_NOT_FOUND;
    }

    //-----------------------------------------------------------------------
    void FileUtils::FileCopy( const std::string& strSrc, const std::string& strDest, bool bOverwrite ) const {

        FILE* pSrcFile;
        FILE* pDestFile;

        struct stat tagStat;

        if( strSrc != strDest )
        {

            if (!FileExists(strSrc))
                EXCEPT(ExceptionEnum::ProjectFileNotFound,
                       std::string("Source file does not exist: \"") + strSrc + "\"");

            //Open the source file for reading.
            pSrcFile = fopen( strSrc.c_str(), "rb" );
            if( pSrcFile == NULL )
            {
                EXCEPT(ExceptionEnum::ProjectIOException,
                       std::string("Unable to open source file for reading: \"") + strSrc + "\"");
            }

			try 
			{
				if (mLogger->IsLevelEnabled(dtUtil::Log::LOG_DEBUG))
					mLogger->LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__, __LINE__, "Source file exists.");

				std::string destFile = strDest;

				FileType ft = GetFileInfo(strDest).fileType;

				//Check to see if the destination is a file or directory.
				if (ft == DIRECTORY)
				{
					if (mLogger->IsLevelEnabled(dtUtil::Log::LOG_DEBUG))
						mLogger->LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__, __LINE__, "Destination is a directory.");

					//If the file is a directory, append the base name of the source file to the destination
					//to make the new file name.
					if (strDest[strDest.size()-1] != FileUtils::PATH_SEPARATOR)
						destFile = strDest + FileUtils::PATH_SEPARATOR + osgDB::getSimpleFileName(strSrc);
					else
						destFile = strDest + osgDB::getSimpleFileName(strSrc);
				}
				else
				{
					if (mLogger->IsLevelEnabled(dtUtil::Log::LOG_DEBUG))
						mLogger->LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__, __LINE__, "Destination is a file.");
				}

				if (FileExists(destFile) && !bOverwrite)
				{
					EXCEPT(ExceptionEnum::ProjectIOException,
						std::string("Destination file exists, but overwriting is turned off: \"") + destFile + "\"");
				}

				pDestFile = fopen( destFile.c_str(), "wb" );

				if( pDestFile == NULL )
				{
					//make sure to close the source file.
					EXCEPT(ExceptionEnum::ProjectIOException,
						std::string("Unable to open destination for writing: \"") + destFile + "\"");
				}

				try 
				{

					if (mLogger->IsLevelEnabled(dtUtil::Log::LOG_DEBUG))
						mLogger->LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__, "Destination opened for reading.");


					stat( strSrc.c_str(), &tagStat );
					long i = 0;
					char buffer[4096]; 
					while (i<tagStat.st_size)
					{
						int readCount = fread(buffer, 1, 4096, pSrcFile );
						if (readCount > 0) 
						{
							fwrite(buffer, 1, readCount, pDestFile );
							i += readCount;
						}
					}
					fclose( pDestFile );
					fclose( pSrcFile );
				}
				catch (dtDAL::Exception& ex1)
				{
					fclose( pDestFile );
					throw ex1;
				}
			}
			catch (dtDAL::Exception& ex)
			{
				fclose( pSrcFile );
				throw ex;
			}

        }
        //if the source equals the destination, this method is really a noop.

    }

    //-----------------------------------------------------------------------
    void FileUtils::FileMove( const std::string& strSrc, const std::string& strDest, bool bOverwrite ) const
    {
        if (GetFileInfo(strSrc).fileType != REGULAR_FILE)
            EXCEPT(ExceptionEnum::ProjectFileNotFound,
                   std::string("Source file was not found or is a Directory: \"") + strSrc + "\"");

        FileType ft = GetFileInfo(strDest).fileType;

        std::string destFile = strDest;

        //Check to see if the destination is a directory.
        if (ft == DIRECTORY)
        {
            if (mLogger->IsLevelEnabled(dtUtil::Log::LOG_DEBUG))
                mLogger->LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__, __LINE__, "Destination is a directory.");

            //Check to see if the destination is a file or directory.
            if (strDest[strDest.size()-1] != FileUtils::PATH_SEPARATOR)
                destFile = strDest + FileUtils::PATH_SEPARATOR + osgDB::getSimpleFileName(strSrc);
            else
                destFile = strDest + osgDB::getSimpleFileName(strSrc);

            ft = GetFileInfo(destFile).fileType;

        }
        else
        {
            if (mLogger->IsLevelEnabled(dtUtil::Log::LOG_DEBUG))
                mLogger->LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__, __LINE__, "Destination is a file.");
        }

        if (ft != FILE_NOT_FOUND && !bOverwrite)
            EXCEPT(ExceptionEnum::ProjectIOException,
                   std::string("Destination file exists and the call was not set to overwrite: \"") + strDest + "\"");


        //first check to see if the file can be moved without copying it.
        if (rename(strSrc.c_str(), destFile.c_str()) == 0)
            return;

        if (mLogger->IsLevelEnabled(dtUtil::Log::LOG_DEBUG))
            mLogger->LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__, "Rename failed, attempting to copy file and delete the source");


        //copy the file
        FileCopy( strSrc, strDest, bOverwrite );

        //attempt to delete the original file.
        if(unlink(strSrc.c_str()) != 0)
            EXCEPT(ExceptionEnum::ProjectIOException,
                   std::string("Unable to delete \"") + strSrc + "\" but file copied to new location.");

    }

    //-----------------------------------------------------------------------
    void FileUtils::FileDelete( const std::string& strFile ) const
    {
        FileType ft = GetFileInfo(strFile).fileType;

        //If the file does not exist, then ignore.
        if (ft == FILE_NOT_FOUND)
            return;

        if (ft != REGULAR_FILE)
            EXCEPT(ExceptionEnum::ProjectIOException,
                   std::string("File \"") + strFile + "\" is a directory.");


        if( unlink(strFile.c_str()) != 0)
            EXCEPT(ExceptionEnum::ProjectIOException,
                   std::string("Unable to delete \"") + strFile + "\".");
    }

    //-----------------------------------------------------------------------
    const struct FileInfo FileUtils::GetFileInfo( const std::string& strFile) const
    {
        struct FileInfo info;

        struct stat tagStat;
        if( stat( strFile.c_str(), &tagStat ) != 0 )
        {
            //EXCEPT(ExceptionEnum::ProjectFileNotFound, std::string("Cannot open file ") + strFile);
            info.fileType = FILE_NOT_FOUND;
            return info;
        }

        info.baseName = osgDB::getSimpleFileName(strFile);
        info.fileName = strFile;
        info.path = osgDB::getFilePath(strFile);

        info.size = tagStat.st_size;
        info.lastModified = tagStat.st_mtime;

        if (S_ISDIR(tagStat.st_mode))
            info.fileType = DIRECTORY;
        else
            //Anything else is a regular file, including special files
            //this is incomplete, but not a case that we deemed necessary to handle.
            //Symbolic links should NOT show up because stat was called, not lstat.
            info.fileType = REGULAR_FILE;

        return info;
    }


    //-----------------------------------------------------------------------
    void FileUtils::ChangeDirectory(const std::string& path)
    {
        ChangeDirectoryInternal(path);
        mStackOfDirectories.clear();
    }

    //-----------------------------------------------------------------------
    const std::string& FileUtils::CurrentDirectory() const
    {
        return mCurrentDirectory;
    }

    //-----------------------------------------------------------------------
    void FileUtils::ChangeDirectoryInternal(const std::string& path)
    {
        if (chdir(path.c_str()) == -1)
        {
            EXCEPT(ExceptionEnum::ProjectFileNotFound, std::string("Cannot open directory ") + path);
        }
        char buf[512];
        getcwd(buf, 512);
        mCurrentDirectory = buf;

        if (mLogger->IsLevelEnabled(dtUtil::Log::LOG_DEBUG))
        {
            std::string message("Changed directory to \"");
            message += mCurrentDirectory;
            message += "\".";
            mLogger->LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__, __LINE__, message.c_str());
        }
    }
    //-----------------------------------------------------------------------
    void FileUtils::PushDirectory(const std::string& path)
    {
        if (mLogger->IsLevelEnabled(dtUtil::Log::LOG_DEBUG))
        {
            std::string message("Pushing Directory \"");
            message += path;
            message += "\".";
            mLogger->LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__, __LINE__, message.c_str());
        }
        std::string old = mCurrentDirectory;
        ChangeDirectoryInternal(path);
        mStackOfDirectories.push_back(old);
    }

    //-----------------------------------------------------------------------
    void FileUtils::PopDirectory()
    {
        if(mLogger->IsLevelEnabled(dtUtil::Log::LOG_DEBUG))
        {
            mLogger->LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__, __LINE__, "Popping Directory.");
        }

        if (mStackOfDirectories.empty())
            return;

        ChangeDirectoryInternal(mStackOfDirectories.back());
        mStackOfDirectories.pop_back();
    }

    //-----------------------------------------------------------------------
    const std::string FileUtils::GetAbsolutePath(const std::string& relativePath) const
    {
        std::string result;
        std::string old = mCurrentDirectory;
        try
        {
            if (chdir(relativePath.c_str()) == -1)
            {
                EXCEPT(ExceptionEnum::ProjectFileNotFound, std::string("Cannot open directory ") + relativePath);
            }
            char buf[512];
            getcwd(buf, 512);
            result = buf;
        }
        catch (const Exception& ex)
        {
            if (chdir(old.c_str()) == -1)
            {
                mLogger->LogMessage(dtUtil::Log::LOG_ERROR, __FUNCTION__, __LINE__,
                    "Attempting to reset current directory to \"%s\", but an error occured doing so.", old.c_str());
            }
            throw ex;
        }
        if (chdir(old.c_str()) == -1)
        {
            mLogger->LogMessage(dtUtil::Log::LOG_ERROR, __FUNCTION__, __LINE__,
                "Attempting to reset current directory to \"%s\", but an error occured doing so.", old.c_str());
        }
        return result;
    }

    //-----------------------------------------------------------------------
    DirectoryContents FileUtils::DirGetFiles( const std::string& path ) const
    {
        FileInfo ft = GetFileInfo(path);
        if (ft.fileType == FILE_NOT_FOUND)
            EXCEPT(ExceptionEnum::ProjectFileNotFound,
                   std::string("Path not Found: \"") + path + "\".");
        else if (ft.fileType == REGULAR_FILE)
            EXCEPT(ExceptionEnum::ProjectIOException,
                   std::string("Path does not specify a directory: \"") + path + "\".");

        return osgDB::getDirectoryContents(path);
    }

    //-----------------------------------------------------------------------
    DirectoryContents FileUtils::DirGetSubs( const std::string& path ) const {
        DirectoryContents vec;

        DirectoryContents dirCont = DirGetFiles(path);

        for (DirectoryContents::const_iterator i = dirCont.begin(); i != dirCont.end(); ++i) {
            if (GetFileInfo(path + PATH_SEPARATOR + *i).fileType == DIRECTORY && (*i != ".") && (*i != "..")) {
                vec.push_back(*i);
            }
        }

        return vec;
    }


    void FileUtils::InternalDirCopy(const std::string& srcPath,
        const std::string& destPath, bool bOverwrite) const
    {
        FileType destFileType = GetFileInfo(destPath).fileType;
        //std::cout << "Copying " << srcPath << " to " << destPath << std::endl;

        if (destFileType == REGULAR_FILE)
            EXCEPT(ExceptionEnum::ProjectFileNotFound,
                   std::string("The destination path must be a directory: \"") + destPath + "\"");

        if (destFileType == FILE_NOT_FOUND)
            MakeDirectory(destPath);

        DirectoryContents contents = DirGetFiles(srcPath);
        for (DirectoryContents::iterator i = contents.begin(); i != contents.end(); ++i)
        {
            if (*i == "." || *i == "..")
                continue;
            const std::string newSrcPath = srcPath + PATH_SEPARATOR + *i;
            const std::string newDestPath = destPath + PATH_SEPARATOR + *i;
            FileInfo fi = GetFileInfo(newSrcPath);
            if (fi.fileType == DIRECTORY)
            {
                if (destPath.size() >= newSrcPath.size() &&
                    destPath.substr(0, newSrcPath.size()) == newSrcPath &&
                    (destPath.size() == newSrcPath.size() || destPath[newSrcPath.size()] == PATH_SEPARATOR))
                {
                    if (mLogger->IsLevelEnabled(dtUtil::Log::LOG_WARNING))
                        mLogger->LogMessage(dtUtil::Log::LOG_WARNING, __FUNCTION__, __LINE__, "Can't copy %s into itself.", newSrcPath.c_str());
                }
                else
                {
                    InternalDirCopy(newSrcPath, newDestPath, bOverwrite);
                }
            }
            else
            {
                FileCopy(newSrcPath, newDestPath, bOverwrite);
            }
        }
    }

    //-----------------------------------------------------------------------
    void FileUtils::DirCopy(const std::string& srcPath,
        const std::string& destPath, bool bOverwrite, bool copyContentsOnly) const
    {

        if (!DirExists(srcPath))
            EXCEPT(ExceptionEnum::ProjectFileNotFound,
                   std::string("Source directory does not exist: \"") + srcPath + "\"");

        FileType destFileType = GetFileInfo(destPath).fileType;

        if (destFileType == REGULAR_FILE)
            EXCEPT(ExceptionEnum::ProjectFileNotFound,
                   std::string("The destination path must be a directory: \"") + destPath + "\"");

        bool createDest = destFileType == FILE_NOT_FOUND;

        std::string fullSrcPath = GetAbsolutePath(srcPath);
        //from here, the code can assume srcPath exists.
        if (mLogger->IsLevelEnabled(dtUtil::Log::LOG_DEBUG))
            mLogger->LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__, __LINE__, "Source directory \"%s\" exists.", fullSrcPath.c_str());

        std::string fullDestPath;

        if (createDest)
        {
            MakeDirectory(destPath);
            fullDestPath = GetAbsolutePath(destPath);
            if (mLogger->IsLevelEnabled(dtUtil::Log::LOG_DEBUG))
                mLogger->LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__, __LINE__, "Destination directory \"%s\" has been created.",
                                    fullDestPath.c_str());
        }
        else
        {
            fullDestPath = GetAbsolutePath(destPath);
            if (mLogger->IsLevelEnabled(dtUtil::Log::LOG_DEBUG))
                mLogger->LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__, __LINE__, "Destination directory \"%s\" exists.",
                                    fullDestPath.c_str());

            if ( (copyContentsOnly && fullSrcPath == fullDestPath)
                  || (!copyContentsOnly && osgDB::getFilePath(fullSrcPath) == fullDestPath) )
            {
                EXCEPT(ExceptionEnum::ProjectException,
                       std::string("The source equals the destination: \"") + srcPath + "\"");
            }

            if (!copyContentsOnly)
            {
                const std::string& srcName = osgDB::getSimpleFileName(fullSrcPath);
                fullDestPath += PATH_SEPARATOR + srcName;
                if (!DirExists(fullDestPath))
                    MakeDirectory(fullDestPath);

                if (mLogger->IsLevelEnabled(dtUtil::Log::LOG_DEBUG))
                    mLogger->LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__, __LINE__,
                        "Destination directory \"%s\" created - copyContentsOnly is false.",
                        fullDestPath.c_str());
            }
            else
            {
                if (mLogger->IsLevelEnabled(dtUtil::Log::LOG_DEBUG))
                    mLogger->LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__, __LINE__,
                        "Destination directory \"%s\" exists - copyContentsOnly is true.",
                        fullDestPath.c_str());
            }
        }


        InternalDirCopy(fullSrcPath, fullDestPath, bOverwrite);
    }

    //-----------------------------------------------------------------------
    bool FileUtils::DirDelete( const std::string& strDir, bool bRecursive )
    {
        if (bRecursive)
        {
            if (mLogger->IsLevelEnabled(dtUtil::Log::LOG_DEBUG))
                mLogger->LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__, __LINE__,
                "Attempting to recursively delete %s.", strDir.c_str());
            try
            {
                PushDirectory(strDir);
                try
                {
                    RecursDeleteDir(true);
                }
                catch(const Exception& ex)
                {
                    PopDirectory();
                    throw ex;
                }

                PopDirectory();
            }
            catch (const Exception& ex)
            {
                //if we get a file not found trying to recurse into the top directory
                //then the directory does not exist, so there is no need to throw an exception.
                if (ex.TypeEnum() == ExceptionEnum::ProjectFileNotFound && !DirExists(strDir))
                {
                    if (mLogger->IsLevelEnabled(dtUtil::Log::LOG_DEBUG))
                        mLogger->LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__, __LINE__,
                        "Directory %s doesn't exist to delete. Ignoring.", strDir.c_str());
                    return true;
                }
                else
                    throw ex;
            }
        }
        else
        {
            if (mLogger->IsLevelEnabled(dtUtil::Log::LOG_DEBUG))
                mLogger->LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__, __LINE__,
                "Attempting to delete %s, but not recursively.", strDir.c_str());
        }

        errno = 0;
        if( rmdir( strDir.c_str() ) != 0 )
        {
            if (!bRecursive && errno == ENOTEMPTY)
            {
                return false;
            }
            else if (errno == ENOENT)
            {
                if (mLogger->IsLevelEnabled(dtUtil::Log::LOG_DEBUG))
                    mLogger->LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__, __LINE__,
                    "Directory %s doesn't exist to delete. Ignoring.", strDir.c_str());
                return true;
            }
            else
            {
                EXCEPT(ExceptionEnum::ProjectIOException,
                    std::string("Unable to delete directory \"") + strDir + "\":" + strerror(errno));

            }
        }

        return true;
    }

    void FileUtils::MakeDirectory(const std::string& strDir) const
    {
        if (!iMakeDirectory(strDir))
        {
            FileType ft = GetFileInfo(strDir).fileType;
            if (ft == REGULAR_FILE)
                EXCEPT(ExceptionEnum::ProjectIOException, std::string("Cannot create directory. ")
                    + strDir + " is an existing non-directory file.");
            else if (ft == DIRECTORY)
            {
                return;
            }

            if (!DirExists(osgDB::getFilePath(strDir)))
            {
                EXCEPT(ExceptionEnum::ProjectFileNotFound, std::string("Cannot create directory ")
                    + strDir + ". Parent directory doesn't exist.");
            }
            else
            {
                EXCEPT(ExceptionEnum::ProjectIOException, std::string("Cannot create directory ") + strDir + ".");
            }
        }
    }


    //-----------------------------------------------------------------------
    //void FileUtils::dirMove( const std::string& strSrc, const std::string& strDest, bool bOverwrite )
    //{
    //    return true;
    //};

    //-----------------------------------------------------------------------
    bool FileUtils::DirExists( const std::string& strDir ) const
    {
        return GetFileInfo(strDir).fileType == DIRECTORY;
    }

        //-----------------------------------------------------------------------
    void FileUtils::RecursDeleteDir( bool bRecursive )
    {
        //this method assumes one is IN the directory that you want to delete.
        DirectoryContents dirCont = DirGetFiles(mCurrentDirectory);

        //iterate over all of the directory contents.
        for(DirectoryContents::const_iterator i = dirCont.begin(); i != dirCont.end(); ++i)
        {
            FileType ft = GetFileInfo(*i).fileType;
            if (ft == REGULAR_FILE)
			{
                //Delete regular files.
				errno = 0;
				if (unlink(i->c_str()) < 0) 
				{
					EXCEPT(ExceptionEnum::ProjectIOException,
						std::string("Unable to delete directory \"") + *i + "\":" + strerror(errno));
					
				}
			}
            else if ((*i != ".") && (*i != "..") && ft == DIRECTORY && bRecursive )
            {
                //if it's a directory and it's not the "." or ".." special directories,
                //change into that directory and recurse.
                ChangeDirectoryInternal(*i);
                RecursDeleteDir( true );
                //now that the directory is empty, remove it.
				errno = 0;
				if( rmdir( i->c_str() ) != 0 )
				{
					if (errno == ENOENT)
					{
						if (mLogger->IsLevelEnabled(dtUtil::Log::LOG_DEBUG))
							mLogger->LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__, __LINE__,
							"Directory %s doesn't exist to delete. Ignoring.", (mCurrentDirectory + PATH_SEPARATOR + *i).c_str());
						return;
					}
					else
					{
						EXCEPT(ExceptionEnum::ProjectIOException,
							std::string("Unable to delete directory \"") + mCurrentDirectory + PATH_SEPARATOR + *i + "\":" + strerror(errno));

					}
				}
            }
        }

        //change up so that when the method ends, the directory is ready to be removed.
        ChangeDirectoryInternal(std::string(".."));
    }

    //-----------------------------------------------------------------------
    FileUtils::FileUtils()
    {
        mLogger = &dtUtil::Log::GetInstance(std::string("fileutils.cpp"));
        //assign the current directory
        ChangeDirectory(".");
    }

    //-----------------------------------------------------------------------
    FileUtils::~FileUtils() {}

}

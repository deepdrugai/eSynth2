/*
 *  This file is part of esynth.
 *
 *  esynth is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  esynth is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with esynth.  If not, see <http://www.gnu.org/licenses/>.
 */

 /*
  * eSynth 2.0
  * Author: C. Alvin 7/27/2022
  */

#ifndef _FILE_UTILITIES_GUARD
#define _FILE_UTILITIES_GUARD 1

// Requires C++-17
#include <filesystem>

#include <algorithm>
#include <string>

#include "Utilities.h"

//
// This class bifurcates the input list into valid / invalid files based on existence.
// We assume these file names have been screened as valid for eSynth by InfileNameAnalyzer.
//
class FileUtilities
{
  public:
    /*
     * Verify that each of the valid files exists at the specified location
     *
     * @input: path including a file name (that has already been screened as being acceptable)
     */
    static bool exists(const std::string& path)
    {
        // Requires C++-17:
        return std::filesystem::exists(path);
    }

    /*
     * Given a file path, extract the file name
     */
    static std::string getFileFromPath(const std::string& p)
    {
        // Requires C++-17:
        std::filesystem::path thepath = p;

        return thepath.filename().string();
    }

    //static std::string getLastDirectoryFromPath(const std::string& path)
    //{
    //    // Acquire the file name by removing all path elements: preceding '/', '\'
    //    std::string prefix = "";
    //    std::string fileName = "";
    //    if (path.find("/") != std::string::npos)
    //    {
    //        Utilities::splitLast(path, prefix, fileName, "/");
    //        return fileName;
    //    }
    //    if (path.find("\\") != std::string::npos)
    //    {
    //        Utilities::splitLast(path, prefix, fileName, "\\");
    //        return fileName;
    //    }

    //    // No paths specified; default to the input file name
    //    return path;
    //}

    /*
     *
     * If the directory exists, the file/path is not valid for writing.
     *    return an incremental value with the same name prefix
     */
    static std::string getSuffixedDirectory(const std::string& name)
    {
        // If the directory does not exist, do nothing
        if (!exists(name)) return name;

        //
        // Construct a new name with a 'count' suffix
        // file-1, file-2, etc.
        //
        // Get the 'file': lowest level name
        // e.g.   /var/tmp  -> tmp
        // e.g.   /var/tmp/ -> tmp
        //
        std::filesystem::path p{ name };
        std::filesystem::path fName;
        std::filesystem::path parentPath;

        // Do we have a raw directory name given (not  xyxy/ , but q/w/e/r/hereiam)
        if (p.has_filename())
        {
            if (p.has_parent_path()) parentPath = p.parent_path();
            fName = p.filename();
        }
        else
        {
            fName = p.parent_path().filename();
            if (fName.has_parent_path()) parentPath = fName.parent_path();
        }

        // No file name or parent directory provided
        if (fName.string().empty()) return "";

        // Replace the filename with the suffixed name (maintaining the given path)
        return parentPath.replace_filename(getSuffixedFile(fName.string())).string();
    }

    /*
     * return an incremental value with the same name prefix
     */
    static std::string getSuffixedFile(const std::string& name)
    {
        //
        // Does it have a '-X' suffix already? If so, find it.
        //
        std::string prefix = "";
        std::string suffix = "";
        Utilities::splitLast(name, prefix, suffix, "-");

        if (suffix.empty() || !isInteger(suffix)) return name + "-1";

        // Increment the suffix
        return prefix + "-" + std::to_string(atoi(suffix.c_str()) + 1);
    }

    static bool isInteger(const std::string& str)
    {
        if (str.empty()) return false;

        return std::find_if(str.begin(),
                            str.end(),
                            [](unsigned char c) { return !std::isdigit(c); }) == str.end();
    }
};

#endif

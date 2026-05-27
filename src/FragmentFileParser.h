#ifndef _FRAGMENT_FILE_PARSER_GUARD
#define _FRAGMENT_FILE_PARSER_GUARD 1

#include <string>
#include <vector>
#include <iostream>

// TODO: Guards around selecting C++ standard

// #ifdef __cplusplus

// Requires C++-14
#define _SILENCE_EXPERIMENTAL_FILESYSTEM_DEPRECATION_WARNING
#include <experimental/filesystem>
namespace fs = std::experimental::filesystem;

// Requires C++-17
//#include <filesystem>
// namespace fs = std::filesystem;

#include <openbabel/obconversion.h>

#include "Utilities.h"
#include "Constants.h"

class FragmentFileParser
{
protected:
    std::map<std::string, Constants::FRAGMENT_TYPE> _valid;
    std::vector<std::string> _invalid;
    std::vector<Brick *> _bricks;
    std::vector<Linker *> _linkers;
    std::vector<std::string> _invalidFileNames;
    std::map<std::string, Constants::FRAGMENT_TYPE> _validFileNames;

public:
    std::vector<Brick *> getBricks() const { return _bricks; }
    std::vector<Linker *> getLinkers() const { return _linkers; }
    std::vector<std::string> getInvalidFileNames() const { return _invalidFileNames; }
    std::map<std::string, Constants::FRAGMENT_TYPE> getValidFiles() const { return _validFileNames; }

public:
    FragmentFileParser(const std::map<std::string, Constants::FRAGMENT_TYPE> &infiles) : _bricks{}, _linkers{}, _invalidFileNames{}, _validFileNames{}
    {
        readInputFiles(infiles);
    }

protected:
    //
    // Parse each input data files
    //
    bool readInputFiles(const std::map<std::string, Constants::FRAGMENT_TYPE> &infiles)
    {
        for (auto const &infile : infiles)
        {
            readMoleculeFile(infile.first.c_str(), infile.second);
        }

        return true;
    }

    void readMoleculeFile(const char *fileName, Constants::FRAGMENT_TYPE fType)
    {
        //
        // Input parser conversion functionality for Open babel
        //
        OpenBabel::OBConversion obConversion;
        obConversion.SetInFormat("SDF");

        //
        // Open the file, split the current molecule into Molecule Data (prefix)
        // and Our Data (Suffix)
        //
        std::ifstream infile;
        infile.open(fileName);

        std::string name = "UNKNOWN";
        std::string prefix = "";
        std::string suffix = "";

        while (!infile.eof() && infile.good() && splitMolecule(infile, name, prefix, suffix))
        {
            //
            // If the name of molecule is not given, overwrite it with
            // the name of the file.
            //
            if (name == "UNKNOWN")
            {
                name = "####   ";
                name += fileName;
                name += "    ####";
            }

            if (g_debug_output)
                std::cerr << "Name: " << std::endl
                          << name << std::endl;
            if (g_debug_output)
                std::cerr << "Prefix: " << std::endl
                          << prefix << std::endl;
            if (g_debug_output)
                std::cerr << "Suffix: " << std::endl
                          << suffix << std::endl;

            // Create and parse using Open Babel
            OpenBabel::OBMol *mol = new OpenBabel::OBMol();
            // bool notAtEnd =
            obConversion.ReadString(mol, prefix);

            // Assign all needed data to the molecule (comment data)
            Molecule *local = createLocalMolecule(mol,
                                                  fType == Constants::FRAGMENT_TYPE::BRICK ? BRICK : LINKER,
                                                  name, suffix);

            if (!local->isValid())
            {
                _invalidFileNames.push_back(fileName);
            }
            else
            {
                // Adding the File Names that are parsed properly
                if (fType != Constants::FRAGMENT_TYPE::ERROR)
                    _validFileNames[fileName] = fType;

                if (g_debug_output)
                    std::cout << "Local: " << *local << "|" << std::endl;

                // Add to the linker or brick list as needed.
                addMolecule(fType, local);
            }

            // We don't keep a copy of the OpenBabel molecule anymore.
            delete mol;
        }
    }

    bool splitMolecule(std::ifstream &infile, std::string &name,
                       std::string &prefix, std::string &suffix)
    {

        prefix = "";
        suffix = "";

        std::string line = "";

        // Eat #### in large files (if it exists)
        eatWhiteLines(infile);
        if (infile.peek() == '#')
        {
            getline(infile, line);
            name = line;
            eatWhiteLines(infile);
        }

        getline(infile, line);
        prefix += line + '\n';

        // Nothing left to read...
        if (infile.eof() || infile.fail())
            return false;

        // Read the prefix (end indicated by END)
        while (line.find("END") == std::string::npos)
        {
            getline(infile, line);
            prefix += line + '\n';
        }

        // Add '$$$$' to the prefix.
        // prefix += "\n$$$$";

        // Set suffix equal to remainder of the file
        while (line.find("$$$$") == std::string::npos)
        {
            if (!getline(infile, line))
                break;
            suffix += line + '\n';
        }

        return true;
    }

    Molecule *createLocalMolecule(OpenBabel::OBMol *mol, MoleculeT mType,
                                  const std::string &name, std::string &suffix)
    {

        //
        // Add the suffix as comment data to the actual OBMol object.
        //
        OpenBabel::OBCommentData *cData = new OpenBabel::OBCommentData();
        cData->SetAttribute("Comment");
        cData->SetData(suffix);
        mol->SetData(cData);

        //
        // Create this particular molecule type based on the name of the file.
        //
        if (mType == LINKER)
        {
            return new Linker(mol, name);
        }
        else if (mType == BRICK)
        {
            return new Brick(mol, name);
        }

        return 0;
    }

    void addMolecule(Constants::FRAGMENT_TYPE fType, Molecule *molecule)
    {

        switch (fType)
        {
        case Constants::FRAGMENT_TYPE::BRICK:
            _bricks.push_back(static_cast<Brick *>(molecule));
            break;

        case Constants::FRAGMENT_TYPE::LINKER:
            _linkers.push_back(static_cast<Linker *>(molecule));
            break;

        case Constants::FRAGMENT_TYPE::FREE_ATOM:
            _linkers.push_back(static_cast<Linker *>(molecule));
            // std::cerr << "Internal unexpected FREE_ATOM type." << std::endl;
            break;

        default:
            std::cerr << "Unrecognized input fragment type." << std::endl;
        }
    }
};

#endif
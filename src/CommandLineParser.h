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

#ifndef _COMMAND_LINE_PARSER
#define _COMMAND_LINE_PARSER

#include <string>
#include <vector>
#include <iostream>
#include <iomanip> // std::setw
#include <cstring>
#include <cstdlib>
#include <map>

#include "Options.h"
#include "FileUtilities.h"
#include "Utilities.h"
#include "Constants.h"

/*
 * Singleton Options object 
 */
class CommandLineParser
{
  protected:
    int _argc;
    const char** _argv;

  public:
    CommandLineParser(int argc, const char** argv) : _argc{ argc }, _argv{ argv }
    {}

    bool parse()
    {
        // Parse all the command-line arguments
        doParse(_argc, _argv);

        // For debug mainly: echoArguments(_userDefined);

        // Process the arguments to verify reasonable values
        // if usage or version is requested, we leave without any further information shared
        if (!processArguments()) return false;

        echoArguments(_runtimeArgs);

        return true;
    }

    ~CommandLineParser()
    {
        // Don't delete Options*, let it pass externally to main and is deleted there
    }

    //
    // Accessors
    //
    std::vector<std::string> getFiles() { return _files; }

  protected:
    std::vector<std::string> _files;

    // Parsed arguments
    std::map<std::string, std::string> _userDefined;

    // Verified arguments
    mutable std::map<std::string, std::string> _runtimeArgs;

  private:

    //
    //
    // Functionality to take-in the command-line arguments into a dictionary
    //
    // Populates _userDefined
    //
    void doParse(int argc, const char** argv)
    {
        // Skip over the program name
        for (int i = 1; i < argc; i++)
        {
            // If the input does not have prefix '-', it is
            // considered to be a file 
            if (!isOption(i, argv))
            {
                if (!Utilities::contains(_files, std::string(argv[i]))) _files.push_back(argv[i]);
                else
                    std::cerr << "File " << argv[i]
                              << " is redundant and will be ignored." << std::endl;
            }
        }
    }

    /*
     * if the current argument is an option, process it 
     * 
     * @input: index in the argv array
     * @input: argv command-line array of C-style strings
     * 
     * @output: true if the current index indicates an option from the user
     */
    bool isOption(int& index, const char** argv)
    {
        if (argv[index][0] != '-') return false;

        std::string arg = std::string(argv[index]).substr(1);

        if (handleSingletonOption(arg)) return true;

        if (!handlePairedOption(arg, argv[++index]))
        {
            std::cerr << "Unrecognized option specifier -" << arg
                      << "; it will be ignored." << std::endl;
        }

        return true;
    }

    /*
     * Handle singleton options that begin with '-'
     *     Their specification by the user turns the option 'on'.
     *
     *  Singleton options include:
     *   
     */
    bool handleSingletonOption(std::string arg)
    {
        if (!Utilities::contains(Constants::CMD_ARGS_SINGLETON, arg)) return false;

        // Add to our map as a singleton option
        _userDefined[arg] = "";

        return true;
    }

    /*
     * Handle options that begin with '-'
     *     if the singletone option is not recognized, we check for a paired option.
     *
     *  Singleton options include:
     *
     */
    bool handlePairedOption(const std::string& arg, const std::string& arg_val)
    {
        if (!Utilities::contains(Constants::CMD_ARGS_PAIRED, arg)) return false;

        // Add to our map as a singleton option
        _userDefined[arg] = arg_val;

        return true;
    }

    /*
     * After parsing, report the command-line options that will be used
     */
    void echoArguments(const std::map<std::string, std::string>& args)
    {
        //
        // Output command-line arguments
        //
        std::cout << "The run will use the following command-line options; "
                  << "for unspecified arguments, default values are shown:" << std::endl;

        unsigned maxW = maxKeyWidth(args);

        for (auto it = args.begin(); it != args.end(); it++)
        {
            std::cout << '\t' << std::left << std::setw(maxW + 4) << ("-" + it->first)
                      << (it->second != "" ? " : " + it->second : "") << std::endl;
        }

        //
        // Output files
        //
        std::cout << "The following files were specified:" << std::endl;

        for (auto const& file : _files)
        {
            std::cout << '\t' << file << std::endl;
        }
    }

    //
    //
    //
    // Functionality for processing (ensuring goodness of) user command-line arguments
    //
    // Populates _runtimeArgs
    //

    /*
     * @return true if version or usage is requested
     */
    bool usageVersionRequested() const
    {
        bool use = _userDefined.find(Constants::CMD_ARG_USAGE) != _userDefined.end();
        bool vers = _userDefined.find(Constants::CMD_ARG_VERSION) != _userDefined.end();

        // Neither requested
        if (!use && !vers) return false;

        // Handle requests
        if (vers) version();
        if (use) usage();

        return true;
    }

    /*
     * Loop through the user-defined map of arguments to verify
     * the provided information.
     */
    bool processArguments() const
    {
        // Handle version information or usage information
        if (usageVersionRequested()) return false;

        //
        // If the level bound has been specified, process it.
        // It influences other checks.
        //
        auto it = _userDefined.find(Constants::CMD_ARG_LEVEL_BOUND);
        if (it != _userDefined.end())
        {
            unsigned bound = atoi((it->second).c_str());

            if (bound <= 0)
            {
                std::cerr << "Default level bound must be positive; not "
                          << it->second << std::endl;
            }

            // In the case that the input value fails to be read, 0.0 is returned
            if (bound > Constants::MAX_SYNTH_LEVEL_BOUND)
            {
                std::cerr << "Level bound must be less than "
                          << Constants::MAX_SYNTH_LEVEL_BOUND
                          << " not " << bound << std::endl;
            }

            // the set function will default to 20 for erroneous values
            Options::setLevelBound(bound);
        }

        //
        // Handle the rest of the arguments
        //
        for (auto it = _userDefined.begin(); it != _userDefined.end(); it++)
        {
            processArgument(it->first, it->second);
        }

        //
        // After processing the arguments, collect all the relevant runtime information
        //
        _runtimeArgs[Constants::CMD_ARG_OUTPUT_DIR] = Options::OUTPUT_DIRECTORY;
        _runtimeArgs[Constants::CMD_ARG_TANIMOTO_COEFF] = std::to_string(Options::TANIMOTO);
        _runtimeArgs[Constants::CMD_ARG_LEVEL_BOUND] = std::to_string(Options::USER_DEFINED_LEVEL_BOUND);
        _runtimeArgs[Constants::CMD_ARG_PROB_LEVEL] = std::to_string(Options::PROBABILITY_PRUNE_LEVEL_START);

        return true;
    }

    void processArgument(const std::string& arg,
                         const std::string& value) const
    {
        //
        // Singleton arguments
        //
        if (arg == Constants::CMD_ARG_SMI_ONLY)
        {
            Options::setSMILESOutputOnly();
            _runtimeArgs[Constants::CMD_ARG_SMI_ONLY] = "";
        }
        else if (arg == Constants::CMD_ARG_SERIAL)
        {
            Options::setSerialExecution();
            _runtimeArgs[Constants::CMD_ARG_SERIAL] = "";
        }

        //
        // Paired arguments
        //
        else if (arg == Constants::CMD_ARG_OUTPUT_FILE)
        {
            // Does the output file exist?
            if (!FileUtilities::exists(value))
            {
                std::cerr << "Specified output file " << value
                          << " already exists, will use ";
            }
            else
            {
                Options::setOutputFile(value);
                _runtimeArgs[Constants::CMD_ARG_OUTPUT_FILE] = value;
            }
        }

        else if (arg == Constants::CMD_ARG_OUTPUT_DIR)
        {
            Options::setOutputDirectory(FileUtilities::getSuffixedDirectory(value));
        }

        else if (arg == Constants::CMD_ARG_VALIDATION_FILE)
        {
            // Does the input file exist?
            if (!FileUtilities::exists(value))
            {
                std::cerr << "Specified validation file " << value
                          << " does not exist. Validation will not be performed." << std::endl;
            }
            else
            {
                Options::setValidationFile(value);
                _runtimeArgs[Constants::CMD_ARG_VALIDATION_FILE] = value;
            }
        }

        // space required between:  -tc <int>
        else if (arg == Constants::CMD_ARG_TANIMOTO_COEFF)
        {
            double coeff = atof(value.c_str());

            if (coeff <= 0)
            {
                std::cerr << "Default tanimoto coeffecient must be in the range (0, 1]"
                    << "input Tanimoto is invalid: " << value << std::endl;
            }

            // In the case that the input value fails to be read, 0.0 is returned
            if (coeff > 1)
            {
                std::cerr << "Default tanimoto coeffecient must be in the range (0, 1]"
                    << "input is " << coeff << std::endl;
            }

            // the set function will default to 0.95 for erroneous values
            Options::setTanimotoCoeff(coeff);
        }

        else if (arg == Constants::CMD_ARG_LEVEL_BOUND)
        {
            // This has already been handled above
        }

        else if (arg == Constants::CMD_ARG_PROB_LEVEL)
        {
            unsigned level = atoi(value.c_str());

            if (level <= 0)
            {
                std::cerr << "Default level bound must be zero; not "
                    << value << std::endl;
            }

            // In the case that the input value fails to be read, 0.0 is returned
            if (level > Constants::MAX_SYNTH_LEVEL_BOUND)
            {
                std::cerr << "Level bound must be less than "
                    << Constants::MAX_SYNTH_LEVEL_BOUND
                    << " not " << level << std::endl;
            }

            // the set function will default to 20 for erroneous values
            Options::setProbLevel(level);
        }
    }

    //
    //
    // Help functionality
    //
    //

  public:
    /*
     * Usage information; output of the possible options
     */
    static void usage()
    {
        std::cout << Constants::ESYNTH_EXECUTABLE
            << " may be executed with the following options:"
            << std::endl;

        unsigned maxW = maxKeyWidth(Constants::USAGE_MAP);

        for (auto it = Constants::USAGE_MAP.begin(); it != Constants::USAGE_MAP.end(); it++)
        {
            std::cout << std::left << std::setw(maxW + 4) << ("-" + it->first) << it->second << std::endl;
        }
    }

    /*
     * Version information
     */
    static void version()
    {
        std::cout << Constants::ESYNTH_VERSION << std::endl;
    }

  protected:
    /*
     * @return: maximum width required for the keys; ensures even columnar output
     */
    template <typename T>
    static unsigned maxKeyWidth(const std::map<T, T>& m)
    {
        unsigned max = 0;
        for (auto it = m.begin(); it != m.end(); it++)
        {
            max = max > it->first.size() ? max : it->first.size();
        }
        return max;
    }
};

#endif
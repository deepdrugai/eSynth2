/*
 * SPDX-License-Identifier: MIT
 * Copyright (c) 2026 deepdrugai
 */

#include <fstream>
#include <string>


#include <openbabel/mol.h>
#include <openbabel/obconversion.h>


int main(int argc,char **argv)
{
    if (argc != 2)
    {
        std::cerr << "Usage: compliantwriter <SDF-FILE>.sdf" << std::endl; 
        return 1;
    }

    char* program_name = argv[0];
    char* infilename = argv[1];

    // SMI reader
    OpenBabel::OBConversion SDF_conv_SMI(&std::cin, &std::cout);

    // set conversion type(s) and verify it worked
    if(!SDF_conv_SMI.SetInAndOutFormats("SDF","SMI"))
    {
        std::cerr << "SetInAndOutFormats failed!" << std::endl;
        return 1;
    }


    std::ifstream ifs;

    // Read the file
    ifs.open(infilename);
    if (!ifs)
    {
        std::cerr << program_name << ": cannot read input file!" << std::endl;
        exit (-1);
    }

    OpenBabel::OBMol mol;
    if (!SDF_conv_SMI.Read(&mol, &ifs)) return 0;

    std::string smi = SDF_conv_SMI.WriteString(&mol);

    smi = smi.substr(0, smi.find('\t'));

    std::cout << smi; // << std::endl;

    return 0;
}

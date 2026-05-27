# eSynth

eSynth is a graph-based molecular synthesis tool written in C++.
The author is Dr. Chris Alvin. The source code is shared here with the permission granted. All credit goes to Dr. Chris Alvin, [Computational Systems Biology Group](http://brylinski.org/), and DeepDrug.ai.

If you find this tool is useful to you, please cite this paper:   

Naderi, Misagh, Chris Alvin, Yun Ding, Supratik Mukhopadhyay, and Michal Brylinski. "A graph-based approach to construct target-focused libraries for virtual screening." Journal of Cheminformatics 8, no. 1 (2016): 14.

## Updates for esynth2

This version of esynth2 combines the branch updates into a single synthesis workflow that parses fragment inputs, validates naming and extensions, and builds molecules only from accepted fragments. The runtime now supports skipping free-atom inputs, directing output to a chosen directory or file suffix, and constraining synthesis depth with a fragment-level bound.

The builder capabilities were also expanded in this release. esynth2 now includes unique-fragment construction, merged-fragment construction, looser atom-type matching, and optional linker-to-linker connections.

- Removed hard-coded molecular property filters based on Lipinski's Rule of Five, giving users control over which filters to apply for a given study
- Added support for multiple bonds to a single brick atom, eliminating a reconstruction failure mode present in the original
- Added a targeted reconstruction (unique-build) mode for soundness validation, allowing the original molecule to be tested for presence among recombinants without exhaustive enumeration
- Reads connectivity metadata from eMolFrag2's richer SDF output, enabling more general reconstruction including correct handling of super-fragments, free atoms, and unambiguous aromaticity information

Overall, esynth2 reflects the merged branch work as a more configurable and practical tool for large-scale fragment-driven molecule generation.

## Command-Line Flags

Use `eSynth -usage` to print the full built-in help text. The supported flags are:

- `-skip-fa` skips free-atom input files.
- `-usage` prints usage information and exits.
- `-version` prints the version string and exits.
- `-unique-build` treats all input fragments as unique when constructing molecules.
- `-merged-fragments` enables unique builds after fragment merging.
- `-loose` uses looser atom typing when connecting fragments.
- `-allow-ll` allows linker-to-linker connections.
- `-o <file>` sets the output file suffix.
- `-odir <path>` sets the output directory.
- `-v <file>` enables on-the-fly validation with the given `.mol2` file.
- `-tc <float>` sets the Tanimoto coefficient used for molecule equivalence checks.
- `-hl <int>` sets the maximum fragment count allowed in a molecule.

Input fragment files are provided as positional arguments.

## License

This project is licensed under the MIT License. See the LICENSE file for details.

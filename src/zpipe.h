/*
 * SPDX-License-Identifier: MIT
 * Copyright (c) 2026 deepdrugai
 */

#ifndef _ZLIB_INTERFACE_GUARD
#define _ZLIB_INTERFACE_GUARD 1

// Compress the given file to the given output file.
void zlib_compress(const std::string& infile, const std::string& outfile);

#endif

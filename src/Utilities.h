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
  * Author: C. Alvin 8/2022
  */

#ifndef _UTILITIES_GUARD_H
#define _UTILITIES_GUARD_H

#include <algorithm>
#include <vector>
#include <cctype>
#include <string>
#include <numeric>
#include <map>

/*
 * A class to facilitate scoping (for clarity in code).
 */
class Utilities
{
  public:
    template <typename T>
    static bool contains(const std::vector<T>& vec, const T& val)
    {
        return std::find(vec.begin(), vec.end(), val) != vec.end();
    }
	
    /*
     * @return: maximum width required for the keys; ensures even columnar output
     * 
     * The datatype of the values is inconsequential.
     */
    template <typename T, typename T2>
    static unsigned maxKeyWidth(const std::map<T, T2>& m)
    {
        unsigned max = 0;
        for (auto it = m.begin(); it != m.end(); it++)
        {
            max = max > it->first.size() ? max : it->first.size();
        }
        return max;
    }

    void eatWhiteLines(std::istream& in)
    {
        while (isspace(in.peek()))
        {
            while (in.get() != '\n')
            {
            }
        }
    }

    void eatWhiteToNewLineOrChar(std::istream& in)
    {
        for (char c = in.peek(); c != '\n' && isalnum(c); c = in.peek())
        {
            in.get();
        }
    }

    /*
     * Convert the given string to lower case (modifies input string)
     */
    static void tolower(std::string& s)
    {
        std::transform(s.begin(), s.end(), s.begin(), [](unsigned char c) { return std::tolower(c); });
    }

    /*
     * Split string s into two halves by seeking the FIRST occurrence of delimiter
     *
     *      prefix<delim>suffix
     * CTA: test if delimiter is never found
     */
    static void splitFirst(const std::string& s, std::string& prefix,
                           std::string& suffix, const std::string& delimiter)
    {
        split(s.find(delimiter), delimiter.size(), s, prefix, suffix);
    }

    /*
     * Split string s into two halves by seeking the LAST occurrence of delimiter
     *
     *      prefix<delim>suffix
     * CTA: test if delimiter is never found
     */
    static void splitLast(const std::string& s, std::string& prefix,
                          std::string& suffix, const std::string& delimiter)
    {
        split(s.rfind(delimiter), delimiter.size(), s, prefix, suffix);
    }

    /*
     *  Mimic Python join function for strings
     */
    template<class T>
    static std::string join(const std::string& delim, const T& items)
    {
        std::string s = "";

        for (const auto& item : items)
        {
            if (!s.empty()) s += delim;
            s += item;
        }

        return s;
    }

  private:
    // private function to help splitting
    // _____XXXXXX____
    //      ^     ^
    //      |     | leftEnd + gap
    //      |
    //      leftEnd
    //
    static void split(size_t leftEnd, size_t gap, const std::string& s, std::string& prefix, std::string& suffix)
    {
        if (leftEnd >= std::string::npos)
        {
            prefix = s;
            suffix = "";
            return;
        }

        if (leftEnd + gap >= std::string::npos)
        {
            prefix = s.substr(leftEnd);
            suffix = "";
            return;
        }

        prefix = s.substr(0, leftEnd);
        suffix = s.substr(leftEnd + gap);
    }
};

//
// v1.0
//
#include <cmath>
#include <iostream>

using namespace std;


// log base 2 of the input value.
double log2(double value);

// How many bits in binary to represent this number?
unsigned int numBinaryBits(unsigned int value);

string MakeString(const char[], int);
std::string MakeString(const char s1[], const char s2[]);
std::string MakeString(const char s1[], std::string s2);

template<class T>
bool Contains(std::vector<T> list, const T& val)
{
    for (int i = 0; i < list.size(); i++)
    {
        if (list[i].Equals(val)) return true;
    }
    return false;
}

//
// To simulate instanceof in Java.
//
template<typename CheckType, typename InstanceType>
bool instanceOf(InstanceType &Instance)
{
  return dynamic_cast<CheckType*>(Instance) != NULL;
}

void eatWhiteToNewLineOrChar(std::istream&);
void eatWhiteLines(std::istream&);


void MakeBoolVector(vector<bool>& vec, int size);
bool ContainsFalse(const vector<bool>& vec);

//
// Directory related functionality
//
void MakeDirectory(const std::string& theDir);
bool DoesDirectoryExist(const std::string& theDir);
void CleanDirectory(const std::string& theDir);

//
// Probability distributions
//
float NormPdf(float x, float loc, float scale);
float CauchyPdf(float x, float loc, float scale);
float LogisticPdf(float x, float loc, float scale);
float WaldPdf(float x, float loc, float scale);
float LaplacePdf(float x, float loc, float scale);


//
// Macros for simplifying code a bit.
//
#define foreach_bricks(it, expr) \
  for(std::vector<Brick*>::const_iterator it = (expr).begin(); \
      it != (expr).end(); it++)

#define foreach_linkers(it, expr) \
  for(std::vector<Linker*>::const_iterator it = (expr).begin(); \
      it != (expr).end(); it++)

#define foreach_molecules(it, expr) \
  for(std::vector<Molecule*>::const_iterator it = (expr).begin(); \
      it != (expr).end(); it++)

#define foreach_bonds(it, expr) \
  for(std::vector<Bond>::const_iterator it = (expr).begin(); \
      it != (expr).end(); it++)

#define foreach_atoms(it, expr) \
  for(std::vector<Atom*>::const_iterator it = (expr).begin(); \
      it != (expr).end(); it++)

#define foreach_nodes(it, expr) \
  for(std::vector<FragmentGraphNode*>::const_iterator it = (expr).begin(); \
      it != (expr).end(); it++)

#define foreach_subnodes(it, expr) \
  for(std::vector<FragmentSubNode*>::const_iterator it = (expr).begin(); \
      it != (expr).end(); it++)

#define foreach_uints(it, expr) \
  for(std::vector<unsigned int>::const_iterator it = (expr).begin(); \
      it != (expr).end(); it++)

#define foreach_obmols(it, expr) \
  for(std::vector<OpenBabel::OBMol>::iterator it = (expr).begin(); \
      it != (expr).end(); it++)

#define foreach_obmol_points(it, expr) \
  for(std::vector<OpenBabel::OBMol*>::iterator it = (expr).begin(); \
      it != (expr).end(); it++)

#endif

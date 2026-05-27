/*
 * SPDX-License-Identifier: MIT
 * Copyright (c) 2026 deepdrugai
 */

#ifndef _FIXED_SORTED_LIST_GUARD
#define _FIXED_SORTED_LIST_GUARD 1


#include <cstring> // memset
#include <sstream>


#include "Constants.h"


class FixedSortedList
{
  public:
    FixedSortedList(unsigned int cap) : sz(0), capacity(cap)
    {
        values = new short[capacity];
        ids = new short[capacity];
        
        // Set all values to be -1
        memset(values, -1, capacity * sizeof(short));
        memset(ids, -1, capacity * sizeof(short));
    }
    
    ~FixedSortedList()
    {
        delete[] values;
        delete[] ids;
    }

    short add(short val, short id)
    {
        // Check the capacity of the list
        if (sz == capacity) throw "Capacity reached in FixedSortedList";

        // Check for containment already
        short ret_id = contains(val);
        if (ret_id != NOT_FOUND) return ret_id;
        
        //
        // Add to the list (in order going from the back to the front--insertion sort style)
        //
        int index;
        for (index = sz; index > 0 && values[index - 1] > val; index--)
        {
            values[index] = values[index - 1];
            ids[index] = ids[index - 1];
        }

        values[index] = val;
        ids[index] = id;
        
        sz++;

        return id;
    }

    // Search for a particular value using binary search
    short contains(short val) const
    {
        if (this->empty()) return NOT_FOUND;

        int index = binarySearch(val, 0, sz - 1);

        return index == NOT_FOUND ? NOT_FOUND : ids[index];
    }

    bool empty() const { return sz == 0; }
   
    std::string toString() const
    {
        std::ostringstream oss;

        //  Traverse the map to print all maps
        for (int index = 0; index < sz; index++)
        {
            oss << "(" << values[index] << ", " << ids[index] << ") ";
        }

        return oss.str();
    }

    friend std::ostream& operator<< (std::ostream& os, const FixedSortedList& fsl)
    {
        os << fsl.toString();

        return os;
    }

 
  private:
    // Parallel arrays: <values, unique edge ids>
    short* values;
    short* ids;

    // Occupied size
    unsigned short sz;

    // Overall upper bound of size
    unsigned int capacity;

    int midpoint(int a, int b) const { return (a + b) / 2; }
    
    int binarySearch(int key, int imin, int imax) const
    {
        if (imax < imin) return NOT_FOUND;

        // calculate midpoint to cut set in half
        int imid = midpoint(imin, imax);
 
        // key is in lower subset
        if (values[imid] > key) return binarySearch(key, imin, imid - 1);

        // key is in upper subset
        else if (values[imid] < key) return binarySearch(key, imid + 1, imax);

        // Key is found
        else return imid;
    }
};

#endif

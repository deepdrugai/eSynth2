/*
 * SPDX-License-Identifier: MIT
 * Copyright (c) 2026 deepdrugai
 */

#ifndef _ID_FACTORY_GUARD
#define _ID_FACTORY_GUARD

class IdFactory
{
  public:
    IdFactory();
    IdFactory(unsigned int dictatedMin);

    unsigned int getNextId() { return current++; }
    unsigned int min() const { return minId; } 
    void reset() { current = minId; }

  private:
    unsigned int minId;
    unsigned int current;
   
    const static unsigned int DEFAULT_ID = 0;
};

#endif

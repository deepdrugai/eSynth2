/*
 * SPDX-License-Identifier: MIT
 * Copyright (c) 2026 deepdrugai
 */

#include "IdFactory.h"

IdFactory::IdFactory()
{
    minId = IdFactory::DEFAULT_ID;
    current = IdFactory::DEFAULT_ID;
}

IdFactory::IdFactory(unsigned int dictatedMin)
{
    minId = dictatedMin;
    current = dictatedMin;
}



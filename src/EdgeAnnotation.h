/*
 * SPDX-License-Identifier: MIT
 * Copyright (c) 2026 deepdrugai
 */

#ifndef _EDGE_ANNOTATION_GUARD
#define _EDGE_ANNOTATION_GUARD 1

#include <string>

class EdgeAnnotationT
{
  public:
    std::string justification;
    bool active;

    EdgeAnnotationT()
    {
        justification = "";
        active = true;
    }

    EdgeAnnotationT(const std::string& just, int act) : justification(just), active(act) { }
    bool IsActive() { return active; }
};

#endif

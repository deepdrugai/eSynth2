/*
 * SPDX-License-Identifier: MIT
 * Copyright (c) 2026 deepdrugai
 */

#ifndef _EDGE_AGGREGATOR_GUARD
#define _EDGE_AGGREGATOR_GUARD 1

#include <vector>

#include "Molecule.h"
#include "EdgeAnnotation.h"

//
// An aggregation class of information to pass back from instantiation.
//
class EdgeAggregator
{
  public:
    std::vector<unsigned int> antecedent;
    Molecule* consequent;
    EdgeAnnotationT* annotation;
        
    EdgeAggregator(const std::vector<unsigned int>& ante, Molecule* c, EdgeAnnotationT* ann)
                  : antecedent(ante), consequent(c), annotation(ann)
    {
    }

    ~EdgeAggregator()
    {
        // This annotation should persist into the hypergraph.
        // For now we delete; it is not-needed.
        delete annotation;
    }
};

#endif

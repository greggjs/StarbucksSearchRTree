//
//  greggjsStarbucks.h
//  StarbucksSearchRTree
//
//  Created by Jake Gregg on 10/25/12.
//
//

#ifndef StarbucksSearchRTree_greggjsStarbucks_h
#define StarbucksSearchRTree_greggjsStarbucks_h

#import "RTree.h"
#import "Entry.h"
#import "Starbucks.h"
#import <string.h>

class greggjsStarbucks : public Starbucks {
public:
    RTree<Entry, Entry, 2, double>();starbucks_tree_;
    int importData(Entry* c, string fileName);
    virtual void build(Entry* c, int n);
    virtual Entry* getNearest(double x, double y);
};

#endif

//
//  Starbucks.h
//  StarbucksSearchRTree
//
//  Created by Jake Gregg on 10/25/12.
//
//

#ifndef StarbucksSearchRTree_Starbucks_h
#define StarbucksSearchRTree_Starbucks_h

/*
 * This is an "abstract class" ... it contains a list of the public methods of the class, but
 *  they are all "pure virtual," which means they are not implemented anywhere in the code.
 *
 * An abstract class is used like an interface in Java: The only purpose of this class is to be the base
 *  class for your own data structure. You should make a .h and .cpp for you data structure ... in my
 *  case I made brinkmwjStarbucks.h and brinkmwjStarbucks.cpp, and my class was called "brinkmwjStarbucks"
 */

#pragma once
#include <string>
#import "Entry.h"
using namespace std;

class Starbucks {
public:
	/*
	 * add all entries in the array to your data structure
	 *
	 * The "c" parameter is a pointer to an array of all the entries to be added, and n is the length of
	 * the array.
	 *
	 * Note: If you detect that two items have the same coordinates, then do not add the new item
	 *       that has the same coordinates as another item. This is guaranteed to happen, by the way,
	 *       because some Starbucks locations are listed in the database twice. We will define two locations
	 *       to be the "same location" if both |x1 - x2| <= 0.00001 and |y1 - y2| < 0.00001
	 */
	virtual void build(Entry* c, int n) = 0;
	
	/*
	 * Return a pointer to the entry that is closest to the given coordinates. Your
	 *  answer may be approximate, but then you will lose points on the "Accuracy" quality measure
	 */
	virtual Entry* getNearest(double x, double y) = 0;
};

#endif

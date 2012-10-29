//
//  Entry.h
//  StarbucksSearchRTree
//
//  Created by Jake Gregg on 10/25/12.
//
//

#ifndef StarbucksSearchRTree_Entry_h
#define StarbucksSearchRTree_Entry_h

/*****
 * Author   : brinkmwj
 * Date     : 2012-10-08
 * Sources  : All code is original
 * Purpose  : This class is designed to be an "Interface." Your starbucks data structure should be
 *            a sub-class of this class, but you should not implement this class. Instead, you should
 *            create a <uniqueid>Starbucks class, in the files <uniqueid>Starbucks.h and
 *            <uniqueid>Starbucks.cpp
 *            Declare your class (in your <uniqueid>Starbucks.h) like this:
 *  class brinkmwjStarbucks : public Starbucks {
 *    // declaration of member variables and methods here
 *  };
 *            In your class you should declare all of your methods to be virtual: This is how
 *            polymorphism is accomplished in C++. Do NOT copy the part that says "= 0" that I use
 *            below. That part means that the function, though declared, will not have any implementation.
 * Note     : DO NOT CHANGE THIS FILE!!!
 */

#pragma once
#include <string>
using namespace std;

/*
 * Note that the origin is in the LOWER LEFT corner, unlike in previous assignments.
 */
class Entry {
public:
	string identifier;
	double x;
	double y;
};



#endif

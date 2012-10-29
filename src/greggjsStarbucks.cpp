//
//  greggjsStarbucks.cpp
//  StarbucksSearchRTree
//
//  Created by Jake Gregg on 10/25/12.
//
//

#include "greggjsStarbucks.h"
#include "Starbucks.h"
#include "Entry.h"
#include <iostream.h>
#include <fstream.h>
#include <string.h>
#include <vector.h>

using namespace std;

int greggjsStarbucks::importData(Entry* importedData, string fileName){
    
	try{
		ifstream fid (fileName, ifstream::in);
        
		int index = 0;
		char* buffer = new char[256];
        
		while(!fid.eof()){
			Entry* temp = new Entry;
            
			fid.getline(buffer,256,',');
			temp->identifier = (string)buffer;
            
			fid.getline(buffer,256,',');
			temp->x  = atof(buffer); 
            
			fid.getline(buffer,256,'\r');
			temp->y  = atof(buffer); 
            
			importedData[index] = *temp;
            
			//checking output for correctness:
			//console() << (locations->back())->identifier << ","<< (locations->back())->x << ","<< (locations->back())->y << std::endl ;
            
			index++;
		}
        
		fid.close();
        
		return index;
        
	}catch(ios_base::failure){
        
		cout << "Error Reading File\n";
		return NULL;
        
	}
}

void greggjsStarbucks::build(Entry* c, int n) {
    for (int i = 0; i < n; ++i)
        
        starbucks_tree_.insert(c[i], <#const Entry *a_max#>, <#void *const &a_dataId#>)
}

Entry* greggjsStarbucks::getNearest(double x, double y) {
    
}

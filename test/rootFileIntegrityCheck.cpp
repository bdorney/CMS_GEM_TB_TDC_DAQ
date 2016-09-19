//
//  rootFileIntegrityCheck.cpp
//  
//
//  Created by Brian L Dorney on 26/10/15.
//
//

#include <fstream>
#include <iostream>
#include <stdio.h>

#include "TFile.h"

int main(){
    std::string strFileList = "/localdata/2015/FTM/FileList_FTM.txt";
    std::string strLine;
    
    ifstream stream_file( strFileList.c_str() );
    
    std::cout<<"List of Bad Files:\n";
    std::cout<<"IsOpen\tIsZombie\tFilename\n";
    while (getline(stream_file, strLine) ) {
        TFile *file_InputROOTFile = new TFile(strLine.c_str(), "READ", "", 1);
        
        if (!file_InputROOTFile->IsOpen() || file_InputROOTFile->IsZombie() ) {
            std::cout<< file_InputROOTFile->IsOpen() << "\t" << file_InputROOTFile->IsZombie() << "\t"<<strLine.c_str()<<std::endl;
        }
    }
    
    return 0;
} //End Main
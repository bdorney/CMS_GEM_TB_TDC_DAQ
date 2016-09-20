
#ifndef ____QualityControlUtilityFunctions__
#define ____QualityControlUtilityFunctions__

//C++ Includes
#include <algorithm>
#include <fstream>
#include <iostream>
#include <map>
#include <numeric>
#include <stdio.h>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

//Framework Includes

//ROOT Includes
#include "TColor.h"
#include "TFile.h"

/* 
 * //boolean
 * convert2bool -> converts a string from the set {t,true,1,f,false,0} to boolean (case-insenstive)
 *
 * //File I/O
 * getlineNoSpaces -> gets a line from a file, leading/trailing spaces and tabs are removed
 *
 * //Math
 * addVal -> Performs addition of a value to a stl container; for use with stl algorithms
 *
 * //printers
 * printClassMethodMsg() -> Prints a message to the user indicating class/method template: "charClass::charMethod() - charMsg"
 * printROOTFileStatus -> prints the status bits of a ROOT file
 * printStreamStatus -> prints the status bits of an std::ifstream
 *
 * //string manipulation
 * getCommaSeparatedList -> returns a std::vector<std::string> of a comma separated list, ignoring whitespaces
 * getCharSeparatedList -> returns a std::vector<std::string> of a character separated list, ignoring whitespaces
 * getParsedLine -> returns a std::pair<std::string,std::string> of a "Field,Value" under the format "Field = 'Value';" note the 4 important characters used for parsing {=,',',;}
 * getString -> converts an input type to a string
 * stofSafe -> converts a string to a float; checks for non-numeric characters; offers user chance to correct;
 * stoiSafe -> converts a string to an int; checks for non-numeric characters; offers user chance to correct
 *
 */

namespace QualityControl {
    //Function Declaration
    //========================================================================================
    //boolean
    bool convert2bool(std::string str, bool &bExitSuccess);
    
    //File I/O
    std::istream & getlineNoSpaces(std::istream & stream, std::string & str);
    
    //Math
    //Can't seem to get the templated form above to work
    struct addVal{
        int iVal;
        
        addVal(int iAdd){
            iVal = iAdd;
        }
        
        void operator()(int &iInput) const{
            iInput += iVal;
        }
        //usage example:
        //std::for_each(myvec.begin(), myvec.end(), addVal(1.0));
    };
    
    //printers
    void printClassMethodMsg(const char charClass[], const char charMethod[], const char charMsg[]);
    void printROOTFileStatus(TFile *file_ROOT);
    //void printROOTFileStatus(std::shared_ptr<TFile> file_ROOT);
    void printStreamStatus(std::ifstream &inputStream);
    void printStreamStatus(std::fstream &inputStream);
    void printStringNotFoundMsg(std::string strCallingMethod, std::string strPatternNotFound, std::string strLine, std::string strFilename);
    
    //string manipulation
    std::vector<std::string> getCharSeparatedList(std::string strInput, char cDelimiter);
    
    std::pair<std::string,std::string> getParsedLine(std::string strInputLine, bool &bExitSuccess);
    
    template<class TConversion>
    std::string getString(TConversion input);
    
    float stofSafe(std::string strInputValue);
    float stofSafe(std::string strInputField, std::string strInputValue);
    
    int stoiSafe(std::string strInputValue);
    int stoiSafe(std::string strInputField, std::string strInputValue);
    
    
    //Code Begins for TEMPLATED functions
    //========================================================================================
    
    //string manipulation
    //----------------------------------------------------------------------------------------
    //template method must be defined in the header file
    template<class TConversion>
    std::string getString(TConversion input){
        std::stringstream sstream;
        sstream<<input;
        return sstream.str();
    } //End getString()
} //End namespace QualityControl


#endif

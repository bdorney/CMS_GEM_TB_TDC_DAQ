//
//  TimingUtilityFunctions.cpp
//  
//
//  Created by Brian L Dorney on 04/11/15.
//
//

#include "QualityControlUtilityFunctions.h"

using std::cout;
using std::endl;

//using namespace QualityControl;

//Function Code Begins
//========================================================================================
//boolean
//----------------------------------------------------------------------------------------
bool QualityControl::convert2bool(std::string str, bool &bExitSuccess){
    //convert to upper case
    std::transform(str.begin(), str.end(), str.begin(), toupper);
    
    //Empty String?
    if ( str.empty() ) {
        bExitSuccess = false;
        return false;
    }
    
    //Input recognized?
    if ( !(str.compare("T")==0 || str.compare("TRUE")==0 || str.compare("1")==0
           || str.compare("F")==0 || str.compare("FALSE")==0 || str.compare("0")==0) ) {
        bExitSuccess = false;
        return false;
    }
    
    bExitSuccess = true;
    
    return (str.compare("T")==0 || str.compare("TRUE")==0 || str.compare("1")==0);
} //End convert2bool()

//file I/O
//----------------------------------------------------------------------------------------
std::istream & QualityControl::getlineNoSpaces(std::istream & stream, std::string & str){
    //get the line
    std::getline(stream, str);
    
    if (str.find("'",0) == std::string::npos ) { //Case: Header
        str.erase(remove(str.begin(),str.end(), ' ' ), str.end() ); //spaces
        str.erase(remove(str.begin(),str.end(), '\t' ), str.end() );//tabs
    } //End Case: Header
    else{ //Case: line input
        int iFirstQuote = str.find("'",0);
        int iLastQuote = str.rfind("'");
        
        //Spaces
        if (str.find(' ') != std::string::npos ) { //Case: Spaces Found
            str.erase(remove(str.begin(),str.begin()+iFirstQuote, ' ' ), str.begin()+iFirstQuote ); //Until first single-quote
            str.erase(remove(str.begin()+iLastQuote,str.end(), ' ' ), str.end() ); //Until last single-quote
        } //End Case: Spaces Found
        
        //tabs
        if(str.find('\t') != std::string::npos){ //Case: Tabs Found
            str.erase(remove(str.begin(),str.begin()+iFirstQuote, '\t' ), str.begin()+iFirstQuote ); //Until first single-quote
            str.erase(remove(str.begin()+iLastQuote,str.end(), '\t' ), str.end() ); //Until last single-quote
        } //End Case: Tabs Found
    } //End Case: line input
    
    return stream;
} //End getlineNoSpaces()

//Math
//----------------------------------------------------------------------------------------

//printers
//----------------------------------------------------------------------------------------
//Prints a message to the user indicating class/method template: "charClass::charMethod() - charMsg"
void QualityControl::printClassMethodMsg(const char charClass[], const char charMethod[], const char charMsg[]){
    //cout<< (strClass + "::" + strMethod + "() - " + strMsg ).c_str() << endl;
    cout<< charClass << "::" << charMethod << "() - " << charMsg << endl;
    
    return;
} //End Uniformity::printClassMethodMsg

//Prints the status bits of an input TFile
void QualityControl::printROOTFileStatus(TFile * file_ROOT){
    std::cout << "Input ROOT File Status:\n";
    std::cout << "\tIsOpen() = " << file_ROOT->IsOpen() << std::endl;
    std::cout << "\tIsZombie() = " << file_ROOT->IsZombie() << std::endl;
    
    return;
} //End printROOTFileStatus

//Prints All Bit Flags for an input ifstream
void QualityControl::printStreamStatus(std::ifstream &inputStream){
    std::cout << "Input File Stream Bit Status:\n";
    std::cout << " good()=" << inputStream.good() << std::endl;
    std::cout << " eof()=" << inputStream.eof() << std::endl;
    std::cout << " fail()=" << inputStream.fail() << std::endl;
    std::cout << " bad()=" << inputStream.bad() << std::endl;
    
    return;
} //End treeProducerTDC::printStreamStatus()

//Prints All Bit Flags for an input fstream
void QualityControl::printStreamStatus(std::fstream &inputStream){
    std::cout << "Input File Stream Bit Status:\n";
    std::cout << " good()=" << inputStream.good() << std::endl;
    std::cout << " eof()=" << inputStream.eof() << std::endl;
    std::cout << " fail()=" << inputStream.fail() << std::endl;
    std::cout << " bad()=" << inputStream.bad() << std::endl;
    
    return;
} //End treeProducerTDC::printStreamStatus()

//Prints an error message to the user if a specific pattern (strPatternNotFound) is not found during file I/O
//Also informs the user which calling method and file the pattern is not found under
void QualityControl::printStringNotFoundMsg(std::string strCallingMethod, std::string strPatternNotFound, std::string strLine, std::string strFilename){
    std::cout<<"String '" << strPatternNotFound << "' Not Found in line:\n";
    std::cout<< strLine <<std::endl;
    std::cout<<"Exiting " << strCallingMethod << ", Cross-Check input File:\n";
    std::cout<<strFilename<<std::endl;
    
    return;
} //End printStringNotFoundMsg()

//string manipulation
//----------------------------------------------------------------------------------------
//Safe conversion to float
//Scientific notation not supported
float QualityControl::stofSafe(std::string strInputValue){
    bool bInputUnderstood = false;
    bool bManualEntry = false;
    
    float ret_float = -1;
    
    std::string strUserInput;
    
    if (strInputValue.find_first_not_of( "-0123456789." ) == std::string::npos) { //Case: only numeric data
        ret_float =  std::stof(strInputValue);
    } //End Case: only numeric data
    else{ //Case: non numeric data entered
        std::cout<<"QualityControl::stofSafe() - Sorry numeric conversion failed\n";
        std::cout<<"\tValue = " << strInputValue << std::endl;
        std::cout<<"\tNonumeric characters present (sorry scientific notation not supported)\n";
        std::cout<<"\tWould you like to enter a value manually [y/N]?\n";
        
        while (!bInputUnderstood) { //Loop requesting manual entry
            std::cin>>strUserInput;
            
            std::transform(strUserInput.begin(), strUserInput.end(), strUserInput.begin(), toupper);
            
            if (0 == strUserInput.compare("Y") || 0 == strUserInput.compare("YES") || 0 == strUserInput.compare("1") ) { //Case: User wants to correct via manual entry
                bInputUnderstood = true;
                bManualEntry = true;
            } //End Case: User wants to correct via manual entry
            else if ( 0 == strUserInput.compare("N") || 0 == strUserInput.compare("NO") || 0 == strUserInput.compare("0") ){ //Case: User does not wish for manual entry
                bInputUnderstood = true;
                bManualEntry = false;
            } //End Case: User does not wish for manual entry
            else{ //Case: User Input Not Understood
                std::cout<<"\tSorry your input was not understood\n";
                std::cout<<"\tPlease answer from the set {y,yes,1,N,NO,0}\n";
                std::cout<<"\tWould you like to enter a value manually {y,yes,1,N,NO,0}?\n";
            } //End Case: User Input Not Understood
        } //End Loop requesting manual entry
        
        if (bManualEntry) { //Case: Manual Entry
            //std::cout<<"\tOkay, for Field = " << strInputField << " Enter a Numeric Value:\n";
            std::cout<<"\tOkay, Enter a Numeric Value:\n";
            std::cin>>strUserInput;
            
            ret_float = stofSafe(strUserInput); //Recursion
        } //End Case: Manual Entry
    } //End Case: non numeric data entered
    
    return ret_float;
} //End QualityControl::stofSafe()

float QualityControl::stofSafe(std::string strInputField, std::string strInputValue){
    bool bInputUnderstood = false;
    bool bManualEntry = false;
    
    float ret_float = -1;
    
    std::string strUserInput;
    
    if (strInputValue.find_first_not_of( "-0123456789." ) == std::string::npos) { //Case: only numeric data
        ret_float =  std::stof(strInputValue);
    } //End Case: only numeric data
    else{ //Case: non numeric data entered
        std::cout<<"QualityControl::stofSafe() - Sorry numeric conversion failed\n";
        std::cout<<"\tField = " << strInputField << std::endl;
        std::cout<<"\tValue = " << strInputValue << std::endl;
        std::cout<<"\tNonumeric characters present (sorry scientific notation not supported)\n";
        std::cout<<"\tWould you like to enter a value manually [y/N]?\n";
        
        while (!bInputUnderstood) { //Loop requesting manual entry
            std::cin>>strUserInput;
            
            std::transform(strUserInput.begin(), strUserInput.end(), strUserInput.begin(), toupper);
            
            if (0 == strUserInput.compare("Y") || 0 == strUserInput.compare("YES") || 0 == strUserInput.compare("1") ) { //Case: User wants to correct via manual entry
                bInputUnderstood = true;
                bManualEntry = true;
            } //End Case: User wants to correct via manual entry
            else if ( 0 == strUserInput.compare("N") || 0 == strUserInput.compare("NO") || 0 == strUserInput.compare("0") ){ //Case: User does not wish for manual entry
                bInputUnderstood = true;
                bManualEntry = false;
            } //End Case: User does not wish for manual entry
            else{ //Case: User Input Not Understood
                std::cout<<"\tSorry your input was not understood\n";
                std::cout<<"\tPlease answer from the set {y,yes,1,N,NO,0}\n";
                std::cout<<"\tWould you like to enter a value manually {y,yes,1,N,NO,0}?\n";
            } //End Case: User Input Not Understood
        } //End Loop requesting manual entry
        
        if (bManualEntry) { //Case: Manual Entry
            std::cout<<"\tOkay, for Field = " << strInputField << " Enter a Numeric Value:\n";
            std::cin>>strUserInput;
            
            ret_float = stofSafe(strInputField, strUserInput); //Recursion
        } //End Case: Manual Entry
    } //End Case: non numeric data entered
    
    return ret_float;
} //End QualityControl::stofSafe()

int QualityControl::stoiSafe(std::string strInputValue){
    bool bInputUnderstood = false;
    bool bManualEntry = false;
    
    int ret_int = -1;
    
    std::string strUserInput;
    
    if (strInputValue.find_first_not_of( "-0123456789" ) == std::string::npos) { //Case: only numeric data
        ret_int =  std::stoi(strInputValue);
    } //End Case: only numeric data
    else{ //Case: non numeric data entered
        std::cout<<"QualityControl::stoiSafe() - Sorry numeric conversion failed\n";
        std::cout<<"\tValue = " << strInputValue << std::endl;
        std::cout<<"\tNonumeric characters present (or maybe you gave a floating point number instead?)\n";
        std::cout<<"\tWould you like to enter a value manually [y/N]?\n";
        
        while (!bInputUnderstood) { //Loop requesting manual entry
            std::cin>>strUserInput;
            
            std::transform(strUserInput.begin(), strUserInput.end(), strUserInput.begin(), toupper);
            
            if (0 == strUserInput.compare("Y") || 0 == strUserInput.compare("YES") || 0 == strUserInput.compare("1") ) { //Case: User wants to correct via manual entry
                bInputUnderstood = true;
                bManualEntry = true;
            } //End Case: User wants to correct via manual entry
            else if ( 0 == strUserInput.compare("N") || 0 == strUserInput.compare("NO") || 0 == strUserInput.compare("0") ){ //Case: User does not wish for manual entry
                bInputUnderstood = true;
                bManualEntry = false;
            } //End Case: User does not wish for manual entry
            else{ //Case: User Input Not Understood
                std::cout<<"\tSorry your input was not understood\n";
                std::cout<<"\tPlease answer from the set {y,yes,1,N,NO,0}\n";
                std::cout<<"\tWould you like to enter a value manually {y,yes,1,N,NO,0}?\n";
            } //End Case: User Input Not Understood
        } //End Loop requesting manual entry
        
        if (bManualEntry) { //Case: Manual Entry
            std::cout<<"\tOkay, Enter a Numeric Value:\n";
            std::cin>>strUserInput;
            
            ret_int = stoiSafe(strUserInput); //Recursion
        } //End Case: Manual Entry
    } //End Case: non numeric data entered
    
    return ret_int;
} //End QualityControl::stoiSafe()

int QualityControl::stoiSafe(std::string strInputField, std::string strInputValue){
    bool bInputUnderstood = false;
    bool bManualEntry = false;
    
    int ret_int = -1;
    
    std::string strUserInput;
    
    if (strInputValue.find_first_not_of( "-0123456789" ) == std::string::npos) { //Case: only numeric data
        ret_int =  std::stoi(strInputValue);
    } //End Case: only numeric data
    else{ //Case: non numeric data entered
        std::cout<<"QualityControl::stoiSafe() - Sorry numeric conversion failed\n";
        std::cout<<"\tField = " << strInputField << std::endl;
        std::cout<<"\tValue = " << strInputValue << std::endl;
        std::cout<<"\tNonumeric characters present (or maybe you gave a floating point number instead?)\n";
        std::cout<<"\tWould you like to enter a value manually [y/N]?\n";
        
        while (!bInputUnderstood) { //Loop requesting manual entry
            std::cin>>strUserInput;
            
            std::transform(strUserInput.begin(), strUserInput.end(), strUserInput.begin(), toupper);
            
            if (0 == strUserInput.compare("Y") || 0 == strUserInput.compare("YES") || 0 == strUserInput.compare("1") ) { //Case: User wants to correct via manual entry
                bInputUnderstood = true;
                bManualEntry = true;
            } //End Case: User wants to correct via manual entry
            else if ( 0 == strUserInput.compare("N") || 0 == strUserInput.compare("NO") || 0 == strUserInput.compare("0") ){ //Case: User does not wish for manual entry
                bInputUnderstood = true;
                bManualEntry = false;
            } //End Case: User does not wish for manual entry
            else{ //Case: User Input Not Understood
                std::cout<<"\tSorry your input was not understood\n";
                std::cout<<"\tPlease answer from the set {y,yes,1,N,NO,0}\n";
                std::cout<<"\tWould you like to enter a value manually {y,yes,1,N,NO,0}?\n";
            } //End Case: User Input Not Understood
        } //End Loop requesting manual entry
        
        if (bManualEntry) { //Case: Manual Entry
            std::cout<<"\tOkay, for Field = " << strInputField << " Enter a Numeric Value:\n";
            std::cin>>strUserInput;
            
            ret_int = stoiSafe(strInputField, strUserInput); //Recursion
        } //End Case: Manual Entry
    } //End Case: non numeric data entered
    
    return ret_int;
} //End QualityControl::stoiSafe()

std::pair<std::string,std::string> QualityControl::getParsedLine(std::string strInputLine, bool &bExitSuccess){
    //Variable Declaration
    int iPos_Equals = strInputLine.find("=",0);
    int iPos_End    = strInputLine.find(";",0);
    int iPos_Quote1 = strInputLine.find("'",0); //Position of first single-quote
    //int iPos_Quote2 = strInputLine.rfind("'");  //Position of last single-quote
    int iPos_Quote2 = strInputLine.find("'",iPos_Quote1+1); //Position of last single-quote

    std::string strFieldName;
    std::string strFieldValue;
    
    //Check to make sure iPos_Equals found in input string
    if (iPos_Equals == std::string::npos && strInputLine.find("[",0) == std::string::npos ) {
        printStringNotFoundMsg("TimingUtilityFunctions::getParsedLine","=",strInputLine,"See Next Error Msg for Filename" );
        
        bExitSuccess = false;
        
        return std::make_pair("","");
    }
    
    //Check to make sure iPos_End found in input string
    if (iPos_End == std::string::npos && strInputLine.find("[",0) == std::string::npos) {
        printStringNotFoundMsg("TimingUtilityFunctions::getParsedLine",";",strInputLine,"See Next Error Msg for Filename" );
        
        bExitSuccess = false;
        
        return std::make_pair("","");
    }
    
    //Check to make sure iPos_Quote1 found in input string
    if (iPos_Quote1 == std::string::npos && strInputLine.find("[",0) == std::string::npos) {
        printStringNotFoundMsg("TimingUtilityFunctions::getParsedLine","'",strInputLine,"See Next Error Msg for Filename" );
        
        bExitSuccess = false;
        
        return std::make_pair("","");
    }
    
    //Check to make sure iPos_Quote2 found in input string
    if (iPos_Quote2 == std::string::npos && strInputLine.find("[",0) == std::string::npos) {
        printStringNotFoundMsg("TimingUtilityFunctions::getParsedLine","'",strInputLine,"See Next Error Msg for Filename" );
        
        bExitSuccess = false;
        
        return std::make_pair("","");
    }
    
    //Get the Strings
    strFieldName    = strInputLine.substr(0,iPos_Equals);
    strFieldValue   = strInputLine.substr(iPos_Quote1+1, iPos_Quote2 - iPos_Quote1 - 1);
    
    //Set the Exit Flag
    bExitSuccess = true;
    
    //Return the Pair
    return std::make_pair(strFieldName,strFieldValue);
} //End getParsedLine()

//Returns a vector of strings taken from a comma separated list; ignores whitespaces in between elements
std::vector<std::string> QualityControl::getCharSeparatedList(std::string strInput, char cDelimiter){
    //Variable Declaration
    std::vector<std::string> vec_retStrings;
    
    //Remove all white spaces and tabs
    if( !(strInput.find(' ') == std::string::npos) ) strInput.erase(remove(strInput.begin(), strInput.end(), ' '), strInput.end() );
    if( !(strInput.find('\t')== std::string::npos) ) strInput.erase(remove(strInput.begin(), strInput.end(), '\t'), strInput.end() );
    
    if (strInput.find(cDelimiter) == std::string::npos ) { //Case: Comma's not found
        vec_retStrings.push_back(strInput);
    } //End Case: Comma's not found
    else{ //Case: Comma separated list given!
        //Declare Variables
        std::stringstream sstream(strInput);
        
        std::string strEntry;
        
        //Give the input string to the string stream
        //sstream<<strInput;
        
        //Get Entries
        while ( std::getline(sstream, strEntry, cDelimiter) ){ //Loop through the string
            vec_retStrings.push_back(strEntry);
        } //End loop through the string
    } //End Case: Comma separated list given!
    
    return vec_retStrings;
} //End getCommaSeparatedList()

//Unsorted
//----------------------------------------------------------------------------------------


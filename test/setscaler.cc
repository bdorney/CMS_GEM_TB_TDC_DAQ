#include <iostream>
#include "CAENVMElib.h"
#include <inttypes.h>
//#include "caenUtil.h"


using namespace std;

long BHandle;
//int32_t BHandle;

void help(){
    cout << "Usage: setscaler n\n"
      "  n: count limit from 0 to 1024\n\n";
}

int main(int argc, char* argv[]){

  if(argc!=2 || strcmp(argv[1], "-h")==0){
    help();
    return 0;
  }

  int limit = strtoul(argv[1], 0, 0);
  if(limit<0 || limit>1024){
    help();
    return -1;
  }

  //CAEN PCI/VME I/F initialization:
  const CVBoardTypes  VMEBoard=cvV2718;
  const short         Link=0;
  const short         Device=0;
  
  cout << "Initializing CAEN I/F...";
  CVErrorCodes ret = CAENVME_Init( VMEBoard, Device, Link, &BHandle );
  if(ret!=0){
    cout << "Failed to intialize CAEN I/F. "
         << CAENVME_DecodeError(ret) << "\n";
    return 1;
  }
  cout << " Done\n";

  const int autoReset = 1;
  ret = CAENVME_SetScalerConf(BHandle, limit & 0x3FF, autoReset, cvInputSrc0, cvInputSrc1, cvManualSW);
	
  if( ret != cvSuccess) {
		cout << "Failed to configure scaler with error '" << CAENVME_DecodeError( ret ) << "[" << ret << "]" << "'\n";
  }
  ret = CAENVME_SetOutputConf(BHandle, cvOutput4, cvDirect, cvActiveHigh, cvMiscSignals);
  if(ret!=cvSuccess) {
    cout << "Failed to configure output signal with error '" << CAENVME_DecodeError(ret) << "'\n";
  }

  ret
    = CAENVME_SetInputConf(BHandle, cvInput0, cvDirect, cvActiveHigh);
  if(ret!=cvSuccess) {
    cout << "Failed to configure input signal with error '" << CAENVME_DecodeError(ret) << "'\n";
  }

  ret = CAENVME_EnableScalerGate(BHandle);
  if(ret!=cvSuccess) {
    cout << "Failed to enable scaler with error '" << CAENVME_DecodeError(ret) << "'\n";
  }

  int period = 20;
  int width = 10;
  int npulses = 100;

  ret = CAENVME_SetPulserConf( BHandle, cvPulserA, period, width, cvUnit25ns, npulses, cvInputSrc0, cvInputSrc0 );
  if( ret != cvSuccess ) {
    cout << "Failed to configure pulser with error '" << CAENVME_DecodeError( ret ) << " [ "<< ret << " ]" << "'\n";
  }

  ret = CAENVME_SetOutputConf(BHandle, cvOutput0, cvDirect, cvActiveLow, cvMiscSignals);
  if(ret!=cvSuccess) {
    cout << "Failed to configure output signal with error '" << CAENVME_DecodeError(ret) << "'\n";
  }

  ret = CAENVME_SetInputConf( BHandle, cvInput1, cvInverted, cvActiveLow );
  if(ret!=cvSuccess) {
    cout << "Failed to configure input signal with error '" << CAENVME_DecodeError(ret) << "'\n";
  }

   ret = CAENVME_StopPulser( BHandle, cvPulserA );
   if(ret!=cvSuccess) {
     cout << "Failed  with error '" << CAENVME_DecodeError(ret) << "'\n";
   }
  
   ret = CAENVME_StartPulser(BHandle, cvPulserA);
   if(ret!=cvSuccess) {
     cout << "Failed  with error '" << CAENVME_DecodeError(ret) << "'\n";
   }

   unsigned short data;

   ret = CAENVME_ReadRegister(BHandle, cvInMuxRegSet, &data);
   if(ret!=cvSuccess) {
     cout << "Failed  with error '" << CAENVME_DecodeError(ret) << "'\n";
   }
	 cout << "Data = " << data << endl;
   data |= (1<<1);

   ret = CAENVME_WriteRegister(BHandle, cvInMuxRegSet, data);
   if(ret!=cvSuccess) {
     cout << "Failed  with error '" << CAENVME_DecodeError(ret) << "'\n";
   }

   if(BHandle!=0){
     CAENVME_End(BHandle);
   }
  return 0;
}

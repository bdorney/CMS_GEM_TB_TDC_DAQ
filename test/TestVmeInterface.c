#include "IDaqVmeInterface.hh"

#include <sys/time.h> // needed for gettimeofday
#include <time.h>     // needed for nanosleep
#include <unistd.h>   // needed for sleep

int main(){
	timespec rt, at;
	rt.tv_sec = 0;
	rt.tv_nsec = 5000;
	
	IDaqVmeInterface vmeInt;
	vmeInt.Connect();
	vmeInt.Reset();
	sleep( 1 );
	vmeInt.SetInputConf( 0, 1 );
	vmeInt.SetOutputSource( 4, cvManualSW );
	//vmeInt->SetOutputSource( 4, cvInputSrc0 );
	vmeInt.SetOutputLevel( 4, IDaqEnable );
	vmeInt.SetOutputLevel( 4, IDaqDisable );
	
	int trigger0 = 0;
	int trigger1 = 0;	
	while( 1 != 2 ){
		vmeInt.CheckInputLevel();
		trigger0 = vmeInt.GetInputLevel( 0 );
		trigger1 = vmeInt.GetInputLevel( 1 );
		cout << trigger0 << " : " << trigger1 << endl;
		//if ( nanosleep( &rt, &at ) != 0 ) cout << "Nanosleep failed" << endl;
	}
	vmeInt.Disconnect();
	return 0;
}


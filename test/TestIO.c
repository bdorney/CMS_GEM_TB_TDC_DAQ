#include <sys/time.h> // needed for gettimeofday
#include <time.h>     // needed for nanosleep
#include "IDaqVmeInterface.hh"
#include "IDaqV513.hh"

IDaqVmeInterface * vmeInt;

int main( int argc, char **argv ){
	timespec rt, at;
	rt.tv_sec = 0;
	//  rt.tv_nsec = 100000;
	rt.tv_nsec = 10; // timeout in nsec
	//int maxEvents = 100;
	vmeInt = new IDaqVmeInterface();
	vmeInt->Connect();
	vmeInt->Reset();
	sleep( 1 );
	IDaqV513 * trigRecv = new IDaqV513();
	trigRecv->SetBaseAddress( 0x00C00B00 );
	trigRecv->SetVmeInterface( vmeInt );
	trigRecv->Reset();
	//Set the trigger reciever channel (0) and veto channel (1)
	cout << endl << "Trigger channel configuration: " << endl;
	trigRecv->SetChannelStatusReg( 0, V513_Input, V513_Negative, V513_Glitched, V513_Transparent );
	cout << endl << "Veto channel configuration: " << endl;
	trigRecv->SetChannelStatusReg( 1, V513_Output, V513_Negative, V513_Glitched, V513_Transparent );
	trigRecv->Clear();

	cout << "Input Reg: " << trigRecv->GetInputReg() << endl;
	trigRecv->SetOutput( 1, 1 );
	//trigRecv->SetOutput( 0, 0 );
	cout << "Input Reg: " << trigRecv->GetInputReg() << endl;
	trigRecv->Clear();
	cout << "Input Reg: " << trigRecv->GetInputReg() << endl;
	while( 1 ){
		trigRecv->SetOutput( 1, 1 );
		if ( nanosleep( &rt, &at ) != 0 ) cout << "Nanosleep failed" << endl;
		trigRecv->SetOutput( 1, 0 );
		if ( nanosleep( &rt, &at ) != 0 ) cout << "Nanosleep failed" << endl;
	}
	cout << "Test done." << endl;

	
	return 0;
}


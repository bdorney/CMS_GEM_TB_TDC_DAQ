#include "IDaqManager.hh"
#include "IDaqVmeInterface.hh"
#include "IDaqV792.hh"
#include "IDaqV775.hh"

int main(){
	
	IDaqManager daq;
	IDaqVmeInterface * vmeInt = new IDaqVmeInterface();
	vmeInt->Connect();
	IDaqV792 * qdc = new IDaqV792();
	qdc->SetBaseAddress( 0xF00B0000 );
	qdc->SetVmeInterface( vmeInt );
	//qdc->SetGeoAddress( 1 );
	
	IDaqV775 * tdc = new IDaqV775();
	tdc->SetBaseAddress( 0x400C0000 );
	tdc->SetVmeInterface( vmeInt );
	tdc->Initialize();
	
	//
	daq.OpenDataFile( "./test.dat" );
	
	daq.SetMaxEventsCount( 500 );
	daq.SetVmeInterface( vmeInt );
	daq.AddModule( qdc );
	daq.AddModule( tdc );
	daq.SetStatusBit( DAQ_READY );
	//daq.Arm();
	daq.StartDAQ(); // DAQ will collect 500 events 
	daq.StopDAQ();
	
	daq.CloseDataFile();
	
	return 0;
}

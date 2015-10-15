//#include "IDaqVmeInterface.hh"
#include "IDaqV792.hh"

int main(){
	
	IDaqVmeInterface * vmeInt = new IDaqVmeInterface();
	vmeInt->Connect();
	
	IDaqV792 * q = new IDaqV792();
	//Set the VME Interface
	q->SetVmeInterface( vmeInt );
	q->Initialize();
	q->Arm();
	/*
	uint16_t Data = 0;
	vmeInt.Read( 0x400c103E, data );
	*/
	cout << "Status = " << q->GetStatus() << endl;
	//cout << "Data = " << data << endl;
	vmeInt->Disconnect();
	return 0;
}
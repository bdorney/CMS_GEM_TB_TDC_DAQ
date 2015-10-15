/*
** RPC DAQ
** TODO: 
** 1.) Class for DAQ readout
** 1.1.) Read input parameters (trigger count, number of events to read) from INI file
** 1.2.) Open Root file, Create ROOT Tree + branches
** 1.3.) Read TDC Hit
** 1.4.) Fill Root Data Tree
** 1.5.) Write Root tree, Close ROOT file.
** 2.) Data analysis class
*/
int main(){
	IniFile IniFile( "daq.ini" );
	int ReadCount = IniFile.Int( "ReadCount", 100 ); 
	IsrRpcDaq * IsrRpcDaq = new IsrRpcDaq();
	IsrRpcDaq->Initialize();
	while( IsrRpcDaq->Read() < ReadCount ){}
	IsrDaq->Finalize();
}

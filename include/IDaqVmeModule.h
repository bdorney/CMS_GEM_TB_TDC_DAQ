#ifndef __IDAQVMEMODULE_H_
#define __IDAQVMEMODULE_H_

#include "IDaqVmeTypes.h"
#include "IDaqVmeInterface.h"

#include <fstream>
#include <iostream>
#include <memory>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <vector>

//using namespace std;

class IDaqVmeVirtualModule{
	public:
		IDaqVmeVirtualModule(){};
		virtual ~IDaqVmeVirtualModule(){};	
		virtual void Initialize(){ status = IDaqGenericError; }
		virtual void Arm(){ status = IDaqGenericError; };	
        virtual int Readout(FILE *fp, uint64_t &fs){ return 0; }
		virtual int Readout( std::vector<uint32_t>& aData ){ return 0; };
		virtual bool IsInitialized(){ return initStatus; }
  	
        virtual IDaqVmeModuleType GetModuleType(){ return moduleType; }
        virtual void SetModuleType( IDaqVmeModuleType mt ){ moduleType = mt;}
        
        virtual uint16_t GetWordSize() { return 0; }
        
        virtual IDaqVmeCode GetStatus(){ return status; };
        virtual std::string GetName(){ return name; }
        virtual void SetName( std::string aName ="Unknown Module" ){ name = aName; }
        virtual void Connect( std::shared_ptr<IDaqVmeInterface> vi ){
            vmeInt = vi ;
            status = IDaqCommError;
            if ( vmeInt) {
                if ( ( status = vmeInt->GetStatus() ) == IDaqSuccess )  connectStatus = true;
            }
        }

        virtual bool IsConnected(){
            connectStatus = false;
            if ( vmeInt ) {
            if ( vmeInt->GetStatus() == IDaqSuccess ) connectStatus = true;
            } else { 
                std::cout << " VME Interface is not defined " << std::endl;
            }
            return connectStatus;
        }

    protected:
        IDaqVmeModuleKind moduleKind;
        IDaqVmeModuleType moduleType;
        //IDaqVmeInterface * vmeInt;
        std::shared_ptr<IDaqVmeInterface> vmeInt;
        IDaqVmeCode status;
        std::string name;
        bool connectStatus;
        bool initStatus;
};

template <typename AddressType, typename DataType >
class IDaqVmeModule : public IDaqVmeVirtualModule {

	public:
		IDaqVmeModule(){
			moduleType = idmGenericModule;
			moduleKind = idmkUnknown;
			SetName();
		};
		
		virtual ~IDaqVmeModule(){};
		
		//void SetVmeInterface( IDaqVmeInterface * aVmeInt ) { vmeInt = aVmeInt; }
        void SetVmeInterface( std::shared_ptr<IDaqVmeInterface> aVmeInt ) { vmeInt = aVmeInt; }
    
		virtual void SetBaseAddress( AddressType aBaseAddress ){ ba = aBaseAddress; }
		virtual AddressType GetBaseAddress(){ return ba; }
		
		DataType * GetData() {return data;}
        uint16_t   GetDataSize() { return bufferSize;}
        uint16_t   GetBufferCapacity() { return bufferSize / sizeof( DataType ); }
        uint16_t   GetWordSize() { return sizeof( DataType ); }
        uint16_t   GetNByteRead(){ return nWordRead * sizeof( DataType ); }
        uint16_t   GetNWordRead(){ return nWordRead; } 
  	
        DataType GetWord( uint16_t iw = 0 ){
            if ( iw < nWordRead) { return  data[ iw ]; }
            else return 0;
		};
  	
  	
  	
		void CheckRegister( const AddressType &aAddress, uint16_t &regval ){
			AddressType addr = ba + aAddress;
			uint16_t data;
			vmeInt->Read( addr, data );
			status = vmeInt->GetStatus();
			if ( status == IDaqSuccess ) regval = data;
		};
		
		void CheckRegister( const AddressType &aAddress, uint32_t &regval ){
			AddressType addr = ba + aAddress;
			uint32_t data;
			vmeInt->Read( addr, data );
			status = vmeInt->GetStatus();
			if ( status == IDaqSuccess ) regval = data;
		};
  	
		void SetRegister( const AddressType &aAddress, const uint16_t &aData, uint16_t &regval ){
			AddressType addr = ba + aAddress;
			vmeInt->Write( addr, aData );
			status = vmeInt->GetStatus();
			if ( status == IDaqSuccess ) regval = aData;
		};
		
		void SetRegister( const AddressType &aAddress, const uint32_t &aData, uint32_t &regval ){
			AddressType addr = ba + aAddress;
			vmeInt->Write( addr, aData );
			status = vmeInt->GetStatus();
			if ( status == IDaqSuccess ) regval = aData ;
		};
  	
		virtual void ReadOutputBuffer( const uint16_t &nw, const AddressType &aAddress = 0 ){
			nWordRead = 0;
			status = IDaqCommError;
			if ( vmeInt ) {
				vmeInt->ReadBLT( ba + aAddress, data[ 0 ], nw, nWordRead );
				status = vmeInt->GetStatus();
				//if ( status != IDaqSuccess ) _nWordRead = nw; else _nWordRead = 0;
    // ret = _vmeInt->ReadBLT( _BA + ad, _outputBuffer[0], nw );
    //      if ( ret == MDSuccess ) { _nWordRead = nw; }
    //      else { _nWordRead = 0; }
			}
		};
		
		
		
        void PrintOutputBuffer( std::ostream &os = std::cout ){
			for (uint16_t iw = 0; iw < nWordRead; ++iw ) {
                os << std::endl << std::showbase << std::hex << data[ iw ];
			}
		};
		
		void WriteOutputBuffer( FILE *fp ){
			fwrite( data, sizeof( DataType ), nWordRead, fp );
		};
		
		virtual void ReadAndWriteOutputBuffer( FILE *fp, uint16_t nw = 1 ){
			ReadOutputBuffer( nw );
			WriteOutputBuffer( fp );
		};	
  	
		virtual void ReadAndWriteOutputBuffer( std::vector<uint32_t> &aData, uint16_t nw = 1 ){
			ReadOutputBuffer( nw );
            std::cout << "nWordsRead = " << nWordRead << std::endl;
			aData.assign( data, data + nWordRead );
			for( int i=0; i < nWordRead; ++i ){
                std::cout << "data[ " << i << " ] = " << data[ i ] << std::endl;
			}
			
		};
	protected:
		
		AddressType ba;
		DataType *  data;	
		uint16_t    bufferSize;       // number of bytes in the buffer
        uint16_t    nWordRead;        // number of DataType words read
  	
};

#endif

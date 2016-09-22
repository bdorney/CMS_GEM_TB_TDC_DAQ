#include "IDaqVmeModule.h"

IDaqVmeVirtualModule::IDaqVmeVirtualModule() : m_vmeInt(new IDaqVmeInterface){
	m_vmeStatus = IDaqSuccess;
	m_vmeKind = idmkUnknown;
    	m_vmeType = idmGenericModule;

        SetName();

	m_bConnectStatus = false;
	m_bInitStatus = false; 
}

void IDaqVmeVirtualModule::Connect( std::shared_ptr<IDaqVmeInterface> inputVmeInt ){
        m_vmeInt = inputVmeInt ;
        m_vmeStatus = IDaqCommError;
        if ( m_vmeInt != nullptr) {
            if ( ( m_vmeStatus = m_vmeInt->GetStatus() ) == IDaqSuccess )  m_bConnectStatus = true;
        }
        
	return;
}

bool IDaqVmeVirtualModule::IsConnected(){
        m_bConnectStatus = false;
        if ( m_vmeInt != nullptr ) {
            if ( m_vmeInt->GetStatus() == IDaqSuccess ) m_bConnectStatus = true;
        }
        else {
            std::cout << " VME Interface is not defined " << std::endl;
        }
        
        return m_bConnectStatus;
}

//================================================================================================


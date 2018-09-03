#pragma once
#include <COBIA.h>

using namespace COBIA;

class MaterialPort :
	public CapeOpenObject<MaterialPort>,
	public CAPEOPEN110::CapeIdentificationAdapter<MaterialPort>,
	public CAPEOPEN110::CapeUnitPortAdapter<MaterialPort> {

	CapeStringImpl name;
	CapeStringImpl &unitOperationName;
	CAPEOPEN110::CapeValidationStatus &unitValidationStatus;
	CAPEOPEN110::CapePortDirection direction;
	CAPEOPEN110::CapeThermoMaterial connectedMaterial;
public:

	const CapeStringImpl getDescriptionForErrorSource() {
		return name + COBIATEXT(" port of ")+unitOperationName; 
	}

	MaterialPort(CapeStringImpl &_unitOperationName,
		CAPEOPEN110::CapeValidationStatus &_unitValidationStatus,
		CAPEOPEN110::CapePortDirection _direction,
		const COBIACHAR *portName) :
		unitOperationName(_unitOperationName),
		unitValidationStatus(_unitValidationStatus),
		direction(_direction),
		name(portName) {
	}

	~MaterialPort() {
	}
	CAPEOPEN110::CapeThermoMaterial getMaterial() {
		return connectedMaterial;
	}

	//CAPEOPEN110::ICapeIdentification

	void getComponentName(/*out*/ CapeString name) {
		name = this->name;
	}
	void putComponentName(/*in*/ CapeString name) {
		throw cape_open_error(COBIAERR_Denied);
	}
	void getComponentDescription(/*out*/ CapeString desc) {
		desc = L"Material port";
	}
	void putComponentDescription(/*in*/ CapeString desc) {
		throw cape_open_error(COBIAERR_Denied);
	}

	//CAPEOPEN110::ICapeUnitPort
	
	CAPEOPEN110::CapePortType getPortType() {
		return CAPEOPEN110::CAPE_MATERIAL;
	}
	CAPEOPEN110::CapePortDirection getDirection() {
		return direction;
	}
	CapeInterface getConnectedObject() {
		return connectedMaterial;
	}
	void Connect(/*in*/ CapeInterface objectToConnect) {
		CAPEOPEN110::CapeThermoMaterial newConnectedMaterial = objectToConnect;
		if (!newConnectedMaterial) {
			//expected a material object
			throw cape_open_error(COBIAERR_NoSuchInterface);
		}
		unitValidationStatus = CAPEOPEN110::CAPE_NOT_VALIDATED;
		connectedMaterial = newConnectedMaterial;
	}
	void Disconnect() {
		unitValidationStatus = CAPEOPEN110::CAPE_NOT_VALIDATED;
		connectedMaterial.clear();
	}

	

	

};

using MaterialPortPtr = CapeOpenObjectSmartPointer<MaterialPort>;

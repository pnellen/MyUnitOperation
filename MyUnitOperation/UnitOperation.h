#pragma once
#include <COBIA.h>
#include "MaterialPort.h"
#include "PortCollection.h"
#include <algorithm>
using namespace COBIA;

class UnitOperation :
	
	public CapeOpenObject<UnitOperation>,
		public CAPEOPEN110::CapeIdentificationAdapter<UnitOperation>,
		public CAPEOPEN110::CapeUnitAdapter<UnitOperation>,
		public CAPEOPEN110::CapeUtilitiesAdapter<UnitOperation>,
		public CAPEOPEN110::CapePersistAdapter<UnitOperation> {
		CapeStringImpl name, description;
		CapeBoolean dirty;
		MaterialPortPtr feed, product;
		PortCollectionPtr portCollection;
		CAPEOPEN110::CapeValidationStatus validationStatus;
		CapeArrayStringImpl flashCondT, flashCondP;
		CapeArrayStringImpl flashPhaseIDs;
		CapeArrayEnumerationImpl<CAPEOPEN110::CapePhaseStatus> flashPhaseStatus;
public:

	const CapeStringImpl getDescriptionForErrorSource() {
		//TODO: implement this function; this function returns a description of the current object for error handling
		// The return value may be changed to any type (by value of reference) that implements const CapeCharacter * c_str()
		// It is recommended to include both the object type and name in the returned description, e.g. "Pump P018"
		return COBIATEXT("UnitOperation")+name; //TODO: change
	}

	//Registration info

	static const CapeUUID getObjectUUID() {
		//Class UUID = 2A2C2421-1760-497F-B894-D7F5FFD5DA95
		return CapeUUID{{0x2a,0x2c,0x24,0x21,0x17,0x60,0x49,0x7f,0xb8,0x94,0xd7,0xf5,0xff,0xd5,0xda,0x95}};
	}



	static void Register(CapePMCRegistrar registrar) {
		registrar.putName(COBIATEXT("My Unit Operation"));
		registrar.putDescription(COBIATEXT("COBIA CAPE-OPEN Unit Operation example"));
		registrar.putCapeVersion(COBIATEXT("1.1"));
		registrar.putComponentVersion(COBIATEXT("1.0.0.0"));
		registrar.putAbout(COBIATEXT("My COBIA Walk Through"));
		registrar.putProgId(COBIATEXT("MyUnitOperation::UnitOperation"));
		registrar.addCatID(CAPEOPEN110::categoryId_Component);
		registrar.addCatID(CAPEOPEN110::categoryId_UnitOperation);
	}
	UnitOperation();
	~UnitOperation() {
	}
	//CAPEOPEN110::ICapeIdentification
	void getComponentName(/*out*/ CapeString name);
	void putComponentName(/*in*/ CapeString name);
	void getComponentDescription(/*out*/ CapeString desc);
	void putComponentDescription(/*in*/ CapeString desc);

	//CAPEOPEN110::ICapeUnit
	CAPEOPEN110::CapeCollection<CAPEOPEN110::CapeUnitPort> ports();
	CAPEOPEN110::CapeValidationStatus getValStatus();
	void Calculate();
	CapeBoolean Validate(/*out*/ CapeString message);

	//CAPEOPEN110::ICapeUtilities
	CAPEOPEN110::CapeCollection<CAPEOPEN110::CapeParameter> getParameters();
	void putSimulationContext(/*in*/ CAPEOPEN110::CapeSimulationContext context);
	void Initialize();
	void Terminate();
	CAPEOPEN110::CapeEditResult Edit();
	
	//CAPEOPEN110::ICapePersist
	void Save(/*in*/ CAPEOPEN110::CapePersistWriter writer,/*in*/ CapeBoolean clearDirty);
	void Load(/*in*/ CAPEOPEN110::CapePersistReader reader);
	CapeBoolean getIsDirty();
};
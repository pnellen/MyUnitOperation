#pragma once
#include <COBIA.h>
#include <vector>

using namespace COBIA;

class PortCollection :
	public CapeOpenObject<PortCollection>,
	public CAPEOPEN110::CapeIdentificationAdapter<PortCollection>,
	public CAPEOPEN110::CapeCollectionAdapter<CAPEOPEN110::CapeUnitPort,PortCollection> {

	CapeStringImpl &unitOperationName;
	std::vector<CAPEOPEN110::CapeUnitPort> elements;
public:

	const CapeStringImpl getDescriptionForErrorSource() {
		return COBIATEXT("Port collection of ") + unitOperationName; 
	}

	
	PortCollection(CapeStringImpl &_unitOperationName) :
		unitOperationName(_unitOperationName){
	}

	~PortCollection() {
	}

	void addPort(CAPEOPEN110::CapeUnitPort port) {
		elements.emplace_back(port);
	}

	//CAPEOPEN110::ICapeIdentification
	
	void getComponentName(/*out*/ CapeString name) {
		name = COBIATEXT("Port collection");

	}

	void putComponentName(/*in*/ CapeString name) {
		throw cape_open_error(COBIAERR_Denied);

	}

	void getComponentDescription(/*out*/ CapeString desc) {
			desc = COBIATEXT("Port collection");

	}

	void putComponentDescription(/*in*/ CapeString desc) {
		throw cape_open_error(COBIAERR_Denied);

	}

	//CAPEOPEN110::ICapeCollection<CAPEOPEN110::ICapeUnitPort>

	CAPEOPEN110::CapeUnitPort Item(/*in*/ CapeInteger index) {
		if ((index<0) || (index>(CapeInteger)elements.size())) {
			throw cape_open_error(COBIAERR_NoSuchItem);
		}
		return elements[index];

	}

	CAPEOPEN110::CapeUnitPort Item(/*in*/ CapeString name) {
		CapeStringImpl elName;
		for (CAPEOPEN110::CapeUnitPort &el : elements) {
			CAPEOPEN110::CapeIdentification identification(el);
			identification.getComponentName(elName);
			if (elName == name) return el;
		}
		throw cape_open_error(COBIAERR_NoSuchItem); //not found

	}

	CapeInteger getCount() {
		return (CapeInteger)elements.size();

	}

	
};

using PortCollectionPtr = CapeOpenObjectSmartPointer<PortCollection>;

#include "UnitOperation.h"
#include <COBIA_PMC.h>
#include "MaterialPort.h"
#include "PortCollection.h"
#include <algorithm>

COBIA_PMC_REGISTER(UnitOperation);

UnitOperation::UnitOperation() :
	name(COBIATEXT("NoName")),
	feed(new MaterialPort(name, validationStatus, CAPEOPEN110::CAPE_INLET, COBIATEXT("Feed"))),
	product(new MaterialPort(name, validationStatus, CAPEOPEN110::CAPE_OUTLET, COBIATEXT("Product"))),
	portCollection(new PortCollection(name)),
	validationStatus(CAPEOPEN110::CAPE_NOT_VALIDATED),
	dirty(false) {
	portCollection->addPort(feed);
	portCollection->addPort(product);
	//prepare T & P flash conditions for product flash
	flashCondT.resize(3);
	flashCondT[0] = COBIATEXT("temperature");
	flashCondT[2] = COBIATEXT("overall");
	flashCondP.resize(3);
	flashCondP[0] = COBIATEXT("pressure");
	flashCondP[2] = COBIATEXT("overall");
}

//CAPEOPEN110::ICapeIdentification
void UnitOperation::getComponentName(/*out*/ CapeString name) {
	name = this->name;
}
void UnitOperation::putComponentName(/*in*/ CapeString name) {
	this->name = name;
	dirty = true;
}
void UnitOperation::getComponentDescription(/*out*/ CapeString desc) {
	desc = description;
}
void UnitOperation::putComponentDescription(/*in*/ CapeString desc) {
	description = desc;
	dirty = true;
}

//CAPEOPEN110::ICapeUnit
CAPEOPEN110::CapeCollection<CAPEOPEN110::CapeUnitPort> UnitOperation::ports() {
	return portCollection;
}
CAPEOPEN110::CapeValidationStatus UnitOperation::getValStatus() {
	return validationStatus;
}
void UnitOperation::Calculate() {
	if (validationStatus != CAPEOPEN110::CAPE_VALID) {
		//the PME must validate the unit prior to calculation
		throw cape_open_error(COBIATEXT("Unit is not in a valid state"));
	}
	CAPEOPEN110::CapeThermoMaterial feedMaterial = feed->getMaterial();
	CAPEOPEN110::CapeThermoMaterial productMaterial = product->getMaterial();
	//this array of property values could be a class member; that would avoid most memory allocations
	// the array is re-used below for various properties.
	CapeArrayRealImpl propValues;
	//copy total flow
	feedMaterial.GetOverallProp(ConstCapeString(COBIATEXT("totalFlow")),
		ConstCapeString(COBIATEXT("mole")), propValues);
	productMaterial.SetOverallProp(ConstCapeString(COBIATEXT("totalFlow")),
		ConstCapeString(COBIATEXT("mole")), propValues);
	//copy P,X and increase T by 100 K
	CapeReal T, P;
	feedMaterial.GetOverallTPFraction(T, P, propValues);
	productMaterial.SetOverallProp(ConstCapeString(COBIATEXT("fraction")),
		ConstCapeString(COBIATEXT("mole")), propValues);
	propValues.resize(1);
	propValues[0] = P;
	productMaterial.SetOverallProp(ConstCapeString(COBIATEXT("pressure")),
		ConstCapeNullString(), propValues);
	propValues[0] = T + 10;
	productMaterial.SetOverallProp(ConstCapeString(COBIATEXT("temperature")),
		ConstCapeNullString(), propValues);
	//allow all phases to take part in product flash
	productMaterial.SetPresentPhases(flashPhaseIDs, flashPhaseStatus);
	//flash product at specified T & P
	CAPEOPEN110::CapeThermoEquilibriumRoutine productEquilibrium(productMaterial);
	productEquilibrium.CalcEquilibrium(flashCondT, flashCondP, ConstCapeNullString());
}
CapeBoolean UnitOperation::Validate(/*out*/ CapeString message) {
	CapeBoolean res = true;
	//check whether all ports are connected, and connected to materials with equal compound lists
	CapeArrayStringImpl refCompIDs, compIDs;
	CapeArrayStringImpl formulae, names, casNumbers;
	CapeArrayRealImpl boilTemps, molecularWeights;
	CAPEOPEN110::CapeCollection<CAPEOPEN110::CapeUnitPort> collection(portCollection);
	for (CAPEOPEN110::CapeUnitPort p : collection) {
		CapeInterface connectedObject = p.getConnectedObject();
		if (!connectedObject) {
			CAPEOPEN110::CapeIdentification portIdentification(p);
			CapeStringImpl portName;
			portIdentification.getComponentName(portName);
			message = COBIATEXT("Port ") + portName + COBIATEXT(" is not connected");
			res = false;
			break;
		}
		CAPEOPEN110::CapeThermoCompounds materialCompounds(connectedObject);
		if (refCompIDs.empty()) {
			materialCompounds.GetCompoundList(refCompIDs, formulae, names,
				boilTemps, molecularWeights, casNumbers);
		}
		else {
			materialCompounds.GetCompoundList(compIDs, formulae, names,
				boilTemps, molecularWeights, casNumbers);
		}
	}
	if (res) {
		//additional checks
		bool sameCompList = (refCompIDs.size() == compIDs.size());
		for (size_t i = 0; (i<compIDs.size()) && (sameCompList); i++) {
			sameCompList = (compIDs[i].compare(refCompIDs[i]) == 0);
		}
		if (!sameCompList) {
			message = COBIATEXT("Connected material streams expose inconsistent compound lists");
			res = false;
		}
		else if (refCompIDs.empty()) {
			message = COBIATEXT("Connected material streams expose zero compound");
			res = false;
		}
	}
	if (res) {
		//prepare the flash phase list for the product flash
		// this remains constant between validations
		CAPEOPEN110::CapeThermoMaterial material = product->getMaterial();
		CAPEOPEN110::CapeThermoPhases materialPhases(material);
		CapeArrayStringImpl stateOfAggregation, keyCompounds;
		materialPhases.GetPhaseList(flashPhaseIDs, stateOfAggregation, keyCompounds);
		flashPhaseStatus.resize(flashPhaseIDs.size());
		std::fill(flashPhaseStatus.begin(), flashPhaseStatus.end(), CAPEOPEN110::CAPE_UNKNOWNPHASESTATUS);
	}
	//update validation status
	validationStatus = (res) ? CAPEOPEN110::CAPE_VALID : CAPEOPEN110::CAPE_INVALID;
	return res;
}

//CAPEOPEN110::ICapeUtilities
CAPEOPEN110::CapeCollection<CAPEOPEN110::CapeParameter> UnitOperation::getParameters() {
	throw cape_open_error(COBIAERR_NotImplemented); //no parameters
}
void UnitOperation::putSimulationContext(/*in*/ CAPEOPEN110::CapeSimulationContext context) {
	//do nothing
}
void UnitOperation::Initialize() {
	//do nothing
}
void UnitOperation::Terminate() {
	//disconnect ports
	CAPEOPEN110::CapeCollection<CAPEOPEN110::CapeUnitPort> collection(portCollection);
	for (CAPEOPEN110::CapeUnitPort p : collection) {
		p.Disconnect();
	}
}
CAPEOPEN110::CapeEditResult UnitOperation::Edit() {
	throw cape_open_error(COBIAERR_NotImplemented); //no GUI
}

//CAPEOPEN110::ICapePersist
void UnitOperation::Save(/*in*/ CAPEOPEN110::CapePersistWriter writer,/*in*/ CapeBoolean clearDirty) {
	writer.Add(ConstCapeString(COBIATEXT("name")), name);
	writer.Add(ConstCapeString(COBIATEXT("description")), description);
	if (clearDirty) {
		dirty = false;
	}
}
void UnitOperation::Load(/*in*/ CAPEOPEN110::CapePersistReader reader) {
	reader.GetString(ConstCapeString(COBIATEXT("name")), name);
	reader.GetString(ConstCapeString(COBIATEXT("description")), description);
}
CapeBoolean UnitOperation::getIsDirty() {
	return dirty;
}

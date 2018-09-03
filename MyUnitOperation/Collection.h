#pragma once
#include <COBIA.h>

using namespace COBIA;

template <typename Element> class Collection :
	public CapeOpenObject<Collection> {

public:

	const CapeStringImpl getDescriptionForErrorSource() {
		//TODO: implement this function; this function returns a description of the current object for error handling
		// The return value may be changed to any type (by value of reference) that implements const CapeCharacter * c_str()
		// It is recommended to include both the object type and name in the returned description, e.g. "Pump P018"
		return COBIATEXT("Collection"); //TODO: change
	}

	Collection() {
	}

	~Collection() {
	}

};

template <typename Element> using CollectionPtr = CapeOpenObjectSmartPointer<Collection<Element>>;

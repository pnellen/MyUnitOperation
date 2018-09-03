#include <COBIA.h>
//this definition prior to COBIA_PMC.h ensures the entry points are created. Do this only in one compilation unit
#define COBIA_PMC_ENTRY_POINTS
//this definition prior to COBIA_PMC.h ensures the a default DllMain entry point is created (if COBIA_PMC_ENTRY_POINTS and _WIN32 are defined)
#define COBIA_PMC_DEFAULT_DLLMAIN
//PMC entry points are defined here:
#include <COBIA_PMC.h>

//! Define registration scope
/*!
PMC module must implement this function to indicate whether object
	registration must be for all users or for the current user.

	Alternatively, define either PMC_REGISTERFORALLUSERS or PMC_REGISTERFORCURRENTUSER 
	prior to including COBIA_PMC.h, if COBIA_PMC_ENTRY_POINTS is defined

   return true if registration is for all users, false if registration is for current user only
*/
bool isPMCRegistrationForAllUsers() {
	return false; //TODO: modify
}

// MAUS WARNING: THIS IS LEGACY CODE.
// FillMaterials.hh
//
// Code to create G4Material instances for each material needed in MICE and store them in the MiceMaterial object
// that is found in the MICERun
//
// 25th May 2006
//
// Malcolm Ellis

#ifndef SIMULATION_FILLMATERIALS_HH
#define SIMULATION_FILLMATERIALS_HH 1

/** Append to the list of mice materials. If materials is NULL, initialises a
 *  new list from scratch and fills it.
 */
class MiceMaterials;
MiceMaterials* fillMaterials(MiceMaterials* materials_list);

#endif


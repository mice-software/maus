/*!
 * \file MC_LinkDef.h
 *
 * \author Alexander Richards, Imperial College London
 * \date 06/01/2012
 *
 * This file is used by the ROOT "rootcint" program in order to generate ROOT dictionaries
 * for the correct MC branch classes (those listed below). The + suffix is part of the 
 * ROOT magic for making sure something or other is included. ;-) 
 */
#ifdef __CINT__

#pragma link C++ class MCChannelID+;
#pragma link C++ class MCPrimaryMomentum+;
#pragma link C++ class MCPrimaryPosition+;
#pragma link C++ class MCHitsElement+;
#pragma link C++ class MCHits+;
#pragma link C++ class MCPrimary+;
#pragma link C++ class MCElement+;
#pragma link C++ class MC+;

#endif

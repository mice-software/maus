/*!
 * \file Digits_LinkDef.h
 *
 * \author Alexander Richards, Imperial College London
 * \date 06/01/2012
 *
 * This file is used by the ROOT "rootcint" program in order to generate ROOT dictionaries
 * for the correct Digits branch classes (those listed below). The + suffix is part of the 
 * ROOT magic for making sure something or other is included. ;-) 
 */
#ifdef __CINT__

#pragma link C++ class ChannelID+;
#pragma link C++ class MCMomentum+;
#pragma link C++ class MCPosition+;
#pragma link C++ class Digit+;
#pragma link C++ class DigitsElement+;
#pragma link C++ class Digits+;

#endif

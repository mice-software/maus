#ifndef DIGITS_STRUCTS_H
#define DIGITS_STRUCTS_H

#include <vector>
#include <cstring>
#include <TObject.h>

struct ChannelID{
  ChannelID():channel_number(0),
	      fiber_number(0),
	      plane_number(0),
	      station_number(0),
	      tracker_number(0){
    type = new char[40]();
    strcpy(type,"");}
  virtual ~ChannelID(){
    delete type;
    type=0;
  }
  int channel_number;
  int fiber_number;
  int plane_number;
  int station_number;
  int tracker_number;
  char* type;
  void clear(){
    channel_number=0;
    fiber_number=0;
    plane_number=0;
    station_number=0;
    tracker_number=0;
    strcpy(type,"");
  }

  ClassDef(ChannelID,1)  //Simple channel_id struct
};

struct MCMomentum{
  double x;
  double y;
  double z;
  void clear(){
    x=0;
    y=0;
    z=0;
  }

  ClassDef(MCMomentum,1)  //Simple mc_mom struct
};

struct MCPosition{
  double x;
  double y;
  double z;
  void clear(){
    x=0;
    y=0;
    z=0;
  }

  ClassDef(MCPosition,1)  //Simple mc_position struct
};

struct Digit{
  ChannelID channel_id;
  MCMomentum mc_mom;
  MCPosition mc_position;
  double number_photoelectrons;
  double time;
  int adc_counts;
  int tdc_counts;

  void clear(){
    channel_id.clear();
    mc_mom.clear();
    mc_position.clear();
    number_photoelectrons=0.;
    time=0.;
    adc_counts=0;
    tdc_counts=0;
  }
  ClassDef(Digit,1)  //Simple digit struct
  
};

struct DigitsElement{
  std::vector<Digit*> digits;
  
  void clear(){
    std::vector<Digit*>::iterator itr;
    for(itr=digits.begin();itr!=digits.end();++itr){ (*itr)->clear(); }
    digits.clear();
  }

  ClassDef(DigitsElement,1)  //Simple digit struct
};

struct Digits{
  std::vector<DigitsElement*> elements;
  
  void clear(){
    std::vector<DigitsElement*>::iterator itr;
    for(itr=elements.begin();itr!=elements.end();++itr){ 
      if((*itr)){ (*itr)->clear();}
    }
    elements.clear();
  }
  ClassDef(Digits,1)  //Simple digit struct
};


#endif

// SpecialHitTextFileIo.cc
//
// M.Ellis 27/8/2005

#include <sstream>
#include <iomanip>

#include "SpecialHitTextFileIo.hh"

SpecialHitTextFileIo::SpecialHitTextFileIo( SpecialHit* hit, int index ) : TextFileIoBase()
{
  miceMemory.addNew( Memory::SpecialHitTextFileIo );
	
  setTxtIoIndex( index );
  m_hit = hit;
}

SpecialHitTextFileIo::SpecialHitTextFileIo( std::string def ) : TextFileIoBase()
{
  miceMemory.addNew( Memory::SpecialHitTextFileIo );
  
  restoreObject( def );
}

std::string	SpecialHitTextFileIo::storeObject()
{
  _edep = m_hit->GetEdep();
  _iaz = m_hit->GetPhiNumber();
  _irad = m_hit->GetRnumber();
  _iax = m_hit->GetZnumber();
  _stepType = m_hit->GetStepType();
  _stepLength = m_hit->GetStepLength();

  _trackID = m_hit->GetTrackID();
  _stationNumber = m_hit->GetStationNo();
  _cellNumber = m_hit->GetCellNo();
  _pos = m_hit->GetPosition();
  _momentum = m_hit->GetMomentum();
  _volName = m_hit->GetVolumeName();
  _time = m_hit->GetTime();
  _energy = m_hit->GetEnergy();
  _pid = m_hit->GetPID();
  _mass = m_hit->GetMass();
  _charge = m_hit->GetCharge();
  _bfield = m_hit->GetBField();
  _efield = m_hit->GetEField();
  _parentrackID = m_hit->GetParenTrackID();
  _ancestrackID = m_hit->GetAncesTrackID();
  _vtxpos = m_hit->GetVtxPosition();
  _vtxtime = m_hit->GetVtxTime();
  _vtxmom = m_hit->GetVtxMomentum();
  _pathLength = m_hit->GetPathLength();
  _properTime = m_hit->GetProperTime();
  
  std::stringstream ss;
  ss << std::setprecision(10);
//  ss << std::fixed;

  ss << txtIoIndex() << " " << _stationNumber << " " << _cellNumber << " " <<  _iax << " " <<  _iaz << " " << _irad << " " << _trackID << " " << _charge << " " <<  _pid << " " << _mass << " " << _pos.x() << " " <<  _pos.y() << " " << _pos.z() << " " << _time << " " << _momentum.x() << " " << _momentum.y() << " " << _momentum.z() << " " << _energy << " " << _edep << " " << _bfield.x() << " " << _bfield.y() << " " << _bfield.z() << " " << _efield.x() << " " << _efield.y() << " " << _efield.z() << " " << _parentrackID << " " << _ancestrackID << " " << _vtxpos.x() << " " << _vtxpos.y() << " " << _vtxpos.z() << " " << _vtxtime << " " << _vtxmom.x() << " " << _vtxmom.y() << " " << _vtxmom.z() << " " << _stepType << " " << _stepLength << " " << _volName.c_str() << " " << _pathLength << " " << _properTime;
  
  if(ss.fail()) _pathLength = _properTime = 0; //backward compatibility
  return( ss.str() );
}

void		SpecialHitTextFileIo::restoreObject( std::string def )
{
  int myindex;
  double _posx, _posy, _posz;
  double _momentumx, _momentumy, _momentumz;
  double _vtxposx, _vtxposy, _vtxposz;
  double _vtxmomx, _vtxmomy, _vtxmomz;
  double _bfieldx, _bfieldy, _bfieldz;
  double _efieldx, _efieldy, _efieldz;

  std::istringstream ist( def.c_str() );

  ist >> myindex >> _stationNumber >> _cellNumber >> _iax >> _iaz >>  _irad
      >> _trackID >> _charge >> _pid >> _mass >> _posx >> _posy >> _posz >> _time
      >> _momentumx >> _momentumy >> _momentumz >> _energy >> _edep
      >> _bfieldx >> _bfieldy >> _bfieldz >> _efieldx >> _efieldy >> _efieldz
      >> _parentrackID >> _ancestrackID
      >> _vtxposx >> _vtxposy >> _vtxposz >> _vtxtime >> _vtxmomx >> _vtxmomy
      >> _vtxmomz >>   _stepType >> _stepLength >> _volName >> _pathLength >> _properTime;

  if(ist.fail()) _pathLength = 0; //backward compatibility
  _pos = Hep3Vector(_posx, _posy, _posz);
  _momentum = Hep3Vector(_momentumx, _momentumy, _momentumz);
  _vtxpos = Hep3Vector(_vtxposx, _vtxposy, _vtxposz);
  _vtxmom = Hep3Vector(_vtxmomx, _vtxmomy, _vtxmomz);
  _bfield = Hep3Vector(_bfieldx, _bfieldy, _bfieldz);
  _efield = Hep3Vector(_efieldx, _efieldy, _efieldz);

  setTxtIoIndex( myindex );
}

SpecialHit*	SpecialHitTextFileIo::makeSpecialHit()
{
  m_hit = new SpecialHit();

  m_hit->SetEdep(_edep);
  m_hit->SetCellNo( _iax, _iaz, _irad );
  m_hit->SetStepType(_stepType);
  m_hit->SetStepLength(_stepLength);

  m_hit->SetTrackID(_trackID);
  m_hit->SetStationNo(_stationNumber);
  m_hit->SetCellNo(_cellNumber);
  m_hit->SetPosition(_pos);
  m_hit->SetMomentum(_momentum);
  m_hit->SetVolumeName(_volName);
  m_hit->SetTime(_time);
  m_hit->SetEnergy(_energy);
  m_hit->SetPID(_pid);
  m_hit->SetMass(_mass);
  m_hit->SetCharge(_charge);
  m_hit->SetBField(_bfield);
  m_hit->SetEField(_efield);
  m_hit->SetParenTrackID(_parentrackID);
  m_hit->SetAncesTrackID(_ancestrackID);
  m_hit->SetVtxPosition(_vtxpos);
  m_hit->SetVtxTime(_vtxtime);
  m_hit->SetVtxMomentum(_vtxmom);
  m_hit->SetPathLength(_pathLength);
  m_hit->SetProperTime(_properTime);

  return m_hit;
}

void		SpecialHitTextFileIo::completeRestoration()
{
}


#include "EmCalHit.hh"
#include "EmCalDigit.hh"

//////////////////////////////////////////////////////////////////////////

EmCalHit::EmCalHit()
{
  miceMemory.addNew( Memory::EmCalHit );
  m_digitVector.clear();
}

//////////////////////////////////////////////////////////////////////////

EmCalHit::~EmCalHit()
{
  miceMemory.addDelete( Memory::EmCalHit );
}

void EmCalHit::AssignDigit(EmCalDigit* aDigit){
  // if the digit already is in the vector, do nothing
  bool duplicateDigit = false;
  for (unsigned int j = 0; j<m_digitVector.size(); j++){
    if (m_digitVector[j]==aDigit){
      duplicateDigit = true;
    }
  }
  if (duplicateDigit==false) m_digitVector.push_back(aDigit);
}

void EmCalHit::UnassignDigit(EmCalDigit* aDigit){
  for (unsigned int j = 0; j<m_digitVector.size(); j++){
    if (m_digitVector[j]==aDigit){
      m_digitVector.erase(m_digitVector.begin()+j);
      break;
    }
  }
}

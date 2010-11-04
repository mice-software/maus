// ZustandVektorTextFileIo.cc
//
// R.Sandstrom 2006-09-06

#include <sstream>

#include "ZustandVektorTextFileIo.hh"
#include "Interface/ZustandVektor.hh"

ZustandVektorTextFileIo::ZustandVektorTextFileIo( ZustandVektor* vector, int index ) : TextFileIoBase()
{
  miceMemory.addNew( Memory::ZustandVektorTextFileIo );
	
  setTxtIoIndex( index );
  m_vector = vector;
}

ZustandVektorTextFileIo::ZustandVektorTextFileIo( std::string def ) : TextFileIoBase()
{
  miceMemory.addNew( Memory::ZustandVektorTextFileIo );
  
  restoreObject( def );
}

std::string ZustandVektorTextFileIo::storeObject()
{
  HepSymMatrix matrix = m_vector->GetFullMatrix();
  HepLorentzVector xLrz = m_vector->GetXLorentz();
  HepLorentzVector pLrz = m_vector->GetPLorentz();

  std::stringstream ss;
  
  ss << txtIoIndex() << " " << m_vector->GetTruth() << " " << m_vector->GetPID()
     << " " << xLrz.x() << " " << xLrz.y() << " " << xLrz.z() << " " << xLrz.t()
     << " " << pLrz.px() << " " << pLrz.py() << " " << pLrz.pz() << " " << pLrz.e();

  // now the sym 8x8 matrix
  for (int row = 0; row < 8; ++row) 
  {
    for (int col = 0; col < 8; ++col)  
    {
      ss << " " << m_vector->GetMatrixElement(row,col);
    }
  }

  return( ss.str() );
}

void    ZustandVektorTextFileIo::restoreObject( std::string def )
{
  int myindex, pid;
  bool truth;
  double x,y,z,t,px,py,pz,E;

  m_vector = new ZustandVektor(); // normally this goes in make method, but this is more compact

  std::istringstream ist( def.c_str() );
  ist >> myindex >> truth >> pid 
      >> x >> y >> z >> t
      >> px >> py >> pz >> E; 

  m_vector->SetTruth(truth);
  m_vector->SetPID(pid);
  m_vector->SetXLorentz(x,y,z,t);
  m_vector->SetPLorentz(px,py,pz,E);
  // now the sym 8x8 matrix
  double element;

  for (int row = 0; row < 8; ++row)
  {
    for (int col = 0; col < 8; ++col) 
    {
      ist >> element;
      m_vector->SetMatrixElement(row,col,element);
    }
  }
  setTxtIoIndex( myindex );
}

ZustandVektor*  ZustandVektorTextFileIo::makeZustandVektor()
{
  return m_vector;
}

void    ZustandVektorTextFileIo::completeRestoration()
{
}

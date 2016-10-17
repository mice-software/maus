// MAUS WARNING: THIS IS LEGACY CODE.
using namespace std;

#include "BTFieldGroup.hh"
#include "Interface/Interpolator.hh"
#include "Interface/Mesh.hh"


////////////////////////// BTFieldGroup Start ////////////////////////

std::vector<int> BTFieldGroup::_gridDefault = std::vector<int>(3,10);

//constructor
BTFieldGroup::BTFieldGroup() :  _closed(true)
{
	_gridSize      = _gridDefault;
	_mesh          = new ThreeDGrid(1., 1., 1., 0., 0., 0., 2, 2, 2);
	int nElements  = _mesh->End().ToInteger()- _mesh->Begin().ToInteger();
	_fieldsToMesh  = std::vector< std::vector<int> >(nElements, std::vector<int>());
}

void BTFieldGroup::Erase(BTField* field, bool willClose)
{
  std::vector<BTField*>::iterator it=find(_fields.begin(), _fields.end(), field);
  int i = it - _fields.begin();
  _isRotated.erase(_isRotated.begin() + i);
  _rotations.erase(_rotations.begin() + i);
  _inverseRotations.erase(_inverseRotations.begin() + i);
  _translations.erase(_translations.begin() + i);
  _scaleFactors.erase(_scaleFactors.begin() + i);
  delete *it;
  _fields.erase(it);
  _closed = false;
  if(willClose) Close();
}

BTFieldGroup::BTFieldGroup(const BTFieldGroup& rhs)
{
	*this = rhs; //is this ok?
	_mesh = (ThreeDGrid*)rhs._mesh->Clone();
}

// Destructor
BTFieldGroup::~BTFieldGroup()
{
	for(unsigned int i=0; i<_fields.size(); i++)
		delete _fields[i];
	delete _mesh;
}

BTFieldGroup& BTFieldGroup::operator=(const BTFieldGroup& rhs )
{
	if (this == &rhs) return *this;
	_translations          = rhs._translations;
	_isRotated             = rhs._isRotated;
	_rotations             = rhs._rotations;
	_inverseRotations      = rhs._inverseRotations;
	_scaleFactors          = rhs._scaleFactors;
	for(unsigned int i=0; i<rhs._fields.size(); i++)
	{
		_fields.push_back(rhs._fields[i]->Clone());
		_fields[i]->SetParentField(this);
	}
	return *this;
}

void BTFieldGroup::AddField(BTField * newField, const Hep3Vector position, double scaleFactor, bool willClose)
{
	_closed = false;
	newField->SetParentField(this);
	_fields.push_back(newField);
	_translations.push_back(position);
	_isRotated.push_back(false);
	_rotations.push_back(HepRotation());
	_inverseRotations.push_back(HepRotation());
	_scaleFactors.push_back(scaleFactor);
	if(willClose) Close();
}

void BTFieldGroup::AddField(BTField * newField, const Hep3Vector position,
                            const HepRotation rotation, double scaleFactor, bool willClose)
{
	_closed = false;
	newField->SetParentField(this);
	_fields.push_back(newField);
	_translations.push_back(position);
	_isRotated.push_back(!rotation.isIdentity());
	_rotations.push_back(rotation.inverse());
	_inverseRotations.push_back(rotation);
	_scaleFactors.push_back(scaleFactor);
	if(willClose) Close();
}

void BTFieldGroup::Close()
{
  if(_closed) return;
  double tol = 1e-9; //double point tolerance
  //bb abbreviates bounding box, bb is a list of vertices of each field's bounding box
  std::vector< std::vector<CLHEP::Hep3Vector> > bb(_fields.size(), std::vector<CLHEP::Hep3Vector>(8));
  CLHEP::Hep3Vector min, max;
  for(unsigned int i=0; i<bb.size(); i++) 
  {
    bb[i] = GetBBVertices(i);
    if(i==0) min = max = bb[0][0];
    for(unsigned int j=0; j<bb[i].size(); j++)
      for(int k=0; k<3; k++)
      {
        if(min[k] > bb[i][j][k]) min[k] = bb[i][j][k]-tol;
        if(max[k] < bb[i][j][k]) max[k] = bb[i][j][k]+tol;
      }
  }

  for(int i=0; i<3; i++) if( (min[i] - max[i]) < tol) {min[i] -= tol*_gridSize[i]; max[i] += tol*_gridSize[i];}
  if(_mesh != NULL) delete _mesh;
  double spacing[3] = {(max[0] - min[0])/double(_gridSize[0]-1),
                       (max[1] - min[1])/double(_gridSize[1]-1),
                       (max[2] - min[2])/double(_gridSize[2]-1)};
  _mesh = new ThreeDGrid(spacing[0], spacing[1], spacing[2], min[0], min[1], min[2], _gridSize[0], _gridSize[1], _gridSize[2]);

  _fieldsToMesh = std::vector< std::vector<int> >( (_mesh->End()-1).ToInteger()+1, std::vector<int>());
  for(unsigned int i=0; i<bb.size(); i++)
  {
    CLHEP::Hep3Vector min=bb[i][0], max=bb[i][0]; //find min/max point in bb[i]
    for(unsigned int j=0; j<bb[i].size(); j++)
      for(int k=0; k<3; k++)
      {
        if(min[k] > bb[i][j][k]) min[k] = bb[i][j][k]-tol;
        if(max[k] < bb[i][j][k]) max[k] = bb[i][j][k]+tol;
      }
    Mesh::Iterator it0(std::vector<int>(3), _mesh), it1(std::vector<int>(3), _mesh), it(std::vector<int>(3), _mesh), end(_mesh->End()-1);
    _mesh->LowerBound(min[0], min[1], min[2], it0);
    _mesh->LowerBound(max[0], max[1], max[2], it1); //remember this is the lower side of upper - loop has to go to upper+1
    for(it[0]=it0[0]; it[0]<=it1[0]+1 && it[0]<=end[0]; it[0]++)
    for(it[1]=it0[1]; it[1]<=it1[1]+1 && it[1]<=end[1]; it[1]++)
    for(it[2]=it0[2]; it[2]<=it1[2]+1 && it[2]<=end[2]; it[2]++)
    {
      int pos = it.ToInteger();
      _fieldsToMesh[pos].push_back(i); //i refers to _fields
    }
  }
  BTField::bbMin = (_mesh->Begin()  ).Position(); //3-vector that is lower corner of bounding box
  BTField::bbMax = (_mesh->End() - 1).Position(); //3-vector that is upper corner of bounding box
  if(BTField::parentField != NULL) ((BTFieldGroup*)BTField::parentField)->Close();
  _closed = true;

//  for(unsigned int i=0; i<_fieldsToMesh.size(); i++) std::cout << _fieldsToMesh[i].size() << " ";
}

std::vector<CLHEP::Hep3Vector> BTFieldGroup::GetBBVertices(int index)
{
  std::vector<CLHEP::Hep3Vector> bb(8); //bounding box vertices in coordinates of field group
  const double *loc_bb[2]; //bounding box in coordinates of _fields[index]
  loc_bb[0] = _fields[index]->BoundingBoxMin();
  loc_bb[1] = _fields[index]->BoundingBoxMax();
//  std::cout << loc_bb[0][0] << " " << loc_bb[0][1] << " " << loc_bb[0][2] << " ** " << loc_bb[0][0] << " " << loc_bb[1][1] << " " << loc_bb[1][2] << std::endl;
  int perms[24] = {1,1,1, 1,1,0, 1,0,1, 0,1,1, 0,0,1, 0,1,0, 1,0,0, 0,0,0}; //permutations (or combinations or whatever)
  for(int j=0; j<8; j++) 
  {
    int p0=perms[j*3], p1=perms[j*3+1], p2=perms[j*3+2];
    bb[j] = CLHEP::Hep3Vector( loc_bb[p0][0], loc_bb[p1][1], loc_bb[p2][2]);
    bb[j] = _rotations[index]*bb[j] + _translations[index];
  }
  return bb;
}

void BTFieldGroup::GetFieldValue( const double Point[4], double *EMfield ) const
{
	if(!_closed) throw(MAUS::Exceptions::Exception(MAUS::Exceptions::recoverable, "Attempt to GetFieldValue when mesh not closed (internal error)", "BTFieldGroup::GetFieldValue"));
	double * LocalPoint;
	double   LocalEMField[6];
	double   scaleFactor=1;
	//optimisation - if I have loads of non-overlapping fields, I limit the number of fields I check to only those with Bounding Boxes overlapping Point
	std::vector<int> fields  = GetFields(Point); 
	unsigned int     nFields = fields.size();
//	std::cout << nFields << " ";
	EMfield[0]=0;
	EMfield[1]=0;
	EMfield[2]=0;
	EMfield[3]=0;
	EMfield[4]=0;
	EMfield[5]=0;
	for (unsigned int field_index=0; field_index<nFields; field_index++)
	{
		int i = fields[field_index];
                LocalEMField[0]=0;
                LocalEMField[1]=0;
                LocalEMField[2]=0;
                LocalEMField[3]=0;
                LocalEMField[4]=0;
                LocalEMField[5]=0;
		//Watch point - the local point may go awry here...
		LocalPoint = GetLocalCoordinates(Point, i);
		_fields[i]->GetFieldValue(LocalPoint, LocalEMField);

		if(_isRotated[i])
		{
			Hep3Vector ElectricField(LocalEMField[3],LocalEMField[4],LocalEMField[5]);
			Hep3Vector MagneticField(LocalEMField[0],LocalEMField[1],LocalEMField[2]);
			ElectricField   = _rotations[i]*ElectricField;
			MagneticField   = _rotations[i]*MagneticField;
			LocalEMField[0] = MagneticField[0];
			LocalEMField[1] = MagneticField[1];
			LocalEMField[2] = MagneticField[2];
			LocalEMField[3] = ElectricField[0];
			LocalEMField[4] = ElectricField[1];
			LocalEMField[5] = ElectricField[2];
		}
		scaleFactor = _scaleFactors[i];

                EMfield[0] += scaleFactor*LocalEMField[0]; // add fields
                EMfield[1] += scaleFactor*LocalEMField[1];
                EMfield[2] += scaleFactor*LocalEMField[2];
                EMfield[3] += scaleFactor*LocalEMField[3];
                EMfield[4] += scaleFactor*LocalEMField[4];
                EMfield[5] += scaleFactor*LocalEMField[5];
		delete [] LocalPoint;
	}


}

double *  BTFieldGroup::GetLocalCoordinates(const double Point[4], unsigned int fieldNumber) const
{
	double * LocalPoint = new double[4];
	LocalPoint[0] = Point[0] - _translations[fieldNumber][0];
	LocalPoint[1] = Point[1] - _translations[fieldNumber][1];
	LocalPoint[2] = Point[2] - _translations[fieldNumber][2];
 
	if (_isRotated[fieldNumber])
	{
		Hep3Vector PositionInLocalCoords = Hep3Vector(LocalPoint[0], LocalPoint[1], LocalPoint[2]);
		PositionInLocalCoords = _inverseRotations[fieldNumber] * PositionInLocalCoords;
		LocalPoint[0] = PositionInLocalCoords[0];
		LocalPoint[1] = PositionInLocalCoords[1];
		LocalPoint[2] = PositionInLocalCoords[2];
	}
	//Phasing is handled by the BTField as the field group doesn't want to know
	//whether it is dealing with a PillBox, another field group or something else
	LocalPoint[3] = Point[3];

	return LocalPoint;
}

RFData BTFieldGroup::SetThePhase(Hep3Vector Position, double time, double energy)
{
	double Point[4] = {Position[0], Position[1], Position[2], time};
	double * LocalPoint;
	RFData rfDataOutput;
	for(unsigned int i=0; i<_fields.size(); i++)
	{
		LocalPoint   = GetLocalCoordinates(Point, i);

		RFData rfData = _fields[i]->SetThePhase(Hep3Vector(LocalPoint[0], LocalPoint[1], 
		                                                 LocalPoint[2]), time, energy);
		if(rfData.GetFrequency() > 0) rfDataOutput = rfData;
		delete [] LocalPoint;
	}
	return rfDataOutput;
}

bool BTFieldGroup::IsPhaseSet() const
{
	for(unsigned int i=0; i<_fields.size(); i++)
		if(!_fields[i]->IsPhaseSet()) return false;
	return true;
}


Hep3Vector BTFieldGroup::GetLocalPosition(const BTField * field) const
{
	for(unsigned int i=0; i<_fields.size(); i++)
		if(field == _fields[i])
			return Hep3Vector(_translations[i][0], _translations[i][1], _translations[i][2]);

	return Hep3Vector(0,0,0);//should be an exception here
}

HepRotation BTFieldGroup::GetLocalRotation(const BTField * field) const
{
	for(unsigned int i=0; i<_fields.size(); i++)
		if(field == _fields[i])
			return _rotations[i];

	return HepRotation(); //unit matrix - but should be an exception here
}


void BTFieldGroup::Print(std::ostream & out) const
{
	for(unsigned int fieldIndex=0; fieldIndex < _fields.size(); fieldIndex++)
		_fields[fieldIndex]->Print(out);

}

double BTFieldGroup::GetScaleFactor(const BTField * aField) const
{
	double scaleFactor = ( (parentField==NULL) ? 1. : parentField->GetScaleFactor(this) );
	for(unsigned int i=0; i<_fields.size(); i++)
		if(_fields[i] == aField) return scaleFactor *= _scaleFactors[i];
	return scaleFactor; //Should throw an exception here
}

CLHEP::HepLorentzVector BTFieldGroup::GetVectorPotential(CLHEP::HepLorentzVector position) const
{
	CLHEP::HepLorentzVector A(0,0,0,0);
	for(unsigned int i=0; i<_fields.size(); i++)
	{
		CLHEP::HepLorentzVector localPosition = GetLocalCoordinates(position, i);
		CLHEP::HepLorentzVector localA = _fields[i]->GetVectorPotential(localPosition)*_scaleFactors[i];
		if(_isRotated[i])
			localA = _rotations[i]*localA;
		A += localA;
	}
	return A;
}

CLHEP::HepLorentzVector  BTFieldGroup::GetLocalCoordinates(CLHEP::HepLorentzVector position,
                                                           unsigned int fieldNumber) const
{
	double point[4] = {position[0], position[1], position[2], position[3]};
	double *localPoint = GetLocalCoordinates(point, fieldNumber);
	CLHEP::HepLorentzVector outPoint(localPoint[0], localPoint[1], localPoint[2], localPoint[3]);
	delete [] localPoint;
	return outPoint;
}

bool BTFieldGroup::DoesFieldChangeEnergy() const 
{
	for(int i=0; i<int(_fields.size()); i++) 
	if(_fields[i]->DoesFieldChangeEnergy()) return true; 
	return false;
}


////////////////////// BTFieldGroup End ///////////////////////////


////////////////////// BTFieldAmalgamation Start /////////////////////////
std::vector<BTFieldAmalgamation*> BTFieldAmalgamation::amalgamations = std::vector<BTFieldAmalgamation*>();

BTFieldAmalgamation::BTFieldAmalgamation(double r_max_, double length_, double z_step_, double r_step_, std::string interpolation_, field_type type_) 
                   : fieldMap(NULL), amalgamated(false), r_max(r_max_), z_length(length_), r_step(r_step_), z_step(z_step_), interpolation(interpolation_), type(type_)
{
  amalgamations.push_back(this);
}

BTFieldAmalgamation::~BTFieldAmalgamation()
{ if(fieldMap != NULL) delete fieldMap; }

void BTFieldAmalgamation::GetFieldValue( const double Point[4], double *EMfield ) const
{
  if(!amalgamated) BTFieldGroup::GetFieldValue(Point, EMfield);
  else             fieldMap->GetFieldValue(Point, EMfield);
}

void BTFieldAmalgamation::AddField(BTField * newField, const Hep3Vector position, double scaleFactor)
{
  if(fieldMap != NULL) delete fieldMap;
  BTFieldGroup::AddField(newField, position, scaleFactor);
  amalgamated = false;
}

void BTFieldAmalgamation::Print(std::ostream& out) const
{
  std::string field = "Inactive amalgamation with";
  if(amalgamated) field = "Amalgamation with";
  out << field;
  BTField::Print(out);
  out << "{\n";
  for(unsigned int i=0; i<BTFieldGroup::GetFields().size(); i++)
  {
    out << "    ";
    BTFieldGroup::GetFields()[i]->Print(out);
  }
  out << "}\n";
}

void BTFieldAmalgamation::AmalgamateAll()
{
  for(unsigned int i=0; i<amalgamations.size(); i++)
    if(!amalgamations[i]->IsAmalgamated()) amalgamations[i]->AmalgamateThis();
}

void BTFieldAmalgamation::AmalgamateThis()
{
  int numberOfXCoords = static_cast<int>(r_max/r_step)+1;
  int numberOfYCoords = static_cast<int>(z_length/z_step)+1;
  TwoDGrid* myGrid = new TwoDGrid(r_step, z_step, 0., -z_length/2., numberOfXCoords, numberOfYCoords);
  double**  Br = new double*[numberOfXCoords];
  double**  Bz = new double*[numberOfXCoords];
  for(int i=0; i<numberOfXCoords; i++)
  {
    Br[i] = new double[numberOfYCoords];
    Bz[i] = new double[numberOfYCoords];
    for(int j=0; j<numberOfYCoords; j++)
    {
      double point[4] = {myGrid->x(i+1), 0., myGrid->y(j+1), 0.};
      double field[6] = {0.,0.,0.,0.,0.,0.};
      GetFieldValue(point, field);
      Br[i][j] = field[0];
      Bz[i][j] = field[2];
    }
  }

  Interpolator3dSolenoidalTo3d * myInt = new Interpolator3dSolenoidalTo3d(myGrid, Br, Bz, interpolation);
  fieldMap    = new MagFieldMap(myInt);
  amalgamated = true;

}


////////////////////// BTFieldAmalgamation End ///////////////////////////



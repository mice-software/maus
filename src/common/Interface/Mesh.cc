#include "Interface/Mesh.hh"
#include "Interface/Squeal.hh"
#include <iomanip>
#include <math.h>
///////// Mesh ////////
Mesh::Mesh() 
{}

Mesh::~Mesh() 
{}

/////// End of Mesh //////

/////// TwoDGrid ///////
TwoDGrid::TwoDGrid() : _x(2,0), _y(2,0), _xSize(2), _ySize(2), _maps(), _constantSpacing(false)
{
  _x[1] = 1.; _y[1] = 1.;
}

TwoDGrid::TwoDGrid(double dX, double dY, double minX, double minY, int numberOfXCoords, int numberOfYCoords)
        :  _x(), _y(), _xSize(numberOfXCoords), _ySize(numberOfYCoords), _maps(), _constantSpacing(true)
{
    if(numberOfXCoords < 2 || numberOfYCoords < 2) throw(Squeal(Squeal::recoverable, "2D Grid must be at least 2x2 grid", "TwoDGrid::TwoDGrid(...)"));
    for(int i=0; i<numberOfXCoords; i++) _x.push_back(minX+i*dX);
    for(int j=0; j<numberOfYCoords; j++) _y.push_back(minY+j*dY);
    SetConstantSpacing();
}

TwoDGrid::TwoDGrid(int xSize, const double *x, int ySize, const double *y)  : _x(x, x+xSize), _y(y, y+ySize), _xSize(xSize), _ySize(ySize), _maps(), _constantSpacing(false) 
{
  if(xSize < 2 || ySize < 2) throw(Squeal(Squeal::recoverable, "2D Grid must be at least 2x2 grid", "TwoDGrid::TwoDGrid(...)"));
  SetConstantSpacing();
}

TwoDGrid::TwoDGrid(std::vector<double> x, std::vector<double> y)  : _x (x), _y(y), _xSize(x.size()), _ySize(y.size()), _maps(), _constantSpacing(false)
{
  if(_xSize < 2 || _ySize < 2) throw(Squeal(Squeal::recoverable, "2D Grid must be at least 2x2 grid", "TwoDGrid::TwoDGrid(...)"));
  SetConstantSpacing();
}

TwoDGrid::~TwoDGrid() 
{
}

Mesh::Iterator TwoDGrid::Begin() const 
{
    return Mesh::Iterator(std::vector<int>(2,1), this);
}

Mesh::Iterator TwoDGrid::End()   const 
{
    std::vector<int> end(2, 1); end[0] = _xSize+1; return Mesh::Iterator(end, this);
}

void TwoDGrid::Position(const Mesh::Iterator& it, double * position) const
{
    position[0] = x(it._state[0]);
    position[1] = y(it._state[1]);
}

void TwoDGrid::CentrePosition(const Mesh::Iterator& it, double * position) const
{
    if( it._state[0]>=xSize() ) position[0] = MaxX() + ( x(xSize() ) - x(xSize()-1) )*(it._state[0]-xSize()+0.5) ;
    else if( it._state[0]<0   ) position[0] = MinX() + (x(2)-x(1))*(it._state[0]-0.5);
    else                        position[0] = 0.5*(x(it._state[0]+1)+x(it._state[0]));

    if( it._state[1]>=ySize() ) position[1] = MaxY() + ( y(ySize() ) - y(ySize()-1) )*(it._state[1]-ySize()+0.5) ;
    else if( it._state[1]<0   ) position[1] = MinY() + (y(2)-y(1))*(it._state[1]-0.5);
    else                        position[1] = 0.5*(y(it._state[1]+1)+y(it._state[1]));
}

Mesh::Iterator& TwoDGrid::AddEquals(Mesh::Iterator& lhs, int difference) const
{
    if(difference < 0) return SubEquals(lhs, -difference);
    lhs._state[0] += difference/(_ySize);
    lhs._state[1] += difference%(_ySize);
    if(lhs._state[1] > _ySize) {lhs._state[1] -= _ySize; lhs._state[0]++;}
    return lhs;
}

Mesh::Iterator& TwoDGrid::SubEquals(Mesh::Iterator& lhs, int difference) const
{
    if(difference < 0) return AddEquals(lhs, -difference);
    lhs._state[0] -= difference/(_ySize);
    lhs._state[1] -= difference%(_ySize);
    if(lhs._state[1] < 1) {lhs._state[1] += _ySize; lhs._state[0]--;}
    return lhs;
}

Mesh::Iterator& TwoDGrid::AddEquals(Mesh::Iterator& lhs, const Mesh::Iterator& rhs) const
{
    return AddEquals(lhs, rhs.ToInteger());
}

Mesh::Iterator& TwoDGrid::SubEquals(Mesh::Iterator& lhs, const Mesh::Iterator& rhs) const
{
    return SubEquals(lhs, rhs.ToInteger());
}

Mesh::Iterator& TwoDGrid::AddOne   (Mesh::Iterator& lhs) const
{
    if(lhs._state[1] == this->ySize()) {lhs._state[1] = 1; ++lhs._state[0];}
    else ++lhs._state[1];
    return lhs;
}

Mesh::Iterator& TwoDGrid::SubOne   (Mesh::Iterator& lhs) const
{
    if(lhs._state[1] == 1) {lhs._state[1] = this->ySize(); --lhs._state[0];}
    else --lhs._state[1];
    return lhs;
}
//Check relative position
bool TwoDGrid::IsGreater(const Mesh::Iterator& lhs, const Mesh::Iterator& rhs) const
{
    if(lhs._state[0] > rhs._state[0]) return true;
    else if (lhs._state[0] == rhs._state[0] && lhs._state[1] > rhs._state[1]) return true;
    return false;
}

void TwoDGrid::Remove(VectorMap* map) //remove *map if it exists; delete this if there are no more VectorMaps
{
    std::vector<VectorMap*>::iterator it = std::find(_maps.begin(), _maps.end(), map); 
    if(it<_maps.end()) { _maps.erase(it);  }
    if(_maps.begin() == _maps.end()) { delete this; }
} 

void TwoDGrid::Add(VectorMap* map) //add *map if it has not already been added
{
    std::vector<VectorMap*>::iterator it = std::find(_maps.begin(), _maps.end(), map); 
    if(it==_maps.end()) { _maps.push_back(map);    }
} 

void TwoDGrid::SetConstantSpacing()
{
    _constantSpacing = true;
    for(unsigned int i=0; i<_x.size()-1; i++) if( fabs(1-(_x[i+1]-_x[i])/(_x[1]-_x[0])) > 1e-9 ) _constantSpacing = false;
    for(unsigned int i=0; i<_y.size()-1; i++) if( fabs(1-(_y[i+1]-_y[i])/(_y[1]-_y[0])) > 1e-9 ) _constantSpacing = false;
}

Mesh::Iterator TwoDGrid::Nearest(const double* position) const
{
    std::vector<int> index(2);
    LowerBound(position[0], index[0], position[1], index[1]);
    if(index[0] < _xSize-1 && index[0] >= 0)
        index[0] += (2*(position[0] - _x[index[0]])  > _x[index[0]+1]-_x[index[0]] ? 2 : 1);
    else 
        index[0] += 1;
    if(index[1] < _ySize-1 && index[1] >= 0) 
        index[1] += (2*(position[1] - _y[index[1]])  > _y[index[1]+1]-_y[index[1]] ? 2 : 1);
    else 
        index[1] += 1;
    if(index[0] < 1) index[0] = 1; 
    if(index[1] < 1) index[1] = 1; 
    if(index[0] > _xSize) index[0] = _xSize; 
    if(index[1] > _ySize) index[1] = _ySize; 
   return Mesh::Iterator(index, this);
}


///// End of TwoDGrid /////

///// Mesh::Iterator /////

Mesh::Iterator::Iterator() {;}
Mesh::Iterator::Iterator(const Mesh::Iterator& in) : _mesh(in._mesh), _state(in._state)   {;}
Mesh::Iterator::Iterator(std::vector<int> state, const Mesh* mesh) : _mesh(mesh), _state(state) {;} 
Mesh::Iterator::~Iterator() {;}

const Mesh::Iterator&  Mesh::Iterator::operator= (const Mesh::Iterator& rhs)
{
    _mesh  = rhs._mesh;
    _state = rhs._state;
    return *this;
}

void Mesh::Iterator::Position(double* point) const
{
    _mesh->Position(*this, point);
}

std::vector<double> Mesh::Iterator::Position()              const
{
    std::vector<double> PointV(_mesh->PositionDimension());
    _mesh->Position(*this, &PointV[0]);
    return PointV;
}

Mesh::Iterator& operator++(Mesh::Iterator& lhs)      {return lhs._mesh->AddOne(lhs);}
Mesh::Iterator& operator--(Mesh::Iterator& lhs)      {return lhs._mesh->SubOne(lhs);}
Mesh::Iterator  operator++(Mesh::Iterator& lhs, int) {Mesh::Iterator copy = lhs; lhs._mesh->AddOne(lhs); return copy;}
Mesh::Iterator  operator--(Mesh::Iterator& lhs, int) {Mesh::Iterator copy = lhs; lhs._mesh->SubOne(lhs); return copy;}

Mesh::Iterator  operator- (const Mesh::Iterator& lhs, const Mesh::Iterator& rhs) 
{
    Mesh::Iterator lhsCopy(lhs); 
    Mesh::Iterator lhsNew(lhs._mesh->SubEquals(lhsCopy, rhs)); 
    return lhsNew;
}

Mesh::Iterator  operator- (const Mesh::Iterator& lhs, const int& difference) 
{
    Mesh::Iterator lhsCopy(lhs); 
    Mesh::Iterator lhsNew(lhs._mesh->SubEquals(lhsCopy, difference)); 
    return lhsNew;
}

Mesh::Iterator  operator+ (const Mesh::Iterator& lhs, const Mesh::Iterator& rhs) 
{
    Mesh::Iterator lhsCopy(lhs); 
    Mesh::Iterator lhsNew(lhs._mesh->AddEquals(lhsCopy, rhs)); 
    return lhsNew;
}

Mesh::Iterator  operator+ (const Mesh::Iterator& lhs, const int& difference) 
{
    Mesh::Iterator lhsCopy(lhs); 
    Mesh::Iterator lhsNew(lhs._mesh->AddEquals(lhsCopy, difference)); 
    return lhsNew;
}


Mesh::Iterator& operator+= (Mesh::Iterator& lhs, const Mesh::Iterator& rhs) { return lhs._mesh->AddEquals(lhs, rhs); }
Mesh::Iterator& operator-= (Mesh::Iterator& lhs, const Mesh::Iterator& rhs) { return lhs._mesh->SubEquals(lhs, rhs); }
Mesh::Iterator& operator+= (Mesh::Iterator& lhs, const int& rhs) { return lhs._mesh->AddEquals(lhs, rhs); }
Mesh::Iterator& operator-= (Mesh::Iterator& lhs, const int& rhs) { return lhs._mesh->SubEquals(lhs, rhs); }


bool operator>=(const Mesh::Iterator& lhs, const Mesh::Iterator& rhs)
{
    if(lhs._mesh->IsGreater(lhs, rhs) || lhs==rhs) return true;
    return false;
}

bool operator<=(const Mesh::Iterator& lhs, const Mesh::Iterator& rhs)
{
    if(lhs._mesh->IsGreater(lhs, rhs)) return false;
    return true;
}

bool operator>(const Mesh::Iterator& lhs, const Mesh::Iterator& rhs)
{
    return !(lhs<=rhs);
}

bool operator<(const Mesh::Iterator& lhs, const Mesh::Iterator& rhs)
{
    return !(lhs>=rhs);
}

bool operator==(const Mesh::Iterator& lhs, const Mesh::Iterator& rhs) 
{
    if(lhs._state==rhs._state) 
        return true; 
    return false;
}

bool operator!=(const Mesh::Iterator& lhs, const Mesh::Iterator& rhs)  
{
    if(lhs._mesh==rhs._mesh && lhs._state==rhs._state) 
        return false; 
    return true;
}

std::ostream& operator<<(std::ostream& out, const Mesh::Iterator& it)
{
    out << std::setw(5) << it.ToInteger() << " ** ";
    for(unsigned int i=0; i<it.State().size(); i++)    out << std::setw(5) << it[i] << " ";
    out << "** ";
    for(unsigned int i=0; i<it.Position().size(); i++) out << std::scientific << std::setprecision(3) << std::setw(12) << it.Position()[i] << " ";
    return out;
}


////// Mesh::Iterator end /////

////// ThreeDGrid //////

ThreeDGrid::ThreeDGrid() : _x(2, 0), _y(2, 0), _z(2, 0), _xSize(0), _ySize(0), _zSize(0), _maps(0), _constantSpacing(false) 
{
  SetConstantSpacing();
  _x[1] = _y[1] = _z[1] = 1.;
}

ThreeDGrid::ThreeDGrid(int xSize, const double *x, int ySize, const double *y, int zSize, const double *z) 
             : _x (x, x+xSize), _y(y, y+ySize), _z(z, z+zSize), _xSize(xSize), _ySize(ySize), _zSize(zSize), _maps(), _constantSpacing(false) 
{
  SetConstantSpacing();
  if(_xSize < 2 || _ySize < 2 || _zSize <2) throw(Squeal(Squeal::recoverable, "3D Grid must be at least 2x2x2 grid", "ThreeDGrid::ThreeDGrid(...)"));
}

ThreeDGrid::ThreeDGrid(std::vector<double> x, std::vector<double> y, std::vector<double> z)
             : _x (x), _y(y), _z(z), _xSize(x.size()), _ySize(y.size()), _zSize(z.size()), _maps(), _constantSpacing(false) 
{
  SetConstantSpacing();
  if(_xSize < 2 || _ySize < 2 || _zSize <2) throw(Squeal(Squeal::recoverable, "3D Grid must be at least 2x2x2 grid", "ThreeDGrid::ThreeDGrid(...)"));
}

ThreeDGrid::ThreeDGrid(double dX, double dY, double dZ, double minX, double minY, double minZ, 
                       int numberOfXCoords, int numberOfYCoords, int numberOfZCoords)
        :  _x(numberOfXCoords), _y(numberOfYCoords), _z(numberOfZCoords), _xSize(numberOfXCoords), _ySize(numberOfYCoords), _zSize(numberOfZCoords), _maps(), _constantSpacing(true)
{
    for(int i=0; i<numberOfXCoords; i++) _x[i] = minX+i*dX;
    for(int j=0; j<numberOfYCoords; j++) _y[j] = minY+j*dY;
    for(int k=0; k<numberOfZCoords; k++) _z[k] = minZ+k*dZ;

    SetConstantSpacing();
}

ThreeDGrid::~ThreeDGrid() 
{
}

//state starts at 1,1,1
Mesh::Iterator ThreeDGrid::Begin() const 
{
    return Mesh::Iterator(std::vector<int>(3,1), this);
}

Mesh::Iterator ThreeDGrid::End()   const 
{
    std::vector<int> end(3, 1); end[0] = _xSize+1; return Mesh::Iterator(end, this);
}

void ThreeDGrid::Position(const Mesh::Iterator& it, double * position) const
{
    position[0] = x(it._state[0]);
    position[1] = y(it._state[1]);
    position[2] = z(it._state[2]);
}


Mesh::Iterator& ThreeDGrid::AddEquals(Mesh::Iterator& lhs, int difference) const
{
    if(difference < 0) return SubEquals(lhs, -difference);

    lhs._state[0] +=  difference/(_ySize*_zSize);
    difference     = difference%(_ySize*_zSize);
    lhs._state[1] += difference/(_zSize);
    lhs._state[2] += difference%(_zSize);

    if(lhs._state[1] > _ySize) {lhs._state[0]++; lhs._state[1] -= _ySize;}
    if(lhs._state[2] > _zSize) {lhs._state[1]++; lhs._state[2] -= _zSize;}

    return lhs;
}
/*
Mesh::Iterator& TwoDGrid::SubEquals(Mesh::Iterator& lhs, int difference) const
{
    if(difference < 0) return AddEquals(lhs, -difference);
    lhs._state[0] -= difference/(_ySize);
    lhs._state[1] -= difference%(_ySize);
    if(lhs._state[1] < 1) {lhs._state[1] += _ySize; lhs._state[0]--;}
    return lhs;
}
*/
Mesh::Iterator& ThreeDGrid::SubEquals(Mesh::Iterator& lhs, int difference) const
{
    if(difference < 0) return AddEquals(lhs, -difference);

    lhs._state[0] -= difference/(_ySize*_zSize);
    difference     = difference%(_ySize*_zSize);
    lhs._state[1] -= difference/(_zSize);
    lhs._state[2] -= difference%(_zSize);

    while(lhs._state[2] < 1) {lhs._state[1]--; lhs._state[2] += _zSize;}
    while(lhs._state[1] < 1) {lhs._state[0]--; lhs._state[1] += _ySize;}

    return lhs;
}


Mesh::Iterator& ThreeDGrid::AddEquals(Mesh::Iterator& lhs, const Mesh::Iterator& rhs) const
{
    return AddEquals(lhs, rhs.ToInteger());
}

Mesh::Iterator& ThreeDGrid::SubEquals(Mesh::Iterator& lhs, const Mesh::Iterator& rhs) const
{
    return SubEquals(lhs, rhs.ToInteger());
}

Mesh::Iterator& ThreeDGrid::AddOne   (Mesh::Iterator& lhs) const
{
    if(lhs._state[1] == _ySize && lhs._state[2] == _zSize) {++lhs._state[0]; lhs._state[1] = lhs._state[2] = 1;}
    else if(lhs._state[2] == _zSize) {++lhs._state[1]; lhs._state[2] = 1;}
    else ++lhs._state[2];
    return lhs;
}

Mesh::Iterator& ThreeDGrid::SubOne   (Mesh::Iterator& lhs) const
{
    if(lhs._state[1] == 1 && lhs._state[2] == 1) {--lhs._state[0]; lhs._state[1] = _ySize; lhs._state[2] = _zSize;}
    else if(lhs._state[2] == 1) {--lhs._state[1]; lhs._state[2] = _zSize;}
    else --lhs._state[2];
    return lhs;
}
//Check relative position
bool ThreeDGrid::IsGreater(const Mesh::Iterator& lhs, const Mesh::Iterator& rhs) const
{
    if(lhs._state[0] > rhs._state[0]) return true;
    else if (lhs._state[0] == rhs._state[0] && lhs._state[1] > rhs._state[1]) return true;
    else if (lhs._state[0] == rhs._state[0] && lhs._state[1] == rhs._state[1] && lhs._state[2] > rhs._state[2]) return true;
    return false;
}

void ThreeDGrid::Remove(VectorMap* map) //remove *map if it exists; delete this if there are no more VectorMaps
{
    std::vector<VectorMap*>::iterator it = std::find(_maps.begin(), _maps.end(), map); 
    if(it<_maps.end()) { _maps.erase(it);  }
    if(_maps.begin() == _maps.end()) { delete this; }
} 

void ThreeDGrid::Add(VectorMap* map) //add *map if it has not already been added
{
    std::vector<VectorMap*>::iterator it = std::find(_maps.begin(), _maps.end(), map); 
    if(it==_maps.end()) { _maps.push_back(map);    }
}

void ThreeDGrid::SetConstantSpacing()
{
    _constantSpacing = true;
    for(unsigned int i=0; i<_x.size()-1; i++) if( fabs(1-(_x[i+1]-_x[i])/(_x[1]-_x[0])) > 1e-9 ) {_constantSpacing = false; return;}
    for(unsigned int i=0; i<_y.size()-1; i++) if( fabs(1-(_y[i+1]-_y[i])/(_y[1]-_y[0])) > 1e-9 ) {_constantSpacing = false; return;}
    for(unsigned int i=0; i<_z.size()-1; i++) if( fabs(1-(_z[i+1]-_z[i])/(_z[1]-_z[0])) > 1e-9 ) {_constantSpacing = false; return;}
}

Mesh::Iterator ThreeDGrid::Nearest(const double* position) const
{
    std::vector<int> index(3);
    LowerBound(position[0], index[0], position[1], index[1], position[2], index[2]);
    if(index[0] < _xSize-1 && index[0]>=0) index[0] += (2*(position[0] - _x[index[0]])  > _x[index[0]+1]-_x[index[0]] ? 2 : 1);
    else index[0]++;
    if(index[1] < _ySize-1 && index[1]>=0) index[1] += (2*(position[1] - _y[index[1]])  > _y[index[1]+1]-_y[index[1]] ? 2 : 1);
    else index[1]++;
    if(index[2] < _zSize-1 && index[2]>=0) index[2] += (2*(position[2] - _z[index[2]])  > _z[index[2]+1]-_z[index[2]] ? 2 : 1);
    else index[2]++;
    if(index[0] < 1) index[0] = 1; 
    if(index[1] < 1) index[1] = 1; 
    if(index[2] < 1) index[2] = 1; 
    if(index[0] > _xSize) index[0] = _xSize; 
    if(index[1] > _ySize) index[1] = _ySize; 
    if(index[2] > _zSize) index[2] = _zSize; 
    return Mesh::Iterator(index, this);
}

/// ////// ThreeDGrid END //////

/// ////// NDGrid ///////
NDGrid::NDGrid() : _coord(), _maps(), _constantSpacing(false) 
{
  for(unsigned int i=0; i<2; i++) {_coord.push_back(std::vector<double>(2,0)); _coord.back()[1] = 1.;}
}



NDGrid::NDGrid(std::vector<int> size, std::vector<const double *> gridCoordinates) : _coord(), _maps(), _constantSpacing(false)
{ 
  for(unsigned int i=0; i<size.size(); i++) 
  {
    if(size[i] < 2) 
      throw(Squeal(Squeal::recoverable, "ND Grid must be at least 2x2x...x2 grid", "NDGrid::NDGrid(...)"));
    _coord.push_back(std::vector<double>(gridCoordinates[i], gridCoordinates[i] + size[i]) ); 
  }
  SetConstantSpacing();
}

NDGrid::NDGrid(int nDimensions, int* size, double* spacing, double* min)  : _coord(nDimensions), _maps(), _constantSpacing(true)
{
    for(int i=0; i<nDimensions; i++) 
    {
        if(size[i] < 2) throw(Squeal(Squeal::recoverable, "ND Grid must be at least 2x2x...x2 grid", "NDGrid::NDGrid(...)"));
        _coord[i] = std::vector<double>(size[i]);
        for(unsigned int j=0; j<_coord[i].size(); j++) _coord[i][j] = min[i] + j*spacing[i];
    }
}

NDGrid::NDGrid(std::vector< std::vector<double> > gridCoordinates) : _coord(gridCoordinates), _maps(), _constantSpacing(false) 
{
  for(unsigned int i=0; i<gridCoordinates.size(); i++) 
      if(gridCoordinates[i].size() < 2) throw(Squeal(Squeal::recoverable, "ND Grid must be at least 2x2x...x2 grid", "NDGrid::NDGrid(...)"));
  SetConstantSpacing();
}


double* NDGrid::newCoordArray  ( const int& dimension)  const
{
    double * array = new double[_coord[dimension].size() ];
    for(unsigned int i=0; i<_coord[dimension].size(); i++) array[i] = _coord[dimension][i];
    return array;
}

//Mesh::Iterator wraps around a std::vector<int>
//it[0] is least significant, it[max] is most signifcant
Mesh::Iterator& NDGrid::AddEquals(Mesh::Iterator& lhs, int difference) const
{
    if(difference < 0) return SubEquals(lhs, -difference);
    std::vector<int> index  (_coord.size(),0);
    std::vector<int> content(_coord.size(),1);
    for(int i=int(index.size()-2); i>=0; i--) content[i] = content[i+1]*_coord[i+1].size(); //content could be member variable
    for(int i=0; i<int(index.size()); i++)
    {
        index[i]    = difference/content[i];
        difference -= index[i] * content[i];
    }
    for(unsigned int i=0; i<index.size(); i++) lhs._state[i] += index[i];
    for(int i=int(index.size())-1; i>0; i--) 
    {
        if(lhs._state[i] > int(_coord[i].size()) ) 
        {
            lhs._state[i-1]++;
            lhs._state[i] -= _coord[i].size();
        }
    }

    return lhs;
}

Mesh::Iterator& NDGrid::SubEquals(Mesh::Iterator& lhs, int difference) const
{
    if(difference < 0) return AddEquals(lhs, -difference);
    std::vector<int> index  (_coord.size(),0);
    std::vector<int> content(_coord.size(),1);
    for(int i=int(index.size()-2); i>=0; i--) content[i] = content[i+1]*_coord[i+1].size(); //content could be member variable
    for(int i=0; i<int(index.size()); i++)
    {
        index[i]    = difference/content[i];
        difference -= index[i] * content[i];
    }
    for(unsigned int i=0; i<index.size();   i++) lhs._state[i] -= index[i];
    for(int i=int(index.size())-1; i>0; i--) 
    {
        if(lhs._state[i] < 1)
        {
            lhs._state[i-1]--;
            lhs._state[i] += _coord[i].size();
        }
    }
    return lhs;
}

Mesh::Iterator& NDGrid::AddEquals(Mesh::Iterator& lhs, const Mesh::Iterator& rhs) const
{
    return AddEquals(lhs, rhs.ToInteger());
}

Mesh::Iterator& NDGrid::SubEquals(Mesh::Iterator& lhs, const Mesh::Iterator& rhs) const
{
    return SubEquals(lhs, rhs.ToInteger());
}

Mesh::Iterator& NDGrid::AddOne   (Mesh::Iterator& lhs) const
{
    int i=_coord.size()-1;
    while(lhs[i] == int(_coord[i].size()) && i>0) {lhs[i]=1; i--;}
    lhs[i]++;
    return lhs;
}

Mesh::Iterator& NDGrid::SubOne   (Mesh::Iterator& lhs) const
{
    lhs[_coord.size()-1] -= 1;

    int i   = _coord.size()-1;
    while(lhs[i] == 0 && i>0) 
    {
        lhs._state[i] = _coord[i].size();
        i--;
        lhs._state[i]--;
    }
    return lhs;
}

void NDGrid::SetConstantSpacing()
{
    _constantSpacing = true;
    for(unsigned int i=0; i<_coord.size(); i++)
        for(unsigned int j=0; j<_coord[i].size()-1; j++) 
            if( fabs(1-(_coord[i][j+1]-_coord[i][j])/(_coord[i][1]-_coord[i][0])) > 1e-9 ) 
            {_constantSpacing = false; return;}
}

bool NDGrid::IsGreater(const Mesh::Iterator& lhs, const Mesh::Iterator& rhs) const
{
    unsigned int i = 0;
    while(lhs._state[i] == rhs._state[i] && i<rhs._state.size()-1) i++; //if all equal; rhs[i] = rhs.last
    return (lhs[i] > rhs[i]);
}

int NDGrid::ToInteger(const Mesh::Iterator& lhs) const
{
    int difference = 0;
    std::vector<int> index  (_coord.size(),0);
    std::vector<int> content(_coord.size(),1);
    for(int i=int(index.size()-2); i>=0; i--) content[i]  = content[i+1]*_coord[i+1].size(); //content could be member variable
    for(int i=0; i<int(index.size()); i++)         difference += (lhs._state[i]-1) * (content[i]);

    return difference;
}

Mesh::Iterator NDGrid::Nearest(const double* position) const
{
    std::vector<int>    index(_coord.size());
    std::vector<double> pos  (position, position+_coord.size());
    LowerBound      (pos, index);
    for(unsigned int i=0; i<_coord.size(); i++)
    {
        if(index[i] < int(_coord[i].size()-1) && index[i] >= 0) index[i] += (2*(position[i] - _coord[i][index[i]])  > _coord[i][index[i]+1]-_coord[i][index[i]] ? 2 : 1);
        else                                   index[i]++;
        if(index[i] < 1)                     index[i] = 1; 
        if(index[i] > int(_coord[i].size())) index[i] = _coord[i].size(); 
    }
    return Mesh::Iterator(index, this);
}

////// NDGrid END ////////

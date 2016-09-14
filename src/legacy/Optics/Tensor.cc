#include "Tensor.hh"
#include "Tensor3.hh"

/*
const Exception Tensor::_outOfRange = MAUS::Exceptions::Exception(MAUS::Exceptions::recoverable, "Tensor index out of range",       "Tensor::Tensor");
const Exception Tensor::_outOfRank  = MAUS::Exceptions::Exception(MAUS::Exceptions::recoverable, "Tensor rank out of range",        "Tensor::Tensor");
const Exception Tensor::_lowRank    = MAUS::Exceptions::Exception(MAUS::Exceptions::recoverable, "Do not use Tensor for rank <= 3", "Tensor::Tensor");
*/

Tensor::Tensor(std::vector<int> size, double val) : _data(NULL), _size(size)
{
	SetTensor(size, val);
}

Tensor::Tensor(int size[],  int rank, double val) : _data(NULL)
{
	std::vector<int> sizeV(size, size+rank);
	SetTensor(sizeV, val);
}

Tensor::Tensor(int size,    int rank, double val) : _data(NULL)
{
	std::vector<int> sizeV(rank, size);
	SetTensor(sizeV, val);
}

Tensor::~Tensor()
{
	for(int i=0; i<_size[0]; i++)
		if(_data[i]) delete _data[i];
	if(_data && _size[0]>0) delete [] _data;
}

void Tensor::SetTensor(std::vector<int> size, double val)
{
	_size = size;
	if(size.size() < 4) throw(MAUS::Exceptions::Exception(MAUS::Exceptions::recoverable, "Do not use Tensor for rank <= 3", "Tensor::Tensor"));
	else if(size.size() == 4)
	{
		_data = new Tensor*[size[0]];
		for(int i=0; i<size[0]; i++)
			_data[i] = new Tensor3(size[1], size[2], size[3], val);
	}
	else
	{
		_data = new Tensor*[size[0]];
		std::vector<int> newSize;
		for(unsigned int i=1; i<size.size(); i++)
			newSize.push_back(size[i]);
		for(int i=0; i<size[0]; i++)
			_data[i] = new Tensor(newSize, val);
	}

	SetAllFields(val);
	_size   = size;
}

void Tensor::SetAllFields(double value)
{
	for(int i=0; i<_size[0]; i++)
		_data[i]->SetAllFields(value);

}

void Tensor::Print(std::ostream& out) const
{
	for(unsigned int i=0; i<_size.size(); i++)
		out << "\n";
	for(int i=0; i<_size[0]; i++)
		out << (*_data[i]);
}

std::ostream& operator<<(std::ostream& out, const Tensor& t1)
{
	t1.Print(out); 
	return out;
}

bool Tensor::IsZero() const
{
	for(int i=0; i<_size[0]; i++)
		if(!_data[i]->IsZero()) return false;
	return true;
}

Tensor operator/(const Tensor& t1,    const double& d1)
{
	Tensor tOut = t1;
	for(int i=0; i<t1._size[0]; i++) 
		(*tOut._data[i]) = (*tOut._data[i])/d1; 
	return tOut;
}

Tensor Tensor::Poisson(Tensor t1) const
{
	//:T_i::T_j: is of order i+j-3
	int    rankOut = GetRank() + t1.GetRank() - 3;
	Tensor tOut(6, rankOut, 0);
	//:T_i::T_j: = dT_i/du_p d/dConj{u_p} [dT_j/du_q d/dConj{u_q}]
	//so tOut[i1][i2]...[ik] = dGdUi(j, v)*Delta(i, GetConjugate(j))*pow(-1, j+1);
	
	//THIS IS THE BIT I AM WORKING ON - TRYING TO FIGURE IT OUT!

	return tOut;
}

/*
std::vector<Tensor*> Tensor::Exponential(int maxOrder) const
{
	//:T_i:^n is of order n*(i-2)+1
	//maxPower can be non-integer
	double  maxPower  = (maxOrder-1)/(double)(GetRank()-2);
	int     nFact     = 1; //n!
	std::vector<Tensor*> out; // sum(:t:^n)
	out.push_back(this->Clone());
	for(int n=2; (double)n<=maxPower; n++)
	{
		nFact *= n;
		Tensor* current = out.back(); 
		out.push_back(new Tensor(current->Poisson(*this)/nFact));
	}	
	return out;
}

CLHEP::HepVector Tensor::ExponentialPoisson(CLHEP::HepVector v, int maxOrder) const
{
	std::vector<Tensor*> ExpT = Exponential(maxOrder);
	CLHEP::HepVector out;
	for(unsigned int i=0; i<ExpT.size(); i++)
		out += ExpT[i]->Poisson(v);
	return out;
}

CLHEP::HepVector Tensor::Poisson(CLHEP::HepVector v) const;
{
	int              dimension = v.num_row();
	CLHEP::HepVector vOut(dimension);
	std::vector<int> index(_size.size());
	return vOut;
}
*/
/*
	CLHEP::HepVector     Poisson(CLHEP::HepVector v) const;
	CLHEP::HepMatrix     Poisson(CLHEP::HepMatrix m) const;
	Tensor               Poisson(Tensor           t) const;
*/

//Turn int index into an index for element of symmetric tensor
std::vector<int> Tensor::IndexToIndices(int index, int tensorSize, int startDimension)
{
	index++;
	std::vector<int> indices(1,-1);
	std::vector<int> subIndices(1,0);
	int hex        = tensorSize ;
	for(int i=0; i<index; i++)
	{
//		for(unsigned int i=0; i<indices.size(); i++) std::cout << indices[i] << " ";
		if     (indices.front() == hex-1) { indices = std::vector<int>(indices.size()+1, 0); indices.back()--;}
		if     (indices.back()  == hex-1)
		{
			int j=indices.size()-1;
			while(indices[j] == hex-1) j--;
			for(int k=indices.size()-1; k>=j; k--) indices[k] = indices[j]+1;
		}
		else    indices.back()++;
	}
//	std::cout << std::endl;
	for(unsigned int i=0; i<indices.size(); i++) indices[i] += startDimension;
	return indices;
}
/*
	std::vector<int> indices(0);
	//convert from decimal to heximal
	int hex        = tensorSize - 1;
	for(int i=0; i<=index; i++)
	{
		int j = indices.size()-1;
		if(j >= 0) while(indices[j]  == hex && j>=0) j--;
		if(j >= 0) indices[j]++;
		else       indices = std::vector<int>(indices.size()+1);
	}
	return indices;
}
*/

double Tensor::Get(std::vector<int> position) const
{
	if(int(position.size()) != GetRank()) throw(MAUS::Exceptions::Exception(MAUS::Exceptions::recoverable, "Get rank different to Tensor rank", "Tensor::Get"));
	const Tensor* val = this;
	if(GetRank()>3)
	{
		for(unsigned int i=position.size()-1; i>3; i--)
			val = val->_data[position[i]];
		val = val->_data[position[3]];
	}
	return ( *((Tensor3*)val) )[position[0]][position[1]][position[2]];
}

void   Tensor::Set     (std::vector<int> position, double value)
{
	Tensor* val = this;
	if(GetRank()>3)
	{
		for(unsigned int i=position.size()-1; i>3; i--)
			val = val->_data[position[i]];
		val = val->_data[position[3]];
	}
	( *((Tensor3*)val) )[position[0]][position[1]][position[2]] = value;
}



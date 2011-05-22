#include "Tensor3.hh"

Tensor3::Tensor3(int a, int b, int c, double val) : Tensor(0, 9), _a(a), _b(b), _c(c)
{
	for(int i=0; i<c; i++)
	{
		_data.push_back(CLHEP::HepMatrix(a,b,0));
		if(i<a && i<b && i<c)
			_data[i][i][i] = val;
	}
}

Tensor3&  Tensor3::operator*=(double t)
{
	for(int ia=0; ia<_a; ia++)
		for(int ib=0; ib<_b; ib++)
			for(int ic=0; ic<_c; ic++)
				_data[ia][ib][ic]*=t;
	return *this;
}

Tensor3&  Tensor3::operator/=(double t)
{
	for(int ia=0; ia<_a; ia++)
		for(int ib=0; ib<_b; ib++)
			for(int ic=0; ic<_c; ic++)
				_data[ia][ib][ic]/=t;
	return *this;
}

Tensor3&  Tensor3::operator+=(double t)
{
	for(int ia=0; ia<_a; ia++)
		for(int ib=0; ib<_b; ib++)
			for(int ic=0; ic<_c; ic++)
				_data[ia][ib][ic]+=t;
	return *this;
}

Tensor3&  Tensor3::operator-=(double t)
{
	for(int ia=0; ia<_a; ia++)
		for(int ib=0; ib<_b; ib++)
			for(int ic=0; ic<_c; ic++)
				_data[ia][ib][ic]-=t;
	return *this;
}

Tensor3  Tensor3::operator-() const
{
	Tensor3 out(*this);
	out *= -1;
	return out;
}

Tensor3 operator/(const Tensor3& t1,    const double d1)
{
	Tensor3          out  = t1;
	std::vector<int> size = out.Size();
	for(int ia=0; ia<size[0]; ia++)
		for(int ib=0; ib<size[1]; ib++)
			for(int ic=0; ic<size[2]; ic++)
				out[ia][ib][ic]/=d1;
	return out;
}

CLHEP::HepVector Tensor3::Poisson(CLHEP::HepVector v) const
{
	//find :T:v
	//loop over upper diagonal of \mathbf{T}, add the partial derivative wrt \vec{v}.
	int    dimension = v.num_row();
	CLHEP::HepVector vOut(dimension);
	for(int i=0; i<dimension; i++)
		for(int j=0; j<dimension; j++)
			vOut[i] += dGdUi(j, v)*Delta(i, GetConjugate(j))*pow(-1, j+1);
	return vOut;
}

std::vector<Tensor*> Tensor3::Exponential(int maxOrder) const
{
//:T_i:^n is of order n*(i-2)+1
//so :T_3:^n is of order n+1
	int     maxPower  = maxOrder-1;
	int     nFact     = 1; //n!
	std::vector<Tensor*> out; // sum(:t:^n)
	out.push_back(this->Clone());
	for(int n=2; n<=maxPower; n++)
	{
		nFact          *= n;
		Tensor* current = out.back(); 
		out.push_back(new Tensor(current->Poisson(*this)/nFact));
	}
	return out;
}

CLHEP::HepVector Tensor3::ExponentialPoisson(CLHEP::HepVector v, int maxOrder) const
{
	std::vector<Tensor*> ExpT = Exponential(maxOrder);
	CLHEP::HepVector out;
	for(unsigned int i=0; i<ExpT.size(); i++)
		out += ExpT[i]->Poisson(v);
	return out;
}

double Tensor3::dGdUi(int i, CLHEP::HepVector v) const
{
	double dGdUi     = 0;
	int    dimension = v.num_row();
	for(int p=0; p<dimension; p++)
		for(int q=p; q<dimension; q++)
			for(int r=q; r<dimension; r++)
				dGdUi += _data[p][q][r]*(v[q]*v[r]*Delta(i,p) + v[p]*v[r]*Delta(i,q) + v[p]*v[q]*Delta(i,r));
	return dGdUi;
}

void Tensor3::Print(std::ostream& out) const
{
	for(int i=0; i<_c; i++)
	out << _data[i] << "\n";
}

bool Tensor3::IsZero() const
{
	for(int i=0; i< _a; i++)
		for(int j=0; i<_b; j++)
			for(int k=0; k<_c; k++)
				if(_data[i][j][k] > 1e-10 || _data[i][j][k] < -1e-10) return false;
	return true;
}

Tensor3 operator+(const Tensor3& t1, const Tensor3& t2)
{
	Tensor3 t3(t1);
	std::vector<int> size = t3.Size();
	for(int ia=0; ia<size[0]; ia++)
		for(int ib=0; ib<size[1]; ib++)
			for(int ic=0; ic<size[2]; ic++)
				t3[ia][ib][ic] = t1[ia][ib][ic]+t2[ia][ib][ic];
	return t3;
}

Tensor3 operator-(const Tensor3& t1, const Tensor3& t2)
{
	Tensor3 t3(t1);
	std::vector<int> size = t3.Size();
	for(int ia=0; ia<size[0]; ia++)
		for(int ib=0; ib<size[1]; ib++)
			for(int ic=0; ic<size[2]; ic++)
				t3[ia][ib][ic] = t1[ia][ib][ic]-t2[ia][ib][ic];
	return t3;
}

std::ostream& operator<<(std::ostream& out, const Tensor3& t1)
{
	t1.Print(out);
	return out;
}

void Tensor3::SetAllFields(double value)
{
	std::vector<int> size = Size();
	for(int ia=0; ia<size[0]; ia++)
		for(int ib=0; ib<size[1]; ib++)
			for(int ic=0; ic<size[2]; ic++)
				(*this)[ia][ib][ic] = value;
}

std::vector<int> Tensor3::Size() const
{
	std::vector<int> size(3);
	size[0] = _a;
	size[1] = _b;
	size[2] = _c;
	return size;
}

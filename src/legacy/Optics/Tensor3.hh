//Tensor3.hh
//3rd rank tensor (i.e. one up from a matrix)
//Interfaces between general n-rank tensor and rank-2, rank-1 HepMatrix and HepVector
//Saves me having to rewrite the useful linear optics type methods

#ifndef Tensor3_hh
#define Tensor3_hh

#include "CLHEP/Matrix/Matrix.h"
#include "CLHEP/Matrix/Vector.h"

#include <vector>

#include "Tensor.hh"

class Tensor3 : public Tensor
{
public:
	Tensor3() : Tensor(0,9), _data(), _a(0), _b(0), _c(0) {;}
	Tensor3(int a, int b, int c, double val=0.);
	~Tensor3() {;}

	Tensor* Clone() const {return new Tensor3(*this);}

	CLHEP::HepMatrix&       operator[](int a)          {return _data[a];}
	const CLHEP::HepMatrix& operator[](int a) const    {return _data[a];}
	CLHEP::HepMatrix&       operator()(int a)          {return _data[a-1];}
	const CLHEP::HepMatrix& operator()(int a) const    {return _data[a-1];}
	double&                 operator()(int a, int b, int c)       {return _data[a-1][b-1][c-1];}
	const double&           operator()(int a, int b, int c) const {return _data[a-1][b-1][c-1];}

//	std::vector<int>  Size() const;// {return _size;}
//	std::vector<int>  size() const;// {return _size;}
	Tensor3&           operator*=(double t);
	Tensor3&           operator/=(double t);
	Tensor3&           operator+=(double t);
	Tensor3&           operator-=(double t);
	Tensor3            operator-() const;

	//Poisson bracket operation
	CLHEP::HepVector     Poisson(CLHEP::HepVector v) const;
	//Return exp(:T3:) calculated up to some order
	std::vector<Tensor*> Exponential(int maxOrder)   const;
	CLHEP::HepVector     ExponentialPoisson(CLHEP::HepVector v, int maxOrder) const;
	//
	void Print(std::ostream& out) const;
	bool IsZero()                 const;
	int  GetRank()                const {return 3;}
	std::vector<int> Size()       const;

	void SetAllFields(double value);


private:
	friend Tensor3 operator+(const Tensor3& t1,    const Tensor3& t2);
	friend Tensor3 operator-(const Tensor3& t1,    const Tensor3& t2);
	friend Tensor3 operator*(const Tensor3& t1,    const Tensor3& t2);
	friend Tensor3 operator*(const CLHEP::HepMatrix& t1,  const Tensor3& t2);
	friend Tensor3 operator*(const Tensor3& t1,    const CLHEP::HepVector& v1);
	friend Tensor3 operator/(const Tensor3& t1,    const double t);

	int GetConjugate(int k) const {if(k==0||k==2||k==4) return k+1; else return k-1;}
	int Delta(int i, int j) const {if(i==j) return 1; else return 0;}
	double dGdUi(int i, CLHEP::HepVector v) const;

	std::vector<CLHEP::HepMatrix> _data;
	int _a, _b, _c;

};

std::ostream& operator<<(std::ostream& out, const Tensor3& t1);
Tensor3       operator/(const Tensor3& t1,  const double d1);


#endif





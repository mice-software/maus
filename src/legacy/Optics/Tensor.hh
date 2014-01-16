//Tensor.hh
//Generalized Tensor class
//For tensors of rank < 4 use Tensor3, HepMatrix, HepVector
//This places some limits on what methods I can have in this generalised class

#ifndef Tensor_hh
#define Tensor_hh

#include "CLHEP/Matrix/Matrix.h"
#include "CLHEP/Matrix/Vector.h"
#include "Utils/Exception.hh"
//#include "Tensor3.hh"

#include <vector>

class Tensor
{
public:
	Tensor() {;}
	Tensor(std::vector<int> size, double val=1);
	Tensor(int size[],  int rank, double val=1);
	Tensor(int size,    int rank, double val=1); //"Square"

	virtual Tensor* Clone() const {return new Tensor(*this);}


	virtual ~Tensor();

	//Dereference operators?
	Tensor&       operator[](int a)          {return (*_data)[a];}
	Tensor&       operator()(int a)          {return (*_data)[a-1];}
	const Tensor& operator[](int a) const    {return (*_data)[a];}
	const Tensor& operator()(int a) const    {return (*_data)[a-1];}

	virtual std::vector<int>  Size() const {return _size;}
//	std::vector<int>  size() const {return _size;}
	Tensor            operator-() const;

	//Poisson bracket operations NEEDS IMPLEMENTING
	virtual CLHEP::HepVector     Poisson(CLHEP::HepVector v) const {return CLHEP::HepVector(6,0);}
	virtual CLHEP::HepMatrix     Poisson(CLHEP::HepMatrix m) const {return CLHEP::HepMatrix(6,6,0);}
	virtual Tensor               Poisson(Tensor           t) const;
	virtual std::vector<Tensor*> Exponential(int maxOrder)   const {return std::vector<Tensor*>();}
	virtual CLHEP::HepVector     ExponentialPoisson(CLHEP::HepVector v, int maxOrder) const {return CLHEP::HepVector(6,0);}


	//Swap from index that 0,1,2,... to indices that go 0,1,2...,5,00,01,02...05,11,12...55,000,...
	//Turn int index into an index for element of symmetric tensor
	static std::vector<int> IndexToIndices(int index, int tensorSize, int startDimension=0);
	//Turn index for element of symmetric tensor into an int
	static int IndexToIndices(std::vector<int> index, int tensorSize);

	//Get the value at position
	double Get(std::vector<int> position) const;
	//Modify the value at position; indexes from 0
	void   Set     (std::vector<int> position, double value); 
	void   Add     (std::vector<int> position, double value) {Set(position, Get(position)+value);}
	void   Subtract(std::vector<int> position, double value) {Set(position, Get(position)-value);} 

	//Set to be exactly zero
	virtual bool IsZero() const;
	//Return the rank of the tensor
	virtual int  GetRank() const {return _size.size();}

	virtual void Print(std::ostream& out) const;

	//Useful set methods
	void         SetTensor   (std::vector<int> size, double val);
	virtual void SetAllFields(double value);

private:
	friend Tensor    operator+(const Tensor& t1,    const Tensor& t2);
	friend Tensor    operator-(const Tensor& t1,    const Tensor& t2);
	friend Tensor    operator+(const Tensor& t1,    const double& d1);
	friend Tensor    operator-(const Tensor& t1,    const double& d1);
	friend Tensor    operator*(const Tensor& t1,    const double& d1);
	friend Tensor    operator/(const Tensor& t1,    const double& d1);

	//One to one mapping from an int to an index of all fields
	int              fieldIndexToInt(std::vector<int> fieldIndex);
	std::vector<int> intToFieldIndex(int              fieldIndex);

	int GetConjugate(int k) const {if(k==0||k==2||k==4) return k+1; else return k-1;}
	int Delta(int i, int j) const {if(i==j) return 1; else return 0;}
	double dGdUi(int i, CLHEP::HepVector v) const;

	Tensor**            _data;
	std::vector<int>    _size;
};

//implemented:
std::ostream& operator<<(std::ostream& out, const Tensor& t1);// {t1.Print(out); return out;}
Tensor        operator/(const Tensor& t1,   const double& d1);// {for(int i=0; i<t1._size[0]; i++) (*t1._data[i]) = (*t1._data[i])/d1; return t1}

#endif





#include <recpack/EAlgebra.h>
#include <assert.h>
#include <recpack/EGeo.h>

#include <stdio.h>

using namespace Recpack;


//*******************************************
EMatrix Recpack::inverse(const EMatrix& M) {
//*******************************************

  int ierr;
  EMatrix  M_inv = M.inverse(ierr); 
  if (ierr > 0) 
    std::cout << "--ERROR: fail inverting matrix " << ierr << std::endl;
  
  return M_inv;

}

//*******************************************
EMatrix Recpack::transpose(const EMatrix& M) {
//*******************************************

  EMatrix  M_trans = M.T(); 

  return M_trans;

}

//*******************************************
EVector Recpack::crossprod(const EVector& v1, const EVector& v2) {
//*******************************************

  EVector  prod(3,0);

  assert(v1.num_row() == 3 && v2.num_row() == 3);

  prod[0] = v1[1]*v2[2] - v1[2]*v2[1]; 
  prod[1] = v1[2]*v2[0] - v1[0]*v2[2]; 
  prod[2] = v1[0]*v2[1] - v1[1]*v2[0]; 

  return prod;
}

//*******************************************
EVector Recpack::box(const EVector& v, size_t i, size_t n) {
//*******************************************

  EVector v2(n,0); 
  for (size_t j = 0; j < n; j++) v2[j]=v[i+j];
  return v2;
}

//*******************************************
void Recpack::set_box(const EVector& v, size_t i, EVector& v1) {
//*******************************************

  for (size_t j = 0; j < (size_t) v.num_row(); j++) v1[i+j]=v[j];
}

//*******************************************
EMatrix Recpack::box(const EMatrix& m, size_t i1, size_t i2, size_t n) {
//*******************************************

  return box(m,i1,i2,n,n);
}

//*******************************************
EMatrix Recpack::box(const EMatrix& m, size_t i1, size_t i2, 
		     size_t n1, size_t n2) {
//*******************************************

  EMatrix m2(n1,n2,0); 
  for (size_t j1 = 0; j1 < n1; j1++) 
    for (size_t j2 = 0; j2 < n2; j2++) 
      m2[j1][j2]=m[i1+j1][i2+j2];
  return m2;
}

//*******************************************
void Recpack::set_box(const EMatrix& m, size_t i1, size_t i2, EMatrix& m1) {
//*******************************************

  for (size_t j1 = 0; j1 < (size_t) m.num_row(); j1++) 
    for (size_t j2 = 0; j2 < (size_t) m.num_col(); j2++) 
      m1[i1+j1][i2+j2]=m[j1][j2];
}

//*******************************************
void Recpack::valid_cov_matrix(EMatrix& cov) {
//*******************************************

  size_t dim = cov.num_row();
  for (size_t i = 0; i < dim; i++) {
    if (EGeo::is_zero_cov(cov[i][i])) {
      for (size_t j=0; j < dim; j++) 
	{cov[j][i] = 0.; cov[i][j] = 0.; cov[i][i] = EGeo::zero_cov()/100.;}
    }
  }
  
}

//HepStd::ostream& Recpack::operator<<(HepStd::ostream &os, const HepGenMatrix &vector){
//*******************************************
std::ostream& Recpack::operator<<(std::ostream &os, const HepGenMatrix &vector){
//*******************************************

  os << "(";

  /*  
  for (int i=0; i<vector.num_row(); i++){
    os << vector[i];
    if (i<vector.num_row()-1) os << ", ";
  }
  */
  os << ")";    

  return os;
}

//*******************************************
std::string Recpack::print(const EVector& vector, const std::string& format){
//*******************************************

  std::string os=""; 

  char temp[10];
  //  std::string format2 = "%"+format+"f";
  //  if (format=="") format2="%f";
  
  os = "(";

  for (int i=0; i<vector.num_row(); i++){
    //    sprintf(temp,format2.c_str(),vector[i]);
    sprintf(temp,"%f",vector[i]);
    os += temp;
    if (i<vector.num_row()-1) os += ", ";
  }

  os += ")";    

  return os;

}

//*******************************************
std::string Recpack::print(const EMatrix& matrix, const std::string& format){
//*******************************************

  std::string os=""; 

  char temp[10];
  //  std::string format2 = "%"+format+"f";
  //  if (format=="") format2="%f";

  os = "(";

  for (int i=0; i<matrix.num_row(); i++){
    //    sprintf(temp,format2.c_str(),matrix[i][i]);
    sprintf(temp,"%f",matrix[i][i]);
    os += temp;
    if (i<matrix.num_row()-1) os += ", ";
  }

  os += ")";    

  return os;

}

//*******************************************
std::string Recpack::print(const double d, const std::string& format){
//*******************************************

  std::string os=""; 

  char temp[10];
  //  std::string format2 = "%"+format+"f";
  //  if (format=="") format2="%f";
  
  //  sprintf(temp,format2.c_str(),d);
  sprintf(temp,"%f",d);
  os += temp;

  return os;

}




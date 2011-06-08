// MAUS WARNING: THIS IS LEGACY CODE.
/* This file is part of MAUS: http://micewww.pp.rl.ac.uk/projects/maus
 *
 * MAUS is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * MAUS is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with MAUS.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef MATHUTILS_HH
#define MATHUTILS_HH

#include <vector>

/// Calculate the Enge function (e.g. for multipole end fields).

/// The Enge function is given by\n
/// \f$E(x) = 1/g(x)\f$\n
/// where\n
/// \f$g(x) = 1+exp(h(x))\f$\n
/// and\n
/// \f$h(x) = a_0 + a_1 x/\lambda + a_2 x^2/\lambda^2 + \ldots \f$\n
/// Additionally we have implemented DoubleEnge, which is simply
/// \f$D(x) = E(x-x0)-E(-x-x0)-1\f$
/// for e.g. a multipole (which obviously has two ends)
///
/// The derivatives of enge in terms of derivatives of g and the derivatives of
/// g in terms of derivatives of h are stored in a set in indices. Specifically,
/// we store the\n
/// \f$ d^n E/dx^n = Sum_m q_nm0 g^{q_nm1}(0) g^{q_nm2}(1) ... g^(q_nmi)(i-1)\f$
/// where g^n(i) is the nth power of the ith derivative of g. Similarily, The
/// derivatives of g go like\n
/// \f$ d^n g/dx^n = Sum_m q_nm0 exp(h) h^{q_nm1}(1) h^{q_nm2}(2) ...
///                  h^(q_nmi)(i)\f$\n
/// where \f$h\f$ is some polynomial. Using these expressions, one can calculate
/// a recursion relation for higher order derivatives and hence calculate
/// analytical derivatives at arbitrary order.
///
/// h(x) is just a polynomial so we can calculate derivatives easily.

// TODO(Rogers):
// * store enge _ai as _ai/lambda^i? Might be slight speed improvement
class Enge {
  public:
    /// Default constructor
    Enge() : _a(), _lambda(0.) {SetEngeDiffIndices(10);}
    /// Builds Enge function with parameters a_0, a_1, ..., lambda and x0.

    /// max_index is the maximum derivative that will be used in calculation
    /// if, after setup, you find you need to calculate higher derivatives, you
    /// can just call SetEngeDiffIndices(n) where n is the highest derivative
    /// you think you will need.
    Enge(const std::vector<double> a, double x0, double lambda, int max_index)
        : _a(a), _lambda(lambda), _x0(x0) {SetEngeDiffIndices(max_index);}
    ~Enge() {}
    /// Returns the enge parameters (a_i)
    std::vector<double> GetEngeParameters() const {return _a;}
    /// Sets the enge parameters (a_i)
    void                SetEngeParameters(std::vector<double> a) {_a = a;}
    /// Returns the value of lambda
    inline double       GetLambda() const {return _lambda;}
    /// Sets the value of lambda
    inline void         SetLambda(double lambda) {_lambda = lambda;}
    /// Returns the value of x0
    inline double       GetX0() const {return _x0;}
    /// Sets the value of x0
    inline void         SetX0(double x0) {_x0 = x0;}
    /// Returns the value of the Enge function or its \f$n^{th}\f$ derivative.

    /// Please call SetEngeDiffIndices(n) before calling if n > max_index
    double GetEnge(double x, int n) const;
    /// Returns \f$Enge(x-x0) + Enge(-x-x0)-1\f$ and its derivatives
    inline double GetDoubleEnge(double x, int n) const;
    /// Returns \f$h(x)\f$ or its \f$n^{th}\f$ derivative.

    /// Here \f$h(x) = a_0 + a_1 x/\lambda + a_2 x^2/lambda^2 + \ldots \f$
    /// Please call SetEngeDiffIndices(n) before calling if n > max_index
    double HN(double x, int n) const;
    /// Returns \f$g(x)\f$ or its \f$n^{th}\f$ derivative.

    /// Here \f$g(x) = 1+exp(h(x))\f$.
    /// Please call SetEngeDiffIndices(n) before calling if n > max_index
    double GN(double x, int n) const;
    /// Recursively calculate the indices for Enge and H

    /// This will calculate the indices for Enge and H that are required to
    /// calculate the differential up to order n.
    static void   SetEngeDiffIndices(size_t n);
    /// Return the indices for calculating the nth derivative of Enge ito g(x)
    inline static std::vector< std::vector<int> > GetQIndex(int n);
    /// Return the indices for calculating the nth derivative of g(x) ito h(x)
    inline static std::vector< std::vector<int> > GetHIndex(int n);
  private:
    std::vector<double> _a;
    double              _lambda, _x0;

    /// Indexes the derivatives of enge in terms of g
    static std::vector< std::vector< std::vector<int> > > _q;
    /// Indexes the derivatives of g in terms of h
    static std::vector< std::vector< std::vector<int> > > _h;
};

std::vector< std::vector<int> > Enge::GetQIndex(int n) {
  SetEngeDiffIndices(n);
  return _q[n];
}

std::vector< std::vector<int> > Enge::GetHIndex(int n) {
  SetEngeDiffIndices(n);
  return _h[n];
}

double Enge::GetDoubleEnge(double x, int n) const {
  if (n == 0) {
    return (GetEnge(x-_x0, n)+GetEnge(-x-_x0, n))-1.;
  } else {
    if (n%2 != 0) return GetEnge(x-_x0, n)-GetEnge(-x-_x0, n);
    else          return GetEnge(x-_x0, n)+GetEnge(-x-_x0, n);
  }
}

/// Calculate the Tanh function (e.g. for multipole end fields).

/// DoubleTanh function is given by\n
/// \f$T(x) = (tanh( (x+x0)/\lambda )-tanh( (x-x0)/\lambda ))/2\f$\n
/// The derivatives of tanh(x) are given by\n
/// \f$d^p tanh(x)/dx^p = \sum_q I_{pq} tanh^{q}(x)\f$\n
/// where \f$I_{pq}\f$ are calculated using some recursion relation. Using these
/// expressions, one can calculate a recursion relation for higher order
/// derivatives and hence calculate analytical derivatives at arbitrary order.
class Tanh {
  public:
    /// Create a double tanh function

    /// Here x0 is the centre length and lambda is the end length. max_index is
    /// used to set up for differentiation - don't try to calculate
    /// higher differentials than exist in max_index.
    Tanh(double x0, double lambda, int max_index);
    /// Default constructor (initialises x0 and lambda to 0).
    Tanh() : _x0(0.), _lambda(0.) {SetTanhDiffIndices(10);}
    /// Destructor (no mallocs so does nothing)
    ~Tanh() {}
    /// Returns the value of DoubleTanh or its \f$n^{th}\f$ derivative.

    /// Double Tanh is given by\n
    /// \f$d(x) = \f$
    double GetDoubleTanh(double x, int n) const;
    /// Returns the value of tanh((x+x0)/lambda) or its \f$n^{th}\f$ derivative.
    double GetTanh(double x, int n) const;
    /// Returns the value of tanh((x-x0)/lambda) or its \f$n^{th}\f$ derivative.
    double GetNegTanh(double x, int n) const;
    /// Get all the tanh differential indices \f$I_{pq}\f$.

    /// Returns vector of vector of ints where p indexes the differential and
    /// q indexes the tanh power - so
    static std::vector< std::vector<int> > GetTanhDiffIndices(size_t n);
    /// Set the value of tanh differential indices to nth order differentials.
    static void SetTanhDiffIndices(size_t n);
    /// Return lambda (end length)
    inline double GetLambda() const {return _lambda;}
    /// Return x0 (flat top length)
    inline double GetX0() const {return _x0;}
    /// Set lambda (end length)
    inline void   SetLambda(double lambda) {_lambda = lambda;}
    /// Set x0 (flat top length)
    inline void   SetX0(double x0)     {_x0 = x0;}
  private:
    double _x0, _lambda;
    /// _tdi indexes powers of tanh in d^n tanh/dx^n as sum of powers of tanh

    /// For some reason we index as n, +a, -a, but the third index is redundant
    static std::vector< std::vector< std::vector<int> > > _tdi;
};

namespace MathUtils {
/// Remove duplicates in the vector vec, adding some coefficients together.

/// We have a few cases where a vector of vector of ints that is used to index
/// linear sums of some functions. In this case, the first element indexes the
/// coefficient and higher indices index the function (e.g. polynomial power or
/// whatever). In this case, it makes things quicker to add terms with the same
/// polynomial power together.
///
/// CompactVector then looks at the list of indices. If it finds a duplicate, it
/// adds the first index of each list together and then removes the duplication.
/// Returns the compacted vectors, but ordering of the vectors may change.
std::vector< std::vector<int> > CompactVector
                            (std::vector< std::vector<int> > vec);
/// CompactVector helper function, used for sorting
bool GreaterThan(std::vector<int> v1, std::vector<int> v2);
}

#endif

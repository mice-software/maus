#include "rogers_math/PolynomialPatch.hh"

PolynomialPatch::PolynomialPatch(Mesh* grid_points,
                    std::vector<PolynomialVector*> polynomials) {
    grid_points_ = grid_points;
    points_ = polynomials;
    if (grid_points_ == NULL)
        throw MAUS::Exception(MAUS::Exception::recoverable,
                              "PolynomialPatch grid_points_ was NULL",
                              "PolynomialPatch::PolynomialPatch");
    // validate input data
    if (points_.size() == 0) {
        throw MAUS::Exception(
            MAUS::Exception::recoverable,
            "Could not make PolynomialPatch with 0 length polynomials vector",
            "PolynomialPatch::PolynomialPatch");
    }
    Mesh::Iterator end = grid_points_->End();
    if (points_.size() != end.ToInteger()) {
        throw MAUS::Exception(
            MAUS::Exception::recoverable,
            "Could not make PolynomialPatch with bad length polynomials vector",
            "PolynomialPatch::PolynomialPatch");
    }
    point_dimension_ = grid_points_->PositionDimension();
    value_dimension_ = points_[0]->ValueDimension();
    for (size_t i = 0; i < points_.size(); ++i) {
        if (points_[i] == NULL)
            throw MAUS::Exception(MAUS::Exception::recoverable,
                              "PolynomialPatch points_ element was NULL",
                              "PolynomialPatch::PolynomialPatch");
        if (points_[i]->PointDimension() != point_dimension_) {
            std::cerr << "i " << i << " " << points_[i]->PointDimension() << " " << point_dimension_ << std::endl;
            throw MAUS::Exception(
                MAUS::Exception::recoverable,
                "Polynomial with mismatched PointDimension in PolynomialPatch",
                "PolynomialPatch::PolynomialPatch");
        }
        if (points_[i]->ValueDimension() != value_dimension_)
            throw MAUS::Exception(
                MAUS::Exception::recoverable,
                "Polynomial with mismatched ValueDimension in PolynomialPatch",
                "PolynomialPatch::PolynomialPatch");
    }
}

PolynomialPatch* PolynomialPatch::Clone() const {
    throw MAUS::Exception(
        MAUS::Exception::recoverable,
        "PolynomialPatch::CLone not implemented",
        "PolynomialPatch::Clone()");
}

PolynomialPatch::PolynomialPatch()
  : grid_points_(NULL), points_(), point_dimension_(0),
    value_dimension_(0) {
}

PolynomialPatch::~PolynomialPatch() {
    if (grid_points_ != NULL)
        delete grid_points_;
    for (size_t i = 0; i < points_.size(); ++i)
        delete points_[i];
}

void PolynomialPatch::F(const double* point, double* value) const {
    Mesh::Iterator nearest = grid_points_->Nearest(point);
    int points_index = nearest.ToInteger();
    points_[points_index]->F(point, value);
    std::cerr << points_[points_index]->GetCoefficientsAsMatrix();
}

PolynomialPatch* PolynomialPatch::LeastSquaresFit(
                                Mesh* points,
                                std::vector<std::vector<double> > values,
                                int polynomial_order,
                                int smoothing_order) {
    if (points == NULL) {
        throw MAUS::Exception(MAUS::Exception::recoverable,
                          "NULL Mesh input to LeastSquaresFit",
                          "PolynomialPatch::PolynomialPatch");
    }
    if (points->End().ToInteger() != values.size()) {
        throw MAUS::Exception(MAUS::Exception::recoverable,
                          "Mismatch between Mesh and values in LeastSquaresFit",
                          "PolynomialPatch::LeastSquaresFit");
    }
    if (smoothing_order >= polynomial_order) {
        throw MAUS::Exception(MAUS::Exception::recoverable,
                "Smoothing order must be < polynomial_order in LeastSquaresFit",
                "PolynomialPatch::LeastSquaresFit");
    }
    unsigned int pos_dim = points->PositionDimension();
    unsigned int value_dim = values[0].size();
    int npoints = PolynomialVector::NumberOfPolynomialCoefficients
                                                    (pos_dim, polynomial_order);
    std::cerr << "NPoints " << npoints << std::endl;
    Mesh::Iterator begin = points->Begin();
    Mesh::Iterator end = points->End()-1;
    std::vector<PolynomialVector*> polynomials;
    // get the list of points that are needed to make a given poly vector
    int n_coeffs = PolynomialVector::NumberOfPolynomialCoefficients(pos_dim, polynomial_order);
    std::cerr << "NCoeffs " << n_coeffs << std::endl;
    std::vector<std::vector<int> > nearby_points = GetNearbyPoints(pos_dim, value_dim, n_coeffs*4);
    for (Mesh::Iterator it = points->Begin(); it != points->End(); ++it) {
        // find the set of points that can be used to make the polynomial
        std::vector< std::vector<double> > this_points;
        std::vector< std::vector<double> > this_values;

        // now iterate over the index_by_power_list elements - offset; each of
        // these makes a point in the polynomial fit
        std::cerr << "polynomial " << it.ToInteger() << std::endl;
        for (size_t i = 0; i < nearby_points.size() && this_points.size() < n_coeffs; ++i) {
            Mesh::Iterator it_current = it;
            for (size_t j = 0; j < pos_dim; ++j) {
                it_current[j] += nearby_points[i][j];
                if (it_current[j] < begin[j] || it_current[j] > end[j])
                    it_current[j] -= 2*nearby_points[i][j];
            }
            //if (out_of_bounds)
            //    continue;
            for (size_t j = 0; j < it.State().size(); ++j) {
                std::cerr << " i" << j << " " << it_current[j];
            }
            this_points.push_back(it_current.Position());
            this_values.push_back(values[it_current.ToInteger()]);
            for (size_t j = 0; j < this_points.back().size(); ++j) {
                std::cerr << " x" << j << " " << this_points[i][j];
            }
            for (size_t j = 0; j < this_values.back().size(); ++j) {
                std::cerr << " y" << j << " " << this_values[i][j] << " ";
            }
            std::cerr << std::endl;
        }
        // The polynomial is found using least squares algorithm
        polynomials.push_back(PolynomialVector::PolynomialLeastSquaresFit(
            this_points, this_values, polynomial_order
        ));
        if (it.ToInteger()) {// == printout) {
            std::cerr << polynomials.back()->GetCoefficientsAsMatrix();
        }
    }
    return new PolynomialPatch(points, polynomials);
}

// check kth element in check; if it is non-zero, multiply by -1, append to nearby_points, call check on both positive and negative branches
void PolynomialPatch::NearbyPointsRecursive(std::vector<int> check, size_t k, std::vector<std::vector<int> >& nearby_points) {
    if (check[k] != 0) {
        std::vector<int> neg_check = check;
        neg_check[k] *= -1;
        nearby_points.push_back(neg_check);
        if (k+1 < check.size())
            NearbyPointsRecursive(neg_check, k+1, nearby_points);
    }
    if (k+1 < check.size())
        NearbyPointsRecursive(check, k+1, nearby_points);
}

bool sort_predicate(std::vector<int> arg1, std::vector<int> arg2) {
    int sum = 0;
    for (size_t i = 0; i < arg1.size(); ++i) {
        sum += arg1[i]*arg1[i];
    }
    for (size_t i = 0; i < arg2.size(); ++i) {
        sum -= arg2[i]*arg2[i];
    }
    return sum < 0;
}

std::vector<std::vector<int> > PolynomialPatch::GetNearbyPoints(int point_dim, int value_dim, int npoints) {
    std::vector<std::vector<int> > nearby_points;
    PolynomialVector test(point_dim, MMatrix<double>(value_dim, npoints, 0.)); 
    std::vector< std::vector<int> > positive_offset_list;
    for (int i = 0; i < npoints; ++i) {
        positive_offset_list.push_back(test.IndexByPower(i, point_dim));
    }
    // std::sort(positive_offset_list.begin(),
    //          positive_offset_list.end(),
    //          sort_predicate);
    for (int i = 0; i < positive_offset_list.size() && nearby_points.size() < npoints; ++i) {
        std::vector<std::vector<int> > these_nearby_points(1, positive_offset_list[i]);
        // NearbyPointsRecursive(positive_offset_list[i], 0, these_nearby_points);
        nearby_points.insert(nearby_points.end(), these_nearby_points.begin(), these_nearby_points.end());
    }
    nearby_points.erase(nearby_points.begin()+npoints, nearby_points.end());
    return nearby_points;
}


PolynomialVector* PolynomialPatch::GetPolynomialVector(const double* point) const {
    Mesh::Iterator nearest = grid_points_->Nearest(point);
    int points_index = nearest.ToInteger();
    return points_[points_index];
}



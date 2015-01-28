#include "rogers_math/PolynomialPatch.hh"

typedef PolynomialVector::PolynomialCoefficient Coeff;

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

std::vector<Coeff> GetConstraints(
                                  size_t pos_dim,
                                  size_t value_dim,
                                  size_t poly_patch_order) {
    size_t n_coeffs = PolynomialVector::NumberOfPolynomialCoefficients(pos_dim, poly_patch_order*pos_dim+1);
    std::vector<Coeff> constraints;
    for (size_t i = 0; i < n_coeffs; ++i) {
        std::vector<int> index_by_vector = PolynomialVector::IndexByVector(i, pos_dim);
        std::vector<int> index_by_power = PolynomialVector::IndexByPower(i, pos_dim);
        // std::cerr << "Constraints " << i << " ** " << Coeff(index_by_vector, 99, 99.);
        for (size_t j = 0; j < index_by_power.size(); ++j)
            if (index_by_power[j] > poly_patch_order) {
                // std::cerr << " in" << std::endl;
                for (size_t value = 0; value < value_dim; ++value)
                    constraints.push_back(Coeff(index_by_vector, value, 0.));
                break;
            } else if (j+1 == index_by_power.size()) {
               // std::cerr << " out" << std::endl;
            }            
    }
    return constraints;
}

Coeff GetDeltaIterator(Mesh::Iterator it1, Mesh::Iterator it2, int valueIndex) {
    std::vector<int> state1 = it1.State();
    std::vector<int> state2 = it2.State();
    std::vector<int> index_by_vector;
    for (size_t i = 0; i < state1.size(); ++i) {
        for (int j = 0; j < abs(state2[i]-state1[i]); ++j)
            index_by_vector.push_back(i);
    }
    return Coeff(index_by_vector, valueIndex, 0.);
}

/*
Polypatch:
3 3 3 3
 o o o
2 2 2 3
 o o o
1 1 2 3
 o o o
1 1 2 3

PolyVector:
5

4 5

3 4 5

2 3 4 5

1 2 3 4 5
*/
PolynomialPatch* PolynomialPatch::LeastSquaresFit(
                                Mesh* points,
                                std::vector<std::vector<double> > values,
                                size_t poly_patch_order,
                                size_t smoothing_order) {
    if (points == NULL) {
        throw MAUS::Exception(MAUS::Exception::recoverable,
                          "NULL Mesh input to LeastSquaresFit",
                          "PolynomialPatch::LeastSquaresFit");
    }
    if (points->End().ToInteger() != values.size()) {
        throw MAUS::Exception(MAUS::Exception::recoverable,
                          "Mismatch between Mesh and values in LeastSquaresFit",
                          "PolynomialPatch::LeastSquaresFit");
    }
    if (smoothing_order >= poly_patch_order) {
        throw MAUS::Exception(MAUS::Exception::recoverable,
                "Smoothing order must be < polynomial_order in LeastSquaresFit",
                "PolynomialPatch::LeastSquaresFit");
    }
    Mesh* poly_mesh = points->Dual();
    if (poly_mesh == NULL)
        throw MAUS::Exception(MAUS::Exception::recoverable,
                          "Dual of Mesh was NULL",
                          "PolynomialPatch::LeastSquaresFit");
    unsigned int pos_dim = points->PositionDimension();
    unsigned int value_dim = values[0].size();
    Mesh::Iterator begin = points->Begin();
    Mesh::Iterator end = points->End()-1;
    std::vector<PolynomialVector*> polynomials;
    // get the list of points that are needed to make a given poly vector
    std::vector<std::vector<int> > nearby_points = GetNearbyPoints(pos_dim, poly_patch_order);
    size_t nearby_points_size = nearby_points.size();
    std::cerr << "Finding polynomial patch with pos_dim: " << pos_dim << " value_dim: " << value_dim << " poly_patch_order " << poly_patch_order << " nearby_points size: " << nearby_points.size() << " " << std::endl;
    std::vector<Coeff> zero_points = GetConstraints(pos_dim, value_dim, poly_patch_order);
    for (Mesh::Iterator it = poly_mesh->Begin(); it != poly_mesh->End(); ++it) {
        // find the set of points that can be used to make the polynomial
        std::vector< std::vector<double> > this_points;
        std::vector< std::vector<double> > this_values;
        // now iterate over the index_by_power_list elements - offset; each of
        // these makes a point in the polynomial fit
        std::cerr << it.ToInteger() << " " << std::flush;
  
        for (size_t i = 0; i < nearby_points_size; ++i) {
            Mesh::Iterator it_current = Mesh::Iterator(it.State(), points);
            // std::cerr << i << " " << std::flush;
            bool out_of_bounds = false; // element is off the edge of the mesh
            for (size_t j = 0; j < pos_dim; ++j) {
                it_current[j] += nearby_points[i][j];
                if (it_current[j] > end[j])
                    out_of_bounds = true;
            }
            if (out_of_bounds) { // if off the edge, then just constrain to zero
                std::vector<int> index_by_vector;
                for (size_t j = 0; j < value_dim; ++j)
                    zero_points.push_back(GetDeltaIterator(it, it_current, j));
                continue;
            } else { // else fit using values
                this_points.push_back(it_current.Position());
                this_values.push_back(values[it_current.ToInteger()]);
            }
            /*
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
            */
        }
        /*
        std::cerr << "Constraints" << std::endl;
        for (size_t i = 0; i < zero_points.size(); ++i)
            std::cerr << zero_points[i] << std::endl;
        */
        // The polynomial is found using least squares algorithm
        size_t poly_vector_order = pos_dim*poly_patch_order;
        //std::cerr << "Solving for polynomial of order " << poly_vector_order << std::endl;
        
        polynomials.push_back(PolynomialVector::ConstrainedPolynomialLeastSquaresFit(
            this_points, this_values, poly_vector_order+1, zero_points
        ));
        // std::cerr << polynomials.back()->GetCoefficientsAsMatrix();
    }
    return new PolynomialPatch(poly_mesh, polynomials);
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

void PolynomialPatch::NearbyPointsRecursive(std::vector<int> check, size_t check_index, size_t poly_power, std::vector<std::vector<int> >& nearby_points) {
    check[check_index] = poly_power;
    nearby_points.push_back(check);
    if (check_index+1 == check.size())
        return;
    for (int i = 1; i < poly_power; ++i)
        NearbyPointsRecursive(check, check_index+1, i, nearby_points);
    for (int i = 0; i <= poly_power; ++i) {
        check[check_index] = i;
        NearbyPointsRecursive(check, check_index+1, poly_power, nearby_points);
    }
}

std::vector<std::vector<int> > PolynomialPatch::GetNearbyPoints(int point_dim, int polynomial_order) {
    std::vector<std::vector<int> > nearby_points(1, std::vector<int>(point_dim, 0));
    for (size_t this_poly_order = 1; this_poly_order < polynomial_order+1; ++this_poly_order) 
        NearbyPointsRecursive(nearby_points[0], 0, this_poly_order, nearby_points);
    return nearby_points;
}


PolynomialVector* PolynomialPatch::GetPolynomialVector(const double* point) const {
    Mesh::Iterator nearest = grid_points_->Nearest(point);
    int points_index = nearest.ToInteger();
    return points_[points_index];
}



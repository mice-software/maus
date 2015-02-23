#include <vector>
#include <iostream>
#include <string>

#include "math/SolveFactory.hh"
#include "math/PolynomialPatch.hh"
#include "math/PPSolveFactory.hh"

typedef PolynomialVector::PolynomialCoefficient Coeff;

PPSolveFactory::PPSolveFactory(Mesh* points,
                               std::vector<std::vector<double> > values,
                               int poly_patch_order,
                               int smoothing_order) 
  :
    poly_patch_order_(poly_patch_order),
    smoothing_order_(smoothing_order),
    value_dim_(0),
    poly_mesh_(),
    points_(points),
    values_(values),
    polynomials_(),
    this_points_(),
    this_values_(),
    deriv_points_(),
    deriv_values_(),
    deriv_indices_(),
    edge_points_(),
    smoothing_points_() {
    if (points == NULL) {
        throw MAUS::Exception(MAUS::Exception::recoverable,
                          "NULL Mesh input to Solve",
                          "PPSolveFactory::Solve");
    }
    if (points->End().ToInteger() != int(values.size())) {
        throw MAUS::Exception(MAUS::Exception::recoverable,
                          "Mismatch between Mesh and values in Solve",
                          "PPSolveFactory::Solve");
    }
    if (smoothing_order < poly_patch_order) {
        throw MAUS::Exception(MAUS::Exception::recoverable,
                "Polynomial order must be <= smoothing order in Solve",
                "PPSolveFactory::Solve");
    }
    poly_mesh_ = points->Dual();
    if (poly_mesh_ == NULL)
        throw MAUS::Exception(MAUS::Exception::recoverable,
                          "Dual of Mesh was NULL",
                          "PPSolveFactory::Solve");
    value_dim_ = values[0].size();
    int pos_dim = points->PositionDimension();

    edge_points_ = std::vector< std::vector< std::vector<int> > >(pos_dim+1);
    for (int i = 1; i <= pos_dim; ++i)
        edge_points_[i] = GetNearbyPointsSquares(i, 0, smoothing_order-poly_patch_order);
    smoothing_points_ = GetNearbyPointsSquares(pos_dim, poly_patch_order_-1, poly_patch_order_);
}

std::vector<Coeff> PPSolveFactory::GetConstraints(
                                  size_t pos_dim,
                                  size_t value_dim,
                                  size_t poly_patch_order) {
    size_t n_coeffs = PolynomialVector::NumberOfPolynomialCoefficients(pos_dim, poly_patch_order*pos_dim+1);
    std::vector<Coeff> constraints;
    for (size_t i = 0; i < n_coeffs; ++i) {
        std::vector<int> index_by_vector = PolynomialVector::IndexByVector(i, pos_dim);
        std::vector<int> index_by_power = PolynomialVector::IndexByPower(i, pos_dim);
        for (size_t j = 0; j < index_by_power.size(); ++j)
            if (index_by_power[j] > int(poly_patch_order)) {
                for (size_t value = 0; value < value_dim; ++value)
                    constraints.push_back(Coeff(index_by_vector, value, 0.));
                break;
            } else if (j+1 == index_by_power.size()) {
            }            
    }
    return constraints;
}

Coeff PPSolveFactory::GetDeltaIterator(Mesh::Iterator it1, Mesh::Iterator it2, int valueIndex) {
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
PolynomialPatch* PPSolveFactory::LeastSquaresFit(
                                Mesh* points,
                                std::vector<std::vector<double> > values,
                                size_t poly_patch_order) {
    if (points == NULL) {
        throw MAUS::Exception(MAUS::Exception::recoverable,
                          "NULL Mesh input to LeastSquaresFit",
                          "PPSolveFactory::LeastSquaresFit");
    }
    if (points->End().ToInteger() != int(values.size())) {
        throw MAUS::Exception(MAUS::Exception::recoverable,
                          "Mismatch between Mesh and values in LeastSquaresFit",
                          "PPSolveFactory::LeastSquaresFit");
    }
    Mesh* poly_mesh = points->Dual();
    if (poly_mesh == NULL)
        throw MAUS::Exception(MAUS::Exception::recoverable,
                          "Dual of Mesh was NULL",
                          "PPSolveFactory::LeastSquaresFit");
    unsigned int pos_dim = points->PositionDimension();
    unsigned int value_dim = values[0].size();
    Mesh::Iterator begin = points->Begin();
    Mesh::Iterator end = points->End()-1;
    std::vector<PolynomialVector*> polynomials;
    // get the list of points that are needed to make a given poly vector
    std::vector<std::vector<int> > nearby_points = GetNearbyPointsSquares(pos_dim, 0, poly_patch_order);
    size_t nearby_points_size = nearby_points.size();
    std::vector<Coeff> zero_points = GetConstraints(pos_dim, value_dim, poly_patch_order);
    for (Mesh::Iterator it = poly_mesh->Begin(); it != poly_mesh->End(); ++it) {
        // find the set of points that can be used to make the polynomial
        this_points_ = std::vector< std::vector<double> >();
        this_values_ = std::vector< std::vector<double> >();
        // now iterate over the index_by_power_list elements - offset; each of
        // these makes a point in the polynomial fit
  
        for (size_t i = 0; i < nearby_points_size; ++i) {
            Mesh::Iterator it_current = Mesh::Iterator(it.State(), points);
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
                this_points_.push_back(it_current.Position());
                this_values_.push_back(values[it_current.ToInteger()]);
            }
        }
        // The polynomial is found using least squares algorithm
        size_t poly_vector_order = pos_dim*poly_patch_order;
        
        polynomials.push_back(PolynomialVector::ConstrainedPolynomialLeastSquaresFit(
            this_points_, this_values_, poly_vector_order+1, zero_points
        ));
    }
    return new PolynomialPatch(poly_mesh, polynomials);
}

std::vector<double> PPSolveFactory::OutOfBoundsPosition(Mesh::Iterator out_of_bounds_it) {
    const Mesh* mesh = out_of_bounds_it.GetMesh();
    std::vector<int> state = out_of_bounds_it.State();
    std::vector<int> delta = std::vector<int>(state.size(), 2);
    Mesh::Iterator end = mesh->End()-1;
    std::vector<double> pos_1 = mesh->Begin().Position();
    std::vector<double> pos_2 = Mesh::Iterator(delta, mesh).Position();
    for (size_t i = 0; i < state.size(); ++i)
        if (state[i] < 1) {
            state[i] = 1;
        } else if (state[i] > end[i]) { 
            state[i] = end[i];
        }
    std::vector<double> position = Mesh::Iterator(state, mesh).Position();
    state = out_of_bounds_it.State();
    for (size_t i = 0; i < state.size(); ++i)
        if (state[i] < 1) {
            position[i] = (pos_2[i] - pos_1[i])*(state[i]-1) + position[i];
        } else if (state[i] > end[i]) { 
            position[i] = (pos_2[i] - pos_1[i])*(state[i]-end[i]) + position[i];
        }
    return position;
}

void PPSolveFactory::GetPoints() {
    int pos_dim = points_->PositionDimension();
    std::vector< std::vector<int> > data_points = GetNearbyPointsSquares(pos_dim, -1, poly_patch_order_);
    this_points_ = std::vector< std::vector<double> >(data_points.size());
    // now iterate over the index_by_power_list elements - offset; each of
    // these makes a point in the polynomial fit
    for (size_t i = 0; i < data_points.size(); ++i) {
        this_points_[i] = std::vector<double>(pos_dim);
        for (int j = 0; j < pos_dim; ++j)
            this_points_[i][j] = 0.5-data_points[i][j];
     }
}

void PPSolveFactory::GetValues(Mesh::Iterator it) {
    this_values_ = std::vector< std::vector<double> >();
    int pos_dim = it.State().size();
    std::vector< std::vector<int> > data_points = GetNearbyPointsSquares(pos_dim, -1, poly_patch_order_);
    size_t data_points_size = data_points.size();
    Mesh::Iterator end = points_->End()-1;
    // now iterate over the index_by_power_list elements - offset; each of
    // these makes a point in the polynomial fit
    for (size_t i = 0; i < data_points_size; ++i) {
        std::vector<int> it_state = it.State();
        for (int j = 0; j < pos_dim; ++j)
            it_state[j]++;
        Mesh::Iterator it_current = Mesh::Iterator(it_state, points_);
        bool out_of_bounds = false; // element is off the edge of the mesh
        for (int j = 0; j < pos_dim; ++j) {
            it_current[j] -= data_points[i][j];
            out_of_bounds = out_of_bounds ||
                            it_current[j] < 1 ||
                            it_current[j] > end[j];
        }
        if (out_of_bounds) { // if off the edge, then just constrain to zero
            this_values_.push_back(values_[it.ToInteger()]);
        } else { // else fit using values
            this_values_.push_back(values_[it_current.ToInteger()]);
        }
     }
}

void PPSolveFactory::GetDerivPoints() {
    deriv_points_ = std::vector< std::vector<double> >();
    deriv_indices_ = std::vector< std::vector<int> >();
    int pos_dim = points_->PositionDimension();
    // get the outer layer of points
    int delta_order = smoothing_order_ - poly_patch_order_;
    if (delta_order <= 0)
        return;
    for (size_t i = 0; i < smoothing_points_.size(); ++i) {
        // make a list of the axes that are on the edge of the space
        std::vector<int> equal_axes;
        for (size_t j = 0; j < smoothing_points_[i].size(); ++j)
            if (smoothing_points_[i][j] == poly_patch_order_)
                equal_axes.push_back(j);
        std::vector<std::vector<int> > edge_points = edge_points_[equal_axes.size()];
        for (size_t j = 0; j < edge_points.size(); ++j) { // note the first point, 0,0, is ignored
            deriv_indices_.push_back(std::vector<int>(pos_dim));
            deriv_points_.push_back(std::vector<double>(pos_dim));
            for (size_t k = 0; k < smoothing_points_[i].size(); ++k)
                deriv_points_.back()[k] += smoothing_points_[i][k];               
            for (size_t k = 0; k < edge_points[j].size(); ++k) {
                deriv_indices_.back()[equal_axes[k]] += edge_points[j][k];
                deriv_points_.back()[equal_axes[k]] += edge_points[j][k];               
            }
        }
    }
}

void PPSolveFactory::GetDerivs(Mesh::Iterator it) {
    deriv_values_ = std::vector< std::vector<double> >(deriv_points_.size());

    std::vector<double> it_pos = it.Position();

    int pos_dim = it.State().size();
    // get the outer layer of points
    Mesh::Iterator end = it.GetMesh()->End()-1;
    int delta_order = smoothing_order_ - poly_patch_order_;
    if (delta_order <= 0)
        return;
    for (size_t i = 0; i < deriv_points_.size() &&  i < deriv_indices_.size(); ++i) {
        std::vector<double> point = deriv_points_[i];
        deriv_values_[i] = std::vector<double>(value_dim_);
        for (int j = 0; j < pos_dim; ++j)
            point[j] += it_pos[j];
        Mesh::Iterator nearest = poly_mesh_->Nearest(&point[0]);
        if (polynomials_[nearest.ToInteger()] == NULL) {
            deriv_values_[i] = std::vector<double>(value_dim_, 0.);
        } else {
            for (int j = 0; j < pos_dim; ++j)
                point[j] -= nearest.Position()[j];
            polynomials_[nearest.ToInteger()]->FDeriv(&point[0], &deriv_indices_[i][0], &deriv_values_[i][0]);
        }
    }
}

PolynomialPatch* PPSolveFactory::Solve() {
    Mesh::Iterator begin = points_->Begin();
    Mesh::Iterator end = points_->End()-1;
    int mesh_size = poly_mesh_->End().ToInteger();
    polynomials_ = std::vector<PolynomialVector*>(mesh_size, NULL);
    // get the list of points that are needed to make a given poly vector
    GetPoints();
    GetDerivPoints();
    SolveFactory solver(poly_patch_order_,
                        smoothing_order_,
                        poly_mesh_->PositionDimension(),
                        value_dim_,
                        this_points_,
                        deriv_points_,
                        deriv_indices_);
    for (Mesh::Iterator it = poly_mesh_->End()-1; it >= poly_mesh_->Begin(); --it) {
        // find the set of points that can be used to make the polynomial
        GetValues(it);
        // The polynomial is found using simultaneous equation solve
        GetDerivs(it);
        polynomials_[it.ToInteger()] = solver.PolynomialSolve(
                this_points_, this_values_,
                deriv_points_, deriv_values_, deriv_indices_
              );
    }
    return new PolynomialPatch(poly_mesh_, polynomials_);
}

void PPSolveFactory::NearbyPointsRecursive(std::vector<int> check, size_t check_index, size_t poly_power, std::vector<std::vector<int> >& nearby_points) {
    check[check_index] = poly_power;
    nearby_points.push_back(check);
    if (check_index+1 == check.size())
        return;
    for (int i = 1; i < int(poly_power); ++i)
        NearbyPointsRecursive(check, check_index+1, i, nearby_points);
    for (int i = 0; i <= int(poly_power); ++i) {
        check[check_index] = i;
        NearbyPointsRecursive(check, check_index+1, poly_power, nearby_points);
    }
}

std::vector<std::vector<int> > PPSolveFactory::GetNearbyPointsSquares(int point_dim, int poly_order_lower, int poly_order_upper) {
    if (point_dim < 1)
        throw(MAUS::Exception(MAUS::Exception::recoverable, "Point dimension must be > 0", "PPSolveFactory::GetNearbyPointsSquares"));
    if (poly_order_lower > poly_order_upper)
        throw(MAUS::Exception(MAUS::Exception::recoverable, "Polynomial lower bound must be <= polynomial upper bound", "PPSolveFactory::GetNearbyPointsSquares"));
    // poly_order -1 (or less) means no terms
    if (poly_order_upper == poly_order_lower || poly_order_upper < 0)
        return std::vector<std::vector<int> >();
    // poly_order 0 means constant term
    std::vector<std::vector<int> > nearby_points(1, std::vector<int>(point_dim, 0));
    // poly_order > 0 means corresponding poly terms (1 is linear, 2 is quadratic...)
    for (size_t this_poly_order = 0; this_poly_order < size_t(poly_order_upper); ++this_poly_order) 
        NearbyPointsRecursive(nearby_points[0], 0, this_poly_order+1, nearby_points);
    if (poly_order_lower < 0)
        return nearby_points; // no terms in lower bound
    int n_points_lower = 1;
    for (int dim = 0; dim < point_dim; ++dim)
        n_points_lower *= poly_order_lower+1;
    nearby_points.erase(nearby_points.begin(), nearby_points.begin()+n_points_lower);
    return nearby_points;
}

std::vector<std::vector<int> > PPSolveFactory::GetNearbyPointsTriangles(int point_dim, int poly_order_lower, int poly_order_upper) {
    PolynomialVector temp(0, MMatrix<double>());
    int poly0 = temp.NumberOfPolynomialCoefficients(point_dim, poly_order_lower);
    int poly1 = temp.NumberOfPolynomialCoefficients(point_dim, poly_order_upper);
    std::vector< std::vector<int> > nearby_points;
    for (size_t i = poly0; i < size_t(poly1); ++i) {
        nearby_points.push_back(PolynomialVector::IndexByPower(i, point_dim));
    }
    return nearby_points;
}


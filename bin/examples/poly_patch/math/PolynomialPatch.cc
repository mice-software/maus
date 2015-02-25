#include "math/SolveFactory.hh"
#include "math/PolynomialPatch.hh"

PolynomialPatch::PolynomialPatch(Mesh* grid_points,
                    std::vector<SquarePolynomialVector*> polynomials) {
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
    if (int(points_.size()) != end.ToInteger()) {
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
    std::vector<double> point_temp(point_dimension_);
    std::vector<double> nearest_pos = nearest.Position();
    for (size_t i = 0; i < point_dimension_; ++i)
        point_temp[i] = point[i] - nearest_pos[i];
    int points_index = nearest.ToInteger();
    points_[points_index]->F(&point_temp[0], value);
}

SquarePolynomialVector* PolynomialPatch::GetPolynomialVector(const double* point) const {
    Mesh::Iterator nearest = grid_points_->Nearest(point);
    int points_index = nearest.ToInteger();
    return points_[points_index];
}



//This file is a part of G4MICE
//
//G4MICE is free software: you can redistribute it and/or modify
//it under the terms of the GNU General Public License as published by
//the Free Software Foundation, either version 3 of the License, or
//(at your option) any later version.
//
//G4MICE is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//GNU General Public License for more details.
//
//You should have received a copy of the GNU General Public License
//along with G4MICE in the doc folder.  If not, see 
//<http://www.gnu.org/licenses/>.
//

#include "gtest/gtest.h" 

#include <math.h>

#include "src/common/Interface/TriangularMesh.hh"

namespace {

class TriangularMeshTest : public ::testing::Test {
protected:
  TriangularMeshTest() {}
  virtual ~TriangularMeshTest() {}
  virtual void  SetUp();
  virtual void  TearDown();
  TriangularMesh* mesh_random;  //random bunch of points
  TriangularMesh* mesh_kite;    //kite shape (inside the usual corners)
  TriangularMesh* mesh_corners; //corners only
  double grid_size;
  double grid_min[2];
  double grid_max[2];

};

void TriangularMeshTest::SetUp() {
  grid_size = 2;
  grid_min[0] = -grid_size; grid_min[1] = -grid_size;
  grid_max[0] =  grid_size; grid_max[1] =  grid_size;

  double * random[50];
  for(int i=0; i<50; i++) {
    random[i]    = new double[2];
    random[i][0] = sin(1000.*i);
    random[i][1] = cos(2000.*i);
  }
  mesh_random = new TriangularMesh(50, 2, &random[0], &grid_min[0], &grid_max[0]);

  double   kite_d[8] = {1,0, -1,0, 0,0.01, 0,-0.01};
  double * kite[4];
  for(int i=0; i<4; i++) {
    kite[i]    = new double[2];
    kite[i][0] = kite_d[i*2];
    kite[i][1] = kite_d[i*2+1];
  }
  mesh_kite = new TriangularMesh(4, 2, &kite[0], &grid_min[0], &grid_max[0]);

  mesh_corners = new TriangularMesh(0, 2, NULL, &grid_min[0], &grid_max[0]);
}


void TriangularMeshTest::TearDown() {
  delete mesh_random;
  delete mesh_kite;
  delete mesh_corners;
}


TEST_F(TriangularMeshTest, DelaunayConstructorTest) {
  //Test that we can build and delete a mesh with a reasonable number of points without any problems
  //We should end up with a diamond split along short diagonal - that's the whole point
  TriangularMesh* mesh2 = mesh_kite;
  std::list<TriangularMesh::Simplex*> triangles      = mesh2->GetSimplices();
  for( std::list<TriangularMesh::Simplex*>::iterator it=triangles.begin(); it!=triangles.end(); it++) { //there should be no Simplex with both (1,0) and (-1,0)
    bool hasPos=false;
    bool hasNeg=false;
    for(int j=0; j<3; j++) {
      if( fabs((*it)->MyPoint(j).coords[0]-1.)<1e-3 ) hasPos = true;
      if( fabs((*it)->MyPoint(j).coords[0]+1.)<1e-3 ) hasNeg = true;
    }
    EXPECT_TRUE(!(hasPos&&hasNeg));
  }


  TriangularMesh* mesh3 = mesh_corners;
  triangles      = mesh3->GetSimplices();
  EXPECT_TRUE(triangles.size()==2);
  for( std::list<TriangularMesh::Simplex*>::iterator it=triangles.begin(); it!=triangles.end(); it++) {//there should be no Simplex with both (1,0) and (-1,0)
    for(int i=0; i<3; i++) {
      EXPECT_DOUBLE_EQ(2.,fabs((*it)->MyPoint(i).coords[0]));
      EXPECT_DOUBLE_EQ(2.,fabs((*it)->MyPoint(i).coords[1]));
    }
  }
}

} //namespace






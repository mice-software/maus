#include <fstream>
#include <iostream>
#include <iomanip>

#include "gtest/gtest.h"

#include "Interface/Mesh.hh"

void           Test2DGrid      (TwoDGrid&   v_mesh, std::ostream& out, bool constSpacing);
bool           Test3DGrid      (ThreeDGrid& v_mesh, std::ostream& out, bool constSpacing);
bool           TestNDGrid      (NDGrid&     v_mesh, std::ostream& out, bool constSpacing);
bool           TestMeshIterator(Mesh& mesh, std::ostream& out);

void Test2DGrid(TwoDGrid& v_mesh, std::ostream& out, bool constSpacing)
{
    bool testpass = true;
    const TwoDGrid c_mesh = v_mesh;
    std::vector<double> xVec = v_mesh.xVector();
    std::vector<double> yVec = v_mesh.yVector();
    double *            xArr = v_mesh.newXArray();
    double *            yArr = v_mesh.newYArray();
    EXPECT_EQ(int(xVec.size()), c_mesh.xSize());
    EXPECT_EQ(int(yVec.size()), c_mesh.ySize());

    for(unsigned int i=0; i<xVec.size(); i++)
    {
        EXPECT_LT(fabs(xVec[i] - v_mesh.x(i+1)), 1e-9);
        EXPECT_LT(fabs(xVec[i] - c_mesh.x(i+1)), 1e-9);
        EXPECT_LT(fabs(xArr[i] - v_mesh.x(i+1)), 1e-9);
        for(unsigned int j=0; j<yVec.size(); j++)
        {
            EXPECT_LT(fabs(yVec[j] - v_mesh.y(j+1)), 1e-9);
            EXPECT_LT(fabs(yVec[j] - c_mesh.y(j+1)), 1e-9);
            EXPECT_LT(fabs(yArr[j] - v_mesh.y(j+1)), 1e-9);
        }
    }
    out << " val " <<  int(testpass) << " ";
    
    double x    = xVec[0] - (xVec[1] - xVec[0])/2.;
    double y    = yVec[0] - (yVec[1] - yVec[0])/2.;
    int    xInd = -2;
    int    yInd = -2;
    for(unsigned int i=1; i<xVec.size(); i++)
    {
        x = xVec[i-1] + (xVec[i] - xVec[i-1])/2.;
        c_mesh.xLowerBound(x, xInd);
        EXPECT_EQ(xInd, int(i-1));
        for(unsigned int j=1; j<yVec.size(); j++)
        {
            y = yVec[j-1] + (yVec[j] - yVec[j-1])/2.;
            c_mesh.yLowerBound(y, yInd);
            EXPECT_EQ(yInd, int(j-1));
            c_mesh.LowerBound(x,xInd,y,yInd);
            EXPECT_EQ(xInd, int(i-1));
            EXPECT_EQ(yInd, int(j-1));
        }
    }
    x = xVec.back() + 1.;
    c_mesh.xLowerBound(x, xInd);
    EXPECT_EQ(xInd, int(xVec.size()-1));

    y = yVec.back() + 1.;
    c_mesh.yLowerBound(y, yInd);
    EXPECT_EQ(yInd, int(yVec.size()-1));
    c_mesh.LowerBound(x,xInd,y,yInd);
    EXPECT_EQ(xInd, int(xVec.size()-1));
    EXPECT_EQ(yInd, int(yVec.size()-1));

    x = xVec.front() - 1.;
    c_mesh.xLowerBound(x, xInd);
    EXPECT_EQ(xInd, -1);

    y = yVec.front() - 1.;
    c_mesh.yLowerBound(y, yInd);
    EXPECT_EQ(yInd, -1);
    c_mesh.LowerBound(x,xInd,y,yInd);
    EXPECT_EQ(xInd, -1); 
    EXPECT_EQ(yInd, -1);
    out << " lower_bound " << int(testpass) << " "; 

    EXPECT_EQ(c_mesh.Begin().ToInteger(), 0);
    EXPECT_EQ(c_mesh.End().ToInteger(), int(xVec.size() * yVec.size())); // one past the end
    std::vector<double> pos(2);
    std::vector<double> c_pos(2);
    int itIndex = 0;
    for(Mesh::Iterator it = c_mesh.Begin(); it != c_mesh.End(); it++)
    {
        c_mesh.Position(it, &pos[0]);
        EXPECT_EQ(c_mesh.ToInteger(it), itIndex++);
        EXPECT_EQ(c_mesh.x(it[0]), pos[0]);
        EXPECT_EQ(c_mesh.y(it[1]), pos[1]);
    }
    std::vector<int> st(2,0);
    for(st[0]=1; st[0]<int(xVec.size()); st[0]++)
        for(st[1]=1; st[1]<int(yVec.size()); st[1]++)
        {
            Mesh::Iterator it(st, &c_mesh);
            c_mesh.CentrePosition(it, &c_pos[0]);
            EXPECT_LT((xVec[st[0]-1]+xVec[st[0]])/2. - c_pos[0], 1e-9);
            EXPECT_LT(fabs((yVec[st[1]-1]+yVec[st[1]])/2. - c_pos[1]), 1e-9);
        }
    for(Mesh::Iterator it = c_mesh.Begin(); it != c_mesh.End(); it++)
    {
        c_mesh.Position(it, &pos[0]);
        c_pos[0] = pos[0] - 1e-5; c_pos[1] = pos[1] - 1e-5;
        EXPECT_EQ(c_mesh.Nearest(&c_pos[0]), it);
        c_pos[0] = pos[0] - 1e-5; c_pos[1] = pos[1] + 1e-5;
        EXPECT_EQ(c_mesh.Nearest(&c_pos[0]), it);
        c_pos[0] = pos[0] + 1e-5; c_pos[1] = pos[1] - 1e-5;
        EXPECT_EQ(c_mesh.Nearest(&c_pos[0]), it);
        c_pos[0] = pos[0] + 1e-5; c_pos[1] = pos[1] + 1e-5;
        EXPECT_EQ(c_mesh.Nearest(&c_pos[0]), it);
    }
    c_pos[0] = 1e6; c_pos[1] = 1e6;
    EXPECT_LT(c_mesh.Nearest(&c_pos[0]).ToInteger(), c_mesh.End().ToInteger());
    EXPECT_GE(c_mesh.Nearest(&c_pos[0]).ToInteger(), 0);
    c_pos[0] = -1e6; 
    EXPECT_LT(c_mesh.Nearest(&c_pos[0]).ToInteger(), c_mesh.End().ToInteger());
    EXPECT_GE(c_mesh.Nearest(&c_pos[0]).ToInteger(), 0);
    c_pos[1] = -1e6; 
    EXPECT_LT(c_mesh.Nearest(&c_pos[0]).ToInteger(), c_mesh.End().ToInteger());
    EXPECT_GE(c_mesh.Nearest(&c_pos[0]).ToInteger(), 0);
    c_pos[0] =  1e6; 
    EXPECT_LT(c_mesh.Nearest(&c_pos[0]).ToInteger(), c_mesh.End().ToInteger());
    EXPECT_GE(c_mesh.Nearest(&c_pos[0]).ToInteger(), 0);

    out << " iter " << int(testpass) << " ";

    EXPECT_EQ(c_mesh.MinX(), xVec.front());
    EXPECT_EQ(c_mesh.MaxX(), xVec.back ());
    EXPECT_EQ(c_mesh.MinY(), yVec.front());
    EXPECT_EQ(c_mesh.MaxY(), yVec.back ());

    v_mesh.SetX(xVec.size(), xArr);
    v_mesh.SetY(yVec.size(), yArr);
    for(unsigned int i=0; i<xVec.size(); i++)
        EXPECT_LT(fabs(xVec[i] - v_mesh.x(i+1)), 1e-9);
    for(unsigned int i=0; i<yVec.size(); i++)
        EXPECT_LT(fabs(yVec[i] - v_mesh.y(i+1)), 1e-9);
    EXPECT_EQ(c_mesh.PositionDimension(), 2);
    EXPECT_EQ(c_mesh.GetConstantSpacing(), constSpacing);
    v_mesh.SetConstantSpacing();
    EXPECT_EQ(v_mesh.GetConstantSpacing(), constSpacing);
    v_mesh.SetConstantSpacing(!constSpacing);
    EXPECT_NE(v_mesh.GetConstantSpacing(), constSpacing);
    out << " misc " << int(testpass) << " " ;

    delete [] xArr;
    delete [] yArr;
/* NOT TESTED:
    void    Add(VectorMap* map); //add *map if it has not already been added
    void    Remove(VectorMap* map); //remove *map if it exists; delete this if there are no more VectorMaps
*/

}

bool Test3DGrid(ThreeDGrid& v_mesh, std::ostream& out, bool constSpacing)
{
    bool testpass = true;
    const ThreeDGrid& c_mesh = v_mesh;
    std::vector<double> xVec = v_mesh.xVector();
    std::vector<double> yVec = v_mesh.yVector();
    std::vector<double> zVec = v_mesh.zVector();
    double *            xArr = v_mesh.newXArray();
    double *            yArr = v_mesh.newYArray();
    double *            zArr = v_mesh.newZArray();
    testpass &= int(xVec.size()) == c_mesh.xSize();
    testpass &= int(yVec.size()) == c_mesh.ySize();
    testpass &= int(zVec.size()) == c_mesh.zSize();
    out << " setup " << int(testpass) << " ";

    for(unsigned int i=0; i<xVec.size(); i++)
    {
        testpass &= fabs(xVec[i] - v_mesh.x(i+1)) < 1e-9;
        testpass &= fabs(xVec[i] - c_mesh.x(i+1)) < 1e-9;
        testpass &= fabs(xArr[i] - v_mesh.x(i+1)) < 1e-9;
        for(unsigned int j=0; j<yVec.size(); j++)
        {
            testpass &= fabs(yVec[j] - v_mesh.y(j+1)) < 1e-9;
            testpass &= fabs(yVec[j] - c_mesh.y(j+1)) < 1e-9;
            testpass &= fabs(yArr[j] - v_mesh.y(j+1)) < 1e-9;
            for(unsigned int k=0; k<zVec.size(); k++)
            {
                testpass &= fabs(zVec[j] - v_mesh.z(j+1)) < 1e-9;
                testpass &= fabs(zVec[j] - c_mesh.z(j+1)) < 1e-9;
                testpass &= fabs(zArr[j] - v_mesh.z(j+1)) < 1e-9;

            }
          }
    }
    out << " val " <<  int(testpass) << " ";
    double x, y, z;
    int    xInd, yInd, zInd;
    for(unsigned int i=1; i<xVec.size(); i++)
    {
        x = xVec[i-1] + (xVec[i] - xVec[i-1])/2.;
        c_mesh.xLowerBound(x, xInd);
        testpass &= (xInd == int(i-1));
        for(unsigned int j=1; j<yVec.size(); j++)
        {
            y = yVec[j-1] + (yVec[j] - yVec[j-1])/2.;
            c_mesh.yLowerBound(y, yInd);
            testpass &= (yInd == int(j-1));
            for(unsigned int k=1; k<zVec.size(); k++)
            {
                z = zVec[k-1] + (zVec[k] - zVec[k-1])/2.;
                c_mesh.zLowerBound(z, zInd);
                testpass &= (zInd == int(k-1));
                c_mesh.LowerBound(x,xInd,y,yInd,z,zInd);
                testpass &= (xInd == int(i-1)) && (yInd == int(j-1)) && (zInd == int(k-1));
            }
        }
    }
    x = xVec.back() + 1.;
    c_mesh.xLowerBound(x, xInd);
    testpass &= (xInd == int(xVec.size()-1));

    y = yVec.back() + 1.;
    c_mesh.yLowerBound(y, yInd);
    testpass &= (yInd == int(yVec.size()-1));

    z = zVec.back() + 1.;
    c_mesh.zLowerBound(z, zInd);
    testpass &= (zInd == int(zVec.size()-1));

    c_mesh.LowerBound(x,xInd,y,yInd,z,zInd);
    testpass &= (xInd == int(xVec.size()-1)) && (yInd == int(yVec.size()-1)) && (zInd == int(zVec.size()-1));

    x = xVec.front() - 1.;
    c_mesh.xLowerBound(x, xInd);
    testpass &= (xInd == -1);

    y = yVec.front() - 1.;
    c_mesh.yLowerBound(y, yInd);
    testpass &= (yInd == -1);

    z = zVec.front() - 1.;
    c_mesh.zLowerBound(z, zInd);
    testpass &= (zInd == -1);

    c_mesh.LowerBound(x,xInd,y,yInd,z,zInd);
    testpass &= (xInd == -1) && (yInd == -1) && (zInd == -1);
    out << " lower_bound " << int(testpass) << " "; 

    testpass &= c_mesh.Begin().ToInteger() == 0;
    testpass &= c_mesh.End().ToInteger() == int(xVec.size() * yVec.size()*zVec.size()); // one past the end
    std::vector<double> pos(3);
    std::vector<double> c_pos(3);
    int itIndex = 0;
    for(Mesh::Iterator it = c_mesh.Begin(); it != c_mesh.End(); it++)
    {
        c_mesh.Position(it, &pos[0]);
        testpass &= (c_mesh.ToInteger(it) == itIndex++);
        testpass &= fabs(c_mesh.x(it[0])-pos[0])<1e-9 && fabs(c_mesh.y(it[1])-pos[1])<1e-9 && fabs(c_mesh.z(it[2])-pos[2])<1e-9;
    }
    for(Mesh::Iterator it = c_mesh.Begin(); it != c_mesh.End(); it++)
    {
        c_mesh.Position(it, &pos[0]);
        c_pos[0] = pos[0] - 1e-5; c_pos[1] = pos[1] - 1e-5;  c_pos[2] = pos[2] - 1e-5;
        testpass &= c_mesh.Nearest(&c_pos[0]) == it;
        c_pos[0] = pos[0] + 1e-5; c_pos[1] = pos[1] - 1e-5;  c_pos[2] = pos[2] - 1e-5;
        testpass &= c_mesh.Nearest(&c_pos[0]) == it;
        c_pos[0] = pos[0] - 1e-5; c_pos[1] = pos[1] + 1e-5;  c_pos[2] = pos[2] - 1e-5;
        testpass &= c_mesh.Nearest(&c_pos[0]) == it;
        c_pos[0] = pos[0] - 1e-5; c_pos[1] = pos[1] - 1e-5;  c_pos[2] = pos[2] + 1e-5;
        testpass &= c_mesh.Nearest(&c_pos[0]) == it;
        c_pos[0] = pos[0] - 1e-5; c_pos[1] = pos[1] + 1e-5;  c_pos[2] = pos[2] + 1e-5;
        testpass &= c_mesh.Nearest(&c_pos[0]) == it;
        c_pos[0] = pos[0] + 1e-5; c_pos[1] = pos[1] - 1e-5;  c_pos[2] = pos[2] + 1e-5;
        testpass &= c_mesh.Nearest(&c_pos[0]) == it;
        c_pos[0] = pos[0] + 1e-5; c_pos[1] = pos[1] + 1e-5;  c_pos[2] = pos[2] - 1e-5;
        testpass &= c_mesh.Nearest(&c_pos[0]) == it;
        c_pos[0] = pos[0] + 1e-5; c_pos[1] = pos[1] + 1e-5;  c_pos[2] = pos[2] + 1e-5;
        testpass &= c_mesh.Nearest(&c_pos[0]) == it;
    }
    c_pos[0] = 1e6; c_pos[1] = 1e6;
    testpass &= c_mesh.Nearest(&c_pos[0]).ToInteger() < c_mesh.End().ToInteger() && c_mesh.Nearest(&c_pos[0]).ToInteger() >= 0;
    c_pos[0] = -1e6; 
    testpass &= c_mesh.Nearest(&c_pos[0]).ToInteger() < c_mesh.End().ToInteger() && c_mesh.Nearest(&c_pos[0]).ToInteger() >= 0;
    c_pos[1] = -1e6; 
    testpass &= c_mesh.Nearest(&c_pos[0]).ToInteger() < c_mesh.End().ToInteger() && c_mesh.Nearest(&c_pos[0]).ToInteger() >= 0;
    c_pos[0] =  1e6; 
    testpass &= c_mesh.Nearest(&c_pos[0]).ToInteger() < c_mesh.End().ToInteger() && c_mesh.Nearest(&c_pos[0]).ToInteger() >= 0;

    out << " iter " << int(testpass) << " ";

    testpass &= (c_mesh.MinX() == xVec.front());
    testpass &= (c_mesh.MaxX() == xVec.back ());
    testpass &= (c_mesh.MinY() == yVec.front());
    testpass &= (c_mesh.MaxY() == yVec.back ());
    testpass &= (c_mesh.MinZ() == zVec.front());
    testpass &= (c_mesh.MaxZ() == zVec.back ());

    v_mesh.SetX(xVec.size(), xArr);
    v_mesh.SetY(yVec.size(), yArr);
    v_mesh.SetZ(zVec.size(), zArr);
    for(unsigned int i=0; i<xVec.size(); i++) testpass &= fabs(xVec[i] - v_mesh.x(i+1)) < 1e-9;
    for(unsigned int i=0; i<yVec.size(); i++) testpass &= fabs(yVec[i] - v_mesh.y(i+1)) < 1e-9;
    for(unsigned int i=0; i<zVec.size(); i++) testpass &= fabs(zVec[i] - v_mesh.z(i+1)) < 1e-9;
    testpass &= c_mesh.PositionDimension() == 3;
    testpass &= c_mesh.GetConstantSpacing() == constSpacing;
    v_mesh.SetConstantSpacing();
    testpass &= v_mesh.GetConstantSpacing() == constSpacing;
    v_mesh.SetConstantSpacing(!constSpacing);
    testpass &= v_mesh.GetConstantSpacing() != constSpacing;
    out << " misc " << int(testpass) << " " ;

    delete [] xArr;
    delete [] yArr;
    delete [] zArr;
    return testpass;

/* NOT TESTED:
    void    Add(VectorMap* map); //add *map if it has not already been added
    void    Remove(VectorMap* map); //remove *map if it exists; delete this if there are no more VectorMaps
*/
}

bool TestNDGrid(NDGrid& v_mesh, std::ostream& out, bool constSpacing)
{
    bool testpass = true;
    const NDGrid c_mesh = v_mesh;
    std::vector< std::vector<double> > pointVec;
    double ** pointArr = new double*[v_mesh.PositionDimension()];
    for(int i=0; i<v_mesh.PositionDimension(); i++)
    {
        pointVec.push_back(c_mesh.coordVector(i));
        pointArr[i] = c_mesh.newCoordArray(i);
        testpass &= int(pointVec[i].size()) == c_mesh.size(i);
    }
    out << " setup " << int(testpass) << " ";

    for(unsigned int dim=0; dim<pointVec.size(); dim++)
        for(int i=0; i<c_mesh.size(dim); i++)
        {
            testpass &= fabs(pointVec[dim][i] - v_mesh.coord(i+1, dim)) < 1e-9;
            testpass &= fabs(pointArr[dim][i] - v_mesh.coord(i+1, dim)) < 1e-9;
            testpass &= fabs(pointVec[dim][i] - c_mesh.coord(i+1, dim)) < 1e-9;
        }
    out << " val " <<  int(testpass) << " ";

    testpass &= c_mesh.Begin().ToInteger() == 0;
    int meshSize = 1;
    for(int i=0; i<v_mesh.PositionDimension(); i++) meshSize *= v_mesh.size(i);
    testpass &= c_mesh.End().ToInteger() == meshSize; // one past the end
    int itIndex = 0;
    std::vector<double> pos(v_mesh.PositionDimension(), 0.);
    for(Mesh::Iterator it = c_mesh.Begin(); it != c_mesh.End(); it++)
    {
        c_mesh.Position(it, &pos[0]);
        testpass &= (c_mesh.ToInteger(it) == itIndex++);
        for(int i=0; i<v_mesh.PositionDimension(); i++) testpass &= fabs(c_mesh.coord(it[i], i)-pos[i])<1e-9;
    }
    for(Mesh::Iterator it = c_mesh.Begin(); it < c_mesh.End(); it++)
    {
        //TEST NEAREST
        std::vector<double> pos0;
        for(int i=0; i<c_mesh.PositionDimension(); i++)
        {
            c_mesh.Position(it, &pos[0]);
            pos[i] = it.Position()[i]-1e-5;
            testpass &= v_mesh.Nearest(&pos[0]) == it;
            pos[i] = it.Position()[i]+1e-5;
            testpass &= v_mesh.Nearest(&pos[0]) == it;
        }
    }
    pos[0] = 1e6; pos[1] = 1e6;
    testpass &= c_mesh.Nearest(&pos[0]).ToInteger() < c_mesh.End().ToInteger() && c_mesh.Nearest(&pos[0]).ToInteger() >= 0;
    pos[0] = -1e6; 
    testpass &= c_mesh.Nearest(&pos[0]).ToInteger() < c_mesh.End().ToInteger() && c_mesh.Nearest(&pos[0]).ToInteger() >= 0;
    pos[1] = -1e6; 
    testpass &= c_mesh.Nearest(&pos[0]).ToInteger() < c_mesh.End().ToInteger() && c_mesh.Nearest(&pos[0]).ToInteger() >= 0;
    pos[0] =  1e6; 
    testpass &= c_mesh.Nearest(&pos[0]).ToInteger() < c_mesh.End().ToInteger() && c_mesh.Nearest(&pos[0]).ToInteger() >= 0;
    out << " iter " << int(testpass) << " ";

    std::vector<double> x(v_mesh.PositionDimension(), 0.);
    std::vector<int>    index(v_mesh.PositionDimension(), 0);
    for(Mesh::Iterator it=v_mesh.Begin(); it<v_mesh.End(); it++)
    {
        for(int dim=0; dim<v_mesh.PositionDimension(); dim++)
            x[dim] = (v_mesh.coord(it[dim], dim) + 1e-5);
        c_mesh.LowerBound(x, index);
        for(int dim=0; dim<v_mesh.PositionDimension(); dim++)
        {
            testpass &= index[dim] == it[dim]-1;
            c_mesh.coordLowerBound(x[dim], dim, index[dim]);
            testpass &= index[dim] == it[dim]-1;
        }
    }
    x = c_mesh.Begin().Position();
    for(int dim=0; dim<v_mesh.PositionDimension(); dim++)
    {
        x[dim]-=1e-5;
        c_mesh.coordLowerBound(x[dim], dim, index[dim]);
        testpass &= (index[dim] == -1);
    }
    c_mesh.LowerBound(x, index);
    for(int dim=0; dim<v_mesh.PositionDimension(); dim++) testpass &= index[dim] == -1;
    out << " lower_bound " << int(testpass) << " " << std::flush; 

    for(int i=0; i<c_mesh.PositionDimension(); i++) 
    {
        testpass &= (c_mesh.Min(i) == pointVec[i].front()) && (c_mesh.Max(i) == pointVec[i].back());
        v_mesh.SetCoord(i, pointVec[i].size(), pointArr[i]);
        for(unsigned int j=0; j<pointVec[i].size(); j++) 
            testpass &= fabs(pointVec[i][j] - v_mesh.coord(j+1, i)) < 1e-9;
    }
    testpass &= c_mesh.GetConstantSpacing() == constSpacing;
    v_mesh.SetConstantSpacing();
    testpass &= v_mesh.GetConstantSpacing() == constSpacing;
    v_mesh.SetConstantSpacing(!constSpacing);
    testpass &= v_mesh.GetConstantSpacing() != constSpacing;
    out << " misc " << int(testpass) << " " ;

    for(int i=0; i<c_mesh.PositionDimension(); i++)
      delete [] pointArr[i];
    delete [] pointArr;

    return testpass;
}



bool TestMeshIterator(Mesh& mesh, std::ostream& out)
{
    Mesh::Iterator it = mesh.Begin();
    out << "Testing mesh of dimension " << mesh.PositionDimension() << " " << std::endl;
    bool lessThan   = true;
    bool moreThan   = true;
    bool lessThanEq = true;
    bool moreThanEq = true;
    bool equal      = true;
    bool notEqual   = true;
    bool toInteger  = true;
    
    out << std::setw(30) << "mesh.Begin() "               << mesh.Begin()              << std::endl; 
    it = mesh.Begin(); it++;
    out << std::setw(30) << "mesh.Begin()++ "             << it                        << std::endl;
    out << std::setw(30) << "mesh.Begin()+1 "             << mesh.Begin()+1            << std::endl;
    it = mesh.Begin(); it+=1;
    out << std::setw(30) << "mesh.Begin()+=1 "            << it                        << std::endl;
    out << std::setw(30) << "mesh.Begin()+mesh.Begin() "  << mesh.Begin()+mesh.Begin() << std::endl;
    it = mesh.Begin()+10;
    out << std::setw(30) << "mesh.Begin()+(m.Begin()+10)" << mesh.Begin()+it           << std::endl;
    out << std::setw(30) << "mesh.Begin()- -10 "          << mesh.Begin()- (-10)       << std::endl;
    out << std::setw(30) << "mesh.Begin()+0 "             << mesh.Begin()+0            << std::endl;
    out << std::setw(30) << "mesh.Begin()+1 "             << mesh.Begin()+1            << std::endl;
    out << std::setw(30) << "mesh.Begin()+8 "             << mesh.Begin()+8            << std::endl;
    out << std::setw(30) << "mesh.Begin()+10 "            << mesh.Begin()+10           << std::endl;
    out << std::setw(30) << "mesh.Begin()+mesh.End().Int-1 " << mesh.Begin()+(mesh.End().ToInteger()-1) << std::endl;
    out << std::endl;

//    out << std::setw(30) << "mesh.End() "                 << mesh.End()              << std::endl;
    it = mesh.End(); it--;
    out << std::setw(30) << "mesh.End()-- "               << it                      << std::endl;
    out << std::setw(30) << "mesh.End()-1 "               << mesh.End()-1            << std::endl;
    it = mesh.End(); it-=1;
    out << std::setw(30) << "mesh.End()-=1 "              << it                      << std::endl;
    it = mesh.End(); it--;
    out << std::setw(30) << "(mesh.End()--)-mesh.Begin() "<< it-mesh.Begin()         << std::endl;
    it = mesh.Begin()+10;
    out << std::setw(30) << "mesh.End()-(m.Begin()+10)"   << mesh.End()-it           << std::endl;
    out << std::setw(30) << "mesh.End()+ -10 "            << mesh.End()+ (-10)       << std::endl;
    it = mesh.End(); it--;
    out << std::setw(30) << "(mesh.End()--)-0 "           << it-0                    << std::endl;
    out << std::setw(30) << "mesh.End()-1 "               << mesh.End()-1            << std::endl;
    out << std::setw(30) << "mesh.End()-9 "               << mesh.End()-9            << std::endl;
    out << std::setw(30) << "mesh.End()-10 "              << mesh.End()-10           << std::endl;
    out << std::setw(30) << "mesh.End()-mesh.End().Int "  << mesh.End()-mesh.End().ToInteger() << std::endl;

    int  integerOld = -1;
    for(Mesh::Iterator it = mesh.Begin(); it<=mesh.End(); it++)
    {
        toInteger &= (it.ToInteger() == integerOld+1);
        integerOld = it.ToInteger();
        out << integerOld << " ";
    }
    out << "\n" << std::endl;
    
    integerOld = (mesh.End()+1).ToInteger();
    for(Mesh::Iterator it = mesh.End(); it>=mesh.Begin(); it--)
    {
        toInteger &= (it.ToInteger() == integerOld-1);
        integerOld = it.ToInteger();
        out << integerOld << " ";
    }
    out << "\n" << std::endl;

    for(Mesh::Iterator it = mesh.Begin(); it<=mesh.End(); it++)
    {
        for(Mesh::Iterator comp = mesh.Begin(); comp<it; comp++) 
        {
            // note we are testing the operator here...
            EXPECT_TRUE(comp < it);
            EXPECT_TRUE(comp <= it);
            EXPECT_TRUE(comp != it);
            EXPECT_FALSE(comp > it);
            EXPECT_FALSE(comp >= it);
            EXPECT_FALSE(comp == it);
        }
        Mesh::Iterator comp = it;
        EXPECT_FALSE(comp < it);
        EXPECT_TRUE(comp <= it);
        EXPECT_FALSE(comp != it);
        EXPECT_FALSE(comp > it);
        EXPECT_TRUE(comp >= it);
        EXPECT_TRUE(comp == it);
        for(Mesh::Iterator comp = mesh.End(); comp!=it; comp--) 
        {
            EXPECT_FALSE(comp < it);
            EXPECT_FALSE(comp <= it);
            EXPECT_TRUE(comp != it);
            EXPECT_TRUE(comp > it);
            EXPECT_TRUE(comp >= it);
            EXPECT_FALSE(comp == it);
        }
    }
    out <<"lt "<<int(lessThan)<<" mt "<<int(moreThan)<< " lte "<< int(lessThanEq)<<" mte "<< int(moreThanEq)<<" eq "<<int(equal)<<" neq "<<int(notEqual)
        <<" 2int "<<int(toInteger)<<std::endl;

    out << std::endl << std::endl;
    return lessThan && moreThan && lessThanEq && moreThanEq && equal && notEqual && toInteger;
}

TEST(MeshTest, testAll) {
    std::ofstream out;
    double min   [3] = {1, 2, 3};
    double delta [3] = {4, 5, 6};
    int    number[3] = {4, 8, 12};

    std::vector< std::vector<double> > coordinatesConst(3);
    double** coordinatesConstArray = new double*[3];
    for(int i=0; i<3; i++) {
        coordinatesConstArray[i] = new double[number[i]];
        for(int j=0; j<number[i]; j++) {
            coordinatesConst[i].push_back(min[i] + delta[i]*j);
            coordinatesConstArray[i][j] = min[i] + delta[i]*j;
        }
    }

    std::vector< std::vector<double> > coordinatesVar(3);
    for(int i=0; i<3; i++)
        for(int j=0; j<number[i]; j++)
            coordinatesVar[i].push_back( min[i] + delta[i]*j*j);

    TwoDGrid *   const2DGrid1 = new TwoDGrid(delta[0], delta[1], min[0], min[1], number[0], number[1]);
    TwoDGrid *   const2DGrid2 = new TwoDGrid(
                                          number[0], coordinatesConstArray[0],
                                          number[1], coordinatesConstArray[1]
                                    );
    TwoDGrid *   const2DGrid3 = new TwoDGrid(coordinatesConst[0], coordinatesConst[1]);
    TwoDGrid *   var2DGrid1   = new TwoDGrid(coordinatesVar[0],   coordinatesVar[1]  );

    out << "\n\n***********   2d Const 1 **********" << std::endl;
    Test2DGrid(*const2DGrid1, out, true);
    EXPECT_TRUE(TestMeshIterator(*const2DGrid1, out));
    out << "\n\n***********   2d Const 2 **********" << std::endl;
    Test2DGrid(*const2DGrid2, out, true);
    EXPECT_TRUE(TestMeshIterator(*const2DGrid2, out));
    out << "\n\n***********   2d Const 3 ********** " << std::endl;
    Test2DGrid(*const2DGrid3, out, true);
    EXPECT_TRUE(TestMeshIterator(*const2DGrid3, out));
    out << "\n\n***********   2d_Var_1 **********" << std::endl;
    Test2DGrid(*var2DGrid1, out, false);
    EXPECT_TRUE(TestMeshIterator(*var2DGrid1,   out));
    delete const2DGrid1;
    delete const2DGrid2;
    delete const2DGrid3;
    delete var2DGrid1;
    // other memory is cleaned by the TwoDGrid();
    delete [] coordinatesConstArray[2];
    delete [] coordinatesConstArray;

    ThreeDGrid * const3DGrid1 = new ThreeDGrid(delta[0], delta[1], delta[2], min[0], min[1], min[2], number[0], number[1], number[2]);
    ThreeDGrid * const3DGrid2 = new ThreeDGrid(coordinatesConst[0].size(), &coordinatesConst[0][0], 
                                               coordinatesConst[1].size(), &coordinatesConst[1][0], coordinatesConst[2].size(), &coordinatesConst[2][0]);
    ThreeDGrid * const3DGrid3 = new ThreeDGrid(coordinatesConst[0], coordinatesConst[1], coordinatesConst[2]);
    ThreeDGrid * var3DGrid1   = new ThreeDGrid(coordinatesVar[0],   coordinatesVar[1],   coordinatesVar[2]  );

    out << "\n\n******** 3d_Const_1 ********* " << std::endl;
    EXPECT_TRUE(Test3DGrid(*const3DGrid1, out, true));
    EXPECT_TRUE(TestMeshIterator(*const3DGrid1, out));
    out << "\n\n******** 3d_Const_2 ********* " << std::endl;
    EXPECT_TRUE(Test3DGrid(*const3DGrid2, out, true));
    EXPECT_TRUE(TestMeshIterator(*const3DGrid2, out));
    out << "\n\n******** 3d_Const_3 ********* " << std::endl;
    EXPECT_TRUE(Test3DGrid(*const3DGrid3, out, true));
    EXPECT_TRUE(TestMeshIterator(*const3DGrid3, out));
    out << "\n\n******** 3d_Var_1 *********** " << std::endl;
    EXPECT_TRUE(Test3DGrid(*var3DGrid1, out, false));
    EXPECT_TRUE(TestMeshIterator(*var3DGrid1,    out));
    delete const3DGrid1;
    delete const3DGrid2;
    delete const3DGrid3;
    delete var3DGrid1; 

    NDGrid * constNDGrid1 = new NDGrid(coordinatesConst);
    NDGrid * constNDGrid2 = new NDGrid(3, number, delta, min);
    NDGrid * varNDGrid1   = new NDGrid(coordinatesVar);
    out << "\n\n********* Nd_Const_1 ********" << std::endl;
    EXPECT_TRUE(TestNDGrid(*constNDGrid1, out, true));
    EXPECT_TRUE(TestMeshIterator(*constNDGrid1, out));
    out << "\n\n********* Nd_Const_2 ********" << std::endl;
    EXPECT_TRUE(TestNDGrid(*constNDGrid2, out, true));
    EXPECT_TRUE(TestMeshIterator(*constNDGrid2, out));
    out << "\n\n********* Nd_Var_1   ********" << std::endl;
    EXPECT_TRUE(TestNDGrid(*varNDGrid1, out, false));
    EXPECT_TRUE(TestMeshIterator(*varNDGrid1,    out));
    delete constNDGrid1;
    delete constNDGrid2;
    delete varNDGrid1; 
}



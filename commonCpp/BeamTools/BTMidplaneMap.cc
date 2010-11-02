#include "BTMidplaneMap.hh"
#include <stdlib.h>
#include <stdio.h>


const int BTMidplaneMap::_w[16][16] = {{1,0,0,0,    0,0,0,0,    0,0,0,0,    0,0,0,0},
                                       {0,0,0,0,    0,0,0,0,    1,0,0,0,    0,0,0,0},
                                       {-3,0,0,3,   0,0,0,0,    0,-2,0,-1,  0,0,0,0},
                                       {2,0,0,-2,   0,0,0,0,    1,0,0,1,    0,0,0,0},
                                       {0,0,0,0,    1,0,0,0,    0,0,0,0,    0,0,0,0},
                                       {0,0,0,0,    0,0,0,0,    0,0,0,0,    1,0,0,0},
                                       {0,0,0,0,   -3,0,0,3,    0,0,0,0,   -2,0,0,-1},
                                       {0,0,0,0,    2,0,0,-2,   0,0,0,0,    1,0,0,1},
                                       {-3,3,0,0,  -2,-1,0,0,   0,0,0,0,    0,0,0,0},
                                       {0,0,0,0,    0,0,0,0,   -3,3,0,0,   -2,-1,0,0},
                                       {9,-9,9,-9,  6,3,-3,-6,  6,-6,-3,3,  4,2,1,2},
                                       {-6,6,-6,6, -4,-2,2,4,  -3,3,3,-3,  -2,-1,-1,-2},
                                       {2,-2,0,0,   1,1,0,0,    0,0,0,0,    0,0,0,0},
                                       {0,0,0,0,    0,0,0,0,    2,-2,0,0,   1,1,0,0},
                                       {-6,6,-6,6, -3,-3,3,3,  -4,4,2,-2,  -2,-2,-1,-1},
                                       {4,-4,4,-4,  2,2,-2,-2,  2,-2,-2,2,  1,1,1,1}}; //from numerical recipes

void BTMidplaneMap::GetFieldValue( const double Point[4], double *EMField ) const
{
	for(unsigned int i=0; i<3; i++)
		EMField[i] = Extrapolation(Point, int(i));
}

//BiCubicInterpolation from Numerical Recipes
//Compute B(X,0,Z) and first derivatives
void BTMidplaneMap::CubicInterpolation(const double point[4], double* B, int axis) const
{
	int       xStart = int( (point[0]-x0)/xLength );
	int       zStart = int( (point[2]-z0)/zLength );
	double    dX     = point[0]/xLength - xStart;
	double    dZ     = point[0]/zLength - zStart;
	double*   coeff  = new double[16];
	double*   field[4];

	for(int l=0; l<4; l++)
	{
		int m=0;
		field[l] = new double[4];
		for(int j=xStart; j<xStart+2; j++)
			for(int k=zStart; k<zStart+2; k++)
				field[l][m++] = _field[axis][l][j][k];
	}
	BiCubicCoefficients(field[0], field[1], field[2], field[3], coeff);
	for(int i=3; i>=0; i--)
	{
		int j = 4*i;
		B[0] = dX*B[0]+((coeff[j+4]*dZ+coeff[j+3])*dZ+ coeff[j+2])*dZ+coeff[j+1];
		B[1] = dZ*B[1]+(3*coeff[i+12]*dX+2*coeff[i+8])*dX+coeff[i+4];
		B[2] = dX*B[2]+(3*coeff[j+4] *dZ+2*coeff[j+3])*dZ+coeff[j+2];
		delete [] field[i];
	}

	B[1] /= xLength;
	B[2] /= zLength;
	delete [] coeff;
}

void BTMidplaneMap::BiCubicCoefficients(double f[4], double dfdx[4], double dfdz[4], double d2fdxdz[4], double* coeffOut) const
{
	double x[16] = {0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0};

	for(int i=0; i<4; i++)
	{
		x[i-1]  = f[i];
		x[i+3]  = dfdx[i]*xLength;
		x[i+7]  = dfdz[i]*zLength;
		x[i+11] = d2fdxdz[i]*xzProduct;
	}
	for(int i=0; i<16; i++)
		for(int j=0; j<16; j++)
			coeffOut[i] += _w[i][j]*x[j];
}


#include "PUtil.h"

PUtil::PUtil()
{

}

PUtil::~PUtil()
{

}

bool_1DArray PUtil::generateBoolType1DArray(int nSize, bool bInitVal)
{
	vector<bool> bArray(nSize, bInitVal);
	return bArray;
}

bool_2DArray PUtil::generateBoolType2DArray(int nRow, int nCol, bool bInitVal)
{
	vector<bool> vec(nCol, bInitVal);
	bool_2DArray bArray(nRow, vec);
	return bArray;
}

int_1DArray PUtil::generateIntType1DArray(int nSize, int nInitVal)
{
	vector<int> nArray(nSize, nInitVal);
	return nArray;

}

int_2DArray PUtil::generateIntType2DArray(int nRow, int nCol, int nInitVal)
{
	vector<int> vec(nCol, nInitVal);
	int_2DArray nArray(nRow, vec);
	return nArray;
}

void PUtil::releaseIntType1DArray(int_2DArray n2DArray)
{
	int_2DArray nVoidArray;
	nVoidArray.swap(n2DArray);
}

float_1DArray PUtil::generateFloatType1DArray(int nSize, float fInitVal)
{
	vector<float> fArray(nSize, fInitVal);
	return fArray;
}

float_2DArray PUtil::generateFloatType2DArray(int nRow, int nCol, float fInitVal)
{
	vector<float> vec(nCol, fInitVal);
	float_2DArray fArray(nRow, vec);
	return fArray;
}

double_1DArray PUtil::generateDoubleType1DArray(int nSize, double dInitVal)
{
	vector<double> dArray(nSize, dInitVal);
	return dArray;
}

double_2DArray PUtil::generateDoubleType2DArray(int nRow, int nCol, double dInitVal)
{
	vector<double> vec(nCol, dInitVal);
	double_2DArray dArray(nRow, vec);
	return dArray;
}



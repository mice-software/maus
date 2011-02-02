//RFFieldMap
//Revamped pill box class to use new slick phasing

#include "BTRFFieldMap.hh"

std::vector<RFFieldMap*> BTRFFieldMap::_fieldMapList = std::vector<RFFieldMap*>();


BTRFFieldMap::BTRFFieldMap(double frequency, double length, double energyGain, std::string phasingModel, 
                           std::string fileName, std::string fileType)
             : BTPillBox(frequency, length, energyGain, phasingModel), _fieldMap(SetFieldMap(fileName, fileType))
{
  BTField::bbMin = _fieldMap->BoundingBoxMin();
  BTField::bbMax = _fieldMap->BoundingBoxMax();
}

BTRFFieldMap::BTRFFieldMap(double frequency, double length, double peakEField, double timeDelay,
                           std::string fileName, std::string fileType)
            : BTPillBox(frequency, length, peakEField, timeDelay), _fieldMap(SetFieldMap(fileName, fileType))
{
  BTField::bbMin = _fieldMap->BoundingBoxMin();
  BTField::bbMax = _fieldMap->BoundingBoxMax();
}


void BTRFFieldMap::GetPeakFields( const  double Point[4], double *Bfield ) const
{
	_fieldMap->GetFieldValue(Point, Bfield);

}

void BTRFFieldMap::Print(std::ostream & out) const
{
	out << "RFFieldMap ";
	PrintPrivates(out);
}

RFFieldMap* BTRFFieldMap::SetFieldMap(std::string fileName, std::string fileType)
{
	for(unsigned int i=0; i<_fieldMapList.size(); i++)
		if(_fieldMapList[i]->GetFileName() == fileName)
			return _fieldMapList[i];
	_fieldMapList.push_back(new RFFieldMap(fileName, fileType));
	return _fieldMapList.back();
}


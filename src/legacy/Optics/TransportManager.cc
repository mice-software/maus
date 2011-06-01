#include "TransportManager.hh"

TransportManager::TransportManager(std::string rootModuleName) 
                : _rootModule(new MiceModule(rootModuleName)), _model(_rootModule)
{
	SetDaughterMaterials(_rootModule);
	SortMaterials();
	_mapCalc.SetOpticsModel(_model);
}

TransportManager::TransportManager(OpticsModel model, std::string rootModuleName) 
                : _rootModule(new MiceModule(rootModuleName)), _model(model)
{
	SetDaughterMaterials(_rootModule);
	SortMaterials();
	_mapCalc.SetOpticsModel(_model);
}

TransportManager::TransportManager(OpticsModel model) 
                : _rootModule(NULL), _model(model)
{
	_mapCalc.SetOpticsModel(_model);
}

TransportManager::~TransportManager()
{
	if(_rootModule!=NULL) delete _rootModule;
	_rootModule = NULL;	
}

void TransportManager::SetTransport(PhaseSpaceVector referenceParticleIn, double zOut, int order)
{
	int materialIndex    = 0;
	_startMaterial       = 0;
	_map                 = vector<TransferMap>();
	PhaseSpaceVector ref = referenceParticleIn;

	if(_material.size()>0)
	{
		SortMaterials();
		while(_material[materialIndex].GetZ() < ref.z() && materialIndex<(int)_material.size())
			materialIndex++;
		_startMaterial = materialIndex;
		while(_material[materialIndex].GetZ() < zOut && materialIndex<(int)_material.size())
		{
			_map.push_back(_mapCalc.GetTransferMap(ref, _material[materialIndex].GetZ(), order));
			ref = _map.back().GetReferenceOut();
			ref = _material[materialIndex].Transport(ref);
			materialIndex++;
		}
	}
	_map.push_back(_mapCalc.GetTransferMap(ref, zOut, order));
}

void TransportManager::SortMaterials()
{
	for(unsigned int i=0; i<_material.size(); i++)
	{
		bool sorted = false;
		while(!sorted)
		{
			sorted = true;
			for(unsigned int j=0; j<_material.size() ; j++)
				if(_material[j].GetZ() > _material[i].GetZ())
				{
					Material temp = _material[i];
					_material[i]  = _material[j];
					_material[j]  = temp;
					sorted        = false;
				}
		}
	}
}


CovarianceMatrix TransportManager::Transport(const CovarianceMatrix covIn)
{
	CovarianceMatrix cov = covIn;
	for(int i=0; i<(int)_map.size()-1; i++)
	{
		cov  = _map[i]*cov;
		cov  = _material[i+_startMaterial].Transport(cov);
	}
	cov = _map.back()*cov;
	return cov;
}

PhaseSpaceVector TransportManager::Transport(const PhaseSpaceVector psIn)
{
	PhaseSpaceVector ps = psIn;
	_model.SetFields(ps);
	for(int i=0; i<(int)_map.size()-1; i++)
	{
		ps  = _map[i]*ps;
		ps  = _material[i+_startMaterial].Transport(ps);
	}
	ps = _map.back()*ps;
	return ps;
}

PhaseSpaceVector TransportManager::SlowTransport(PhaseSpaceVector psIn)
{
	PhaseSpaceVector ps = psIn;
	_model.SetFields(ps);
	double zOut = _map.back().GetReferenceOut().z();
	for(int i=0; i<(int)_map.size()-1; i++)
	{
		ps = _mapCalc.SlowTransport(ps, _material[i+_startMaterial].GetZ());
		ps = _material[i+_startMaterial].Transport(ps);
	}
	ps = _mapCalc.SlowTransport(ps, zOut);
	return ps;

}

PhaseSpaceVector TransportManager::SlowTransportNoMaterial(PhaseSpaceVector psIn)
{
	PhaseSpaceVector ps = psIn;
	_model.SetFields(ps);
	double zOut = _map.back().GetReferenceOut().z();
	ps = _mapCalc.SlowTransport(ps, zOut);
	return ps;
}

CovarianceMatrix TransportManager::TransportBack(const CovarianceMatrix covIn)
{
	CovarianceMatrix cov = covIn;
	for(int i=(int)_map.size()-1; i>0; i--)
	{
		cov  = _map[i].Inv()*cov;
		cov  = _material[i+_startMaterial-1].TransportBack(cov);
	}
	cov = _map[0].Inv()*cov;
	return cov;
}

PhaseSpaceVector TransportManager::TransportBack(const PhaseSpaceVector psIn)
{
	PhaseSpaceVector ps = psIn;
	_model.SetFields(ps);
	for(int i=(int)_map.size()-1; i>0; i--)
	{
		ps  = _map[i].Inv()*ps;
		ps  = _material[i+_startMaterial-1].TransportBack(ps);
	}
	ps = _map[0].Inv()*ps;
	return ps;
}

PhaseSpaceVector TransportManager::SlowTransportBack(PhaseSpaceVector psIn)
{
	PhaseSpaceVector ps = psIn;
	_model.SetFields(ps);
	double zIn = _map[0].GetReferenceIn().z();
	for(int i=(int)_map.size()-1; i>0; i--)
	{
		ps = _material[i+_startMaterial-1].TransportBack(ps);
		ps = _mapCalc.SlowTransportBack(ps, _material[i+_startMaterial-1].GetZ());
	}
	ps = _mapCalc.SlowTransportBack(ps, zIn);
	return ps;
}

PhaseSpaceVector TransportManager::SlowTransportBackNoMaterial(PhaseSpaceVector psIn)
{
	PhaseSpaceVector ps = psIn;
	_model.SetFields(ps);
	double zIn = _map[0].GetReferenceIn().z();
	ps = _mapCalc.SlowTransportBack(ps, zIn);
	return ps;
}

void TransportManager::SetDaughterMaterials(MiceModule * parent)
{
	
	std::vector<MiceModule*> daughters=parent->allDaughters();
	for(unsigned int i=0; i<daughters.size(); i++)
	{
		MiceModule* mod = daughters[i];
		if(mod->volType()=="Cylinder" || mod->volType() == "Box" 
		   || mod->propertyExistsThis("OpticsMaterialLength", "double"))
			if(mod->propertyString("Material")!="Galactic")
			{
				Material newMat = BuildMaterial(mod);
				if(newMat.GetThickness()>0)
					_material.push_back(BuildMaterial(mod));
			}
		if(mod->propertyExistsThis("UseDaughtersInOptics","bool"))
		{ 
			if(mod->propertyBool("UseDaughtersInOptics")) SetDaughterMaterials(mod);
		}
		else
			SetDaughterMaterials(mod);
	}
}

Material TransportManager::BuildMaterial(MiceModule * module)
{
	double      position = module->globalPosition()[2];
	std::string material = module->propertyString("Material");
	double      length = 0;
	if(module->propertyExistsThis( "OpticsMaterialLength", "double"))
		length = module->propertyDouble("OpticsMaterialLength");
	else if(module->volType()=="Box")   
		length = module->dimensions()[2];
	else if(module->volType()=="Cylinder")  
		length = module->dimensions()[1];
	return Material(length, material, position);

}


void TransportManager::PrintMaterial(std::ostream& out) 
{
	for(int i=0; i<(int)_material.size(); i++) 
		_material[i].Print(out);
}
	
void TransportManager::PrintMaps(std::ostream& out)
{
	for(int i=0; i<(int)_map.size(); i++)  
		out << _map[i];
}

void TransportManager::PrintRotatedMaps(std::ostream& out, double angle)
{
	for(int i=0; i<(int)_map.size(); i++)  
		out << _map[i].Rotate(angle);
}

double TransportManager::Get2DTrace(int axis) const
{
	CLHEP::HepMatrix fullMatrix(6,6,1);
	for(int i=0; i<(int)_map.size(); i++)  
		fullMatrix = fullMatrix*_map[i].GetFirstOrderMap();
	double trace = fullMatrix.sub(axis*2,axis*2+1,axis*2,axis*2+1).trace();
	return trace;	
}

double TransportManager::Get4DTrace() const
{
	CLHEP::HepMatrix fullMatrix(6,6,1);
	for(int i=0; i<(int)_map.size(); i++)  
		fullMatrix = fullMatrix*_map[i].GetFirstOrderMap();
	double trace = (fullMatrix.sub(3,6,3,6)).trace();
	return trace;
}

double TransportManager::Get6DTrace() const
{
	CLHEP::HepMatrix fullMatrix(6,6,1);
	for(int i=0; i<(int)_map.size(); i++)  
		fullMatrix = fullMatrix*_map[i].GetFirstOrderMap();
	double trace = fullMatrix.trace();
	return trace;
}

double TransportManager::GetLarmorAngle()
{
	double angle = 0;
	for(unsigned int i=0; i<_map.size(); i++)
	{
		PhaseSpaceVector refIn  = _map[i].GetReferenceIn();
		double           zOut   = _map[i].GetReferenceOut().z();
		angle                  += _mapCalc.GetLarmorAngle(refIn, zOut);
	}
	return angle;
}

std::vector<TransferMap> TransportManager::GetDecoupledTransferMaps() const
{

	std::vector<TransferMap> decoupledMaps;
	for(unsigned int i=0; i<_map.size(); i++)
	{
		PhaseSpaceVector refIn = _map[i].GetReferenceIn();
		double           zOut  = _map[i].GetReferenceOut().z();
		double           angle = _mapCalc.GetLarmorAngle(refIn, zOut);
		decoupledMaps.push_back(_map[i].Rotate(angle));
	}
	return decoupledMaps;
}

CovarianceMatrix TransportManager::Decompose(double em_t, double em_x, double em_y) const
{
	std::vector<TransferMap> decoupledMaps = GetDecoupledTransferMaps();
	TransferMap              parentMap     = decoupledMaps[0];
	double                   angle         = 0;
	for(unsigned int i=1; i<decoupledMaps.size(); i++)
	{
		parentMap = decoupledMaps[i]*parentMap;
		PhaseSpaceVector refIn = _map[i].GetReferenceIn();
		double           zOut  = _map[i].GetReferenceOut().z();
		angle                 += _mapCalc.GetLarmorAngle(refIn, zOut);
	}
	return parentMap.Decompose(em_t, em_x, em_y).Rotate(-angle);
}


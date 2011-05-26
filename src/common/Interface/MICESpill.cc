// MICESpill.cc
// Spill data
// V.Verguilov 30 January 2009

#include "Interface/MICESpill.hh"


MICESpill::MICESpill(){
		this->Clear();
}

MICESpill::~MICESpill(){
	this->Clear();
}

MICEEvent * MICESpill::GetEvent( int index ){
	return events[ index ];
}

VmeScalerData * MICESpill::GetScaler( int index ){
	return scalers[ index ];
}

int MICESpill::CreateEvents( int N ){
	for( int i = 0; i < N; ++i ){
		MICEEvent * tmp = new MICEEvent();
		this->AddEvent( tmp );
	}
	return events.size();
}

int MICESpill::AddEvent( MICEEvent * aEvent ){
	events.push_back( aEvent );
	return 1;
}

int MICESpill::AddScalerData( VmeScalerData * aScaler ){
	scalers.push_back( aScaler );
	return 1;
}

bool MICESpill::IsValid(){
	VmeScalerData *scaler;
	bool isDone = false;
	bool retVal = false;
	for( unsigned int i=0; ( ( i < scalers.size() ) && !isDone ); ++i ){
		scaler =	GetScaler( i );
		if( scaler->channel() == 1 ){
			isDone = true;
			if( scaler->GetData() == (int)events.size() ){
				std::cout << "Spill is valid!" << std::endl;
				retVal = true;
			} else std::cout << "Spill is NOT valid!" << std::endl;
		}
	}
	return retVal;
}

void MICESpill::Print(){
	std::cout << "This spill contains:" << std::endl;
	std::cout << events.size() << " events." << std::endl;
	std::cout << scalers.size() << " scalers." << std::endl;
	std::cout << "Particle Events: " << std::endl;
}

void MICESpill::Clear(){
	for( unsigned int i = 0; i < events.size(); ++i ) delete events[ i ];
	events.resize( 0 );
	for( unsigned int i = 0; i < scalers.size(); ++i ) delete scalers[ i ];
	scalers.resize( 0 );
}


// MICESpill.hh
// Spill data
// V.Verguilov 30 January 2009

#ifndef MICE_SPILL_HH
#define MICE_SPILL_HH

#include <vector>
#include <map>
#include "dataCards.hh"
#include "MICEEvent.hh"
#include "VmeScalerData.hh"

typedef std::vector<VmeScalerData*> TScalersVector;

class MICESpill
{
	public:
		MICESpill();
		~MICESpill();
		
		// Returns the number of valid (particle) events in the spill
		unsigned int GetNumberOfPartEvents(){ return events.size(); };
		
		
		// Returns a pointer to MICEEvent class which corresponds to the i-th particle event in the spill
		MICEEvent * GetEvent( int index );
		
		// Returns the data in specific channel of the scaler
		VmeScalerData * GetScaler( int index ); 
		TScalersVector GetScalers(){ return scalers; };
		int CreateEvents( int N );
		int AddEvent( MICEEvent * aEvent );
		int AddScalerData( VmeScalerData * Scaler );
		bool IsValid();
		void Clear();
		void Print();
		
	private:
		std::vector< MICEEvent * > events;
		TScalersVector scalers;
		
};


#endif

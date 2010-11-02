#include "StripSpecialHits.hh"
#include "Interface/SpecialHit.hh"

void  stripSpecialHits( MICEEvent& event )
{
  /// This removes and deletes a special hit if it is not in the track ref planes
  for( unsigned int i = 0; i < event.specialHits.size(); )
  {
    if( ( event.specialHits[i]->GetStationNo() == 9001 ||
        event.specialHits[i]->GetStationNo() == 9002 ) == false )
    {
      delete event.specialHits[i];
      event.specialHits.erase(event.specialHits.begin()+i);
    }
    else
     i++;
  }
}

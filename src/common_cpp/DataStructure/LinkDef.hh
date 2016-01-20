/* This file is part of MAUS: http://micewww.pp.rl.ac.uk/projects/maus
 *
 * MAUS is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * MAUS is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with MAUS.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifdef __CINT__

#pragma link off all globals;
#pragma link off all classes;
#pragma link off all functions;

#pragma link C++ class MAUS::Spill+;
#pragma link C++ class MAUS::MAUSEvent<MAUS::Spill>+;
#pragma link C++ class MAUS::Data+;

#pragma link C++ class MAUS::JobHeader+;
#pragma link C++ class MAUS::MAUSEvent<MAUS::JobHeader>+;
#pragma link C++ class MAUS::JobHeaderData+;

#pragma link C++ class MAUS::JobFooter+;
#pragma link C++ class MAUS::MAUSEvent<MAUS::JobFooter>+;
#pragma link C++ class MAUS::JobFooterData+;

#pragma link C++ class MAUS::Image+;
#pragma link C++ class MAUS::CanvasWrapper+;
#pragma link C++ class MAUS::CanvasWrapperArray+;
#pragma link C++ class std::vector<MAUS::CanvasWrapper>;
#pragma link C++ class MAUS::MAUSEvent<MAUS::Image>+;
#pragma link C++ class MAUS::ImageData+;

#pragma link C++ class MAUS::ThreeVector+;
#pragma link C++ class std::vector<int>;

#pragma link C++ class MAUS::DateTime+;

#pragma link C++ class MAUS::RunHeader+;
#pragma link C++ class MAUS::MAUSEvent<MAUS::RunHeader>+;
#pragma link C++ class MAUS::RunHeaderData+;

#pragma link C++ class MAUS::RunFooter+;
#pragma link C++ class MAUS::MAUSEvent<MAUS::RunFooter>+;
#pragma link C++ class MAUS::RunFooterData+;

#pragma link C++ class std::map<std::string, std::string>;
#pragma link C++ class std::pair<std::string, std::string>;
#pragma link C++ class std::vector<MCEvent*>;
#pragma link C++ class std::vector<ReconEvent*>;
#pragma link C++ class MAUS::Scalars+;
#pragma link C++ class MAUS::EMRSpillData+;
#pragma link C++ class MAUS::DAQData+;
#pragma link C++ class std::vector<KLDaq*>;
#pragma link C++ class std::vector<CkovDaq*>;
#pragma link C++ class std::vector<TOFDaq*>;
#pragma link C++ class std::vector<Trigger*>;
#pragma link C++ class std::vector<TOFDaq*>;
#pragma link C++ class std::vector<TOFDaq*>;
#pragma link C++ class std::vector<TriggerRequest*>;
#pragma link C++ class std::vector<Unknown*>;
#pragma link C++ class std::vector<Tag*>;
#pragma link C++ class std::vector<MAUS::TrackerDaq*>;
#pragma link C++ class std::vector<MAUS::TrackerDaq*>;

#pragma link C++ class MAUS::TestBranch+;
#pragma link C++ class MAUS::TestChild+;
#pragma link C++ class std::vector<MAUS::TestChild*>;

#pragma link C++ class MAUS::TOFEvent+;
#pragma link C++ class MAUS::SciFiEvent+;
#pragma link C++ class MAUS::EMREvent+;
#pragma link C++ class MAUS::KLEvent+;
#pragma link C++ class MAUS::TriggerEvent+;
#pragma link C++ class MAUS::GlobalEvent+;

#pragma link C++ class MAUS::MCEvent+;
#pragma link C++ class std::vector<Track>;
#pragma link C++ class std::vector<VirtualHit>;
#pragma link C++ class std::vector<SciFiHit>;
#pragma link C++ class std::vector<TOFHit>;
#pragma link C++ class std::vector<CkovHit>;
#pragma link C++ class std::vector<KLHit>;
#pragma link C++ class std::vector<EMRHit>;
#pragma link C++ class std::vector<SpecialVirtualHit>;

#pragma link C++ class MAUS::VirtualHit+;
#pragma link C++ class MAUS::Primary+;
#pragma link C++ class MAUS::Step+;
#pragma link C++ class MAUS::Track+;
#pragma link C++ class std::vector<Step>;
#pragma link C++ class MAUS::SciFiHit+;
#pragma link C++ class MAUS::SciFiChannelId+;
#pragma link C++ class MAUS::TOFHit+;
#pragma link C++ class MAUS::CkovHit+;
#pragma link C++ class MAUS::KLHit+;
#pragma link C++ class MAUS::TOFChannelId+;
#pragma link C++ class MAUS::CkovChannelId+;
#pragma link C++ class MAUS::KLChannelId+;
#pragma link C++ class MAUS::EMRHit+;
#pragma link C++ class MAUS::EMRChannelId+;
#pragma link C++ class MAUS::SpecialVirtualHit+;
#pragma link C++ class MAUS::SpecialVirtualChannelId+;

#pragma link C++ class MAUS::SciFiNoiseHit+;
#pragma link C++ class std::vector<MAUS::SciFiNoiseHit*>+;

#pragma link C++ class MAUS::VLSB+;
#pragma link C++ class std::vector<MAUS::VLSB>;
#pragma link C++ class MAUS::VLSB_C+;
#pragma link C++ class std::vector<MAUS::VLSB_C>;
#pragma link C++ class MAUS::V1290+;
#pragma link C++ class std::vector<V1290>;
#pragma link C++ class MAUS::EMREvent+;
#pragma link C++ class MAUS::ReconEvent+;
#pragma link C++ class MAUS::V1731+;
#pragma link C++ class std::vector<V1731>;
#pragma link C++ class MAUS::TOFDigit+;
#pragma link C++ class MAUS::V830+;
#pragma link C++ class MAUS::TOFSlabHit+;
#pragma link C++ class MAUS::CkovDigit+;
#pragma link C++ class MAUS::V1724+;
#pragma link C++ class std::vector<V1724>;
#pragma link C++ class MAUS::GlobalEvent+;
#pragma link C++ class MAUS::TOFSpacePoint+;
#pragma link C++ class std::vector<TOFSpacePoint>;
#pragma link C++ class MAUS::Trigger+;
#pragma link C++ class MAUS::Pmt1+;
#pragma link C++ class MAUS::Pmt0+;
#pragma link C++ class MAUS::TOFEventSlabHit+;
#pragma link C++ class std::vector<TOFSlabHit>;
#pragma link C++ class MAUS::TriggerEvent+;
#pragma link C++ class MAUS::CkovEvent+;
#pragma link C++ class std::vector<CkovDigit>;
#pragma link C++ class MAUS::KLDaq+;
#pragma link C++ class MAUS::KLEvent+;
#pragma link C++ class MAUS::TOFDaq+;
#pragma link C++ class MAUS::TOFEvent+;
#pragma link C++ class MAUS::TOFEventSpacePoint+;
#pragma link C++ class MAUS::TOFEventDigit+;
#pragma link C++ class std::vector<TOFDigit>;
#pragma link C++ class MAUS::Unknown+;
#pragma link C++ class MAUS::CkovDaq+;
#pragma link C++ class MAUS::Channels+;
#pragma link C++ class MAUS::TriggerRequest+;
#pragma link C++ class MAUS::DAQData+;
#pragma link C++ class MAUS::CkovA+;
#pragma link C++ class MAUS::CkovB+;
#pragma link C++ class MAUS::Tag+;
#pragma link C++ class MAUS::TrackerDaq+;
#pragma link C++ class MAUS::SciFiEvent+;
#pragma link C++ class MAUS::SciFiDigit+;
#pragma link C++ class std::vector<MAUS::SciFiDigit*>+;
#pragma link C++ class MAUS::SciFiCluster+;
#pragma link C++ class std::vector<MAUS::SciFiCluster*>+;
#pragma link C++ class MAUS::SciFiSpacePoint+;
#pragma link C++ class std::vector<MAUS::SciFiSpacePoint*>+;
#pragma link C++ class MAUS::SciFiBasePRTrack+;
#pragma link C++ class std::vector<MAUS::SciFiBasePRTrack*>+;
#pragma link C++ class MAUS::SciFiStraightPRTrack+;
#pragma link C++ class std::vector<MAUS::SciFiStraightPRTrack*>+;
#pragma link C++ class MAUS::SciFiHelicalPRTrack+;
#pragma link C++ class std::vector<MAUS::SciFiHelicalPRTrack*>+;
#pragma link C++ class MAUS::SciFiTrack+;
#pragma link C++ class std::vector<MAUS::SciFiTrack*>+;
#pragma link C++ class MAUS::SciFiTrackPoint+;
#pragma link C++ class std::vector<MAUS::SciFiTrackPoint*>+;
#pragma link C++ class MAUS::KLDigit+;
#pragma link C++ class MAUS::KLCellHit+;
#pragma link C++ class MAUS::KLEventCellHit+;
#pragma link C++ class MAUS::KLEventDigit+;
#pragma link C++ class std::vector<MAUS::KLDigit>;
#pragma link C++ class std::vector<MAUS::KLCellHit>;
#pragma link C++ class MAUS::DataStructure::Global::BasePoint+;
#pragma link C++ class MAUS::DataStructure::Global::SpacePoint+;
#pragma link C++ class MAUS::DataStructure::Global::TrackPoint+;
#pragma link C++ class MAUS::DataStructure::Global::Track+;
#pragma link C++ class MAUS::DataStructure::Global::PrimaryChain+;
#pragma link C++ class MAUS::DataStructure::Global::TRefTrackPair+;
#pragma link C++ class MAUS::DataStructure::Global::PIDLogLPair+;
#pragma link C++ class std::vector<MAUS::DataStructure::Global::BasePoint*>+;
#pragma link C++ class std::vector<MAUS::DataStructure::Global::SpacePoint*>+;
#pragma link C++ class std::vector<MAUS::DataStructure::Global::TrackPoint*>+;
#pragma link C++ class std::vector<MAUS::DataStructure::Global::Track*>+;
#pragma link C++ class std::vector<MAUS::DataStructure::Global::PrimaryChain*>+;
#pragma link C++ class std::vector<MAUS::DataStructure::Global::TRefTrackPair*>+;
#pragma link C++ class std::vector<MAUS::DataStructure::Global::PIDLogLPair*>+;

#pragma link C++ class MAUS::EMRDaq+;
#pragma link C++ class MAUS::DBBHit+;
#pragma link C++ class MAUS::DBBHitsArray+;
#pragma link C++ class MAUS::DBBSpillData+;
#pragma link C++ class MAUS::EMRBarHit+;
#pragma link C++ class MAUS::EMRPlaneHit+;
#pragma link C++ class MAUS::EMRSpacePoint+;
#pragma link C++ class MAUS::EMRTrack+;
#pragma link C++ class MAUS::EMRTrackPoint+;
#pragma link C++ class MAUS::EMREventTrack+;
#pragma link C++ class std::vector<EMRPlaneHit*>+;
#pragma link C++ class std::vector<EMRBarHit>+;
#pragma link C++ class std::vector<EMRSpacePoint*>+;
#pragma link C++ class std::vector<EMRTrackPoint>+;
#pragma link C++ class std::vector<EMREventTrack*>+;


#pragma link C++ global gROOT;
#pragma link C++ global gEnv;

#endif




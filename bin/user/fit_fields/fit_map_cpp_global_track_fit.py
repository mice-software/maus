import json

import ROOT

from MAUS import MapCppGlobalTrackFit
import maus_cpp.globals
import maus_cpp.field
import Configuration

import scripts.config

class DriveMapCppGlobalTrackFit(object):
    def __init__(self, config):
        self.config = config
        self.maus_conf = ""
        self.mapper = MapCppGlobalTrackFit()

    def setup_maus_config(self):
        str_conf = Configuration.Configuration().getConfigJSON(command_line_args=False)
        json_conf = json.loads(str_conf)
        json_conf["verbose_level"] = 1
        json_conf["simulation_geometry_filename"] = self.config.geometry
        json_conf["global_track_fit_will_require_triplet_space_points"] = True
        json_conf["global_track_fit_tof_sigma_t"] = 0.05
        json_conf["global_track_fit_scifi_sigma_x"] = 0.4
        self.maus_conf = json.dumps(json_conf)
        maus_cpp.globals.birth(self.maus_conf)
        self.mapper.birth(self.maus_conf)

    def do_fit(self, spill_event_dict):
        print "DO FIT 2"
        for data in self.load_event(spill_event_dict):
            yield self.mapper.process(data)

    def load_event(self, spill_event_dict):
        file_name = self.config.reco_file
        print "Loading ROOT file", file_name
        root_file = ROOT.TFile(file_name, "READ") # pylint: disable = E1101

        print "Setting up data tree"
        data = ROOT.MAUS.Data() # pylint: disable = E1101
        tree = root_file.Get("Spill")
        tree.SetBranchAddress("data", data)

        print "Getting some data"
        for i in range(tree.GetEntries()):
            tree.GetEntry(i)
            spill = data.GetSpill()
            if spill.GetDaqEventType() != "physics_event" or \
               spill.GetSpillNumber() not in spill_event_dict:
                continue
            all_reco_events = [reco_event for reco_event in spill.GetReconEvents()]
            for event in spill_event_dict[spill.GetSpillNumber()]:
                if event >= len(all_reco_events):
                    break
                reco_event_vector = spill.GetReconEvents()
                reco_event_vector.resize(0)
                reco_event_vector.push_back(all_reco_events[event])
                yield data

def main():
    fit = DriveMapCppGlobalTrackFit(scripts.config.Config())
    fit.setup_maus_config()
    for fitted_data in fit.do_fit({3:[19]}):#, 12:[5]}):
        pass

if __name__ == "__main__":
    main()

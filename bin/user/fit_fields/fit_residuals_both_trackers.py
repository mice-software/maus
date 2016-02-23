import os
import site

import ROOT
analysis_path = os.environ["MAUS_ROOT_DIR"]+"/bin/user/fit_fields/scripts/"
site.addsitedir(analysis_path)

import scripts.space_point_data_loader
import scripts.minimise_position_residual_fitter
import scripts.config

def main():
    config = scripts.config.Config()
    data_loader = scripts.space_point_data_loader.SpacePointDataLoader(config)
    data_loader.load_data(config.number_of_events)
    fitter = scripts.minimise_position_residual_fitter.MinimisePositionResidualFitter(config)
    for event in data_loader.events:
        print "Space points for spill", event["spill"], "particle", event["particle_number"]
        for sp in event["data"]:
              print "    ", sp
        fitter.fit_track(event["data"], config.fit_detectors)
        fitter.print_fit()
        plot_name = os.path.join(config.plot_dir, "fit_spill-"+str(event["spill"])+"_part-"+str(event["particle_number"]))
        fitter.display_current_event(plot_name)
        fitter.display_current_event(plot_name+"_tku", ["tku"])
        ROOT.gROOT.SetBatch(True)
    fitter.plot_bz()


if __name__ == "__main__":
    main()
    print "Done - press <Enter> to finish"
    raw_input()


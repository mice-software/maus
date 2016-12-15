import time
import sys
import os
import json
import io
import xboa.common
import MAUS

import Configuration # MAUS
import maus_cpp.globals
import maus_cpp.field
import analysis

def get_setup_dict():
    my_dict = {
        "verbose_level":1,
        "output_root_file_name":"tmp.root",
        "input_root_file_name":"/home/cr67/MAUS/work/reco/MAUS-v2.6.5/08681/08681_recon.root",
        "fit_range":0.1,
        "fit_do_cuts":True,
        "fit_tx":True,
        "fit_ty":True,
        "fit_nbins":50 ,
        "fit_refresh_rate":10,
        "fit_file":"fit.txt",
        "fit_w_tx_seed":0.,
        "fit_w_ty_seed":0.,
    }
    my_keys = ""
    for key, value in my_dict.iteritems():
        if type(value) != type(True):
            my_keys += str(key)+" = "+json.dumps(value)+"\n"
        else:
            my_keys += str(key)+" = "+str(value)+"\n"
    return io.StringIO(unicode(my_keys))


def run(datacards):
    my_input = MAUS.InputCppRootData()
    my_map = MAUS.MapPyDoNothing()
    reducer = MAUS.ReduceCppTiltedHelix()
    my_output = MAUS.OutputPyDoNothing()
    sys.argv = sys.argv[0:1]
    MAUS.Go(my_input, my_map, reducer, my_output, datacards)

def main(args):
    setup_string = get_setup_dict()
    run(setup_string)


if __name__ == "__main__":
    main(sys.argv)


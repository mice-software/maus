#  
"""
Generates plots using test data in ref_data and tmp folder
"""
import glob
import sys
sys.path.insert(0, '/home/hep/sm1208/maus/tests/integration/test_simulation/test_physics_model_full')#adds this to path

#imports files, if code imported it is not run
from physics_model_test import plotter
from physics_model_test import geometry
from physics_model_test.all_tests import KSTest


def main():
    """
    Search for test data in geometry.ref_data('./') and geometry.temp('./'); if
    found, plot it
    """
    reference_data = glob.glob(geometry.ref_data("*.ref_data.dat"))
   # reference_data2 = glob.glob(geometry.ref_data("icool.3.30.ref_data.dat"))
 
    test_data = ["MUSCAT_data.dat"]

    plotter.plot(reference_data+test_data)
   # RMS=plotter.histograms[index].GetRMS()
   # MEAN=plotter.histograms[index].GetMean()




if __name__ == "__main__":  #make the file usable as a script as well as an importable module
    main()
    raw_input()

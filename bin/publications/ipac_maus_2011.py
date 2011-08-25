import MAUS

# File with particles to simulation
my_input = MAUS.InputPyJSON("evts_to_sim.json")

# Create an empty array of mappers, then
# populate it with the functionality you
# want to use.
my_map = MAUS.MapPyGroup()

# Add geant4 Monte Carlo simulation
my_map.append(MAUS.MapCppSimulation())

# Add electronics models
my_map.append(MAUS.MapCppTOFDigitization()) 
my_map.append(MAUS.MapCppTrackerDigitization())

# Create set of standard demonstration plots
my_reduce = MAUS.ReducePyMakeDemoPlots()

# Where to save output?
filename = 'simulation.out'

# Create uncompressed file object.
# 'w' means write.
output_file_object = open(filename, 'w')

# Then construct a MAUS output component
my_output = MAUS.OutputPyJSON(output_file)

#  The Go() drives all the components you
#  pass in, then check the file defined
#  above for the output
MAUS.Go(my_input, my_map,
        my_reduce, my_output)

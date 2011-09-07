import MAUS

# File with particles to simulate
my_input = MAUS.InputJSON("evts.json")

# Create an empty array of maps, then
# populate it with the functionality you
# want to use.
my_map = MAUS.MapGroup()

# Add geant4 Monte Carlo simulation
my_map.append(MAUS.MapSimulation())

# Add electronics models
my_map.append(MAUS.MapTOFDigitization())
my_map.append(MAUS.MapTrackerDigitization())

# Create set of standard demo plots
my_reduce = MAUS.ReduceMakeDemoPlots()

# Where to save output?
filename = 'simulation.out'

# Create uncompressed file object.
# 'w' means write.
output_file = open(filename, 'w')

# Then construct a MAUS output component
my_output = MAUS.OutputJSON(output_file)

#  The Go() drives all the components you
#  pass in, then check the file defined
#  above for the output
MAUS.Go(my_input, my_map,
        my_reduce, my_output)


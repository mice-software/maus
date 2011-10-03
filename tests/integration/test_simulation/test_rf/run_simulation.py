import io
import MAUS

def run(input_beam, configuration, out_file):
    document_file = io.StringIO(unicode(input_beam))
    my_input = MAUS.InputPyJSON(document_file)
    my_map = MAUS.MapCppSimulation()
    my_output = MAUS.OutputPyJSON(open(out_file, 'w'))
    MAUS.Go(my_input, my_map, MAUS.ReducePyDoNothing(), my_output, io.StringIO(unicode(configuration)))
  

if __name__ == '__main__':
    run("", "", "")

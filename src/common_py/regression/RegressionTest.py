#  This file is part of MAUS: http://micewww.pp.rl.ac.uk:8080/projects/maus
#
#  MAUS is free software: you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation, either version 3 of the License, or
#  (at your option) any later version.
#
#  MAUS is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#
#  You should have received a copy of the GNU General Public License
#  along with MAUS.  If not, see <http://www.gnu.org/licenses/>.
import ROOT
import collections
import re
import types
#pylint: disable=C0103,E1101,R0913,W0102,R0903,R0903,
#pylint: disable=W0232,E1002,R0912,R0914,R0915,E1001
def printhistos(hist_mon, hist_ref, results):
    """Method that prints out a comparison of the ref and mon histograms."""
    canvas = ROOT.TCanvas(hist_ref.GetName(), hist_ref.GetName())
    hist_ref.SetLineColor(ROOT.kRed)
    hist_ref.SetFillStyle(3005)
    hist_ref.SetFillColor(ROOT.kRed)
    hist_ref.SetName("Reference")
    hist_ref.Draw()

    hist_mon.SetLineColor(ROOT.kBlue)
    hist_mon.SetFillStyle(3004)
    hist_mon.SetFillColor(ROOT.kBlue)
    hist_mon.SetName("Monitored")
    hist_mon.Draw("sames")
    ROOT.gPad.Update()

    stats_ref = hist_ref.FindObject("stats")
    stats_ref.SetTextColor(ROOT.kRed)
    stats_mon = hist_mon.FindObject("stats")
    stats_mon.SetTextColor(ROOT.kBlue)
    height = stats_mon.GetY2NDC() - stats_mon.GetY1NDC()
    stats_mon.SetY2NDC(stats_ref.GetY1NDC() - 0.03)
    stats_mon.SetY1NDC(stats_ref.GetY1NDC() - 0.03 - height)

    l = ROOT.TLatex()
    l.SetTextSize(0.036)
    l.SetNDC()
    step_size = 0.05
    pos = 0.01
    for t, r  in results.iteritems():
        if r.status == 'Failure':
            stats_colour = ROOT.kRed
        elif r.status == 'Warning':
            stats_colour = ROOT.kYellow+2 #mustard yellow
        else:
            stats_colour = ROOT.kGreen
        l.DrawLatex(0.,
                    pos,
                    '#color[{colour}]{{{test:<16}: '\
                        'pVal={result.pval:<6.4f}, '\
                        'warn={result.warning:<6.4f}, '\
                        'fail={result.failure:<6.4f}}}'.\
                        format(colour = stats_colour,
                               test   = t,
                               result = r)
                    )
        pos += step_size
    ROOT.gPad.Update()

    title = canvas.FindObject("title")
    status = [r.status for r in results.values()]
    if 'Failure' in status:
        title.SetTextColor(ROOT.kRed)
    elif 'Warning' in status:
        title.SetTextColor(ROOT.kYellow+2)
    else:
        title.SetTextColor(ROOT.kGreen)        

    canvas.Write()

def KS(mon, ref):
    """KS Test"""
    return ref.KolmogorovTest(mon)
def X2(mon, ref):
    """Chi^2 Test"""
    return ref.Chi2Test(mon)


Test = collections.namedtuple('Test', ['name', 'test', 'warning', 'failure'])
TestResult = collections.namedtuple('TestResult', ['status', 'pval', 
                                                   'warning', 'failure'])

class KolmogorovTest(Test):
    """The KS test object"""
    __slots__ = ()
    def __new__(cls, warning, failure):
        return super(cls, KolmogorovTest).__new__(cls,
                                                  name = 'KolmogorovTest',
                                                  test=KS,
                                                  warning=warning,
                                                  failure=failure)

class Chi2Test(Test):
    """The Chi^2 test object"""
    __slots__ = ()
    def __new__(cls, warning, failure):
        return super(cls, Chi2Test).__new__(cls,
                                            name = 'Chi2Test',
                                            test=X2,
                                            warning=warning,
                                            failure=failure)

#note: adding multiple tests with the same name will result in the
#      'results' dict being updated each time
#      therefore only the LAST tests results will be returned.
#note: the addition of the ROOT.TH1 causes an annoying (Bool_t) 1 
#      to be printed when imporing, worse if done in function
#      then get printed out when running before actual output!  Grr...


def RegressionTest( mon,
                    ref,
                    config={},
                    default_config=[Chi2Test(0.1,0.05),
                                    KolmogorovTest(0.1,0.05)],
                    type_to_extract = ROOT.TH1,
                    fail_on_missing = False,
                    printer = printhistos,
                    printed_filename=None ):
    """
    Perform regression tests on the contents of two ROOT files

    This method takes a filename of a 'monitored' file which is to be regressed against a known and validated standard 'reference'
    file.
    """
    if not isinstance(mon, str):
        raise Exception("'mon' should be of type 'str' and represent the "\
                            "filename of the monitored root file")
    if not isinstance(ref, str):
        raise Exception("'ref' should be of type 'str' and represent the "\
                            "filename of the reference root file")
    if not isinstance(default_config, list):
        raise Exception("'default_config' should be a 'list' of 'Test' "\
                            "objects to perform by default")
    if not isinstance(config, dict):
        raise Exception("'config' should be a 'dict' mapping a regex pattern "\
                            "to a 'list' of 'Test' objects")
    if False in (isinstance(i, Test) for i in default_config):
        raise Exception("An entry in 'default_config' was not a 'Test' object")
    if False in (isinstance(i, str)  for i in config.keys()):
        raise Exception("A key in 'config' was not of type 'str'")
    if False in (isinstance(i, list) for i in config.values()):
        raise Exception("A value in 'config' was not of type 'list'")
    for i in config.values():
        if False in (isinstance(j, Test) for j in i ):
            raise Exception("Within the list of tests defined in 'config' an "\
                                "object not of type 'Test' was found")
    if printer is not None and not isinstance(printer, types.FunctionType):
        raise Exception("'printer' should be of type 'types.FunctionType' "\
                            "taking two extracted objects and a results dict.")


    ref_file = ROOT.TFile(ref)
    mon_file = ROOT.TFile(mon)
    if printer is not None:
        if printed_filename is None:
            printed_filename = mon.replace('.root', '_compared.root')
        output_file = ROOT.TFile(printed_filename, "RECREATE")

    histo_list = [item.GetName() for item in ref_file.GetListOfKeys() \
                      if isinstance(ref_file.Get(item.GetName()),
                                    type_to_extract)]

    results = {}
    for histo in histo_list:
        hist_ref = ref_file.Get(histo)
        hist_mon = mon_file.Get(histo)
        if hist_mon == None:
            if fail_on_missing:
                raise Exception("Object '%s' doesn't exist in the "\
                                    "monitored file." % histo)
            print "Skipping object %s as doesn't exist in "\
                "monitored file" % histo
            continue
        
        tests = [value for key, value in config.iteritems() \
                     if re.match(key, histo) is not None]
        if len(tests) > 1:
            print "Skipping object %s as too many test configs "\
                "found that match" % histo
            continue
        elif len(tests) < 1:
            tests = [default_config]

        results[histo] = {}
        for t in tests[0]:
            status = 'Pass'
            pVal = t.test(hist_mon, hist_ref)
            if pVal < t.warning:
                status = 'Warning'
            if pVal < t.failure:
                status = 'Failure'
            results[histo].update({t.name: TestResult(status, pVal,
                                                      t.warning, t.failure)})

        if printer is not None:
            printer(hist_mon, hist_ref, results[histo])

    ref_file.Close()
    mon_file.Close()
    if printer is not None:
        output_file.Close()
    return results


def AggregateRegressionTests( mon,
                              ref,
                              config={},
                              default_config=[Chi2Test(0.1,0.05),
                                              KolmogorovTest(0.1,0.05)],
                              type_to_extract = ROOT.TH1,
                              fail_on_missing = False,
                              printer = printhistos,
                              printed_filename=None):
    """Agregates all tests and returns whether passes true/false"""
    results = RegressionTest( mon, ref, config, default_config, type_to_extract,
                              fail_on_missing, printer, printed_filename)

    for histo_info in results.values():
        for test_info in histo_info.values():
            if test_info.status != 'Pass':
                return False
    return True

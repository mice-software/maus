/* This file is part of MAUS: http://  micewww.pp.rl.ac.uk:8080/projects/maus
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
 * along with MAUS.  If not, see <http://  www.gnu.org/licenses/>.
 */

#include "Recon/Global/PIDBase2D.hh"
#include "Utils/Exception.hh"

namespace MAUS {
namespace recon {
namespace global {

  PIDBase2D::PIDBase2D(std::string variable, std::string hypothesis,
		       std::string unique_identifier, int XminBin, int XmaxBin,
		       int XnumBins, int YminBin, int YmaxBin, int YnumBins)
    : PIDBase(variable, hypothesis, unique_identifier,
	      XminBin, XmaxBin, XnumBins, YminBin, YmaxBin, YnumBins) {

    _varhyp = variable + "_" + hypothesis;

    _hist = new TH2F(_varhyp.c_str(),
		     _varhyp.c_str(),
		     _XnumBins, _XminBin, _XmaxBin,
		     _YnumBins, _YminBin, _YmaxBin);
  };

  PIDBase2D::PIDBase2D(TFile* file, std::string variable,
		       std::string hypothesis, int Xmin, int Xmax, int Ymin,
		       int Ymax, int XminBin, int XmaxBin, int YminBin,
		       int YmaxBin)
    : PIDBase(file, variable, hypothesis, Xmin, Xmax, Ymin, Ymax, XminBin,
	      XmaxBin, YminBin, YmaxBin) {
    std::string histname = variable + "_" + hypothesis;

    if (!file || file->IsZombie()) {
      throw(Exception(Exception::recoverable,
		      "File containing MC PID histograms not found.",
		      "Recon::Global::PIDBase2D::PIDBase2D()"));
    }
    _hist = static_cast<TH2F*>(file->Get(histname.c_str()));
    if (!_hist) {
      throw(Exception(Exception::recoverable,
		      "Histogram not found in file.",
		      "Recon::Global::PIDBase2D::PIDBase2D()"));
    }
  };

  PIDBase2D::~PIDBase2D() {
    if (_writeFile) {
      if (!_hist) {
	throw(Exception(Exception::recoverable,
			"Can't write histogram to file.",
			"Recon::Global::PIDBase2D::~PIDBase2D()"));
      }
      if (_nonZeroHistEntries) {
	int bins = _hist->GetSize();
	_addToAllBins = 1/(static_cast<double>(bins));
	for (int i = 0; i < bins; i++) {
	  int entries = _hist->GetBinContent(i);
	  _hist->SetBinContent(i, (static_cast<double>(entries)
				   + _addToAllBins));
	}
      }

      Double_t scale = 1/_hist->Integral();
      _hist->Scale(scale);
      _writeFile->cd();
      _hist->Write();
      _writeFile->Close();
      }
  };

    TH2F* PIDBase2D::Get_hist() {
    return PIDBase2D::_hist;
    }

  double PIDBase2D::logL(MAUS::DataStructure::Global::Track* track) {
    double Xvar = (Calc_Var(track)).first;
    double Yvar = (Calc_Var(track)).second;
    if ((Xvar < _XminBin || Xvar > _XmaxBin) ||
	(Yvar < _YminBin || Yvar > _YmaxBin)) {
      Squeak::mout(Squeak::debug) << "Value of PID variable out of PDF range, "
				  << " Recon::Global::PIDBase2D::logL()"
				  << std::endl;
      return 1;
    } else if ((Xvar < _Xmin || Xvar > _Xmax) ||
	       (Yvar < _Ymin || Yvar > _Ymax)) {
      Squeak::mout(Squeak::debug) << "Value of PID variable out of cut range "
				  << " used for PID, "
				  << "Recon::Global::PIDBase2D::logL()"
				  << std::endl;
      return 2; // return value of 2 is chosen to be picked up when performing
      // efficiency and purity on PID, do not change.
    }
    int binx = _hist->GetXaxis()->FindBin(Xvar);
    int biny = _hist->GetYaxis()->FindBin(Yvar);
    int bin = _hist->GetBin(binx, biny, 0);
    double entries = _hist->GetBinContent(bin);
    if (entries <= 0) {
      Squeak::mout(Squeak::debug) << "Corresponding bin content in PDF is "
				  << "not greater than zero, "
				  << "Recon::Global::PIDBase2D::logL()"
				  << std::endl;
      return 1;
    }
    double probLL = TMath::Log(entries);
    return probLL;
  }

  void PIDBase2D::Fill_Hist(MAUS::DataStructure::Global::Track* track) {
    double Xvar = (Calc_Var(track)).first;
    double Yvar = (Calc_Var(track)).second;
    if ((Xvar < _XminBin || Xvar > _XmaxBin) ||
	(Yvar < _YminBin || Yvar > _YmaxBin)) {
       Squeak::mout(Squeak::debug)
	 << "Calc_Var returned invalid value of "
	 << "PID variable, not added to histogram, "
	 << "Recon::Global::PIDBase2D::Fill_Hist()" << std::endl;
    } else {
      _hist->Fill(Xvar, Yvar);
    }
  }
}
}
}

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

#include "Recon/Global/PIDBase.hh"
#include "Utils/Exception.hh"

namespace MAUS {
namespace recon {
namespace global {

  PIDBase::PIDBase(int minBin, int maxBin, int numBins,
		   std::string variable, std::string hypothesis,
		   std::string unique_identifier)
    : _minBin(minBin), _maxBin(maxBin), _numBins(numBins),
      _var_name(variable), _hyp(hypothesis),
      _unique_identifier(unique_identifier), _writeFile(NULL) {
    char* pMAUS_ROOT_DIR = getenv("MAUS_ROOT_DIR");

    if (!pMAUS_ROOT_DIR) {
      throw(Exception(Exception::recoverable,
		      "Could not find the $MAUS_ROOT_DIR environmental ",
		      "variable, Recon::Global::PIDBase::PIDBase()"));
    }

    /// Set to false as default, enable behaviour within derived classes
    _nonZeroHistEntries = false;

    _varhyp = _var_name + "_" + _hyp;
    char pidDirectory[180];
    snprintf(pidDirectory, sizeof(pidDirectory), "%s/files/PID",
	     pMAUS_ROOT_DIR);
    gSystem->MakeDirectory(pidDirectory);
    char hypDirectory[250];
    snprintf(hypDirectory, sizeof(hypDirectory), "%s/files/PID/%s_%s",
	     pMAUS_ROOT_DIR, _hyp.c_str(), _unique_identifier.c_str());
    gSystem->MakeDirectory(hypDirectory);
    char pidFile[300];
    snprintf(pidFile, sizeof(pidFile), "%s/%s.root", hypDirectory,
	     _varhyp.c_str());
    _filename = std::string(pidFile);
    _directory = std::string(hypDirectory);
    _writeFile = new TFile(pidFile, "RECREATE");
    if (_writeFile->IsZombie()) {
      throw(Exception(Exception::recoverable,
		      "File to write histograms to failed to be created.",
		      "Recon::Global::PIDBase::PIDBase()"));
    }
    _hist = new TH1F(_varhyp.c_str(), _varhyp.c_str(),
		     numBins, minBin, maxBin);
  };

  PIDBase::PIDBase(TFile* file, std::string variable,
		   std::string hypothesis)
    : _var_name(variable), _hyp(hypothesis), _hist(NULL),
      _writeFile(NULL) {
    std::string histname = _var_name + "_" + _hyp;
    if (!file || file->IsZombie()) {
      throw(Exception(Exception::recoverable,
		      "File containing MC PID histograms not found.",
		      "Recon::Global::PIDBase::PIDBase()"));
    }
    _hist = static_cast<TH1F*>(file->Get(histname.c_str()));
    if (!_hist) {
      throw(Exception(Exception::recoverable,
		      "Histogram not found in file.",
		      "Recon::Global::PIDBase::PIDBase()"));
    }
    _minBin = _hist->GetXaxis()->GetXmin();
    _maxBin = _hist->GetXaxis()->GetXmax();
  };

  PIDBase::~PIDBase() {
    if (_writeFile) {
      if (!_hist) {
	throw(Exception(Exception::recoverable,
			"Can't write histogram to file.",
			"Recon::Global::PIDBase::~PIDBase()"));
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
      _hist->Write();
      _writeFile->Close();
    }
  };


  std::string PIDBase::Get_hyp() {
    return PIDBase::_hyp;
  }

  std::string PIDBase::Get_var_name() {
    return PIDBase::_var_name;
  }

  std::string PIDBase::Get_filename() {
    return PIDBase::_filename;
  }

  std::string PIDBase::Get_directory() {
    return PIDBase::_directory;
  }

  TH1F* PIDBase::Get_hist() {
    return PIDBase::_hist;
  }

  double PIDBase::logL(MAUS::DataStructure::Global::Track* track) {
    double var = Calc_Var(track);
    if (var < _minBin || var > _maxBin) {
      Squeak::mout(Squeak::error) << "Value of PID variable out of range, " <<
	"Recon::Global::PIDBase::logL()" << std::endl;
      return 1;
    }
    int bin = _hist->FindBin(var);
    double entries = _hist->GetBinContent(bin);
    if (entries <=0) {
      Squeak::mout(Squeak::error) << "Corresponding bin content in PDF is " <<
	"not greater than zero, Recon::Global::PIDBase::logL()" << std::endl;
      return 1;
    }
    double probLL = TMath::Log(entries);
    return probLL;
  }

  void PIDBase::Fill_TH1(MAUS::DataStructure::Global::Track* track) {
    double var = Calc_Var(track);
    if (var < _minBin || var > _maxBin) {
       Squeak::mout(Squeak::error) << "Calc_Var returned invalid value of " <<
  "PID variable, not added to histogram, " <<
  "Recon::Global::PIDBase::Fill_TH1()" << std::endl;
    } else {
      _hist->Fill(var);
    }
  }
}
}
}

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

  PIDBase::PIDBase(std::string variable, std::string hypothesis,
		   std::string unique_identifier, int XminBin, int XmaxBin,
		   int XnumBins, int YminBin, int YmaxBin, int YnumBins)
    : _var_name(variable), _hyp(hypothesis),
      _unique_identifier(unique_identifier),
      _XminBin(XminBin), _XmaxBin(XmaxBin), _XnumBins(XnumBins),
      _YminBin(YminBin), _YmaxBin(YmaxBin), _YnumBins(YnumBins),
      _writeFile(NULL) {
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
  };

  PIDBase::PIDBase(TFile* file, std::string variable,
		   std::string hypothesis, int Xmin, int Xmax, int Ymin, int Ymax)
    : _var_name(variable), _hyp(hypothesis),
      _Xmin(Xmin), _Xmax(Xmax), _Ymin(Ymin), _Ymax(Ymax), _writeFile(NULL) {
    std::string histname = _var_name + "_" + _hyp;
  };

  PIDBase::~PIDBase() {
    // close file
    if (_writeFile) {
      _writeFile->Close();
      _writeFile->Delete();
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
}
}
}

import os
import subprocess

def fix_geant4_1405():
    """
    Fix for issue #1405 (recommended by G4 devs)
    """
    snippet_in = \
    """
   if(NofInactiveProc==MAXofAtRestLoops){ 
     G4Exception("G4SteppingManager::InvokeAtRestDoItProcs()", "Tracking0013",
                 JustWarning, "No AtRestDoIt process is active!" );
   }
"""
    snippet_out = \
    """
   // Edit for MAUS - See redmine #1404
   if(NofInactiveProc==MAXofAtRestLoops){
     //     G4Exception("G4SteppingManager::InvokeAtRestDoItProcs: No AtRestDoIt process is active. " );
       G4cout << "G4SteppingManager::InvokeAtRestDoItProcs: No AtRestDoIt process is active. " << G4endl;
   } else {
      (*fSelectedAtRestDoItVector)[fAtRestDoItProcTriggered] = NotForced;
   }
"""

    g4_path = "${MAUS_ROOT_DIR}/third_party/source/geant4.9.6.p02/"
    g4_path = os.path.expandvars(g4_path)
    step_path = g4_path+"source/tracking/src/G4SteppingManager2.cc"
    orig_path = step_path+".orig"
    if os.path.exists(step_path) and not os.path.exists(orig_path):
        os.rename(step_path, orig_path)
    fin = open(orig_path)
    stepping_in = fin.read()
    stepping_out = stepping_in.replace(snippet_in, snippet_out)
    fout = open(step_path, "w")
    fout.write(stepping_out)
    print "Modified\n  ", step_path, "\noriginal stored at\n  ", orig_path
    subprocess.Popen(['diff', orig_path, step_path])


if __name__ == "__main__":
    fix_geant4_1405()


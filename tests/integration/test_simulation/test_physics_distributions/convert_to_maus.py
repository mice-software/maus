import xboa.Common

subs = {
'$MICESRC/Applications/Simulation/Simulation':'$MAUS_ROOT_DIR/bin/simulate_mice.py',
"['cards']":"['-configuration_file', 'cards.py']",
"'cards.in': 'cards'":"'cards.in': 'cards.py'",
'_material_': '__material__', 
'_seed_':'__seed__', 
'_pid_':'__pid__', 
'_momentum_': '__momentum__', 
'_thickness_': '__thickness__', 
'_nev_': '__nev__', 
'_step_': '__step__',
"('g4mice_virtual_hit', 'Sim.out.gz')":"('maus_virtual_hit', 'simulation.out')",
}

xboa.Common.substitute('reference_data.in', 'reference_data.dat', subs)


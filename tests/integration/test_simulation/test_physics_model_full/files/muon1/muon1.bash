#!/usr/bin/env bash
wine $MUON1_DIR/muon1.exe -once -b
echo 'Moving'
echo $MUON1_DIR/muon1_output.csv
echo 'to'
echo $MAUS_ROOT_DIR/tmp/test_physics_model_full/
sleep 1
mv $MUON1_DIR/muon1_output.csv $MAUS_ROOT_DIR/tmp/test_physics_model_full/
sleep 1
rm $MUON1_DIR/output_ENDOUT*


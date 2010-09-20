#!/bin/zsh

if [ -z "$1" ]
then echo "Region is missing"
     exit 1
else side=$1
fi

source ~/w0/15.0.0_svn/setup_athena.sh

echo "Using side ${side}"

for j in {1..9}
  do
  echo "
selected_region='LB${side}_m0${j}_'
execfile('macros/cis/update_cis_constants.py', globals())
">tmp/status_${side}A_m0${j}.py
  python tmp/status_${side}A_m0${j}.py
done

for j in {10..64}
  do                                                                                                                         
  echo "
selected_region='$LB{side}_m${j}_'
execfile('macros/cis/update_cis_constants.py', globals())
">tmp/status_${side}A_m${j}.py
  python tmp/status_${side}A_m${j}.py
done   


for j in {1..9}
  do
  echo "
selected_region='EB${side}_m0${j}_'
execfile('macros/cis/update_cis_constants.py', globals())
">tmp/status_${side}C_m0${j}.py
  python tmp/status_${side}C_m0${j}.py
done

for j in {10..64}
  do                                                                                                                         
  echo "
selected_region='EB${side}_m${j}_'
execfile('macros/cis/update_cis_constants.py', globals())
">tmp/status_${side}C_m${j}.py
  python tmp/status_${side}C_m${j}.py
done   

#!/bin/bash

#To read database
#TILE
# from sqlite
# AtlCoolConsole.py "sqlite://x;schema=tileSqlite.db;dbname=COMP200"
# directly from oracle db
# AtlCoolConsole.py COOLONL_TILE/COMP200
#CALO
# from sqlite
# 
# directly from oracle db
# AtlCoolConsole.py COOLONL_CALO/COMP200

usage()
{
cat<<EOF
usage: $0 -options

This script makes sqlite replicas of the Tile/Calo COOL databse.
Options:
-h   Help
-t   Copy Tile folders
-c   Copy Calo folders
EOF
}

# Check number of arguments
if [ $# -eq 0 ]
then
	usage
	exit 1
fi


while getopts "htc" OPTION
do
	case $OPTION in
		h)
			usage
			exit 1
			;;
		t)
			COPY_TILE=1
			;;
		c)
			COPY_CALO=1
			;;
	esac
done

if [ $COPY_TILE ]
then
	# Make tileSqlite.db
	  # DQ folder
	AtlCoolCopy.exe "COOLONL_TILE/COMP200" "sqlite://;schema=tileSqlite.db;dbname=COMP200" -create -copytaglock -f /TILE/OFL01/STATUS/ADC   -t TileOfl01StatusAdc-HLT-UPD1-00
	  # Digi folders
	AtlCoolCopy.exe "COOLONL_TILE/COMP200" "sqlite://;schema=tileSqlite.db;dbname=COMP200" -create -copytaglock -f /TILE/OFL01/NOISE/SAMPLE -t TileOfl01NoiseSample-HLT-UPD1-01
	AtlCoolCopy.exe "COOLONL_TILE/COMP200" "sqlite://;schema=tileSqlite.db;dbname=COMP200" -create -copytaglock -f /TILE/OFL01/NOISE/AUTOCR -t TileOfl01NoiseAutocr-HLT-UPD1-00
	  # Channel folders
	AtlCoolCopy.exe "COOLONL_TILE/COMP200" "sqlite://;schema=tileSqlite.db;dbname=COMP200" -create -copytaglock -f /TILE/OFL01/NOISE/FIT    -t TileOfl01NoiseFit-HLT-UPD1-01
	AtlCoolCopy.exe "COOLONL_TILE/COMP200" "sqlite://;schema=tileSqlite.db;dbname=COMP200" -create -copytaglock -f /TILE/OFL01/NOISE/OF1    -t TileOfl01NoiseOf1-HLT-UPD1-01
	AtlCoolCopy.exe "COOLONL_TILE/COMP200" "sqlite://;schema=tileSqlite.db;dbname=COMP200" -create -copytaglock -f /TILE/OFL01/NOISE/OF2    -t TileOfl01NoiseOf2-HLT-UPD1-01
	  
fi

if [ $COPY_CALO ]
then
	# Make caloSqlite.db
	  # Cell folder
	AtlCoolCopy.exe "COOLONL_CALO/COMP200" "sqlite://;schema=caloSqlite.db;dbname=COMP200" -create -copytaglock -f /CALO/Noise/CellNoise -t CaloNoiseCellnoise-UPD3-00
fi

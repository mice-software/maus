all:
	rootcint -f DataDict.C -c struct.h LinkDef.h
	g++ -o main main.cxx DataDict.C MsgStream.cxx -I. `root-config --libs --cflags`
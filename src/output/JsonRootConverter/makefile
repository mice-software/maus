all:
	rootcint -f DigitsStructsDict.C -c DigitsStructs.h Digits_LinkDef.h
	g++ -o main main.cxx JsonRootConverter.cxx ProcessorFactory.cxx DigitsProcessor.cxx DigitsStructsDict.C ProcessorBase.cxx MsgStream.cxx -L/home/hep/arichard/Maus/maus/third_party/install/lib -ljson -I/home/hep/arichard/Maus/maus/third_party/install/include `root-config --libs --cflags` 
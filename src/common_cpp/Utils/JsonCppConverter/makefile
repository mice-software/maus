DEPS = DigitsProcessor.h MCProcessor.h JsonCppConverter.h DigitsStructs.h MCStructs.h MausEventStruct.h ConverterBase.h
OBJ = DigitsStructs.o MCStructs.o MausEventStruct.o DigitsProcessor.o MCProcessor.o JsonCppConverter.o #DigitsStructsDict.o MCStructsDict.o MausEventStructDict.o
SO = libJsonCppConverter.so
all: $(SO)
	rootcint -f DigitsStructsDict.C -c DigitsStructs.h Digits_LinkDef.h
	rootcint -f MCStructsDict.C -c MCStructs.h MC_LinkDef.h
	rootcint -f MausEventStructDict.C -c MausEventStruct.h MausEvent_LinkDef.h
	g++ -o main main.cxx DigitsStructsDict.C MCStructsDict.C MausEventStructDict.C /home/hep/arichard/Maus/maus/src/legacy/Interface/Squeak.cc /home/hep/arichard/Maus/maus/src/legacy/Interface/Squeal.cc -L. -lJsonCppConverter -I. `root-config --libs --cflags` -L/home/hep/arichard/Maus/maus/third_party/install/lib -ljson -I/home/hep/arichard/Maus/maus -I/home/hep/arichard/Maus/maus/src/common_cpp -I/home/hep/arichard/Maus/maus/src/legacy -I/home/hep/arichard/Maus/maus/third_party/install/include `alex-config --libs --cflags RootStreamer`

# the $ @ and $ ^ mean the contents before and after the : respectively
$(SO): $(OBJ)
	g++ -shared -fPIC -Wl,$@ -o $@ $^ -L/home/hep/arichard/Maus/maus/third_party/install/lib -ljson -I/home/hep/arichard/Maus/maus/src/common_cpp -I/home/hep/arichard/Maus/maus/src/legacy -I/home/hep/arichard/Maus/maus/third_party/install/include `root-config --libs --cflags` -I.
	rm -f *.o

#$ < is the first item in the dependencies list, i.e. the cxx file
%Structs.o: %Structs.cxx $(DEPS)
	g++ -c -fPIC -o $@ $< `root-config --libs --cflags` -I.

%Struct.o: %Struct.cxx $(DEPS)
	g++ -c -fPIC -o $@ $< `root-config --libs --cflags` -I.


%Processor.o: %Processor.cxx $(DEPS)
	g++ -c -fPIC -o $@ $< -L/home/hep/arichard/Maus/maus/third_party/install/lib -ljson -I/home/hep/arichard/Maus/maus/src/common_cpp -I/home/hep/arichard/Maus/maus/src/legacy -I/home/hep/arichard/Maus/maus/third_party/install/include `root-config --libs --cflags` -I.

JsonCppConverter.o: JsonCppConverter.cxx $(DEPS)
	g++ -c -fPIC -o $@ $< -L/home/hep/arichard/Maus/maus/third_party/install/lib -ljson -I/home/hep/arichard/Maus/maus/src/common_cpp -I/home/hep/arichard/Maus/maus/src/legacy -I/home/hep/arichard/Maus/maus/third_party/install/include `root-config --libs --cflags` -I.

clean:
	rm -f *.so
	rm -f *.d
	rm -f main
	rm -f *Dict.C
	rm -f *Dict.h
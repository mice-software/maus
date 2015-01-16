CPPFLAGS := -I${MAUS_ROOT_DIR}/src/common_cpp -I${MAUS_ROOT_DIR}/bin/examples/poly_patch
OBJ_FILES :=  $(patsubst rogers_math/%.cc,object/%.o,$(wildcard rogers_math/*.cc))

object/%.o : rogers_math/%.cc
	@echo "$(OBJ_FILES)"
	$(CXX) -c $(CPPFLAGS) $< -o $@

all : $(OBJ_FILES)
	@echo "Running with ${MAUS_ROOT_DIR} on "
	@echo `uname -a` # note this Makefile fails on Ubuntu due to ROOT errors
	g++ main.cc -o poly_patch \
	                      -I${MAUS_ROOT_DIR}/ \
	                      -I${MAUS_ROOT_DIR}/src/common_cpp \
	                      -I${MAUS_ROOT_DIR}/bin/examples/poly_patch \
	                      -I${MAUS_THIRD_PARTY}/third_party/install/include \
	                      -I${ROOTSYS}/include \
	                      -L${MAUS_ROOT_DIR}/build/ \
	                      -L${MAUS_THIRD_PARTY}/third_party/install/lib \
	                      $(OBJ_FILES) \
	                      `root-config --ldflags` \
	                      `${ROOTSYS}/bin/root-config --glibs` \
	                      -lMausCpp \
	                      -lgsl \
	                      -lgtest \
	                       -Wl,--no-as-needed

clean:
	rm load_root_file;
	rm *.png;

load_root_file :
	@echo "Running with ${MAUS_ROOT_DIR} on "
	@echo `uname -a` # note this Makefile fails on Ubuntu due to ROOT errors
	g++ test_reduce_cpp_tilted_helix.cc -o test_reduce_cpp_tilted_helix \
	                      -I${MAUS_ROOT_DIR}/ \
	                      -I${MAUS_ROOT_DIR}/src/common_cpp \
	                      -I${MAUS_ROOT_DIR}/src/legacy \
	                      -I${MAUS_THIRD_PARTY}/third_party/install/include \
	                      -I${MAUS_THIRD_PARTY}/third_party/install/include/python2.7 \
	                      -I${ROOTSYS}/include \
	                      -L${MAUS_ROOT_DIR}/build/ \
	                      `root-config --ldflags` \
	                      `${ROOTSYS}/bin/root-config --glibs` \
	                      -lMausCpp \
	                       -Wl,--no-as-needed

clean:
	rm load_root_file;
	rm *.png;

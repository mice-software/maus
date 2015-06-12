load_root_file :
	@echo "Running with ${MAUS_ROOT_DIR} on "
	@echo `uname -a` # note this Makefile fails on Ubuntu due to ROOT errors
	g++ load_root_file.cc -o load_root_file \
	                      -I${MAUS_ROOT_DIR}/ \
	                      -I${MAUS_ROOT_DIR}/src/common_cpp \
	                      -I${MAUS_ROOT_DIR}/src/legacy \
	                      -I${MAUS_THIRD_PARTY}/third_party/install/include \
	                      -I${MAUS_THIRD_PARTY}/third_party/install/include/root \
	                      -I${ROOTSYS}/include \
	                      -L${MAUS_ROOT_DIR}/build/ \
	                      `root-config --ldflags` \
	                      `${ROOTSYS}/bin/root-config --glibs` \
	                      -lMausCpp \
	                       -Wl,--no-as-needed

clean:
	rm load_root_file;
	rm *.png;

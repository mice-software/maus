CC = g++
CFLAGS = -DDEBUG -ggdb -Wall

all : scifi_offline_viewer scifi_analysis scifi_momentum_analysis_new

scifi_offline_viewer : scifi_offline_viewer.cc
	@echo "Running with ${MAUS_ROOT_DIR} on "
	@echo `uname -a` # note this Makefile fails on Ubuntu due to ROOT errors
	$(CC) $(CFLAGS) scifi_offline_viewer.cc -o scifi_offline_viewer \
	                      -I${MAUS_ROOT_DIR}/ \
	                      -I${MAUS_ROOT_DIR}/src/common_cpp \
	                      -I${MAUS_ROOT_DIR}/src/legacy \
	                      -I${MAUS_THIRD_PARTY}/third_party/install/include \
	                      -I${ROOTSYS}/include \
	                      -L${MAUS_ROOT_DIR}/build/ \
	                      `root-config --ldflags` \
	                      `${ROOTSYS}/bin/root-config --glibs` \
	                      -lMausCpp \
	                       -Wl,--no-as-needed

scifi_analysis : scifi_analysis.cc
	$(CC) $(CFLAGS) scifi_analysis.cc -o scifi_analysis \
	                      -I${MAUS_ROOT_DIR}/ \
	                      -I${MAUS_ROOT_DIR}/src/common_cpp \
	                      -I${MAUS_ROOT_DIR}/src/legacy \
	                      -I${MAUS_THIRD_PARTY}/third_party/install/include \
	                      -I${ROOTSYS}/include \
	                      -L${MAUS_ROOT_DIR}/build/ \
	                      `root-config --ldflags` \
	                      `${ROOTSYS}/bin/root-config --glibs` \
	                      -lMausCpp \
	                       -Wl,--no-as-needed


scifi_momentum_analysis_new : scifi_momentum_analysis_new.cc
	@echo "Running with ${MAUS_ROOT_DIR} on "
	@echo `uname -a` # note this Makefile fails on Ubuntu due to ROOT errors
	$(CC) $(CFLAGS) scifi_momentum_analysis_new.cc -o scifi_momentum_analysis_new \
	                      -I${MAUS_ROOT_DIR}/ \
	                      -I${MAUS_ROOT_DIR}/src/common_cpp \
	                      -I${MAUS_ROOT_DIR}/src/legacy \
	                      -I${MAUS_THIRD_PARTY}/third_party/install/include \
	                      -I${ROOTSYS}/include \
	                      -L${MAUS_ROOT_DIR}/build/ \
	                      `root-config --ldflags` \
	                      `${ROOTSYS}/bin/root-config --glibs` \
	                      -lMausCpp \
	                       -Wl,--no-as-needed

clean:
	rm scifi_offline_viewer scifi_analysis scifi_momentum_analysis_new;


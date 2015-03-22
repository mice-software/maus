


PROGRAMS  = emittance_recon_example

INCLUDES  = -I${MAUS_ROOT_DIR}/ \
            -I${MAUS_ROOT_DIR}/src/common_cpp \
	          -I${MAUS_ROOT_DIR}/src/legacy \
	          -I${MAUS_ROOT_DIR}/src \
	          -I${MAUS_THIRD_PARTY}/third_party/install/include \
	          -I${ROOTSYS}/include

LIBRARIES = -L${MAUS_ROOT_DIR}/build/ \
	          `root-config --ldflags` \
	          `${ROOTSYS}/bin/root-config --glibs` \
	          -lMausCpp

OPTIONS   = -g -Wl,--no-as-needed




.PHONY : variable_check _all all clean 




all : variable_check ${PROGRAMS}


variable_check :
	@if [ -z ${MAUS_ROOT_DIR} ] ; then                      \
      echo "MAUS_ROOT_DIR Not Set."                      ;\
      echo "Please Source the \"env.sh\" script."        ;\
      exit 1                                             ;\
    else                                                  \
      echo "MAUS_ROOT_DIR Set to ${MAUS_ROOT_DIR}"       ;\
      echo "On `uname -a`"                               ;\
      echo "Building, Please Wait..."                    ;\
      echo                                               ;\
    fi


${PROGRAMS} : % : %.cc
	g++ -o $@ $^ ${OPTIONS} ${INCLUDES} ${LIBRARIES}

clean:
	rm -f ${PROGRAMS}


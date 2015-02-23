CPPFLAGS := -I${MAUS_ROOT_DIR}/src/common_cpp -I${MAUS_ROOT_DIR}/bin/examples/poly_patch -g -pg -Wall -I${MAUS_THIRD_PARTY}/third_party/install/include -I${ROOTSYS}/include \

MATH_OBJ_FILES :=  $(patsubst math/%.cc,object/%.o,$(wildcard math/*.cc))

TEST_OBJ_FILES :=  $(patsubst test/%.cc,object/%.o,$(wildcard test/*.cc))

object/%.o : test/%.cc
	@echo "Test files\n$(TEST_OBJ_FILES)"
	$(CXX) -c $(CPPFLAGS) $< -o $@

object/%.o : math/%.cc
	@echo "$Math files\n(MATH_OBJ_FILES)"
	$(CXX) -c $(CPPFLAGS) $< -o $@

all : $(TEST_OBJ_FILES) $(MATH_OBJ_FILES)
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
	                      $(MATH_OBJ_FILES) \
	                      $(TEST_OBJ_FILES) \
	                      `root-config --ldflags` \
	                      `root-config --glibs` \
	                      -lMausCpp \
	                      -lgsl \
	                      -lgtest \
	                       -Wl,--no-as-needed -g -pg

clean:
	rm $(MATH_OBJ_FILES);
	rm $(TEST_OBJ_FILES);
	rm poly_patch;


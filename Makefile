#   A Makefile that just links into scons.  This is provided to help
# people who are used to make.

all:
	scons
	@echo 'You can run "scons" directly to compile MAUS.'

doc:
	scons doc
	@echo 'You can run "scons doc" directly to extract/generate docs.'

clean:
	scons -c
	@echo 'You can run "scons -c" directly to clean build.'

.PHONY: doc clean all

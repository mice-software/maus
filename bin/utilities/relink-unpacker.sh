#!/bin/sh

if [ "${MAUS_ROOT_DIR}" == "" ]; then
   echo "You must source env.sh first."
   exit 1
fi

if [ "$MAUS_UNPACKER_VERSION" == "" ]; then
   echo "Hmm...something is wrong. I can't determine MAUS_UNPACKER_VERSION."
   echo "Please make sure you are running a MAUS version > 0.9.3"
   exit 1
else
   unpack_vn=${MAUS_UNPACKER_VERSION}
fi

# the directory in which the libraries go
libdir=${MAUS_THIRD_PARTY}/third_party/install/lib

# name of the softlink
linkname="libMDUnpack.so"

# names of the libraries built by 53unpacking.sh
unp_step1="libMDUnpack_StepI.so"
unp_step4="libMDUnpack_StepIV.so"

# if there is no soft link either maus not installed or unpacker installation failed
if [ ! -L ${libdir}/$linkname ]; then
   echo "I can't find a link from ${libdir}/${linkname}"
   echo "Make sure you have have run the MAUS installer. Or re-run third_party/bash/53unpacking.bash"
   exit 1
else
   # make sure that the step1 and step4 libraries exist
   if [ ! -e ${libdir}/$unp_step1 ] \
      && [ ! -e ${libdir}/$unp_step4 ]; then
       echo "Could not find $unp_step1 and $unp_step4 libraries. Make sure you have run the third party installation"
   else
       # what does the current link point to
       cur_unp_vn=`readlink ${libdir}/$linkname`
       # the library we want to link to based on MAUS_UNPACKER_VERSION
       req_unp_vn="libMDUnpack_${MAUS_UNPACKER_VERSION}.so" 
       if [ "$cur_unp_vn" == "$req_unp_vn" ]; then
           echo ""
           echo "Hmm the version you want $MAUS_UNPACKER_VERSION is already linked"
           echo ""
           ls -l ${libdir}/$linkname
           echo ""
           echo "If you changed the unpacker version in configure or env.sh you must re-source the environment"
           exit
       fi
       echo "Linking to $req_unp_vn"
       cd ${libdir}
       # remove the softlink
       ln -sfn $req_unp_vn $linkname
       echo "Done"
       cd ${MAUS_ROOT_DIR}
       ls -l ${libdir}/$linkname
   fi
fi

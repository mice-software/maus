#!/bin/bash
# ROOT
if [ -n "${MAUS_ROOT_DIR+x}" ]; then

if [ -e "${MAUS_ROOT_DIR}/third_party/source/Python-2.7.tgz" ]
then
  echo "Python exists"
else
  echo "Python doesn't exist.  Downloading..."

  url=http://www.python.org/ftp/python/2.7/Python-2.7.tgz

  #wget --directory-prefix=${MAUS_ROOT_DIR}/third_party/source http://micewww.pp.rl.ac.uk:8080/attachments/download/72/Python-2.7.tgz
  wget --directory-prefix=${MAUS_ROOT_DIR}/third_party/source ${url}

  echo "These next two strings should agree (otherwise the file didn't download properly):"
  md5sum ${MAUS_ROOT_DIR}/third_party/source/Python-2.7.tgz
  cat ${MAUS_ROOT_DIR}/third_party/md5/Python-2.7.tgz.md5 

  echo
  echo "but if they don't agree, then please run:"
  echo
  echo "rm ${MAUS_ROOT_DIR}/third_party/source/Python-2.7.tgz"
  echo 
  echo "then download the file manually and put it in the 'source' directory"

fi

else
echo MAUS_ROOT_DIR is not set.  Please run: ./configure
fi
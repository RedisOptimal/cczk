#! /bin/bash
if [ -e debug -a -d debug ]; then
  cd debug
  rm -rf *
else
  mkdir debug
  cd debug
fi

echo "###############"
echo "Testing in Unit"
echo "###############"

BASE_ENV=~/env

cmake -DCMAKE_BUILD_TYPE=DEBUG -DCMAKE_PREFIX_PATH=$BASE_ENV -DCMAKE_INSTALL_PREFIX=$BASE_ENV ..
make
#ctest -D Experimental
#ctest -D ExperimentalMemCheck
#ctest -D ExperimentalSubmit

echo "###############"
echo "Deploy"
echo "###############"

#cmake -DCMAKE_BUILD_TYPE=RELEASE -DCMAKE_PREFIX_PATH=$BASE_ENV -DCMAKE_INSTALL_PREFIX=$BASE_ENV ..
#make
#make install

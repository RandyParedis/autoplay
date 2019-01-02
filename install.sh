#!/usr/bin/env bash

set -e

b=$(tput bold)
n=$(tput sgr0)
root=$(pwd)

# Load .config.ini
. ./.config.ini

# Get OS
os="Unknown"
if [[ "$(uname)" == "Darwin" ]]; then
    # Do something under Mac OS X platform
    os="Mac OSX"
elif [[ "$(expr substr $(uname -s) 1 5)" == "Linux" ]]; then
    # Do something under GNU/Linux platform
    os="Linux"
elif [[ "$(expr substr $(uname -s) 1 10)" == "MINGW32_NT" ]]; then
    # Do something under 32 bits Windows NT platform
    os="Windows32"
elif [[ "$(expr substr $(uname -s) 1 10)" == "MINGW64_NT" ]]; then
    # Do something under 64 bits Windows NT platform
    os="Windows64"
fi

echo "Installing ${b}autoplay${n}"
echo "  - ${b}autoplay version:${n} ${AUTOPLAY_VERSION}"
echo "  - ${b}OS type:${n}          ${os}"
if [[ "$os" == "Windows32" || "$os" == "Windows64" ]]; then
    echo "${b}autoplay${n} does not support Windows!"
fi

echo ""


# -- GTEST
if [[ "${GTEST_LOC}" == "" ]]; then
    GTEST_LOC=/tmp
fi
if [[ -f /usr/lib/libgtest.a ]]; then
    echo "  - ${b}GTest 1.8.0${n} dependency found."
else
    echo "  - Installing ${b}GTest 1.8.0${n} in ${b}${GTEST_LOC}${n}..."
    cd ${GTEST_LOC}
    sudo wget https://github.com/google/googletest/archive/release-1.8.0.tar.gz
    sudo tar xf release-1.8.0.tar.gz
    cd googletest-release-1.8.0/googletest
    sudo mkdir -p bld; cd bld

    set +e
    sudo cmake ..
    sudo make
    sudo cp -a ../include/gtest /usr/include
    sudo cp -a *.a /usr/lib/
    set -e
    
    cd ${root}
    echo "  - Installed ${b}GTest 1.8.0${n} in ${b}${GTEST_LOC}${n}."
fi


# -- BOOST
boost_dir=/usr/include/boost
if [[ "${BOOST_ROOT}" == "" ]]; then
    BOOST_ROOT=${boost_dir}
fi
boost_install_dir=${boost_dir}-install

if [[ -d ${boost_dir} ]]; then
    echo "  - ${b}Boost 1.66.0${n} dependency found. (To reinstall, please remove the ${b}${boost_dir}${n} directory)"
else
    echo "  - Installing ${b}Boost 1.66.0${n} in ${b}${boost_dir}${n}..."
    export BOOST_NO_SYSTEM_PATHS=ON
    if [[ -z "$(ls -A ${BOOST_ROOT})" ]]; then
        cd ${boost_install_dir}
        wget https://dl.bintray.com/boostorg/release/1.66.0/source/boost_1_66_0.tar.gz -q
        tar xf boost_1_66_0.tar.gz > /dev/null 2>&1
        cd boost_1_66_0/
        ./bootstrap.sh --prefix=${BOOST_ROOT} --with-libraries=chrono,date_time,filesystem,iostreams,locale,regex,system,thread
        ./b2 -q -a -j2 -d0 --disable-filesystem2 cxxflags="-v -std=c++11" threading=multi install
        cd ${root};
    else
        echo " => Already have Boost cache"
    fi
    export LD_LIBRARY_PATH=${BOOST_ROOT}/lib:${LD_LIBRARY_PATH}
    sudo rm -f /usr/local/lib/libboost*.dylib*
    sudo ln -s ${BOOST_ROOT}/lib/*.{so,dylib}* /usr/local/lib
    echo "  - Installed ${b}Boost 1.66.0${n} in ${b}${boost_dir}${n}."
fi


# -- TRNG (Tina's Random Number Generator)
if [[ "${TRNG_LOC}" == "" ]]; then
    TRNG_LOC=${root}/deps/trng
fi
trng_dir=${TRNG_LOC}
trng_install_dir=${root}/dependencies/trng

if [[ -d ${trng_dir} ]]; then
    echo "  - ${b}TRNG${n} dependency found. (To reinstall, please remove the ${b}${trng_dir}${n} directory)"
else
    echo "  - Installing ${b}TRNG${n} in ${b}${trng_dir}${n}..."
    mkdir -p ${trng_dir}
    cd ${trng_install_dir}
    autoreconf --force --install
    sudo ./configure --prefix=${trng_dir}
    sudo make && sudo make install
    cd ${root}
    echo "  - Installed ${b}TRNG${n} in ${b}${trng_dir}${n}."
fi


# -- ALSA (RtMIDI)
if [[ "${os}" == "Linux" ]]; then
    echo "  - Fetching ${b}ALSA${n}..."
    sudo apt-get install -y libasound-dev alsa-base alsa-oss
fi

# -- autoplay
echo "  - Installing ${b}autoplay${n}..."
mkdir build; cd build
cmake -DTRNG_LOCATION=${trng_dir} ..
make install
cd ${root}
echo "  - Installed ${b}autoplay${n}..."
# Matthew M Reid. install open mpi shell script
 
# install destination
installDIR="/usr/local"
# First get necessary dependencies
sudo apt-get update
sudo apt-get install -y autotools-dev autoconf-dev automake-dev g++ build-essential gfortran
# remove any obsolete libraries
sudo apt-get autoremove
 
# Build using maximum number of physical cores
n=`cat /proc/cpuinfo | grep "cpu cores" | uniq | awk '{print $NF}'`
 
# grab the necessary files
wget http://www.open-mpi.org/software/ompi/v1.6/downloads/openmpi-1.6.5.tar.gz
tar xzvf openmpi-1.6.5.tar.gz
cd openmpi-1.6.5
 
echo "Beginning the installation..."
./configure --prefix=$installDIR
make -j $n
make install
# with environment set do ldconfig
sudo ldconfig
echo
echo "...done."

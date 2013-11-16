osrocks
=======

This is a class project for SJSU FA13 CMPE 180-94 by the team OS Rocks   


Steps to build & run the server program in LINUX:

1. Get a copy of server1.cpp from the project GitHub repository (by either forking the whole repository or just copy & paste the code). 

2. Install g++ compiler 

3. Download boost (http://sourceforge.net/projects/boost/files/boost/1.55.0/) and unzip the file

4. In the boost root directory (*path of unzipped folder*/boost_1_55_0/), run bootstrap program with the option of building regex library only: 
$ ./bootstrap.sh --with-libraries=regex

5. In boost root directory, run the installation program: 
$ ./b2 install

6. Change settings in IDE (set include directory and library directory) before using IDE integrated builder
OR use the following compiler & linker command to build server1.cpp:
$ g++ -I*boost root directory*  -O0 -g3 -Wall -c -fmessage-length=0 -o server.o server1.cpp
$ g++ -L*boost root directory*/stage/lib -o "server.exe" server.o -lboost_regex 

7. Set the value of environment variable:
$ export LD_LIBRARY_PATH=*boost root directory*/stage/lib

8. Run the server program:
$ ./server.exe




Steps to build & run the client program in LINUX:

1. Get a copy of multiclient.cpp as well as openmpi_install.sh from the project GitHub repository (by either forking the whole repository or just copy & paste the code). 

2. In the direcotry that contains openmpi_installer.sh, change the execution permission of this file:
$ sudo chmod 775 openmpi_installer.sh

3. In the direcotry that contains openmpi_installer.sh, run the script to get & build openmpi and set up its runtime:
$ ./openmpi_installer.sh

4. In the directory that contains multiclient.cpp, build the client program using MPI c++ builder:
$ mpic++ multiclient.cpp -o client.exe

5. Run the client program under MPI framework:
$ mpirun -np *number of client process to be created* ./client.exe *ip address of server*

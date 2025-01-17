# Linux Toolchain Installation

This toolchain installation has been verified on x64 Ubuntu 22.04.2 LTS

## Getting a C++ compiler

`sudo apt-get install build-essential`

## Installing cmake

* `sudo apt-get install cmake`

## Dependencies

### Install and Build gRPC and Protobufs

Carefully follow [these steps](https://grpc.io/docs/languages/cpp/quickstart/)

### Install and Build jwt-cpp

1. Install openSSL eg. `sudo apt-get install libssl-dev`
2. Run `git clone https://github.com/Thalhammer/jwt-cpp && cd jwt-cpp && mkdir build && cd build && cmake -DCMAKE_BUILD_TYPE=Release ..  && make && sudo make install`

## Optionally Install doxygen

`sudo apt-get install doxygen`

### graphviz installation

Doxygen can add inheritance diagrams and call trees if the program called `dot`
which is part of graphviz is installed.

`sudo apt-get install graphviz`

## Optionally Install Google Test

```
sudo apt-get install libgtest-dev
cd /usr/src/gtest
sudo cmake CMakeLists.txt
sudo make
sudo cp lib/*.a /usr/lib
``` 

To build without Google Test, empty the build folder and run
`cmake .. -G Ninja -DUNIT_TESTING=OFF`

## Clone Catena and install node
```
git clone https://github.com/rossvideo/Catena.git
cd ~/Catena/tools/codegen
node -v # Node must be version 14 or above
npm install
```
If your version of node is below 14 then update it to the latest version with the following commands:
```
sudo apt-get update
sudo apt-get install -y nodejs npm
sudo npm cache clean -f
sudo npm install -g n
sudo n stable
```
Restart the terminal and run `node -v` to check that node successfully  updated.

## Building Catena
```
cd ~/Catena/sdks/cpp
mkdir build
cd build
cmake .. -G Ninja \
	-D CMAKE_BUILD_TYPE=Release   \
    -D CONNECTIONS=gRPC
ninja
```


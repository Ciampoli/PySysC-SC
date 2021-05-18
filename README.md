# PySysC-SC

A simple C++/SystemC/CMake project to test pysysc

# How to build

> Currently only Linux and MacOS are tested


```
cd PySysC-SC
mkdir build
cd build
conan install .. --build=missing
cmake -DBUILD_SHARED_LIBS=ON ..
cmake --build .
```



# PySysC-SC

A simple C++/SystemC/CMake project to test pysysc

# How to build

> Currently only Linux and MacOS are tested


```
pip install conan
conan remote add minres https://api.bintray.com/conan/minres/conan-repo
cd PySysC-SC
mkdir build
cd build
conan install .. --build=missing
cmake -DBUILD_SHARED_LIBS=ON ..
cmake --build .
```

## Notes

If you encounter issues when linking wrt. c++11 symbols you might have run into GCC ABI incompatibility introduced from GCC 5.0 onwards. You can fix this by adding '-s compiler.libcxx=libstdc++11' to the conan call or changing compiler.libcxx to
```
compiler.libcxx=libstdc++11
```
in $HOME/.conan/profiles/default or run

```
conan profile update settings.compiler.libcxx=libstdc++11 default
```


# PySysC-SC

A simple C++/SystemC/CMake project to test pysysc

# How to build

> Currently only Linux and MacOS are tested. 

The install instruction below assume using bash.

```
git clone --recursive -b develop https://git.minres.com/SystemC/PySysC-SC.git
cd PySysC-SC
python3 -mvenv .venv
. .venv/bin/activate
pip install "conan<2.0"
cmake -S . -B build -DBUILD_SHARED_LIBS=ON -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
cmake --build build -j24
export SYSTEMC_HOME=$(grep -A1 rootpath_systemc\] build/conanbuildinfo.txt | tail -1)
export STDCXX=17
pip install "cppyy<3.0" 
pip install https://github.com/Minres/PySysC/tarball/master
pip install scc/contrib/pysysc/
python3 router_example.py
python3 router_example2.py
python3 modules.py
```

These steps clone the PySysC-SC repository and builds the shared libraries using conan 1.x.
This also downloads and builds SYSTEMC (as a conan package).
Then it installs the python packages cppyy, PySysC, and PySysC.SCC using the same settings as the cmake build.
Eventually it runs the 3 examples.

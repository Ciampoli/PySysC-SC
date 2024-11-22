# PySysC-SC

A simple C++/SystemC/CMake project to test pysysc

# How to build

> Currently only Linux and MacOS are tested. 

The install instruction below assume using bash.

```sh
# clone the PySysC-SC repository 
git clone --recursive -b develop https://git.minres.com/SystemC/PySysC-SC.git
cd PySysC-SC
# install conan 1.x into virtual environment
python3 -mvenv .venv
. .venv/bin/activate
pip install "conan<2.0"
# build the SystemC libraries needed
cmake -S . -B build -DBUILD_SHARED_LIBS=ON -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
cmake --build build -j24
# install cppyy and PySysC
export SYSTEMC_HOME=$(grep -A1 rootpath_systemc\] build/conanbuildinfo.txt | tail -1)
export STDCXX=17
pip install "cppyy<3.0" 
pip install https://github.com/Minres/PySysC/tarball/master
pip install scc/contrib/pysysc/
# run the exampoes
python3 router_example.py
python3 router_example2.py
python3 modules.py
```

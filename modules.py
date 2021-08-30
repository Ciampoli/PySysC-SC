#
# Copyright (c) 2019 -2021 MINRES Technolgies GmbH
#
# SPDX-License-Identifier: Apache-2.0
#

import os.path
import logging
import cppyy
from cppyy import gbl as cpp
import pysysc
from pysysc.structural import Connection, Module, Signal, Simulation

###############################################################################
# setup  and load
###############################################################################
logging.basicConfig(level=logging.DEBUG)
build_type='Debug'
###############################################################################
myDir = os.path.dirname( os.path.realpath(__file__))
pysysc.read_config_from_conan(os.path.join(myDir, 'build/%s/conanfile.txt'%build_type), build_type)
pysysc.load_systemc()
###############################################################################
logging.debug("Loading SC-Components lib")
# pysysc.add_include_path(os.path.join(myDir, 'scc/incl'))
# pysysc.add_library('scc.h', os.path.join(myDir, 'build/%s/lib/libscc.so'%build_type))
pysysc.add_include_path(os.path.join(myDir, 'scc/src/common'))
pysysc.add_library('scc_util.h', os.path.join(myDir, 'build/%s/scc/src/common/libscc-util.so'%build_type))
pysysc.add_include_path(os.path.join(myDir, 'scc/third_party'))
pysysc.add_include_path(os.path.join(myDir, 'scc/third_party/scv-tr/src'))
pysysc.add_library('scv-tr.h', os.path.join(myDir, 'build/%s/scc/third_party/scv-tr/src/libscv-tr.so'%build_type))
pysysc.add_include_path(os.path.join(myDir, 'scc/src/sysc'))
pysysc.add_library('scc_sysc.h', os.path.join(myDir, 'build/%s/scc/src/sysc/libscc-sysc.so'%build_type))
pysysc.add_include_path(os.path.join(myDir, 'scc/src/components'))
cppyy.include('scc_components.h')
###############################################################################
logging.debug("Loading Components lib")
pysysc.add_include_path(os.path.join(myDir, 'vp_components'))
pysysc.add_library('components.h', os.path.join(myDir, 'build/%s/vp_components/libvp_components.so'%build_type))

###############################################################################
# define toplevel class
###############################################################################
num_of_mem = 4

from pysysc.sysc import ScModule
class TopModule(ScModule):
    
    def __init__(self, name):
        ScModule.__init__(self, name)
        ###############################################################################
        # instantiate
        ###############################################################################
        self.clk_gen = Module(cpp.ClkGen).create("clk_gen")
        self.rst_gen = Module(cpp.ResetGen).create("rst_gen")
        self.initiator = Module(cpp.Initiator).create("initiator")
        self.memories = [Module(cpp.Memory).create("mem%d"%idx) for idx in range(0,num_of_mem)]
        self.router = Module(cpp.Router[num_of_mem]).create("router")
        ###############################################################################
        # connect them
        ###############################################################################
        self.clk = Signal("clk").src(self.clk_gen.clk_o).sink(self.initiator.clk_i).sink(self.router.clk_i)
        [self.clk.sink(m.clk_i) for m in self.memories]
        self.rst = Signal("rst").src(self.rst_gen.reset_o).sink(self.initiator.reset_i).sink(self.router.reset_i)
        [self.rst.sink(m.reset_i) for m in self.memories]
        Connection().src(self.initiator.socket).sink(self.router.target_socket)
        [Connection().src(self.router.initiator_socket.at(idx)).sink(m.socket) for idx,m in enumerate(self.memories)]
        self.ScThread("RunThread")
        
    def EndOfElaboration(self):
        print("Elaboration finished")
        
    def StartOfSimulation(self):
        print("Simulation started")
        
    def EndOfSimulation(self):
        print("Simulation finished")

    def RunThread(self):
        print("Starting RunThread")
        while(cpp.sc_core.sc_time_stamp()<cpp.sc_core.sc_time(500, cpp.sc_core.SC_NS)):
            #self.pyScWait(self.clk.signal.value_changed_event())
            self.ScWait(cpp.sc_core.sc_time(100, cpp.sc_core.SC_NS))
            print("Hello from Thread %s, @ %s"%(self.name(), cpp.sc_core.sc_time_stamp().to_string()))
###############################################################################
# configure
###############################################################################
Simulation.setup(logging.root.level)
###############################################################################
# instantiate
###############################################################################
#from modules import TopModule
dut = Module(TopModule).create("dut")
###############################################################################
# run if it is standalone
###############################################################################
if __name__ == "__main__":
    Simulation.configure(enable_vcd=False)
    Simulation.run()
    logging.debug("Done")

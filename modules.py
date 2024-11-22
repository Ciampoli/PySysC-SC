#
# Copyright (c) 2019 -2021 MINRES Technolgies GmbH
#
# SPDX-License-Identifier: Apache-2.0
#

import logging
import os.path

import pysysc
import pysysc.scc as scc
from cppyy import gbl as cpp
from pysysc.structural import Connection, Module, Signal, Simulation

###############################################################################
# setup  and load
###############################################################################
logging.basicConfig(level=logging.DEBUG)
###############################################################################
myDir = os.path.dirname(os.path.realpath(__file__))
pysysc.load_systemc(17)
###############################################################################
logging.debug("Loading SC-Components lib")
scc.load_lib(myDir)
###############################################################################
logging.debug("Loading Components lib")
pysysc.add_include_path(os.path.join(myDir, "vp_components"))
pysysc.add_library("components.h", "libvp_components.so", myDir)
###############################################################################
# configure
###############################################################################
scc.setup(logging.root.level)
scc.configure(enable_trace=False)
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
        self.memories = [
            Module(cpp.Memory).create("mem%d" % idx) for idx in range(0, num_of_mem)
        ]
        self.router = Module(cpp.Router[num_of_mem]).create("router")
        ###############################################################################
        # connect them
        ###############################################################################
        self.clk = (
            Signal("clk")
            .src(self.clk_gen.clk_o)
            .sink(self.initiator.clk_i)
            .sink(self.router.clk_i)
        )
        [self.clk.sink(m.clk_i) for m in self.memories]
        self.rst = (
            Signal("rst")
            .src(self.rst_gen.reset_o)
            .sink(self.initiator.reset_i)
            .sink(self.router.reset_i)
        )
        [self.rst.sink(m.reset_i) for m in self.memories]
        Connection().src(self.initiator.socket).sink(self.router.target_socket)
        [
            Connection().src(self.router.initiator_socket.at(idx)).sink(m.socket)
            for idx, m in enumerate(self.memories)
        ]
        self.ScThread("RunThread")

    def EndOfElaboration(self):
        print("Elaboration finished")

    def StartOfSimulation(self):
        print("Simulation started")

    def EndOfSimulation(self):
        print("Simulation finished")

    def RunThread(self):
        print("Starting RunThread")
        while cpp.sc_core.sc_time_stamp() < cpp.sc_core.sc_time(500, cpp.sc_core.SC_NS):
            # self.pyScWait(self.clk.signal.value_changed_event())
            self.ScWait(cpp.sc_core.sc_time(100, cpp.sc_core.SC_NS))
            print(
                "Hello from Thread %s, @ %s"
                % (self.name(), cpp.sc_core.sc_time_stamp().to_string())
            )


###############################################################################
# instantiate
###############################################################################
# from modules import TopModule
dut = Module(TopModule).create("dut")
###############################################################################
# run if it is standalone
###############################################################################
if __name__ == "__main__":
    Simulation.run()
    logging.debug("Done")

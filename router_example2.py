import logging
import os.path

import pysysc
import pysysc.scc as scc
import pysysc.structural as struct
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
scc.configure(enable_trace=True)
###############################################################################
# instantiate
###############################################################################
clk_gen = Module(cpp.ClkGen).create("clk_gen")
rst_gen = Module(cpp.ResetGen).create("rst_gen")
initiator = Module(cpp.Initiator).create("initiator")
memories = [Module(cpp.Memory).create("mem%d" % i) for i in range(2)]
router = Module(cpp.Router[len(memories)]).create("router")
###############################################################################
# connect it
###############################################################################
clk = Signal("clk").src(clk_gen.clk_o).sink(initiator.clk_i).sink(router.clk_i)
[clk.sink(m.clk_i) for m in memories]
rst = Signal("rst").src(rst_gen.reset_o).sink(initiator.reset_i).sink(router.reset_i)
[rst.sink(m.reset_i) for m in memories]
Connection().src(initiator.socket).sink(router.target_socket)
[
    Connection().src(router.initiator_socket.at(idx)).sink(m.socket)
    for idx, m in enumerate(memories)
]
###############################################################################
# run if it is standalone
###############################################################################
struct.dump_structure()
simcontext = cpp.sc_core.sc_get_curr_simcontext()
objects = cpp.sc_core.sc_get_top_level_objects(simcontext)
if __name__ == "__main__":
    Simulation.run()
    logging.debug("Done")

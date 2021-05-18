import os.path
import logging
from cppyy import gbl as cpp
import pysysc
import pysysc.structural as struct
from pysysc.structural import Connection, Module, Signal, Simulation

###############################################################################
# setup  and load
###############################################################################
logging.basicConfig(level=logging.INFO)
build_type='Debug'
###############################################################################
myDir = os.path.dirname( os.path.realpath(__file__))
pysysc.load_systemc()
###############################################################################
logging.debug("Loading SC-Components lib")
pysysc.add_include_path(os.path.join(myDir, 'scc/src/sysc'))
pysysc.add_include_path(os.path.join(myDir, 'scc/src/common'))
pysysc.add_include_path(os.path.join(myDir, 'scc/third_party'))
pysysc.add_library('scc_sysc.h', os.path.join(myDir, 'build/%s/scc/src/sysc/libscc-sysc.so'%build_type))
###############################################################################
logging.debug("Loading Components lib")
pysysc.add_include_path(os.path.join(myDir, 'vp_components'))
pysysc.add_library('components.h', os.path.join(myDir, 'build/%s/vp_components/libvp_components.so'%build_type))
###############################################################################
# configure
###############################################################################
Simulation.setup(logging.root.level)
###############################################################################
# instantiate
###############################################################################
clk_gen = Module(cpp.ClkGen).create("clk_gen")
rst_gen = Module(cpp.ResetGen).create("rst_gen")
initiator = Module(cpp.Initiator).create("initiator")
memories = [Module(cpp.Memory).create("mem%d"%i) for i in range(2)]
router = Module(cpp.Router[len(memories)]).create("router")
###############################################################################
# connect it
###############################################################################
clk = Signal("clk").src(clk_gen.clk_o).sink(initiator.clk_i).sink(router.clk_i)
[clk.sink(m.clk_i) for m in memories]
rst = Signal("rst").src(rst_gen.reset_o).sink(initiator.reset_i).sink(router.reset_i)
[rst.sink(m.reset_i) for m in memories]
Connection().src(initiator.socket).sink(router.target_socket)
[Connection().src(router.initiator_socket.at(idx)).sink(m.socket) for idx,m in enumerate(memories)]    
###############################################################################
# run if it is standalone
###############################################################################
struct.dump_structure()
simcontext = cpp.sc_core.sc_get_curr_simcontext()
objects = cpp.sc_core.sc_get_top_level_objects(simcontext)
if __name__ == "__main__":
    Simulation.configure(enable_vcd=True)
    Simulation.run()
    logging.debug("Done")

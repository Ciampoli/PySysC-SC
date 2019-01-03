import os.path
import logging
from cppyy import gbl as cpp
import pysysc
from pysysc.structural import Connection, Module, Signal
###############################################################################
# setup  and load
###############################################################################
myDir = os.path.dirname( os.path.realpath(__file__))
conan_err = pysysc.read_config_from_conan(os.path.join(myDir, 'conanfile.txt'))
pysysc.load_systemc()
logging.debug("Loading SC-Components lib")
pysysc.add_include_path(os.path.join(myDir, 'sc-components/incl'))
pysysc.add_library('scc.h', os.path.join(myDir, 'build/Debug/lib/libsc-components.so'))
logging.debug("Loading Components lib")
pysysc.add_include_path(os.path.join(myDir, 'components'))
pysysc.add_library('components.h', os.path.join(myDir, 'build/Debug/lib/libcomponents.so'))
###############################################################################
# configure
###############################################################################
cpp.scc.init_logging(cpp.logging.INFO, False);
#cpp.scc.init_logging(cpp.logging.WARNING, False);
cpp.sc_core.sc_report_handler.set_actions(cpp.sc_core.SC_ID_MORE_THAN_ONE_SIGNAL_DRIVER_, cpp.sc_core.SC_DO_NOTHING);
###############################################################################
# instantiate
###############################################################################
clk_gen = Module(cpp.ClkGen).create("clk_gen")
rst_gen = Module(cpp.ResetGen).create("rst_gen")
initiator = Module(cpp.Initiator).create("initiator")
memories = [Module(cpp.Memory).create(name) for name in ["mem0", "mem1", "mem2", "mem3"]]
router = Module(cpp.Router[4]).create("router")
###############################################################################
# connect it
###############################################################################
clk = Signal("clk").src(clk_gen.clk_o).sink(initiator.clk_i).sink(router.clk_i)
[clk.sink(m.clk_i) for m in memories]
rst = Signal("rst").src(rst_gen.reset_o).sink(initiator.reset_i).sink(router.reset_i)
[rst.sink(m.reset_i) for m in memories]
Connection().src(initiator.socket).sink(router.target_socket)
for idx,m in enumerate(memories):
    Connection().src(router.initiator_socket.at(idx)).sink(m.socket)
    
###############################################################################
# run if it is standalone
###############################################################################
if __name__ == "__main__":
    cpp.sc_core.sc_in_action=True
    cpp.sc_core.sc_start()
    logging.debug("Done")

import os.path
import logging
from cppyy import gbl as cpp
import pysysc
###############################################################################
# setup  and load
###############################################################################
build_type='Debug'
logging.basicConfig(level=logging.DEBUG)
###############################################################################
myDir = os.path.dirname( os.path.realpath(__file__))
res=pysysc.read_config_from_conan(os.path.join(myDir, 'conanfile.txt'), build_type)
pysysc.load_systemc()
###############################################################################
logging.debug("Loading SC-Components lib")
pysysc.add_include_path(os.path.join(myDir, 'sc-components/incl'))
pysysc.add_library('scc.h', os.path.join(myDir, 'build/%s/lib/libsc-components.so'%build_type))
###############################################################################
logging.debug("Loading Components lib")
pysysc.add_include_path(os.path.join(myDir, 'components'))
pysysc.add_library('components.h', os.path.join(myDir, 'build/%s/lib/libcomponents.so'%build_type))
###############################################################################
# configure
###############################################################################
cpp.scc.init_logging(cpp.logging.INFO, False);
cpp.sc_core.sc_report_handler.set_actions(cpp.sc_core.SC_ID_MORE_THAN_ONE_SIGNAL_DRIVER_, cpp.sc_core.SC_DO_NOTHING);
###############################################################################
# instantiate
###############################################################################
clkgen = cpp.ClkGen(cpp.sc_core.sc_module_name("clk_gen"))
rstgen = cpp.ResetGen(cpp.sc_core.sc_module_name("rst_gen"))
initiator = cpp.Initiator(cpp.sc_core.sc_module_name("initiator"))
memories = [cpp.Memory(cpp.sc_core.sc_module_name(name)) for name in ["mem0", "mem1", "mem2", "mem3"]]
router = cpp.Router[4](cpp.sc_core.sc_module_name("router"))
###############################################################################
# signals
##################‚‚‚sS#############################################################
sig_clk = cpp.sc_core.sc_signal[cpp.sc_core.sc_time]("clk")
sig_rst = cpp.sc_core.sc_signal[cpp.sc_dt.sc_logic]("rst")
###############################################################################
# connect it
###############################################################################
clkgen.clk_o(sig_clk)
rstgen.reset_o(sig_rst)
initiator.socket.bind(router.target_socket)
initiator.clk_i(sig_clk)
initiator.reset_i(sig_rst)
router.clk_i(sig_clk)
router.reset_i(sig_rst)
for idx,m in enumerate(memories):
    router.initiator_socket.at(idx).bind(m.socket)
    m.clk_i(sig_clk)
    m.reset_i(sig_rst)
###############################################################################
# run if it is standalone
###############################################################################
if __name__ == "__main__":
    cpp.sc_core.sc_start()
    logging.debug("Done")

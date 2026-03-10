import logging
import os

import pysysc
#NB uses scc copy in contrib/pysysc/src/pysysc/scc
from cppyy import gbl as cpp

###############################################################################
# setup  and load
###############################################################################
logging.basicConfig(level=logging.DEBUG)
###############################################################################
myDir = os.path.dirname(os.path.realpath(__file__))
logging.debug("Setting vars to package homes...")
vars = ['SPDLOG_HOME', 'FMT_HOME', 'BOOST_ROOT']
for name in vars:
    if (name in list(os.environ.keys())):
        pysysc.add_include_path(os.path.join(os.environ[name], 'include'))
    else:
        print('WARNING : ', name, ' env variable not set')
###############################################################################
logging.debug("Loading SystemC...")
if (not pysysc.load_systemc(17)):
    print('Error : failed to load systemc dynamic library')
    exit()
###############################################################################
logging.debug("Loading SC-Components lib")
logging.debug("Working in %s", myDir)
#TODO:
# must introduce an env variable to PYSYSC
libDir = os.path.join(myDir, "build_" + os.environ['OSNICKNAME'])
#TODO:
# Must relate these paths with version effectively installed !
# e.g. I have cci-1.0.1 and path does not exist...
pysysc.add_include_path(os.path.join(myDir, "scc/third_party/cci-1.0.1"))
pysysc.add_include_path(os.path.join(myDir, "scc/third_party/scv-tr/src"))
pysysc.add_include_path(os.path.join(myDir, "scc/src/sysc"))
pysysc.add_include_path(os.path.join(myDir, "scc/src/common"))
pysysc.add_include_path(os.path.join(myDir, "scc/third_party"))
pysysc.add_library("scc_sysc.h", "libscc-sysc.so", libDir)
###############################################################################
logging.debug("Loading Components lib")
pysysc.add_include_path(os.path.join(myDir, "vp_components"))
pysysc.add_library("components.h", "libvp_components.so", libDir)
# might be useful to dump current list of include dirs:
#for inc in sorted(pysysc.includeDirs):
#    print(inc)
###############################################################################
# configure
###############################################################################
cpp.scc.init_logging(cpp.scc.log.INFO, 24, False)
cpp.sc_core.sc_report_handler.set_actions(
    cpp.sc_core.SC_ID_MORE_THAN_ONE_SIGNAL_DRIVER_, cpp.sc_core.SC_DO_NOTHING
)
###############################################################################
# instantiate
###############################################################################
clkgen = cpp.ClkGen(cpp.sc_core.sc_module_name("clk_gen"))
rstgen = cpp.ResetGen(cpp.sc_core.sc_module_name("rst_gen"))
initiator = cpp.Initiator(cpp.sc_core.sc_module_name("initiator"))
memories = [
    cpp.Memory(cpp.sc_core.sc_module_name(name))
    for name in ["mem0", "mem1", "mem2", "mem3"]
]
router = cpp.Router[4](cpp.sc_core.sc_module_name("router"))
###############################################################################
# signals
###############################################################################
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
for idx, m in enumerate(memories):
    router.initiator_socket.at(idx).bind(m.socket)
    m.clk_i(sig_clk)
    m.reset_i(sig_rst)
###############################################################################
# run if it is standalone
###############################################################################
if __name__ == "__main__":
    if os.path.isfile("router_example.json"):
        cfg = cpp.scc.configurer(cpp.std.string("router_example.json"))
    tracer = cpp.scc.tracer("vcd_trace", 1, True)
    cpp.sc_core.sc_start()
    logging.debug("Done")

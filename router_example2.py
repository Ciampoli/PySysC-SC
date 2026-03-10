import logging
import os

import pysysc
#NB uses scc as installed by pip
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
libDir = os.path.join(myDir, "build_" + os.environ['OSNICKNAME'])
scc.load_lib(myDir, libDir)
###############################################################################
logging.debug("Loading Components lib")
pysysc.add_include_path(os.path.join(myDir, "vp_components"))
pysysc.add_library("components.h", "libvp_components.so", libDir)
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

# Might be useful to dump the module names
#logging.debug("Get modules name:")
#print(clk_gen.name())
#print(rst_gen.name())
#print(initiator.name())
#for mem in memories:
#    print(mem.name());
#print(router.name())

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

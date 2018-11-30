import os.path
import cppyy
from cppyy import gbl as cpp
import pysysc as scpy
###############################################################################
# setup  and load
###############################################################################
myDir = os.path.dirname( os.path.realpath(__file__))
conan_err = scpy.read_config_from_conan(os.path.join(myDir, 'conanfile.txt'))
scpy.load_systemc()
scpy.add_include_path(os.path.join(myDir, 'components'))
scpy.add_library('components.h', os.path.join(myDir, 'build/Debug/lib/libcomponents.so'))
###############################################################################
# configure
###############################################################################
cpp.init_logging(5)
cpp.sc_core.sc_report_handler.set_actions(cpp.sc_core.SC_ID_MORE_THAN_ONE_SIGNAL_DRIVER_, cpp.sc_core.SC_DO_NOTHING);
###############################################################################
# instantiate
###############################################################################
initiator = cpp.Initiator(cpp.sc_core.sc_module_name("initiator"))
memories = [cpp.Memory(cpp.sc_core.sc_module_name(name)) for name in ["mem0", "mem1", "mem2", "mem3"]]
router = cpp.Router[4](cpp.sc_core.sc_module_name("router"))
###############################################################################
# connect it
###############################################################################
initiator.socket.bind(router.target_socket)
for idx,m in enumerate(memories):
    router.initiator_socket.at(idx).bind(m.socket)
###############################################################################
# run if it is standalone
###############################################################################
if __name__ == "__main__":
    cpp.sc_core.sc_in_action=True
    cpp.sc_core.sc_start()
    print("Done")

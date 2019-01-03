#ifndef TARGET_H
#define TARGET_H

// Needed for the simple_target_socket
#define SC_INCLUDE_DYNAMIC_PROCESSES

#include "systemc"
using namespace sc_core;
using namespace sc_dt;
using namespace std;

#include "tlm.h"
#include "tlm_utils/simple_target_socket.h"


// Target module representing a simple memory

struct Memory: sc_module
{
  // TLM-2 socket, defaults to 32-bits wide, base protocol
  tlm_utils::simple_target_socket<Memory> socket;
  sc_core::sc_in<sc_core::sc_time> clk_i;
  sc_core::sc_in<sc_dt::sc_logic>  reset_i;


  enum { SIZE = 256 };
  const sc_time LATENCY;

  SC_HAS_PROCESS(Memory);

  Memory(sc_core::sc_module_name nm);

protected:
  // TLM-2 blocking transport method
  void b_transport( tlm::tlm_generic_payload& trans, sc_time& delay );

  // TLM-2 forward DMI method
  bool get_direct_mem_ptr(tlm::tlm_generic_payload& trans,
                                  tlm::tlm_dmi& dmi_data);
  // TLM-2 debug transaction method
  unsigned int transport_dbg(tlm::tlm_generic_payload& trans);

  int mem[SIZE];
  static unsigned int mem_nr;
};

unsigned int Memory::mem_nr = 0;

#endif

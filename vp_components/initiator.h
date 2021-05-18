#ifndef INITIATOR_H
#define INITIATOR_H

#include "systemc"
using namespace sc_core;
using namespace sc_dt;
using namespace std;

#include "tlm.h"
#include "tlm_utils/simple_initiator_socket.h"
#include <scc/utilities.h>

// Initiator module generating generic payload transactions

struct Initiator: sc_module
{
  // TLM-2 socket, defaults to 32-bits wide, base protocol
  tlm_utils::simple_initiator_socket<Initiator> socket;

  sc_core::sc_in<sc_core::sc_time> clk_i;
  sc_core::sc_in<sc_dt::sc_logic>  reset_i;

  SC_HAS_PROCESS(Initiator);

  Initiator( ::sc_core::sc_module_name );

  void thread_process();

  // TLM-2 backward DMI method
  void invalidate_direct_mem_ptr(sc_dt::uint64 start_range, sc_dt::uint64 end_range){
    // Ignore range and invalidate all DMI pointers regardless
    dmi_ptr_valid = false;
  }

  bool dmi_ptr_valid;
  tlm::tlm_dmi dmi_data;

  void trace(sc_core::sc_trace_file* trf) const override {
      TRACE_VAR(trf, dmi_ptr_valid);
  }
};

#endif

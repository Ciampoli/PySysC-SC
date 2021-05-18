#ifndef ROUTER_H
#define ROUTER_H

#include "systemc"
using namespace sc_core;
using namespace sc_dt;
using namespace std;

#include "tlm.h"
#include "tlm_utils/simple_initiator_socket.h"
#include "tlm_utils/simple_target_socket.h"


// *********************************************
// Generic payload blocking transport router
// *********************************************

template<unsigned int N_TARGETS>
struct Router: sc_module
{
  // TLM-2 socket, defaults to 32-bits wide, base protocol
  tlm_utils::simple_target_socket<Router>            target_socket;

  // *********************************************
  // Use tagged sockets to be able to distinguish incoming backward path calls
  // *********************************************

  sc_core::sc_vector<tlm_utils::simple_initiator_socket_tagged<Router>> initiator_socket;

  sc_core::sc_in<sc_core::sc_time> clk_i;
  sc_core::sc_in<sc_dt::sc_logic>  reset_i;

  SC_CTOR(Router)
  : target_socket("target_socket")
  , initiator_socket("socket", N_TARGETS)
  , clk_i("clk_i")
  , reset_i("reset_i")
  {
    // Register callbacks for incoming interface method calls
    target_socket.register_b_transport(       this, &Router::b_transport);
    target_socket.register_get_direct_mem_ptr(this, &Router::get_direct_mem_ptr);
    target_socket.register_transport_dbg(     this, &Router::transport_dbg);

    for (unsigned int i = 0; i < N_TARGETS; i++)
    {
      char txt[20];
      sprintf(txt, "socket_%d", i);

      // *********************************************
      // Register callbacks for incoming interface method calls, including tags
      // *********************************************
      initiator_socket[i].register_invalidate_direct_mem_ptr(this, &Router::invalidate_direct_mem_ptr, i);
    }
  }

  // ****************
  // FORWARD PATH
  // ****************

  // TLM-2 blocking transport method
  virtual void b_transport( tlm::tlm_generic_payload& trans, sc_time& delay )
  {
    sc_dt::uint64 address = trans.get_address();
    sc_dt::uint64 masked_address;
    unsigned int target_nr = decode_address( address, masked_address);

    // Modify address within transaction
    trans.set_address( masked_address );

    // Forward transaction to appropriate target
    initiator_socket[target_nr]->b_transport( trans, delay );
  }

  // TLM-2 forward DMI method
  virtual bool get_direct_mem_ptr(tlm::tlm_generic_payload& trans,
                                  tlm::tlm_dmi& dmi_data)
  {
    sc_dt::uint64 masked_address;
    unsigned int target_nr = decode_address( trans.get_address(), masked_address );
    trans.set_address( masked_address );

    bool status = initiator_socket[target_nr]->get_direct_mem_ptr( trans, dmi_data );

    // Calculate DMI address of target in system address space
    dmi_data.set_start_address( compose_address( target_nr, dmi_data.get_start_address() ));
    dmi_data.set_end_address  ( compose_address( target_nr, dmi_data.get_end_address() ));

    return status;
  }

  // TLM-2 debug transaction method
  virtual unsigned int transport_dbg(tlm::tlm_generic_payload& trans)
  {
    sc_dt::uint64 masked_address;
    unsigned int target_nr = decode_address( trans.get_address(), masked_address );
    trans.set_address( masked_address );

    // Forward debug transaction to appropriate target
    return initiator_socket[target_nr]->transport_dbg( trans );
  }

  // ****************
  // BACKWARD PATH
  // ****************

  // **************************
  // Tagged backward DMI method
  // **************************

  virtual void invalidate_direct_mem_ptr(int id,
                                         sc_dt::uint64 start_range,
                                         sc_dt::uint64 end_range)
  {
    // Reconstruct address range in system memory map
    sc_dt::uint64 bw_start_range = compose_address( id, start_range );
    sc_dt::uint64 bw_end_range   = compose_address( id, end_range );
    target_socket->invalidate_direct_mem_ptr(bw_start_range, bw_end_range);
  }

  // ****************
  // ROUTER INTERNALS
  // ****************

  // Simple fixed address decoding
  inline unsigned int decode_address( sc_dt::uint64 address, sc_dt::uint64& masked_address )
  {
    unsigned int target_nr = static_cast<unsigned int>( (address >> 10) % initiator_socket.size() );
    masked_address = address & 0x3FF;
    return target_nr;
  }

  inline sc_dt::uint64 compose_address( unsigned int target_nr, sc_dt::uint64 address)
  {
    return (target_nr << 10) | (address & 0x3FF);
  }
};

#endif

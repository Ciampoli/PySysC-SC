/*
 * top1.h
 *
 *  Created on: 02.12.2018
 *      Author: eyck
 */

#ifndef TXTOP_H_
#define TXTOP_H_

#include "tx_example_mods.h"
#include <systemc>

class tx_top: public sc_core::sc_module {
public:
    tx_top(sc_core::sc_module_name nm);
    virtual ~tx_top();
    sc_core::sc_clock clk;
    sc_core::sc_signal< bool > rw;
    sc_core::sc_signal< bool > addr_req;
    sc_core::sc_signal< bool > addr_ack;
    sc_core::sc_signal< sc_dt::sc_uint<8> > bus_addr;
    sc_core::sc_signal< bool > data_rdy;
    sc_core::sc_signal< sc_dt::sc_uint<8> > bus_data;
    test t;
    rw_pipelined_transactor tr;
    design duv;

};

#endif /* TXTOP_H_ */

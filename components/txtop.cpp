/*
 * top1.cpp
 *
 *  Created on: 02.12.2018
 *      Author: eyck
 */

#include "txtop.h"
using namespace sc_core;

tx_top::tx_top(sc_core::sc_module_name nm)
: sc_module(nm)
, clk("clk", 20.0, SC_NS, 0.5 ,0.0, SC_NS, true)
, rw("rw")
, addr_req("addr_reg")
, addr_ack("addr_ack")
, bus_addr("bus_addr")
, data_rdy("data_rdy")
, bus_data("bus_data")
, t("t")
, tr("tr")
, duv("duv")
{
    // TODO Auto-generated constructor stub
    // connect them up
    t.transactor(tr);

    tr.clk(clk);
    tr.rw(rw);
    tr.addr_req(addr_req);
    tr.addr_ack(addr_ack);
    tr.bus_addr(bus_addr);
    tr.data_rdy(data_rdy);
    tr.bus_data(bus_data);

    duv.clk(clk);
    duv.rw(rw);
    duv.addr_req(addr_req);
    duv.addr_ack(addr_ack);
    duv.bus_addr(bus_addr);
    duv.data_rdy(data_rdy);
    duv.bus_data(bus_data);

}

tx_top::~tx_top() {
    // TODO Auto-generated destructor stub
}


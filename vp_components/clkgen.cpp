/*
 * clkgen.cpp
 *
 *  Created on: 02.01.2019
 *      Author: eyck
 */

#include <clkgen.h>
#include <scc/utilities.h>

ClkGen::ClkGen(const sc_core::sc_module_name& nm)
: sc_core::sc_module(nm)
, clk_o("clk_o")
{
}

ClkGen::~ClkGen() {
    // TODO Auto-generated destructor stub
}

void ClkGen::end_of_elaboration() {
    clk_o.write(10_ns);
}

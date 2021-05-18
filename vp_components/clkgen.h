/*
 * clkgen.h
 *
 *  Created on: 02.01.2019
 *      Author: eyck
 */

#ifndef VP_COMPONENTS_CLKGEN_H_
#define VP_COMPONENTS_CLKGEN_H_

#include <systemc>

class ClkGen: public sc_core::sc_module {
public:
    sc_core::sc_out<sc_core::sc_time> clk_o;

    ClkGen(const sc_core::sc_module_name&);
    virtual ~ClkGen();
protected:
    void end_of_elaboration() override;
};

#endif /* VP_COMPONENTS_CLKGEN_H_ */

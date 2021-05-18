/*
 * resetgen.h
 *
 *  Created on: 02.01.2019
 *      Author: eyck
 */

#ifndef VP_COMPONENTS_RESETGEN_H_
#define VP_COMPONENTS_RESETGEN_H_

#include <systemc>

class ResetGen: public sc_core::sc_module {
public:
    SC_HAS_PROCESS(ResetGen);

    sc_core::sc_out<sc_dt::sc_logic>  reset_o;

    ResetGen(const sc_core::sc_module_name&);
    virtual ~ResetGen();
protected:

    void thread();
};

#endif /* VP_COMPONENTS_RESETGEN_H_ */

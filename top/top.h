#ifndef TOP_H
#define TOP_H

#include <clkgen.h>
#include <resetgen.h>
#include <initiator.h>
#include <target.h>
#include <router.h>

enum { SLV_CNT=2};
SC_MODULE(Top) {

    ClkGen * clk_gen;
    ResetGen* reset_gen;
    Initiator* initiator;
    Router<SLV_CNT>* router;
    Memory* memory[SLV_CNT];

    Top(const sc_core::sc_module_name& nm)
    : sc_core::sc_module()
    , clk("clk")
    , reset("reset")
    {
        // Instantiate components
        clk_gen=new ClkGen("clk_gen");
        reset_gen=new ResetGen("reset_gen");
        initiator = new Initiator("initiator");
        router = new Router<SLV_CNT>("router");
        for (int i = 0; i < SLV_CNT; i++) {
            char txt[20];
            sprintf(txt, "memory_%d", i);
            memory[i] = new Memory(txt);
        }

        // Bind sockets
        initiator->socket.bind(router->target_socket);
        for (int i = 0; i < SLV_CNT; i++)
            router->initiator_socket[i].bind(memory[i]->socket);
        // connect signals
        clk_gen->clk_o(clk);
        reset_gen->reset_o(reset);
        initiator->clk_i(clk);
        initiator->reset_i(reset);

        router->clk_i(clk);
        router->reset_i(reset);

        for(auto& m: memory){
            m->clk_i(clk);
            m->reset_i(reset);
        }
}
    sc_core::sc_signal<sc_core::sc_time> clk;
    sc_core::sc_signal<sc_dt::sc_logic> reset;
};

#endif

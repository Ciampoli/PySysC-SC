//  -*- C++ -*- <this line is for emacs to recognize it as C++ code>
/*****************************************************************************

  Licensed to Accellera Systems Initiative Inc. (Accellera) 
  under one or more contributor license agreements.  See the 
  NOTICE file distributed with this work for additional 
  information regarding copyright ownership. Accellera licenses 
  this file to you under the Apache License, Version 2.0 (the
  "License"); you may not use this file except in compliance
  with the License.  You may obtain a copy of the License at
 
    http://www.apache.org/licenses/LICENSE-2.0
 
  Unless required by applicable law or agreed to in writing,
  software distributed under the License is distributed on an
  "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
  KIND, either express or implied.  See the License for the
  specific language governing permissions and limitations
  under the License.

 *****************************************************************************/
// this code compiles and runs with our latest prototype for this specification

#include <scv.h>
#include <systemc.h>

// hack to fake a true fifo_mutex
#define fifo_mutex sc_mutex

const unsigned ram_size = 256;

class rw_task_if : virtual public sc_interface {
public:
   typedef sc_uint<8> addr_t;
   typedef sc_uint<8> data_t;
   struct write_t {
     addr_t addr;
     data_t data;
   };

   virtual data_t read(const addr_t*) = 0;
   virtual void write(const write_t*) = 0;
};

SCV_EXTENSIONS(rw_task_if::write_t) {
public:
   scv_extensions<rw_task_if::addr_t> addr;
   scv_extensions<rw_task_if::data_t> data;
   SCV_EXTENSIONS_CTOR(rw_task_if::write_t) {
     SCV_FIELD(addr);
     SCV_FIELD(data);
   }
};

class pipelined_bus_ports : public sc_module {
public:
   sc_in< bool > clk;
   sc_inout< bool > rw;
   sc_inout< bool > addr_req;
   sc_inout< bool > addr_ack;
   sc_inout< sc_uint<8> > bus_addr;
   sc_inout< bool > data_rdy;
   sc_inout< sc_uint<8> > bus_data;

   SC_CTOR(pipelined_bus_ports)
     : clk("clk"), rw("rw"),
       addr_req("addr_req"),
       addr_ack("addr_ack"), bus_addr("bus_addr"),
       data_rdy("data_rdy"), bus_data("bus_data") {}
};

class rw_pipelined_transactor
   : public rw_task_if,
     public pipelined_bus_ports {

   fifo_mutex addr_phase;
   fifo_mutex data_phase;

   scv_tr_stream pipelined_stream;
   scv_tr_stream addr_stream;
   scv_tr_stream data_stream;
   scv_tr_generator<sc_uint<8>, sc_uint<8> > read_gen;
   scv_tr_generator<sc_uint<8>, sc_uint<8> > write_gen;
   scv_tr_generator<sc_uint<8> > addr_gen;
   scv_tr_generator<sc_uint<8> > data_gen;

public:
   rw_pipelined_transactor(sc_module_name nm) :
       pipelined_bus_ports(nm),
       addr_phase("addr_phase"),
       data_phase("data_phase"),
       pipelined_stream("pipelined_stream", "transactor"),
       addr_stream("addr_stream", "transactor"),
       data_stream("data_stream", "transactor"),
       read_gen("read",pipelined_stream,"addr","data"),
       write_gen("write",pipelined_stream,"addr","data"),
       addr_gen("addr",addr_stream,"addr"),
       data_gen("data",data_stream,"data")
   {}
   virtual data_t read(const addr_t* p_addr);
   virtual void write(const write_t * req);
};
class test : public sc_module {
public:
   sc_port< rw_task_if > transactor;
   SC_CTOR(test) {
     SC_THREAD(main);
   }
   void main();
};

class write_constraint : virtual public scv_constraint_base {
public:
   scv_smart_ptr<rw_task_if::write_t> write;
   SCV_CONSTRAINT_CTOR(write_constraint) {
     SCV_CONSTRAINT( write->addr() <= ram_size );
     SCV_CONSTRAINT( write->addr() != write->data() );
   }
};

inline void process(scv_smart_ptr<int> data) {}
class design : public pipelined_bus_ports {
   std::list< sc_uint<8> > outstandingAddresses;
   std::list< bool > outstandingType;
   sc_uint<8>  memory[ram_size];

public:
   SC_HAS_PROCESS(design);
   design(sc_module_name nm) : pipelined_bus_ports(nm) {
     for (unsigned i=0; i<ram_size; ++i) { memory[i] = i; }
     SC_THREAD(addr_phase);
     SC_THREAD(data_phase);
   }
   void addr_phase();
   void data_phase();
};

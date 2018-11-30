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

#include "tx_example_mods.h"

rw_task_if::data_t rw_pipelined_transactor::read(const rw_task_if::addr_t* 
addr) {
   addr_phase.lock();
   scv_tr_handle h = read_gen.begin_transaction(*addr);

   scv_tr_handle h1 = addr_gen.begin_transaction(*addr,"addr_phase",h);
   wait(clk->posedge_event());
   bus_addr = *addr;
   addr_req = 1;
   wait(addr_ack->posedge_event());
   wait(clk->negedge_event());
   addr_req = 0;
   wait(addr_ack->negedge_event());
   addr_gen.end_transaction(h1);
   addr_phase.unlock();

   data_phase.lock();
   scv_tr_handle h2 = data_gen.begin_transaction("data_phase",h);
   wait(data_rdy->posedge_event());
   data_t data = bus_data.read();
   wait(data_rdy->negedge_event());
   data_gen.end_transaction(h2);
   read_gen.end_transaction(h,data);
   data_phase.unlock();

   return data;
}

void rw_pipelined_transactor::write(const write_t * req) {
   scv_tr_handle h = write_gen.begin_transaction(req->addr);
   // ...
   write_gen.end_transaction(h,req->data);
}

void test::main() {
   // simple sequential tests
   for (int i=0; i<3; i++) {
     rw_task_if::addr_t addr = i;
     rw_task_if::data_t data = transactor->read(&addr);
     cout << "at time " << sc_time_stamp() << ": ";
     cout << "received data : " << data << endl;
   }

   scv_smart_ptr<rw_task_if::addr_t> addr;
   for (int i=0; i<3; i++) {

     addr->next();
     rw_task_if::data_t data = transactor->read( addr->get_instance() );
     cout << "data for address " << *addr << " is " << data << endl;
   }

   scv_smart_ptr<rw_task_if::write_t> write;
   for (int i=0; i<3; i++) {
     write->next();
     transactor->write( write->get_instance() );
     cout << "send data : " << write->data << endl;
   }

   scv_smart_ptr<int> data;
   scv_bag<int> distribution;
   distribution.push(1,40);
   distribution.push(2,60);
   data->set_mode(distribution);
   for (int i=0;i<3; i++) { data->next(); process(data); }
}

void design::addr_phase() {
   while (1) {
     while (addr_req.read() != 1) {
       wait(addr_req->value_changed_event());
     }
     sc_uint<8> _addr = bus_addr.read();
     bool _rw = rw.read();

     int cycle = rand() % 10 + 1;
     while (cycle-- > 0) {
       wait(clk->posedge_event());
     }

     addr_ack = 1;
     wait(clk->posedge_event());
     addr_ack = 0;

     outstandingAddresses.push_back(_addr);
     outstandingType.push_back(_rw);
     cout << "at time " << sc_time_stamp() << ": ";
     cout << "received request for memory address " << _addr << endl;
   }
}

void design::data_phase() {
   while (1) {
     while (outstandingAddresses.empty()) {
       wait(clk->posedge_event());
     }
     int cycle = rand() % 10 + 1;
     while (cycle-- > 0) {
       wait(clk->posedge_event());
     }
     if (outstandingType.front() == 0) {
       cout << "reading memory address " << outstandingAddresses.front()
            << " with value " << memory[outstandingAddresses.front().to_ulong()] << endl;
       bus_data = memory[outstandingAddresses.front().to_ulong()];
       data_rdy = 1;
       wait(clk->posedge_event());
       data_rdy = 0;

     } else {
       cout << "not implemented yet" << endl;
     }
     outstandingAddresses.pop_front();
     outstandingType.pop_front();
   }
}


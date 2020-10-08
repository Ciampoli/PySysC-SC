////////////////////////////////////////////////////////////////////////////////
// Copyright 2017 eyck@minres.com
//
// Licensed under the Apache License, Version 2.0 (the "License"); you may not
// use this file except in compliance with the License.  You may obtain a copy
// of the License at
//
//   http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
// WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.  See the
// License for the specific language governing permissions and limitations under
// the License.
////////////////////////////////////////////////////////////////////////////////
/*
 * sc_main.cpp
 *
 *  Created on: 17.09.2017
 *      Author: eyck@minres.com
 */

#include "top.h"
#include <scc/report.h>

using namespace scc;

int sc_main(int argc, char *argv[]) {
    ///////////////////////////////////////////////////////////////////////////
    // configure logging
    ///////////////////////////////////////////////////////////////////////////
    scc::init_logging(log::INFO);
    //scc::init_logging(log::WARNING);

    ///////////////////////////////////////////////////////////////////////////
    // instantiate top level
    ///////////////////////////////////////////////////////////////////////////
    Top i_top("i_top");

    ///////////////////////////////////////////////////////////////////////////
    // run simulation
    ///////////////////////////////////////////////////////////////////////////
    sc_start();
    // todo: provide end-of-simulation macros

    if (!sc_core::sc_end_of_simulation_invoked()) {
        sc_core::sc_stop();
    }
    return 0;
}

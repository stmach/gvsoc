/*
 * Copyright (C) 2018 ETH Zurich and University of Bologna
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/* 
 * Authors: Germain Haugou, ETH (germain.haugou@iis.ee.ethz.ch)
 */

#ifndef __CPU_ISS_LSU_HPP
#define __CPU_ISS_LSU_HPP

#include "iss_core.hpp"

static inline void iss_lsu_load_perf(iss_t *iss, iss_insn_t *insn, iss_addr_t addr, int size, int reg)
{
  iss_pccr_account_event(iss, CSR_PCER_LD, 1);
  iss_lsu_load(iss, insn, addr, size, reg);
}

static inline void iss_lsu_elw_perf(iss_t *iss, iss_insn_t *insn, iss_addr_t addr, int size, int reg)
{
  iss_pccr_account_event(iss, CSR_PCER_LD, 1);
  iss_lsu_elw(iss, insn, addr, size, reg);
}

static inline void iss_lsu_load_signed_perf(iss_t *iss, iss_insn_t *insn, iss_addr_t addr, int size, int reg)
{
  iss_pccr_account_event(iss, CSR_PCER_LD, 1);
  iss_lsu_load_signed(iss, insn, addr, size, reg);
}

static inline void iss_lsu_store_perf(iss_t *iss, iss_insn_t *insn, iss_addr_t addr, int size, int reg)
{
  iss_pccr_account_event(iss, CSR_PCER_ST, 1);
  iss_lsu_store(iss, insn, addr, size, reg);
}

static inline void iss_lsu_check_stack_access(iss_t *iss, int reg, iss_addr_t addr)
{
  if (iss->cpu.csr.stack_conf && reg == 2)
  {
    if (addr < iss->cpu.csr.stack_start || addr >= iss->cpu.csr.stack_end)
    {
      iss_fatal(iss, "SP-based access outside stack (addr: 0x%x, stack_start: 0x%x, stack_end: 0x%x)\n", addr, iss->cpu.csr.stack_start, iss->cpu.csr.stack_end);
    }
  }
}

#endif

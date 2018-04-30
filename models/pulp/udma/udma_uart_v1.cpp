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


#define ARCHI_UDMA_HAS_UART 1

#include "udma_v2_impl.hpp"
#include "archi/udma/udma_periph_v2.h"
#include "archi/utils.h"
#include "vp/itf/uart.hpp"


Uart_periph_v1::Uart_periph_v1(udma *top, int id, int itf_id) : Udma_periph(top, id)
{
  std::string itf_name = "uart" + std::to_string(itf_id);

  top->traces.new_trace(itf_name, &trace, vp::DEBUG);

  channel0 = new Uart_rx_channel(top, this, UDMA_CHANNEL_ID(id), itf_name + "_rx");
  channel1 = new Uart_tx_channel(top, this, UDMA_CHANNEL_ID(id) + 1, itf_name + "_tx");

  top->new_master_port(itf_name, &uart_itf);
}
 

void Uart_periph_v1::reset()
{
  Udma_periph::reset();
  this->set_setup_reg(0);
  this->rx_pe = 0;
  this->tx = 0;
  this->rx = 0;
  this->clkdiv = 0;
  this->parity = 0;
  this->stop_bits = 1;
  this->bit_length = 5;
}


vp::io_req_status_e Uart_periph_v1::status_req(vp::io_req *req)
{
  if (req->get_is_write())
  {
    return vp::IO_REQ_INVALID;
  }
  else
  {
    int tx_busy = channel1->is_busy();
    int rx_busy = channel0->is_busy();

    *(uint32_t *)(req->get_data()) = (tx_busy << UART_TX_BUSY_OFFSET) |
      (rx_busy << UART_RX_BUSY_OFFSET) |
      (this->rx_pe << UART_RX_PE_OFFSET);

    // Parity error is cleared when it is read
    this->rx_pe = 0;

    return vp::IO_REQ_OK;
  }
}



void Uart_periph_v1::set_setup_reg(uint32_t value)
{
  this->setup_reg_value = value;
  this->parity = ARCHI_REG_FIELD_GET(this->setup_reg_value, UART_PARITY_OFFSET, UART_PARITY_WIDTH);
  this->bit_length = ARCHI_REG_FIELD_GET(this->setup_reg_value, UART_BIT_LENGTH_OFFSET, UART_BIT_LENGTH_WIDTH) + 5;
  this->stop_bits = ARCHI_REG_FIELD_GET(this->setup_reg_value, UART_STOP_BITS_OFFSET, UART_STOP_BITS_WIDTH) + 1;
  this->tx = ARCHI_REG_FIELD_GET(this->setup_reg_value, UART_TX_OFFSET, UART_TX_WIDTH);
  this->rx = ARCHI_REG_FIELD_GET(this->setup_reg_value, UART_RX_OFFSET, UART_RX_WIDTH);
  this->clkdiv = ARCHI_REG_FIELD_GET(this->setup_reg_value, UART_CLKDIV_OFFSET, UART_CLKDIV_WIDTH);
}

vp::io_req_status_e Uart_periph_v1::setup_req(vp::io_req *req)
{
  if (req->get_is_write())
  {
    this->set_setup_reg(*(uint32_t *)req->get_data());

    this->trace.msg("Modifying UART configuration (parity: %d, bit_length: %d, stop_bits: %d, tx: %d, rx: %d, clkdiv: %d)\n",
      parity, bit_length, stop_bits, tx, rx, clkdiv);
  }
  else
  {
    *(uint32_t *)req->get_data() = this->setup_reg_value;
  }


  return vp::IO_REQ_OK;
}



vp::io_req_status_e Uart_periph_v1::custom_req(vp::io_req *req, uint64_t offset)
{
  if (req->get_size() != 4)
    return vp::IO_REQ_INVALID;

  if (offset == UART_STATUS_OFFSET)
  {
    return this->status_req(req);
  }
  else if (offset == UART_SETUP_OFFSET)
  {
    return this->setup_req(req);
  }

  return vp::IO_REQ_INVALID;
}





Uart_tx_channel::Uart_tx_channel(udma *top, Uart_periph_v1 *periph, int id, string name)
: Udma_tx_channel(top, id, name), periph(periph)
{
  pending_word_event = top->event_new(this, Uart_tx_channel::handle_pending_word);
}




void Uart_tx_channel::handle_pending_word(void *__this, vp::clock_event *event)
{
  Uart_tx_channel *_this = (Uart_tx_channel *)__this;

  int bit = -1;

  if (_this->state == UART_TX_STATE_START)
  {
    _this->parity = 0;
    _this->state = UART_TX_STATE_DATA;
    bit = 0;
  }
  else if (_this->state == UART_TX_STATE_DATA)
  {
    bit = _this->pending_word & 1;
    _this->pending_word >>= 1;
    _this->pending_bits -= 1;
    _this->parity ^= bit;
    _this->sent_bits++;

    if (_this->pending_bits == 0)
    {
      _this->handle_ready_req_end(_this->pending_req);
      _this->handle_ready_reqs();
    }

    if (_this->sent_bits == _this->periph->bit_length)
    {
      _this->sent_bits = 0;
      if (_this->periph->parity)
        _this->state = UART_TX_STATE_PARITY;
      else
      {
        _this->stop_bits = _this->periph->stop_bits;
        _this->state = UART_TX_STATE_STOP;
      }
    }
  }
  else if (_this->state == UART_TX_STATE_PARITY)
  {
    bit = _this->parity;
    _this->stop_bits = _this->periph->stop_bits;
    _this->state = UART_TX_STATE_STOP;
  }
  else if (_this->state == UART_TX_STATE_STOP)
  {
    bit = 1;
    _this->stop_bits--;
    if (_this->stop_bits == 0)
    {
      _this->state = UART_TX_STATE_START;
    }
  }

  if (bit != -1)
  {
    if (!_this->periph->uart_itf.is_bound())
    {
      _this->top->get_trace()->warning("Trying to send to UART interface while it is not connected\n");
    }
    else
    {
      if (_this->periph->tx)
      {
        _this->next_bit_cycle = _this->top->get_clock()->get_cycles() + _this->periph->clkdiv;
        _this->periph->uart_itf.sync(bit);
      }
    }
  }

  _this->check_state();
}



void Uart_tx_channel::check_state()
{
  if (this->pending_bits != 0 && !pending_word_event->is_enqueued())
  {
    int latency = 1;
    int64_t cycles = this->top->get_clock()->get_cycles();
    if (next_bit_cycle > cycles)
      latency = next_bit_cycle - cycles;

    top->event_enqueue(pending_word_event, latency);
  }
}



void Uart_tx_channel::handle_ready_reqs()
{
  if (this->pending_bits == 0 && !ready_reqs->is_empty())
  {
    vp::io_req *req = this->ready_reqs->pop();
    this->pending_req = req;
    this->pending_word = *(uint32_t *)req->get_data();
    this->pending_bits = req->get_size() * 8;
    this->check_state();
  }
}



void Uart_tx_channel::reset()
{
  Udma_tx_channel::reset();
  this->state = UART_TX_STATE_START;
  this->next_bit_cycle = -1;
  this->sent_bits = 0;
  this->pending_bits = 0;
}



bool Uart_tx_channel::is_busy()
{
  return this->pending_bits != 0 || !ready_reqs->is_empty();
}


bool Uart_rx_channel::is_busy()
{
  return false;
}

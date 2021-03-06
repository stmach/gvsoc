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

#include <vp/vp.hpp>
#include <vp/itf/wire.hpp>
#include <stdio.h>
#include <string.h>

class Switch : public vp::component
{

public:

  Switch(const char *config);

  int build();
  void start();

private:


  vp::wire_master<int>  out_itf;
  int value;
};



Switch::Switch(const char *config)
: vp::component(config)
{
}

int Switch::build()
{
  this->new_master_port("out", &this->out_itf);

  this->value = this->get_config_int("value");

  return 0;
}

void Switch::start()
{
  this->out_itf.sync(this->value);
}

extern "C" void *vp_constructor(const char *config)
{
  return (void *)new Switch(config);
}

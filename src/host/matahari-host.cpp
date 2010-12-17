/* matahari-host.cpp - Copyright (C) 2009 Red Hat, Inc.
 * Written by Arjun Roy <arroy@redhat.com>
 *
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 * 
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifndef WIN32
#include "config.h"
#endif

#include <set>
#include "matahari/mh_agent.h"
#include "qmf/org/matahariproject/Host.h"
#include "qmf/org/matahariproject/EventHeartbeat.h"
#include <sigar.h>

extern "C" {
#include "matahari/host.h"
}

class HostAgent : public MatahariAgent
{
    private:
	ManagementAgent* _agent;
	_qmf::Host* _management_object;
	
    public:
	void heartbeat();
	int setup(ManagementAgent* agent);
	ManagementObject* GetManagementObject() const { return _management_object; }
	status_t ManagementMethod(uint32_t method, Args& arguments, string& text);
};

int
main(int argc, char **argv)
{
    HostAgent agent; 
    int rc = agent.init(argc, argv);
    while (rc == 0) {
	/* Looks like a tight loop, but .heartbeat() contains a call to sleep() */
	agent.heartbeat();
    }
    
    return rc;
}

int
HostAgent::setup(ManagementAgent* agent)
{
  this->_agent = agent;

  _management_object = new _qmf::Host(agent, this);
  agent->addObject(_management_object);

  _management_object->set_update_interval(5);
  _management_object->set_uuid(host_get_uuid());
  _management_object->set_hostname(host_get_hostname());
  _management_object->set_os(host_get_operating_system());
  _management_object->set_wordsize(host_get_cpu_wordsize());
  _management_object->set_arch(host_get_architecture());
  _management_object->set_memory(host_get_memory());
  _management_object->set_cpu_count(host_get_cpu_count());
  _management_object->set_cpu_cores(host_get_cpu_number_of_cores());
  _management_object->set_cpu_model(host_get_cpu_model());

  return 1;
}

Manageable::status_t
HostAgent::ManagementMethod(uint32_t method, Args& arguments, string& text)
{
  switch(method)
    {
    case _qmf::Host::METHOD_SHUTDOWN:
      host_shutdown();
      return Manageable::STATUS_OK;
    case _qmf::Host::METHOD_REBOOT:
      host_reboot();
      return Manageable::STATUS_OK;
    }

  return Manageable::STATUS_NOT_IMPLEMENTED;
}

void
HostAgent::heartbeat()
{
    uint64_t timestamp = 0L, now = 0L;
    sigar_loadavg_t avg;
    sigar_proc_stat_t procs;
    static uint32_t _heartbeat_sequence = 1;

    if(_management_object->get_update_interval() == 0) {
	/* Updates disabled, just sleep */
	qpid::sys::sleep(60);
    }

#ifndef MSVC
    timestamp = ::time(NULL);
#endif

    now = timestamp * 1000000000;
    this->_agent->raiseEvent(_qmf::EventHeartbeat(timestamp, _heartbeat_sequence++));
    _management_object->set_last_updated(now);
    _management_object->set_sequence(_heartbeat_sequence);

    // update the load averages
    host_get_load_averages(&avg);
    host_get_processes(&procs);
    _management_object->set_load_average_1(avg.loadavg[0]);
    _management_object->set_load_average_5(avg.loadavg[1]);
    _management_object->set_load_average_15(avg.loadavg[2]);
    _management_object->set_proc_total(procs.total);
    _management_object->set_proc_sleeping(procs.sleeping);
    _management_object->set_proc_running(procs.running);
    _management_object->set_proc_zombie(procs.zombie);
    _management_object->set_proc_stopped(procs.stopped);
    _management_object->set_proc_idle(procs.idle);
    _management_object->set_free_swap(host_get_swap_free());
    _management_object->set_free_mem(host_get_mem_free());

    qpid::sys::sleep(_management_object->get_update_interval());
}

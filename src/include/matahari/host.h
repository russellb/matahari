/* host.h - Copyright (C) 2009 Red Hat, Inc.
 * Written by Arjun Roy <arroy@redhat.com>
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

/**
 * \file
 * \brief Host API
 * \ingroup coreapi
 */

#ifndef __MH_HOST_H__
#define __MH_HOST_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdlib.h>
#include <sigar.h>
#include <glib.h>

#include "matahari/errors.h"

/**
 * Get a UUID for a host.
 *
 * \param[in] lifetime This function can retrieve a few different UUIDs from
 *            different sources that each have different lifetimes.
 *            The valid lifetimes are:
 *             - "Hardware", unique to the host itself
 *             - "Filesystem", valid for the lifetime of the OS
 *             - "Reboot", reset on each reboot of the host
 *             - "Agent", reset on each execution of the agent serving up this information.
 *             - "Custom", set by mh_host_set_uuid()
 *
 * \return the UUID
 */
const char *
mh_host_get_uuid(const char *lifetime);

/**
 * Set a custom UUID for this host.
 *
 * \param[in] lifetime Which lifetime to set.  The only valid input for
 *            this parameter right now is "Custom".
 * \param[in] uuid the value of the UUID to set
 *
 * \retval 0 success
 * \retval non-zero failure
 */
int
mh_host_set_uuid(const char *lifetime, const char *uuid);

const char *
mh_host_get_hostname(void);

const char *
mh_host_get_operating_system(void);

const char *
mh_host_get_architecture(void);

const char *
mh_host_get_cpu_model(void);

const char *
mh_host_get_cpu_flags(void);

uint64_t
mh_host_get_memory(void);

uint64_t
mh_host_get_mem_free(void);

uint64_t
mh_host_get_swap(void);

uint64_t
mh_host_get_swap_free(void);

int
mh_host_get_cpu_count(void);

int
mh_host_get_cpu_number_of_cores(void);

int
mh_host_get_cpu_wordsize(void);

/**
 * Identify a host.
 *
 * Trigger the system beep to help identify a host.
 *
 * \retval 0 success
 * \retval non-zero failure
 */
int
mh_host_identify(void);

void
mh_host_reboot(void);

void
mh_host_shutdown(void);

void
mh_host_get_load_averages(sigar_loadavg_t *avg);

void
mh_host_get_processes(sigar_proc_stat_t *procs);

/**
 * Set power management profile.
 *
 * \param[in] profile One of profile returned by mh_host_list_power_profiles
 *                    function
 * \return see enum mh_result
 */
enum mh_result
mh_host_set_power_profile(const char *profile);

/**
 * Get current power management profile.
 *
 * \param[out] profile Variable where current profile will be written to.
 *                     Variable must be freed with free().
 *
 * \return see enum mh_result
 */
enum mh_result
mh_host_get_power_profile(char **profile);

/**
 * Get list of all available power management profiles.
 *
 * \note The return of this routine must be freed with
 *       g_list_free_full(returned_list, free);
 *
 * \return list of all profiles.
 */
GList *
mh_host_list_power_profiles(void);

#ifdef __cplusplus
}
#endif

#endif // __MH_HOST_H__

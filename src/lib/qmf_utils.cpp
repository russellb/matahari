/* qmf_utils.cpp - Copyright (C) 2011 Red Hat, Inc.
 * Written by Russell Bryant <rbryant@redhat.com>
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

#ifdef WIN32
#include <windows.h>
#endif

#include "matahari/qmf_utils.h"

#include <qmf/SchemaId.h>
#include <qmf/SchemaProperty.h>
#include <qmf/SchemaMethod.h>
#include <qmf/ConsoleEvent.h>

static bool get_schema(::qmf::Agent &agent,
        const std::string &package_name,
        const std::string &schema_name,
        ::qmf::Schema *schema)
{
    agent.querySchema();

    for (unsigned int i = 0; i < agent.getSchemaIdCount(package_name); i++) {
        ::qmf::SchemaId id = agent.getSchemaId(package_name, i);

        if (id.getName() != schema_name) {
            continue;
        }

        *schema = agent.getSchema(id);

        return true;
    }

    return false;
}

bool
matahari::qmf::schema_has_property(::qmf::Agent &agent,
        const std::string &package_name,
        const std::string &schema_name,
        const std::string &prop_name)
{
    ::qmf::Schema schema;

    if (!get_schema(agent, package_name, schema_name, &schema)) {
        return false;
    }

    for (uint32_t i = 0; i < schema.getPropertyCount(); i++) {
        ::qmf::SchemaProperty p(schema.getProperty(i));

        if (p.getName() == prop_name) {
            return true;
        }
    }

    return false;
}

bool
matahari::qmf::schema_has_method(::qmf::Agent &agent,
        const std::string &package_name,
        const std::string &schema_name,
        const std::string &method_name)
{
    ::qmf::Schema schema;

    if (!get_schema(agent, package_name, schema_name, &schema)) {
        return false;
    }

    for (uint32_t i = 0; i < schema.getMethodCount(); i++) {
        ::qmf::SchemaMethod m(schema.getMethod(i));

        if (m.getName() == method_name) {
            return true;
        }
    }

    return false;
}

bool
matahari::qmf::schema_method_has_arg(::qmf::Agent &agent,
        const std::string &package_name,
        const std::string &schema_name,
        const std::string &method_name,
        const std::string &arg_name)
{
    ::qmf::Schema schema;

    if (!get_schema(agent, package_name, schema_name, &schema)) {
        return false;
    }

    for (uint32_t i = 0; i < schema.getMethodCount(); i++) {
        ::qmf::SchemaMethod m(schema.getMethod(i));

        if (m.getName() == method_name) {
            for (uint32_t j = 0; j < m.getArgumentCount(); j++) {
                ::qmf::SchemaProperty p(m.getArgument(j));

                if (p.getName() == arg_name) {
                    return true;
                }
            }
        }
    }

    return false;
}

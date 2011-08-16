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

/**
 * \file
 * \brief QMF Utilities
 * \ingroup coreapi
 *
 * This API includes a set of helper functions for authors of code using QMF.
 */

#ifndef __MH_QMF_UTILS_H__
#define __MH_QMF_UTILS_H__

#include <qmf/Agent.h>
#include <qmf/Schema.h>

namespace matahari {
namespace qmf {

/**
 * Check to see if a property is included in a given schema.
 *
 * \param agent[in] the agent to check
 * \param package_name[in] the package name to check
 * \param schema_name[in] the schema name to check
 * \param prop_name[in] the property name to check for
 *
 * \retval true the property was found in the schema
 * \retval false the property was not found in the schema
 */
bool
schema_has_property(::qmf::Agent &agent,
        const std::string &package_name,
        const std::string &schema_name,
        const std::string &prop_name);

/**
 * Check to see if a method is included in a given schema.
 *
 * \param agent[in] the agent to check
 * \param schema_name[in] the schema name to check
 * \param prop_name[in] the property name to check for
 * \param method_name[in] the method name to check for
 *
 * \retval true the method was found in the schema
 * \retval false the method was not found in the schema
 */
bool
schema_has_method(::qmf::Agent &agent,
        const std::string &package_name,
        const std::string &schema_name,
        const std::string &method_name);

/*
 * Check to see if a method has a given argument
 *
 * \param agent[in] the agent to check
 * \param schema_name[in] the schema name to check
 * \param prop_name[in] the property name to check for
 * \param method_name[in] the method name to check for
 * \param arg_name[in] the argument name to check for
 *
 * \retval true the argument for the given method was found
 * \retval false the method or argument for the method was not found
 */
bool
schema_method_has_arg(::qmf::Agent &agent,
        const std::string &package_name,
        const std::string &schema_name,
        const std::string &method_name,
        const std::string &arg_name);

}; /* namespace qmf */
}; /* namespace matahari */

#endif /* __MH_QMF_UTILS_H__ */

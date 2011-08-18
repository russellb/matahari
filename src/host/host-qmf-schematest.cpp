/* host-qmf-schematest.cpp - Copyright (C) 2011 Red Hat, Inc.
 * Written by Russell Bryant <rbryant@redhat.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301  USA
 */

#ifndef WIN32
#include "config.h"
#endif

#include <qpid/messaging/Connection.h>
#include <qpid/messaging/Duration.h>
#include <qmf/ConsoleSession.h>
#include <qmf/ConsoleEvent.h>
#include <qmf/Agent.h>
#include <qmf/Data.h>
#include <qmf/DataAddr.h>
#include <qmf/Query.h>
#include <qmf/Schema.h>
#include <qmf/SchemaMethod.h>
#include <qmf/SchemaProperty.h>
#include <qpid/types/Variant.h>

#include "matahari/agent.h"
#include "matahari/qmf_utils.h"

#include <string>
#include <iostream>
#include <sstream>

extern "C" {
#include "matahari/logging.h"
}

using namespace std;
using namespace qmf;
using qpid::types::Variant;
using qpid::messaging::Duration;

int main(int argc, char** argv)
{
    qpid::types::Variant::Map options;
    qpid::types::Variant::Map callOptions;
    string sessionOptions;
    ConsoleEvent event;

    options["api"] = "Host";
    options["host-dns"] = mh_hostname();
    options["interval"] = 0;
    options["timeout"] = 60000;
    options["parameters"] = qpid::types::Variant::Map();

    mh_log_init("host-schematest", LOG_TRACE, TRUE);

    qpid::types::Variant::Map amqpOptions = mh_parse_options("host-schematest", argc, argv, options);

    /* Re-initialize logging now that we've completed option processing */
    mh_log_init("host-schematest", mh_log_level, mh_log_level > LOG_INFO);

    qpid::messaging::Connection connection = mh_connect(options, amqpOptions, TRUE);
    ConsoleSession session(connection, sessionOptions);
    std::stringstream filter;

    filter << "[and";

    filter << ", [eq, _vendor, [quote, 'matahariproject.org']]";
    filter << ", [eq, _product, [quote, 'host']]";
    filter << "]";

    /* Only interested in agents under matahariproject.org vendor
     * Set before opening the session to avoid unwanted events
     */
    session.setAgentFilter(filter.str());
    session.open();

    while (session.getAgentCount() == 0) {
        /* Wait around until some agents show up */
        g_usleep(1000);
    }

    while (options.count("action") == 0) {
        if (!session.nextEvent(event)) {
            g_usleep(1000);
            continue;
        }

        switch (event.getType()) {
        case CONSOLE_AGENT_ADD:
        {
            Agent agent;
            Schema schema;

            agent = event.getAgent();

            cout << "Agent " << agent.getName() << " on " << agent.getAttribute("hostname") << endl;

            agent.querySchema();

            cout << "--> Packages:" << endl;
            for (unsigned int i = 0; i < agent.getPackageCount(); i++) {
                const string &package = agent.getPackage(i);
                cout << "----> Package: " << package << endl;
                cout << "------> Schemas:" << endl;
                for (unsigned int j = 0; j < agent.getSchemaIdCount(package); j++) {
                    SchemaId id = agent.getSchemaId(package, j);

                    cout << "--------> Schema: " << id.getName() << " " << id.getType() << endl;

                    Schema s(agent.getSchema(id, qpid::messaging::Duration::MINUTE));
                    if (j == 0) {
                        schema = s;
                    }

                    cout << "----------> Properties: " << endl;
                    for (unsigned int k = 0; k < s.getPropertyCount(); k++) {
                        SchemaProperty p = s.getProperty(k);
                        cout << "------------> Property: " << p.getName() << endl;
                    }

                    cout << "----------> Methods: " << endl;
                    for (unsigned int k = 0; k < s.getMethodCount(); k++) {
                        SchemaMethod m = s.getMethod(k);
                        cout << "------------> Method: " << m.getName() << endl;
                    }
                }
            }

            //qmf::Schema s(agent.getSchema(qmf::SchemaId(qmf::SCHEMA_TYPE_DATA, "org.matahariproject", "Host")));

            cout << "schema_has_property(org.matahariproject, Host, hostname): " \
                << matahari::qmf::schema_has_property(agent, "org.matahariproject", "Host", "hostname") \
                << endl;
            cout << "schema.getProperty(): " << schema.getProperty("hostname") << endl;
            cout << "schema_has_property(org.matahariproject, Host, foo): " \
                << matahari::qmf::schema_has_property(agent, "org.matahariproject", "Host", "foo") \
                << endl;
            cout << "schema.getProperty(): " << schema.getProperty("foo") << endl;
            cout << "schema_has_method(org.matahariproject, Host, reboot): " \
                << matahari::qmf::schema_has_method(agent, "org.matahariproject", "Host", "reboot") \
                << endl;
            cout << "schema.getMethod(): " << schema.getMethod("reboot") << endl;
            cout << "schema_has_method(org.matahariproject, Host, foo): " \
                << matahari::qmf::schema_has_method(agent, "org.matahariproject", "Host", "foo") \
                << endl;
            cout << "schema.getMethod(): " << schema.getMethod("foo") << endl;
            cout << "schema_method_has_arg(org.matahariproject, Host, get_uuid, lifetime): " \
                << matahari::qmf::schema_method_has_arg(agent, "org.matahariproject", "Host", "get_uuid", "lifetime") \
                << endl;
            qmf::SchemaMethod method(schema.getMethod("get_uuid"));
            cout << "method.getArgument(): " << method.getArgument("lifetime") << endl;
            cout << "schema_method_has_arg(org.matahariproject, Host, get_uuid, foo): " \
                << matahari::qmf::schema_method_has_arg(agent, "org.matahariproject", "Host", "get_uuid", "foo") \
                << endl;
            cout << "method.getArgument(): " << method.getArgument("foo") << endl;

            break;
        }
        case CONSOLE_AGENT_DEL:
            cout << "Agent " << event.getAgent().getName() << " is no longer available" << endl;
            break;
        case CONSOLE_EVENT:
        {
            uint32_t llpc = 0;
            cout << "Event from agent " << event.getAgent().getName() << ": " << endl;
            for ( ; llpc < event.getDataCount(); llpc++) {
                cout << "Event data: " << event.getData(llpc).getProperties() << endl;
            }
            break;
        }
        default:
            cout << "Unknown event "<< event.getType() <<" from agent " << event.getAgent().getName() << ": " << event.getArguments() << endl;
        }
    }

    return 0;
}

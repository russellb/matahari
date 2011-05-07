#!/usr/bin/env python
'''QMF event monitor

Copyright (C) 2011, Russell Bryant <russell@russellbryant.net>

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.

This software is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
General Public License for more details.

You should have received a copy of the GNU General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
'''

import sys
import time
from optparse import OptionParser
import cqpid
from qmf2 import ConsoleSession, ConsoleHandler


SEVERITY_MAP = {
    0 : "EMERGENCY",
    1 : "ALERT",
    2 : "CRITICAL",
    3 : "ERROR",
    4 : "WARNING",
    5 : "NOTICE",
    6 : "INFO",
    7 : "DEBUG"
}


def severity_str(severity):
    if severity in SEVERITY_MAP:
        return SEVERITY_MAP[severity]
    else:
        return "UNKNOWN(%d)" % severity


class EventConsole(ConsoleHandler):
    def __init__(self, options):
        self.options = options
        self.conn = cqpid.Connection("%s:%d" % (self.options.broker,
                                self.options.port))
        self.conn.open()
        self.session = ConsoleSession(self.conn)
        self.session.open()
        ConsoleHandler.__init__(self, self.session)

    def agentAdded(self, agent):
        print "Agent added: %s\n" % agent

    def agentDeleted(self, agent):
        print "Agent deleted: %s\n" % agent

    def eventRaised(self, agent, data, timestamp, severity):
        print "Event [%s]:" % severity_str(severity)
        print " ==> Agent:     %s" % agent
        print " ==> Timestamp: %s" % timestamp
        print " ==> Data:      %s\n" % data.getProperties()


def main(argv=None):
    if argv is None:
        argv = sys.argv

    parser = OptionParser()

    parser.add_option("-b", "--broker", action="store", type="string",
            dest="broker", default="127.0.0.1",
            help="broker hostname or IP address")
    parser.add_option("-p", "--port", action="store", type="int",
            dest="port", default=49000,
            help="broker port number")

    (options, args) = parser.parse_args(argv)

    console = EventConsole(options)

    try:
        console.run()
    except (KeyboardInterrupt, SystemExit):
        pass

    return 0


if __name__ == "__main__":
    sys.exit(main())

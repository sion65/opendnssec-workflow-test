# $Id$
#
# Copyright (c) 2009 NLNet Labs. All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions
# are met:
# 1. Redistributions of source code must retain the above copyright
#    notice, this list of conditions and the following disclaimer.
# 2. Redistributions in binary form must reproduce the above copyright
#    notice, this list of conditions and the following disclaimer in the
#    documentation and/or other materials provided with the distribution.
#
# THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
# IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
# WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
# ARE DISCLAIMED. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY
# DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
# DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
# GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
# INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
# IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
# OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
# IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#

"""Some general utility functions"""

import subprocess
import re
from datetime import datetime
import syslog
from Ft.Xml.XPath import Evaluate
import getpass
import shutil

class ToolException(Exception):
    """General exception class for exceptions when running external
    programs, like the signer tools"""
    pass

def run_tool(command, input_fd=None):
    """Run a system command with Popen(), if input_fd is not given,
       it will be set to PIPE. The subprocess is returned."""
    syslog.syslog(syslog.LOG_INFO, "Run command: '"+" ".join(command)+"'")
    try:
        if (input_fd):
            subp = subprocess.Popen(command, stdin=input_fd,
                                    stdout=subprocess.PIPE,
                                    stderr=subprocess.PIPE)
        else:
            subp = subprocess.Popen(command, stdin=subprocess.PIPE,
                                    stdout=subprocess.PIPE,
                                    stderr=subprocess.PIPE)
        return subp
    except OSError:
        syslog.syslog(syslog.LOG_ERR, "command not found: " + command[0])
        return None

# for a single xml durection object, with only 1 path
# for more elaborate paths, diy
def get_xml_data(xpath, xml, optional=False):
    """Get the element data of the first element specified by the xpath
       that is found in the xml blob. If optional is true, None is
       returned if it cannot be found. Otherwise an exception will be
       raised."""
    try:
        xmlb = Evaluate(xpath, xml)
        if xmlb and len(xmlb) > 0 and xmlb[0].firstChild:
            return xmlb[0].firstChild.data
        elif optional:
            return None
        else:
            raise Exception("Mandatory XML element not found: " + xpath)
    except IndexError:
        if optional:
            return None
        else:
            raise Exception("Mandatory XML element not found: " + xpath)

# months default to 31 days
# does not account for leap-seconds etc
# 3 ways to specify duration
DURATION_REGEX = re.compile("^(?P<negative>-)?P"
                            "(?:(?P<years>[0-9]+)Y)?"
                            "(?:(?P<months>[0-9]+)M)?"
                            "(?:(?P<weeks>[0-9]+)W)?"
                            "(?:(?P<days>[0-9]+)D)?"
                            "(?:T"
                            "(?:(?P<hours>[0-9]+)H)?"
                            "(?:(?P<minutes>[0-9]+)M)?"
                            "(?:(?P<seconds>[0-9]+)S)?"
                            ")?$"
                           )
DURATION_REGEX_ALT = re.compile("^(?P<negative>-)?P"
                                "(?P<years>[0-9]{4})"
                                "(?P<months>[0-9]{2})"
                                "(?P<weeks>)"
                                "(?P<days>[0-9]{2})"
                                "T"
                                "(?P<hours>[0-9]{2})"
                                "(?P<minutes>[0-9]{2})"
                                "(?P<seconds>[0-9]{2})"
                               )
DURATION_REGEX_ALT2 = re.compile("^(?P<negative>-)?P"
                                "(?P<years>[0-9]{4})"
                                "-(?P<months>[0-9]{2})"
                                "(?P<weeks>)"
                                "-(?P<days>[0-9]{2})"
                                "T"
                                "(?P<hours>[0-9]{2})"
                                ":(?P<minutes>[0-9]{2})"
                                ":(?P<seconds>[0-9]{2})"
                               )

def write_p(subp, val, prefix):
    """If val is not None, write prefix + str(val) + "\n" to the stdin
    of subp"""
    if subp.stdin and val:
        syslog.syslog(syslog.LOG_INFO,
                      "write to subp: " +\
                      prefix + str(val))
        subp.stdin.write(prefix)
        subp.stdin.write(str(val))
        subp.stdin.write("\n")

def syslog_facility_int(string):
    """Returns the syslog integer for the given facility string"""
    facilities = {"KERN": syslog.LOG_KERN,
                  "USER": syslog.LOG_USER,
                  "MAIL": syslog.LOG_MAIL,
                  "DAEMON": syslog.LOG_DAEMON,
                  "AUTH": syslog.LOG_AUTH,
                  "LPR": syslog.LOG_LPR,
                  "NEWS": syslog.LOG_NEWS,
                  "UUCP": syslog.LOG_UUCP,
                  "CRON": syslog.LOG_CRON,
                  "LOCAL0": syslog.LOG_LOCAL0,
                  "LOCAL1": syslog.LOG_LOCAL1,
                  "LOCAL2": syslog.LOG_LOCAL2,
                  "LOCAL3": syslog.LOG_LOCAL3,
                  "LOCAL4": syslog.LOG_LOCAL4,
                  "LOCAL5": syslog.LOG_LOCAL5,
                  "LOCAL6": syslog.LOG_LOCAL6,
                  "LOCAL7": syslog.LOG_LOCAL7
                 }
    try:
        return facilities[string.upper()]
    except KeyError:
        print "Warning: unknown syslog facility %s, using DAEMON" % \
              string.upper()
        return syslog.LOG_DAEMON

def datestamp(timestamp):
    """Returns the date (YYYYMMddhhmmss) representation of the given
    timestamp (seconds since epoch)"""
    return datetime.utcfromtimestamp(timestamp).strftime("%Y%m%d%H%M%S")

def parse_duration_part(match, name, multiplier):
    """If the Match object has a group 'name', its value is interpreted
    as an integer, and multiplied with the multiplier. The result is
    returned. If the group does not exist, 0 is returned"""
    grp = match.group(name)
    if grp:
        return multiplier * int(grp)
    else:
        return 0

def parse_duration(duration_string):
    """Parse an XML duration string. The number of seconds represented
       by the string is returned"""
    if not duration_string:
        return None
    match = DURATION_REGEX.match(duration_string)
    result = 0
    if not match:
        # raise error
        match = DURATION_REGEX_ALT.match(duration_string)
        if not match:
            match = DURATION_REGEX_ALT2.match(duration_string)
            if not match:
                raise Exception("Bad duration format: " +duration_string)

    result += parse_duration_part(match, "years", 31556926)
    result += parse_duration_part(match, "months", 2678400)
    result += parse_duration_part(match, "weeks", 604800)
    result += parse_duration_part(match, "days", 86400)
    result += parse_duration_part(match, "hours", 3600)
    result += parse_duration_part(match, "minutes", 60)
    result += parse_duration_part(match, "seconds", 1)

    if match.group("negative"):
        return -result
    else:
        return result

def query_pin(token):
    """Queries for the PIN, which isn't checked further (erroneous
    PIN will simply result in errors later. Token is the associative
    array as created in EngineConfiguration.read_config_file()"""
    pin = getpass.getpass("Please enter the PIN for token " +\
                          token["name"] + ": ")
    return pin

def move_file(source, target):
    """Moves a file; if the target file exists it is deleted"""
    shutil.move(source, target)

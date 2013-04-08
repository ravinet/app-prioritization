#!/usr/bin/python

from mininet.topo import Topo
from mininet.net import Mininet
from mininet.node import Host
from mininet.link import Link
from mininet.cli import CLI
from mininet.util import dumpNodeConnections
from mininet.util import ensureRoot

from subprocess import Popen, PIPE
from time import sleep, time

import sys
import os
import math
import requests

ensureRoot()

class ProtoTester(Topo):
    def __init__(self):

        # Initialise topology
        Topo.__init__(self)

        # Add hosts and switches
        sender = self.addHost('sender', ip='10.0.1.1', mac='00:00:00:00:00:01')
        LTE = self.addHost('LTE', ip='0.0.0.0')
        receiver = self.addHost('receiver', ip='10.0.1.2', mac='00:00:00:00:00:02')

        s1 = self.addSwitch('s1')
        s2 = self.addSwitch('s2')

        # Add links
        self.addLink(sender, s1)
        self.addLink(s1, LTE)
        self.addLink(LTE, s2)
        self.addLink(s2, receiver)
def set_all_IP(net, sender, LTE, receiver):
    sender.sendCmd('ifconfig sender-eth0 10.0.1.1 netmask 255.255.255.0')
    sender.waitOutput()
    LTE.sendCmd('ifconfig LTE-eth0 up')
    LTE.waitOutput()
    LTE.sendCmd('ifconfig LTE-eth1 up')
    LTE.waitOutput()
    receiver.sendCmd('ifconfig receiver-eth0 10.0.1.2 netmask 255.255.255.0')
    receiver.waitOutput()

    sender.sendCmd('echo 1 > /proc/sys/net/ipv6/conf/all/disable_ipv6')
    sender.waitOutput()
    LTE.sendCmd('echo 1 > /proc/sys/net/ipv6/conf/all/disable_ipv6')
    LTE.waitOutput()
    receiver.sendCmd('echo 1 > /proc/sys/net/ipv6/conf/all/disable_ipv6')
    receiver.waitOutput()

def display_routes(net, sender, LTE, receiver):
    print 'sender route...'
    sender.sendCmd('route -n')
    print sender.waitOutput()
    print 'LTE route...'
    LTE.sendCmd('route -n')
    print LTE.waitOutput()
    print 'receiver route...'
    receiver.sendCmd('route -n')
    print receiver.waitOutput()

def run_cellsim(LTE):
    LTE.sendCmd('/home/ravinet/mininet/mininet/multisend/sender/cellsim-setup.sh LTE-eth0 LTE-eth1')
    LTE.waitOutput()
    print "Running cellsim (this will take a few minutes)..."
    LTE.sendCmd('/home/ravinet/mininet/mininet/multisend/sender/cellsim-runner.sh')
    LTE.waitOutput()
    print "done."

def run_datagrump(sender, receiver):
    print "Running flow1receiver...",
    receiver.sendCmd('/home/ravinet/mininet/mininet/datagrump/flow1receiver 9200 >/tmp/receiver-stdout 2>/tmp/receiver-stderr &')
    receiver.waitOutput()
    print "done."
    print "Running flow1sender...",
    sender.sendCmd('/home/ravinet/mininet/mininet/datagrump/flow1sender 10.0.1.2 9200 debug >/tmp/sender-stdout 2>/tmp/sender-stderr &')
    sender.waitOutput()
    print "done."

    print "Running flow2receiver...",
    receiver.sendCmd('/home/ravinet/mininet/mininet/datagrump/flow2receiver 9100 >/tmp/receiver2-stdout 2>/tmp/receiver2-stderr &')
    receiver.waitOutput()
    print "done."
    print "Running flow2sender...",
    sender.sendCmd('/home/ravinet/mininet/mininet/datagrump/flow2sender 10.0.1.2 9100 debug >/tmp/sender-stdout 2>/tmp/sender-stderr &')
    sender.waitOutput()
    print "done."

def print_welcome_message():
    print "####################################################################"
    print "#                                                                  #"
    print "#               6.829 PS 2 Emulated Network Test                   #"
    print "#                                                                  #"
    print "#          running sender <=> cellsim <=> receiver                 #"
    print "#                                                                  #"
    print "#  Debug output in /tmp/{sender,receiver,cellsim}-{stdout,stderr}  #"
    print "#                                                                  #"
    print "####################################################################"
    print

def run_cellsim_topology():
    print_welcome_message()

    os.system( "killall -q controller" )
    os.system( "killall -q cellsim" )
    os.system( "killall -q mysender" )
    os.system( "killall -q myreceiver" )

    topo = ProtoTester()
    net = Mininet(topo=topo, host=Host, link=Link)
    net.start()

    sender = net.getNodeByName('sender')
    LTE = net.getNodeByName('LTE')
    receiver = net.getNodeByName('receiver')

    set_all_IP(net, sender, LTE, receiver)

    #Dump connections
    #dumpNodeConnections(net.hosts)
    #display_routes(net, sender, LTE, receiver)
    
    #run_cellsim(LTE)
    run_datagrump(sender, receiver)

    run_cellsim(LTE)

#    CLI(net)

    net.stop()

def upload_data( username ):
    print "Uploading data to server...",
    os.system( 'gzip --stdout /tmp/cellsim-stdout > /tmp/to-upload.gz' )
    reply = requests.post( 'http://6829.keithw.org/cgi-bin/6829/upload-data',
                           files={'contents': (username, open( '/tmp/to-upload.gz',
                                                               'rb' ))} )
    print "done. Got reply:"
    print
    print reply.text

if __name__ == '__main__':
    if len(sys.argv) != 2:
        print "Usage: sudo %s [username]" % sys.argv[ 0 ]
    else:
        run_cellsim_topology()
        upload_data( sys.argv[ 1 ] )
                        

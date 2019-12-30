from kea import *
from ipaddress import IPv4Address, IPv4Network

subnet = IPv4Network('10.0.0.0/20')
name_servers = [IPv4Address('192.0.3.1'), IPv4Address('192.0.3.2')]

packed_servers = b''.join([addr.packed
                           for addr in name_servers])

class HostInfo:
    def __init__(self, hwaddr):
        self.hwaddr = hwaddr
        self.addr = subnet[42]

def pkt4_receive(handle):
    query = handle.getArgument('query4')
    handle.setContext('hostinfo', HostInfo(query.getHWAddr()))
    return 0

def lease4_select(handle):
    hostinfo = handle.getContext('hostinfo')
    lease = handle.getArgument('lease4')
    lease.addr = str(hostinfo.addr)
    query = handle.getArgument('query4')
    if query.getType() == DHCPREQUEST:
        handle.setStatus(NEXT_STEP_SKIP)
    return 0

def pkt4_send(handle):
    response = handle.getArgument('response4')
    if response.getType() == DHCPNAK:
        response.setType(DHCPACK)
    hostinfo = handle.getContext('hostinfo')
    response.setYiaddr(str(hostinfo.addr))
    for option in (DHO_SUBNET_MASK, DHO_ROUTERS, DHO_DOMAIN_NAME_SERVERS,
                   DHO_DOMAIN_NAME, DHO_DHCP_LEASE_TIME, DHO_DHCP_RENEWAL_TIME,
                   DHO_DHCP_REBINDING_TIME):
        response.delOption(option)
    response.addOption(Option(DHO_SUBNET_MASK).setBytes(subnet.netmask.packed))
    response.addOption(Option(DHO_ROUTERS).setBytes(subnet[1].packed))
    response.addOption(Option(DHO_DOMAIN_NAME_SERVERS).setBytes(packed_servers))
    response.addOption(Option(DHO_DOMAIN_NAME).setString('simplehook.org'))
    response.addOption(Option(DHO_DHCP_LEASE_TIME).setUint32(7200))
    response.addOption(Option(DHO_DHCP_RENEWAL_TIME).setUint32(1800))
    response.addOption(Option(DHO_DHCP_REBINDING_TIME).setUint32(3600))
    return 0

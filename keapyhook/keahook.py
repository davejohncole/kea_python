from kea import *
from ipaddress import IPv4Address, IPv4Network

subnet = IPv4Network('10.0.0.0/20')
name_servers = [IPv4Address('192.0.3.1'), IPv4Address('192.0.3.2')]

packed_servers = b''.join([addr.packed
                           for addr in name_servers])

def lease4_select(handle):
    lease = handle.getArgument('lease4')
    addr = str(subnet[42])
    lease.addr = addr
    handle.setContext('addr', addr)
    if handle.getArgument('query4').getType() == DHCPREQUEST:
        handle.setStatus(NEXT_STEP_SKIP)
    return 0

def pkt4_send(handle):
    response = handle.getArgument('response4')
    if response.getType() == DHCPNAK:
        response.setType(DHCPACK)
    addr = handle.getContext('addr')
    response.setRemoteAddr(addr)
    response.setYiaddr(addr)
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

def load(handle):
    return 0

def unload():
    return 0

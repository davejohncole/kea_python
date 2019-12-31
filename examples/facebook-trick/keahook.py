from kea import *
from ipaddress import IPv4Address, IPv4Network

class Config:
    def __init__(self, conf):
        dhcp4 = conf['Dhcp4']
        self.options = [Option(DHO_DHCP_LEASE_TIME).setUint32(dhcp4.get('valid-lifetime', 7200)),
                        Option(DHO_DHCP_RENEWAL_TIME).setUint32(dhcp4.get('renew-timer', 1800)),
                        Option(DHO_DHCP_REBINDING_TIME).setUint32(dhcp4.get('rebind-timer', 3600))]
        subnet4 = dhcp4.get('subnet4', [])
        logger.info('Dhcp4 %s' % dhcp4)
        if subnet4:
            subnet0 = subnet4[0]
        else:
            subnet0 = {}
        self.subnet = IPv4Network(subnet0.get('subnet', '10.0.0.0/20'))
        logger.info('using subnet %s' % self.subnet)
        self.options.append(Option(DHO_SUBNET_MASK).setBytes(self.subnet.netmask.packed))
        # hard code nameservers
        servers = [IPv4Address('192.0.3.1'), IPv4Address('192.0.3.2')]
        packed_servers = b''.join([addr.packed
                                   for addr in servers])
        self.options.append(Option(DHO_DOMAIN_NAME_SERVERS).setBytes(packed_servers))
        # hard code routers and domainname
        self.options.append(Option(DHO_ROUTERS).setBytes(self.subnet[1].packed))
        self.options.append(Option(DHO_DOMAIN_NAME).setString('simplehook.org'))

class HostInfo:
    def __init__(self, hwaddr):
        self.hwaddr = hwaddr
        self.addr = config.subnet[42]

def load(handle):
    global config
    config = Config(SrvConfig('staging').toElement())
    return 0

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
    for option in config.options:
        response.delOption(option.getType())
        response.addOption(option)
    return 0

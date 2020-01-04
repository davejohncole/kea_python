from kea import *
from ipaddress import IPv4Address, IPv4Network
from prometheus_client import start_http_server, Counter

PKT_RECEIVE = Counter('dhcp4_pkt_receive_total', 'Packets received', ['type'])
PKT_SEND = Counter('dhcp4_pkt_send_total', 'Packets sent', ['type'])

class Config:
    def __init__(self, conf):
        dhcp4 = conf['Dhcp4']
        self.options = [Option(DHO_DHCP_LEASE_TIME).setUint32(dhcp4.get('valid-lifetime', 7200)),
                        Option(DHO_DHCP_RENEWAL_TIME).setUint32(dhcp4.get('renew-timer', 1800)),
                        Option(DHO_DHCP_REBINDING_TIME).setUint32(dhcp4.get('rebind-timer', 3600))]
        self.subnet = IPv4Network('10.0.0.0/8')
        self.options.append(Option(DHO_SUBNET_MASK).setBytes(self.subnet.netmask.packed))
        # hard code nameservers
        servers = [IPv4Address('192.168.3.1'), IPv4Address('192.168.3.2')]
        packed_servers = b''.join([addr.packed
                                   for addr in servers])
        self.options.append(Option(DHO_DOMAIN_NAME_SERVERS).setBytes(packed_servers))
        # hard code routers and domainname
        self.options.append(Option(DHO_ROUTERS).setBytes(self.subnet[1].packed))
        self.options.append(Option(DHO_DOMAIN_NAME).setString('facebook.com'))

def load(handle):
    global config, type_to_label
    config = Config(CfgMgr().getStagingCfg().toElement())
    type_to_label = dict([(v, k[4:].lower())
                          for k, v in globals().items()
                          if k.startswith('DHCP')])
    start_http_server(9100)
    return 0

def pkt4_receive(handle):
    query = handle.getArgument('query4')
    PKT_RECEIVE.labels(type=type_to_label.get(query.getType(), 'unknown')).inc()
    # client must request address in Option 82, suboption 1.
    o = query.getOption(DHO_DHCP_AGENT_OPTIONS)
    if not o:
        raise RuntimeError('client must send option %s' % DHO_DHCP_AGENT_OPTIONS)
    o = o.getOption(1)
    if not o:
        raise RuntimeError('missing suboption 1 in option %s' % DHO_DHCP_AGENT_OPTIONS)
    handle.setContext('requested-addr', o.getString())
    return 0

def lease4_select(handle):
    lease = handle.getArgument('lease4')
    lease.addr = handle.getContext('requested-addr')
    query = handle.getArgument('query4')
    if query.getType() == DHCPREQUEST:
        handle.setStatus(NEXT_STEP_SKIP)
    return 0

def pkt4_send(handle):
    response = handle.getArgument('response4')
    if response.getType() == DHCPNAK:
        response.setType(DHCPACK)
    PKT_SEND.labels(type=type_to_label.get(response.getType(), 'unknown')).inc()
    addr = handle.getContext('requested-addr')
    response.setYiaddr(addr)
    for option in config.options:
        response.delOption(option.getType())
        response.addOption(option)
    return 0

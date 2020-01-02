import os
import time
import re
import codecs
from ipaddress import IPv4Network


class Child:

    def __init__(self, mac, addr):
        self.mac = mac
        self.addr = addr
        self.pid = os.fork()
        if self.pid:
            return
        devnull = os.open('/dev/null', os.O_WRONLY)
        os.dup2(devnull, 1)
        os.dup2(devnull, 2)
        os.close(devnull)
        opt_val = '01%02x%s' % (len(self.addr), codecs.encode(bytes(self.addr, 'utf-8'), 'hex').decode('utf-8'))
        cmd = 'dhtest -i eth0 -m %s --custom-dhcp-option 82,hex,%s' % (self.mac, opt_val)
        os.execv('/usr/local/bin/dhtest', cmd.split())

    def get_result(self):
        # dhclient writes lease details in file with name == self.mac
        # Client_mac: 02:42:ac:1c:05:02
        # Acquired_ip: 192.168.0.1
        # Server_id: 172.28.5.3
        # Host_mac: 02:42:AC:1C:05:03
        # IP_listen: False. Pid: 0
        try:
            s = open(self.mac).read()
            os.remove(self.mac)
        except OSError:
            return None
        m = re.search(r'Acquired_ip: (.*)', s, re.M)
        if m:
            return m.group(1)
        return None


class RateReport:

    def __init__(self, freq):
        self.freq = freq
        self.last_report = time.time()
        self.times = []
        self.total = 0

    def add_event(self):
        self.total += 1
        now = time.time()
        if now - self.last_report >= self.freq:
            # remove all times older than freq
            pos = 0
            cutoff = now - self.freq
            while pos < len(self.times) and self.times[pos] < cutoff:
                pos += 1
            self.times = self.times[pos:]
            print('%s (%.1f / sec)' % (self.total, len(self.times) / self.freq))
            self.last_report = now
        self.times.append(now)


class Runner:

    def __init__(self):
        self.children = {}
        self.network = IPv4Network('10.0.0.0/8')
        self.num_children = 0
        self.errors = 0
        self.rate = None

    def run(self):
        self.rate = RateReport(3)
        while self.num_children < 10000:
            if len(self.children) < 50:
                self.start_child()
            else:
                self.wait_child()
        while self.children:
            self.wait_child()

    def start_child(self):
        self.num_children += 1
        mac = '%012d' % self.num_children
        bits = []
        for pos in range(0, len(mac), 2):
            bits.append(mac[pos:pos+2])
        mac = ':'.join(bits)
        addr = str(self.network[self.num_children])
        child = Child(mac, addr)
        self.children[child.pid] = child

    def wait_child(self):
        pid, status = os.waitpid(-1, 0)
        self.rate.add_event()
        child = self.children[pid]
        addr = child.get_result()
        if addr != child.addr:
            self.errors += 1
        del self.children[pid]


def main():
    os.chdir('/tmp')
    runner = Runner()
    runner.run()
    print('%s errors' % runner.errors)


if __name__ == '__main__':
    main()

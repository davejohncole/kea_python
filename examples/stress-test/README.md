# stress-test
Demonstrate multiple threads in Python code with a performance test thrown in.

To run this hook you will need to install `curl` and `python3-prometheus-client`.

Start the kea image:
```
djc@laptop:~/play/kea_python$ make run-kea
root@e55a47190d9c:/# cd /workdir/
root@e55a47190d9c:/workdir# apt-get install curl python3-prometheus-client
root@9b06fba64b09:/workdir# /usr/local/sbin/kea-dhcp4 -c examples/stress-test/kea.conf
```

In another shell start bash in the kea container:
```
djc@laptop:~/play/kea_python$ docker exec -it kea bash
root@e55a47190d9c:/# while true; do curl -s http://localhost:9100/ | grep ^dhcp && echo; sleep 0.2; done
```
This will force the Python hook code to execute the prometheus client background thread every 0.2
seconds.  Doing this at the same time as performing a DHCP stress test is a nice way to see that you
can execute Python threads in Kea.

Now the dhtest image:
```
djc@laptop:~/play/kea_python$ make run-dhtest
root@e01939ffd0f4:/# cd /workdir
root@e01939ffd0f4:/workdir# python3 examples/stress-test/stress_test.py --help  
usage: stress_test.py [-h] [--total TOTAL] [--parallel PARALLEL]

optional arguments:
  -h, --help           show this help message and exit
  --total TOTAL        total number of requests to perform
  --parallel PARALLEL  number of parallel requests to perform
root@e01939ffd0f4:/workdir# python3 examples/stress-test/stress_test.py --parallel 50 --total 10000
00:01: 634 at 634/sec
00:02: 1392 at 696/sec
00:03: 1971 at 669/sec
00:04: 2660 at 676/sec
00:05: 3378 at 662/sec
00:06: 4031 at 687/sec
00:07: 4795 at 709/sec
00:08: 5501 at 704/sec
00:09: 6227 at 729/sec
00:10: 7009 at 737/sec
00:11: 7719 at 740/sec
00:12: 8468 at 747/sec
00:13: 9274 at 755/sec
total 10000 at 715/sec with 0 errors
```

As soon as you start the stress test you should start to see output from the `curl` commands.  You do not
see any output initially because the metrics defined in the Python hook have labels.  The time-series for
each metric cannot be created until a label value is defined.

The output from the `curl | grep` should look something like:
```
dhcp4_pkt_send_total{type="offer"} 21182.0
dhcp4_pkt_send_total{type="ack"} 21182.0
dhcp4_pkt_receive_total{type="request"} 21182.0
dhcp4_pkt_receive_total{type="discover"} 21182.0
```

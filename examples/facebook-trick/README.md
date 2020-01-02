# facebook-trick
Implement the function described by facebook in this post:
  https://code.facebook.com/posts/845909058837784/using-isc-kea-dhcp-in-our-data-centers/

Unfortunately the behaviour they describe is for a pre 1.0 release of Kea.  Their trick
no longer works without changes.  When the following code runs in `lease4_select()`:
```python
    if query.getType() == DHCPREQUEST:
        handle.setStatus(NEXT_STEP_SKIP)
```
Kea decides to respond with a NAK packet because you are telling it not to save the lease.
The `pkt4_send()` callout handles this by replacing the NAK with an ACK.

Start the kea image:
```
djc@dave:~/play/kea_python$ make docker-run-kea
root@9b06fba64b09:/# cd /workdir/
root@9b06fba64b09:/workdir# /usr/local/sbin/kea-dhcp4 -c examples/facebook-trick/kea.conf
```

Run the dhtest image:
```
djc:~/play/kea_python$ make docker-run-dhtest
root@95d5aa56a590:/# dhtest -i eth0
DHCP discover sent	 - Client MAC : 02:42:ac:1c:05:02
DHCP offer received	 - Offered IP : 172.28.5.42
DHCP request sent	 - Client MAC : 02:42:ac:1c:05:02
DHCP ack received	 - Acquired IP: 172.28.5.42
```

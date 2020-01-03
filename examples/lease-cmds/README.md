# command-callout

Start the kea image:
```
djc@laptop:~/play/kea_python$ make run-kea
root@9b06fba64b09:/# cd /workdir/
root@9b06fba64b09:/workdir# /usr/local/sbin/kea-dhcp4 -c examples/command-callout/kea.conf
```

Run a shell inside the kea image:
```
djc@laptop:~/play/kea_python$ docker exec -it kea bash
root@9b06fba64b09:/# echo '{"command":"yo"}' | socat UNIX:/var/run/kea4.sock -
{ "status": "cool" }root@9b06fba64b09:/# 
```
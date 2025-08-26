# host-cmds
This example was implemented by following the REST interface described in the Kea
documentation at https://kea.readthedocs.io/en/kea-2.2.0/arm/hooks.html#cb-cmds-configuration-backend-commands.

Run a MySQL instance with the host reservations schema.  Kea will not start if it cannot connect
to the database.
```
djc@laptop:~/play/kea_python$ make run-mysql
```

Start the kea image and run the Python implementation of the `cb-cmds` hook.:
```
djc@laptop:~/play/kea_python$ make run-kea
root@ae43fd3e78da:/workdir# /usr/local/sbin/kea-dhcp4 -c examples/cb-cmds/kea.conf
```

Then exec a bash shell in the kea container and run the `sendcmd.py` program:
```
djc@laptop:~/play/kea_python$ docker exec -it kea bash
root@4cebcb709530:/# cd /workdir/examples/cb-cmds/
root@4cebcb709530:/workdir/examples/host-cmds# python3 -B sendcmd.py
```

Try a DHCP request
```
djc@laptop:~/play/kea_python$ make run-dhtest
root@9bf4e713d72c:/# dhtest -i eth0 -m 02:42:ac:1c:05:02
```

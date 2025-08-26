# host-cmds
This example was implemented by following the REST interface described in the Kea
documentation at https://kea.readthedocs.io/en/kea-1.8.2/arm/hooks.html#host-cmds-host-commands.

Run a MySQL instance with the host reservations schema.  Kea will not start if it cannot connect
to the database.
```
djc@laptop:~/play/kea_python$ make run-mysql
```

Start the kea image and run the Python implementation of the `host-cmds` hook.:
```
djc@laptop:~/play/kea_python$ make run-kea
root@ae43fd3e78da:/source# cd /workdir
root@ae43fd3e78da:/workdir# /usr/local/sbin/kea-dhcp4 -c examples/host-cmds/kea.conf
```

Then exec a bash shell in the kea container and run the `sendcmd.py` program:
```
djc@laptop:~/play/kea_python$ docker exec -it kea bash
root@4cebcb709530:/# cd /workdir/examples/host-cmds/
root@4cebcb709530:/workdir/examples/host-cmds# python3 -B sendcmd.py 
```

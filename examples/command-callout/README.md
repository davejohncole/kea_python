# command-callout

Start the kea image:
```
djc@dave:~/play/kea_python$ make docker-run-kea
root@9b06fba64b09:/# cd /workdir/
root@9b06fba64b09:/workdir# ./run-example examples/command-callout
```

Run a shell inside the kea image:
```
djc:~/play/kea_python$ docker exec -it kea bash
root@9b06fba64b09:/# echo '{"command":"yo"}' | socat UNIX:/var/run/kea4.sock -
{ "status": "cool" }root@9b06fba64b09:/# 
```
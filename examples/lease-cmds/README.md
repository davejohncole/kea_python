# lease-cmds
This example is a re-implementation IPv4 functionality of the `lease-cmds` hook that
comes with Kea.

Start the kea image and run the Python implementation of the `lease-cmds` hook.:
```
djc@laptop:~/play/kea_python$ make run-kea
root@ae43fd3e78da:/source# cd /workdir
root@ae43fd3e78da:/workdir# /usr/local/sbin/kea-dhcp4 -c examples/lease-cmds/kea.conf
```

Then in another shell start a command shell in the kea container in order to run the test
harness that exercises all of the implemented commands:
```
djc@laptop:~/play/kea_python$ docker exec -it kea bash
root@ae43fd3e78da:/source# cd /workdir/examples/lease-cmds/
root@ae43fd3e78da:/workdir/examples/lease-cmds# python3 -m nose
.............................
----------------------------------------------------------------------
Ran 29 tests in 0.045s

OK
```

Now in the first shell, hit ^C to kill Kea and restart it using the official `lease-cmds` hook:
```
root@ae43fd3e78da:/workdir# /usr/local/sbin/kea-dhcp4 -c examples/lease-cmds/kea-reference.conf 
```

Now run the tests again:
```
djc@laptop:~/play/kea_python$ docker exec -it kea bash
root@ae43fd3e78da:/source# cd /workdir/examples/lease-cmds/
root@ae43fd3e78da:/workdir/examples/lease-cmds# python3 -m nose
.............................
----------------------------------------------------------------------
Ran 29 tests in 0.035s

OK
```

The Python implementation of `lease-cmds` is not identical to the C++ library.  There are some
differences between the text returned in server responses.  In the `assertResponse` method the
response is considered correct if the text matches one of two possible values.

For example:
```
    def test_bad_addr_value(self):
        response = send_command('lease4-add',
                                addr='oops')
        self.assertResponse({
                "text": ("Failed to convert 'oops' to address: Failed to convert string to "
                         "address 'oops': Invalid argument(<wire>:0:29)"),
                "result": 1
            },
            response,
            "Failed to convert string to address 'oops': Invalid argument")
```
This test sends a bad address and then checks for either the C++ error text:
```
Failed to convert 'oops' to address: Failed to convert string to address 'oops': Invalid argument(<wire>:0:29)
```

or the error text from the Python implementation:
```
Failed to convert string to address 'oops': Invalid argument
```
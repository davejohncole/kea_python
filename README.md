# Kea Python
Develop Kea hooks in Python.

This hook embeds a Python3 interpreter in Kea and provides a thin wrapper around a subset
of the Kea classes to allow you to develop hooks in Python.  Where possible the classes in
the kea Python module implement the same API as the Kea class so that there are no surprises
when using the module.  You should be able to follow the Kea developer guide at
https://jenkins.isc.org/job/Kea_doc/doxygen/.

After you have built the kea module you can get a list of the contents using:
```
pydoc3 kea
```

Kea documentation is found at https://gitlab.isc.org/isc-projects/kea/wikis/home and can be
downloaded from https://www.isc.org/download/.

This project uses docker images so that the DHCP server and clients can be run on your
development workstation in a virtual network.  Installation of Docker Engine - Community as
is described at https://docs.docker.com/install/linux/docker-ce/ubuntu/ (for Ubuntu).

## Installation on CentOS 7
To build and install this kea_python on CentOS you will need to perform the following (if you
have not already done so):
```
$ sudo yum -y update
$ sudo yum -y install epel-release
$ sudo yum -y install make gcc-c++
$ sudo yum -y install kea kea-hooks kea-devel python3-devel libffi-devel
```
This will install all of the software required to build this project.

If you have manually compiled and installed Kea from source then the following process should
still work, although you will need to install the python3-devel and libffi-devel packages.

Clone the kea_python project, compile it, and install it:
```
$ git clone https://github.com/davejohncole/kea_python.git
$ cd kea_python
$ make build
$ sudo make install
```
The build target creates a `settings.mk` in the parent directory that is used in the build process
for the keahook and keamodule subdirectories.  `settings.mk` contains the location of
dependencies required to build.  It will look something like this:
```
PYTHON_INC = /usr/include/python3.6m
KEA_INC = /usr/include/kea
KEA_HOOKS = /usr/lib64/kea/hooks
KEA_LIBS = /usr/lib64
```
If those settings are not correct then you can manually edit the file.  It will only be re-created
if you remove it manually or by running `make clean`.

## Loading the kea_python hook
To load this hook into kea you will need a `hook-libraries` section in `kea.conf` that looks something
like the following:
```
        "hooks-libraries": [{
            "library": "/usr/lib64/kea/hooks/libkea_python.so",
            "parameters": {
                "libpython": "libpython3.6m.so",
                "module": "/usr/local/lib/kea/keahook.py"
            }
        }],
```
Three things need to be specified:
1. The full path to the kea_python hook in `"library"`.  The `KEA_HOOKS` directory defined in `settings.mk`
   is where the hook will be installed.
1. The name of the python3 shared library in `"libpython"`.  On CentOS 7 you cn determine this by running:
   ```
   $ rpm -ql python3-devel | grep libpython
   /usr/lib64/libpython3.6m.so
   ```
   You only need the filename, not the full path.
1. The full path to the Python module that implements your hook logic in `"module"`.  You can install your
   Python hook anywhere you like.  The directory containing the hook will be added to PYTHONPATH so you
   can install other modules and packages into the same directory.

The reason you specify libpython in kea.conf is because kea_python is not linked with libpython.  This is
due to the way Kea dynamically loads hook libraries.  Kea uses dlopen(3) with the RTLD_LOCAL flag which
prevents any symbols defined in the hook library from being used for subsequent dynamic linking.  Python
would not be able to load any extension modules, including those that are part of Python.

To work around this problem the kea_python hook is not linked with libpython, it instead uses dlopen(3)
with the RTLD_GLOBAL flag to load libpython after it is loaded so that Python symbols are available Python
extension modules.

## Examples
There are a number of examples showing how to use the module.  The notable ones are
* [lease-cmds](examples/lease-cmds) is a re-implementation of the IPv4 functionality in the lease-cmds hook
  bundled with Kea.
* [facebook-trick](examples/facebook-trick) demonstrates how to completely replace the lease allocation
  process.  It was inspired by a Facebook presentation to usenix.  The PDF slides are currenlt available
  at https://www.usenix.org/sites/default/files/conference/protected-files/srecon15europe_slides_failla.pdf
* [host-cmds](examples/host-cmds) is an implementation (without reference to source code) of the REST API
  described at https://kea.readthedocs.io/en/kea-1.6.2/arm/hooks.html#host-cmds-host-commands

## Using docker to experiment
Download a Kea .tar.gz from https://downloads.isc.org/isc/kea/ and place it in your working
directory.

Most of the commands you will need to run are in a `Makefile`.  You can view the targets
by simply running `make`:
```
djc@laptop:~/play/kea_python$ make
run on host
  build-kea-dev   - build kea-dev:1.6.2 image
  build-kea       - build kea:1.6.2 image
  build-dhtest    - build dhtest image
  run-kea-dev     - run kea-dev:1.6.2 shell
  run-kea         - run kea:1.6.2 shell
  run-mysql       - run mariadb for kea with schema for 1.6.2
  run-dhtest      - run dhtest shell
run on host or inside kea-dev shell
  build           - build-hook and build-module
  install         - install-hook and install-module
  clean           - remove all generated files
  build-hook      - build libkea_python
  build-module    - build kea extension module
  clean-module    - delete kea extension module build files
  install-hook    - install libkea_python
  install-module  - install kea extension module
  test-module     - run unit tests for kea extension module
```

By default the project works with kea 1.6.2.  You can override that by specifying the version
in the environment:
```
djc@laptop:~/play/kea_python$ VER=1.7.5 make
run on host
  build-kea-dev   - build kea-dev:1.7.5 image
  build-kea       - build kea:1.7.5 image
  build-dhtest    - build dhtest image
  run-kea-dev     - run kea-dev:1.7.5 shell
  run-kea         - run kea:1.7.5 shell
  run-mysql       - run mariadb for kea with schema for 1.7.5
  run-dhtest      - run dhtest shell
run on host or inside kea-dev shell
  build           - build-hook and build-module
  install         - install-hook and install-module
  clean           - remove all generated files
  build-hook      - build libkea_python
  build-module    - build kea extension module
  clean-module    - delete kea extension module build files
  install-hook    - install libkea_python
  install-module  - install kea extension module
  test-module     - run unit tests for kea extension module
```

The first thing you need to do is build the `kea-dev` image.  This takes quite a while, but you
will only need to do it once.
```
djc@laptop:~/play/kea_python$ make build-kea-dev
```
The resulting image is close to 4G as it contains the Kea source, the installed Kea server and
a complete development environment.

This image intended to be used for C/C++ development on the `kea_python` hook.  If you are only
interested in working in Python then as soon as the build is complete you immediately build the
`kea` image.
```
djc@laptop:~/play/kea_python$ make build-kea
```
The `kea` image uses `kea-dev` to compile the `kea_python` hook and then discards all of the
development related files.  The saving is huge:
```
djc@laptop:~/play/kea_python$ docker images
REPOSITORY          TAG                 IMAGE ID            CREATED             SIZE
kea                 1.6.2               f09ca68bf21a        5 minutes ago       363MB
kea-dev             1.6.2               465d0a9fc510        2 hours ago         3.85GB
debian              stretch-slim        2b343cb3b772        5 weeks ago         55.3MB
```

## Running examples
Most of the examples rely on `dhtest` to perform DHCP transactions.  You can build an image
containing `dhtest` using `make`:
```
djc@laptop:~/play/kea_python$ make build-dhtest 
```

Use `make run-kea` to run the `kea` image.  This creates a docker network and runs a command
shell.  Your working directory is mounted as `/workdir` inside the container.

For example, to run the facebook-trick example:
```
djc@laptop:~/play/kea_python$ make run-kea
root@92ddf6f5e9be:/# cd /workdir
root@92ddf6f5e9be:/workdir# /usr/local/sbin/kea-dhcp4 -c examples/facebook-trick/kea.conf 
```

Then in another shell:
```
djc@laptop:~/play/kea_python$ make run-dhtest 
root@375f50ed4699:/# dhtest -i eth0
```

## Performing development on kea_python
The `kea-dev` image is used when making changes to the `kea_python.so` hook.  All you need
to do is the following:
```
djc@laptop:~/play/kea_python$ make run-kea-dev 
root@a742a1b8b485:/source# cd /workdir
root@a742a1b8b485:/workdir# make install
```
Once the build finishes your container is able to run python hooks in the same was as the `kea`
image.  Your working directory is mounted as `/workdir` in the container.

Any time you make a change to the `keamodule` code you should rebuild it and run the unit tests
like this:
```
root@a742a1b8b485:/workdir# make clean-module build-module test-module
```

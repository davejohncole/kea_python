PYTHON_INCLUDE=$(shell python3 config.py pyinc)
KEA_INCLUDE=$(shell python3 config.py keainc)
KEA_HOOKS=$(shell python3 config.py keahooks)

libkea_python.so: load_unload.cc messages.cc version.cc
	g++ -std=c++11 -I $(KEA_INCLUDE) -I $(PYTHON_INCLUDE) -L /usr/local/lib -fpic -shared -o libkea_python.so \
		load_unload.cc messages.cc version.cc \
		-lkea-dhcpsrv -lkea-dhcp++ -lkea-hooks -lkea-log -lkea-util -lkea-exceptions

messages.cc: messages.mes
	kea-msg-compiler messages.mes

install: libkea_python.so
	install libkea_python.so $(KEA_HOOKS)

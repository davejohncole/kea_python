include ../settings.mk

libkea_python.so: load_unload.cc messages.cc version.cc
	g++ -std=c++11 -I $(KEA_INC) -I $(PYTHON_INC) -L /usr/local/lib -fpic -shared -o libkea_python.so \
		load_unload.cc messages.cc version.cc \settings
		-lkea-dhcpsrv -lkea-dhcp++ -lkea-hooks -lkea-log -lkea-util -lkea-exceptions

messages.cc: messages.mes
	kea-msg-compiler messages.mes

install: libkea_python.so
	install libkea_python.so $(KEA_HOOKS)

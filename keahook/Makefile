include ../settings.mk

ifeq ($(wildcard $(KEA_LIBS)/libkea-dhcp.so),)
KEA_DHCP=kea-dhcp++
else
KEA_DHCP=kea-dhcp
endif

libkea_python.so: load_unload.cc messages.cc version.cc
	g++ -std=c++14 -I $(KEA_INC) -I $(PYTHON_INC) -L /usr/local/lib -fpic -shared -o libkea_python.so \
		load_unload.cc messages.cc version.cc \
		-lkea-dhcpsrv -l$(KEA_DHCP) -lkea-hooks -lkea-log -lkea-util -lkea-exceptions

messages.cc: messages.mes
	kea-msg-compiler messages.mes

clean:
	rm -f libkea_python.so

install: libkea_python.so
	install libkea_python.so $(KEA_HOOKS)

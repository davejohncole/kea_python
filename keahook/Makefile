libkea_python.so: load_unload.cc messages.cc version.cc
	g++ -I /usr/local/include/kea -I /usr/include/python3.5m -L /usr/local/lib -fpic -shared -o libkea_python.so \
		load_unload.cc messages.cc version.cc \
		-lkea-dhcpsrv -lkea-dhcp++ -lkea-hooks -lkea-log -lkea-util -lkea-exceptions

messages.cc: messages.mes
	kea-msg-compiler messages.mes

install: libkea_python.so
	install libkea_python.so /usr/local/lib/kea/hooks

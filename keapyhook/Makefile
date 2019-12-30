libdhcp_python.so: load_unload.cc keapy_messages.cc version.cc
	g++ -I /usr/local/include/kea -I /usr/include/python3.5m -L /usr/local/lib -fpic -shared -o libdhcp_python.so \
		load_unload.cc keapy_messages.cc version.cc \
		-lkea-dhcpsrv -lkea-dhcp++ -lkea-hooks -lkea-log -lkea-util -lkea-exceptions

keapy_messages.cc: keapy_messages.mes
	kea-msg-compiler keapy_messages.mes

install: libdhcp_python.so
	install libdhcp_python.so /usr/local/lib/kea/hooks

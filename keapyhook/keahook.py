import time

with open('/keapy/keapyhook/kea.log', 'a') as fp:
    fp.write(time.strftime('%Y-%m-%d %H:%M:%S'))
    fp.write('\n')

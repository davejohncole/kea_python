import kea


def make_traceback():
    x = 1 / 0


kea.logger.info('log from python!')
make_traceback()
kea.logger.info('imported hook')

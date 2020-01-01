from kea import *

def yo_callout(handle):
    logger.info('yo_callout was called: %s' % handle.getArgument('command'))
    handle.setArgument('response', {'status': 'cool'})
    return 0

def load(handle):
    handle.registerCommandCallout('yo', yo_callout)
    return 0

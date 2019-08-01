CONFIG += ordered

TEMPLATE = subdirs

SUBDIRS +=  http-server\
            CVTool \
            CVClient \
    CVMonitor

CVTool.depends = http-server
CVMonitor.depends = http-server

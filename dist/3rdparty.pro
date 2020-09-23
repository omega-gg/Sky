PATH = $$_PRO_FILE_PWD_/../..

TEMPLATE = subdirs

CONFIG += ordered

SUBDIRS = $$PATH/3rdparty \
          $$PATH/assets \
          $$PATH/backend \
          $$PATH/VLC \
          $$PATH/libtorrent \
          $$PATH/docker \
          $$PATH/VBML \
          $$PATH/status \

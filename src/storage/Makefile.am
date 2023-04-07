AM_CPPFLAGS = -I$(srcdir)/. -I$(builddir)/.. -I$(srcdir)/.. $(CXX14_FLAGS) -I$(srcdir)/include \
              $(PROTOBUF_CFLAGS) $(SSL_CFLAGS) $(SSL_CFLAGS) $(CRYPTO_CFLAGS) 

AM_CXXFLAGS = $(PICKY_CXXFLAGS) $(EXTRA_CXXFLAGS)

noinst_LIBRARIES = libggstorage.a

libggstorage_a_SOURCES = backend.hh backend.cc \
                         backend_s3.hh backend_s3.cc \
                         backend_redis.hh backend_redis.cc \
                         backend_gs.hh backend_gs.cc \
                         backend_jiffy.hh backend_jiffy.cc



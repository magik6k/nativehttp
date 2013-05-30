#Makefile for NativeHTTP
#By Magik6000
#For more please visit nativehttp.org

OUT = ./bin/nativehttp
TOUT = ./bin/httptest
STATOUT = ./bin/nativestat.so

FLAGS = -std=c++0x -O2 -w -Iinclude -Isrc -march=native
DBGFLAGS = -std=c++0x -g -Iinclude -Isrc -DNHDBG

LIBS += -rdynamic
LIBS += -lpthread
LIBS += -ldl
LIBS += -lSDL
LIBS += -lsqlite3
LIBS += -lssl
LIBS += -lcrypto
LIBS += -lrt

TESTLIBS += $(LIBS)
TESTLIBS += -lSDL_net

NHD = ./src
NHTD = ./testsrc
STATD = ./nativestat/src

NHS = \
	$(NHD)/core/api.cpp \
	$(NHD)/core/main.cpp \
	$(NHD)/core/system.cpp \
	$(NHD)/data/cfg.cpp \
	$(NHD)/data/cookie.cpp \
	$(NHD)/data/mime.cpp \
	$(NHD)/data/nbase.cpp \
	$(NHD)/data/postget.cpp \
	$(NHD)/data/sqlite.cpp \
	$(NHD)/data/superstring/typed/char/misc.cpp \
	$(NHD)/data/superstring/typed/double/convert.cpp \
	$(NHD)/data/superstring/typed/int32/convert.cpp \
	$(NHD)/data/superstring/typed/int32/core.cpp \
	$(NHD)/data/superstring/typed/int32/misc.cpp \
	$(NHD)/data/superstring/typed/int64/convert.cpp \
	$(NHD)/data/superstring/typed/size/convert.cpp \
	$(NHD)/data/superstring/typed/string/content.cpp \
	$(NHD)/data/superstring/typed/string/convert.cpp \
	$(NHD)/data/superstring/typed/string/core.cpp \
	$(NHD)/data/superstring/typed/string/misc.cpp \
	$(NHD)/data/superstring/typed/string/position.cpp \
	$(NHD)/data/superstring/typed/superstring/core.cpp \
	$(NHD)/data/superstring/typed/superstring/misc.cpp \
	$(NHD)/data/superstring/typed/superstring/position.cpp \
	$(NHD)/data/superstring/core.cpp \
	$(NHD)/data/superstring/file.cpp \
	$(NHD)/data/superstring/token.cpp \
	$(NHD)/data/session/api.cpp \
	$(NHD)/data/session/key.cpp \
	$(NHD)/data/session/manage.cpp \
	$(NHD)/data/session/storage.cpp \
	$(NHD)/http/data.cpp \
	$(NHD)/http/error.cpp \
	$(NHD)/http/executor/excore.cpp \
	$(NHD)/http/executor/executor.cpp \
	$(NHD)/http/executor/header.cpp \
	$(NHD)/http/executor/post.cpp \
	$(NHD)/http/executor/queue.cpp \
	$(NHD)/http/filesender.cpp \
	$(NHD)/http/init.cpp \
	$(NHD)/http/manager.cpp \
	$(NHD)/http/net/net.cpp \
	$(NHD)/http/net/bsd/disconnect.cpp \
	$(NHD)/http/net/bsd/listen.cpp \
	$(NHD)/http/net/bsd/recive.cpp \
	$(NHD)/http/net/bsd/send.cpp \
	$(NHD)/http/net/bsd/start.cpp \
	$(NHD)/http/net/ssl/disconnect.cpp \
	$(NHD)/http/net/ssl/listen.cpp \
	$(NHD)/http/net/ssl/recive.cpp \
	$(NHD)/http/net/ssl/send.cpp \
	$(NHD)/http/net/ssl/start.cpp \
	$(NHD)/http/stat/api.cpp \
	$(NHD)/http/stat/data.cpp \
	$(NHD)/http/stat/init.cpp \
	$(NHD)/http/stat/internal.cpp \
	$(NHD)/pagemap/pagemap.cpp \
	$(NHD)/pagemap/init.cpp \
	$(NHD)/pagemap/reload.cpp \
	$(NHD)/pagemap/dirmap.cpp \
	$(NHD)/pagemap/loaders/so.cpp \
	$(NHD)/pagemap/loaders/nhp.cpp \
	$(NHD)/pagemap/loaders/file.cpp \
	$(NHD)/pagemap/reloaders/so.cpp \
	$(NHD)/pagemap/reloaders/nhp.cpp \
	$(NHD)/utils/memory.cpp \
	$(NHD)/utils/nhp_to_cpp.cpp \
	$(NHD)/utils/thread.cpp

NHTS = \
	$(NHTD)/main.cpp
STATS = \
	$(STATD)/assets.cpp \
	$(STATD)/dtg.cpp \
	$(STATD)/main.cpp

NHO = $(NHS:%.cpp=%.o)
NHOD = $(NHS:%.cpp=%.dbg.o)

NHTO = $(NHTS:%.cpp=%.o)

STATO = $(STATS:%.cpp=%.o)


all: prepare nativehttp btest nativestat
	$(CXX) $(FLAGS) $(NHO) $(LIBS) -o $(OUT)
	$(CXX) $(FLAGS) $(NHTO) $(TESTLIBS) -o $(TOUT)
	$(CXX) $(FLAGS) -shared -fPIC $(STATO) -o $(STATOUT)

prepare:
	mkdir -p bin

debug: nativehttp_dbg
	$(CXX) $(DBGFLAGS) $(NHOD) $(LIBS) -o $(OUT)


directories:
	mkdir -p /etc/nativehttp
	mkdir -p /var/www/error
	mkdir -p /usr/include/nativehttp

install_files:
	cp ./bin/nativehttp /usr/bin
	cp ./files/bin/* /usr/bin
	cp ./files/etc/* /etc/nativehttp
	cp ./files/var/error/* /var/www/error
	cp ./include/* /usr/include/nativehttp


install: directories perm install_files

install_stat:
	cp ./bin/nativestat.so /var/www/nativestat.so

localdirs:
	mkdir -p ./local
	mkdir -p ./local/bin
	mkdir -p ./local/etc
	mkdir -p ./local/include
	mkdir -p ./local/var
	mkdir -p ./local/var/www
	mkdir -p ./local/var/www/error

local: localdirs perm
	cp ./bin/nativehttp ./local/bin/nativehttp-bin
	cp ./files/local/bin/* ./local/bin
	cp ./files/etc/* ./local/etc
	cp ./files/var/error/* ./local/var/www/error
	cp ./bin/nativestat.so ./local/var/www/nativestat.so
	cp ./include/* ./local/include

perm:
	chmod +x ./files/bin/* ./files/local/bin/*

remove:
	rm -rf /etc/nativehttp
	rm -rf /usr/include/nativeht
	rm -f /usr/bin/nativehttp
	rm -f /usr/bin/nativehttp-d
	rm -f /var/www/nativestat.so

nativehttp: $(NHO)

nativehttp_dbg: $(NHOD)

nativehttp_fst: $(NHOF)

btest: $(NHTO)

nativestat: $(STATO)

%.dbg.o: %.cpp
	$(CXX) $(DBGFLAGS) -c -o $@ $<

%.o: %.cpp
	$(CXX) $(FLAGS) -c -o $@ $<

clean:
	rm -f $(NHO) $(NHOD) $(NHOF) $(OUT) $(NHTO) $(TOUT) $(STATO) $(STATOUT) 
	rm -rfv ./local


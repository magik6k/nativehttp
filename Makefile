#Makefile for NativeHTTP
#By Magik6000
#For more please visit nativehttp.org

OUT = ./bin/nativehttp
TOUT = ./bin/httptest
STATOUT = ./bin/nativestat.so

FLAGS = -std=c++0x -O3 -w -Iinclude -Isrc -march=native
DBGFLAGS = -std=c++0x -g -Iinclude -Isrc -DNHDBG
FASTFLAGS = -std=c++0x -O3 -w -Iinclude -Isrc -march=native -DNHFAST

LIBS += -rdynamic
LIBS += -ldl
LIBS += -lSDL
LIBS += -lSDL_net
LIBS += -lsqlite3

NHD = ./src
NHTD = ./testsrc
STATD = ./nativestat/src

NHS = \
	$(NHD)/core/api.cpp \
	$(NHD)/core/main.cpp \
	$(NHD)/core/nhp_to_cpp.cpp \
	$(NHD)/core/system.cpp \
	$(NHD)/data/cfg.cpp \
	$(NHD)/data/cookie.cpp \
	$(NHD)/data/mime.cpp \
	$(NHD)/data/nbase.cpp \
	$(NHD)/data/postget.cpp \
	$(NHD)/data/superstring.cpp \
	$(NHD)/data/sqlite.cpp \
	$(NHD)/data/session/api.cpp \
	$(NHD)/data/session/key.cpp \
	$(NHD)/data/session/manage.cpp \
	$(NHD)/data/session/storage.cpp \
	$(NHD)/http/data.cpp \
	$(NHD)/http/error.cpp \
	$(NHD)/http/executor.cpp \
	$(NHD)/http/filesender.cpp \
	$(NHD)/http/in.cpp \
	$(NHD)/http/init.cpp \
	$(NHD)/http/manager.cpp \
	$(NHD)/http/newclient.cpp \
	$(NHD)/http/sender.cpp \
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
	$(NHD)/pagemap/reloaders/nhp.cpp 

NHTS = \
	$(NHTD)/main.cpp
STATS = \
	$(STATD)/assets.cpp \
	$(STATD)/dtg.cpp \
	$(STATD)/main.cpp

NHO = $(NHS:%.cpp=%.o)
NHOD = $(NHS:%.cpp=%.dbg.o)
NHOF = $(NHS:%.cpp=%.fst.o)

NHTO = $(NHTS:%.cpp=%.o)

STATO = $(STATS:%.cpp=%.o)

all: nativehttp btest nativestat
	$(CXX) $(FLAGS) $(NHO) $(LIBS) -o $(OUT)
	$(CXX) $(FLAGS) $(NHTO) $(LIBS) -o $(TOUT)
	$(CXX) $(FLAGS) -shared -fPIC $(STATO) -o $(STATOUT)

debug: nativehttp_dbg
	$(CXX) $(DBGFLAGS) $(NHOD) $(LIBS) -o $(OUT)

fast: nativehttp_fst
	$(CXX) $(FASTFLAGS) $(NHOF) $(LIBS) -o $(OUT)

directories:
	mkdir -p /etc/nativehttp
	mkdir -p /var/www/error
	mkdir -p /usr/include/nativehttp

install_files:
	cp ./bin/nativehttp /usr/bin
	cp ./etc/* /etc/nativehttp
	cp ./var/error/* /var/www/error
	cp ./include/* /usr/include/nativehttp


install: directories install_files

install_stat: all
	cp ./bin/nativestat.so /var/www/nativestat.so

remove:
	rm -rf /etc/nativehttp
	rm -rf /usr/include/nativehttp
	rm -f /usr/bin/nativehttp
	rm -f /var/www/nativestat.so

nativehttp: $(NHO)

nativehttp_dbg: $(NHOD)

nativehttp_fst: $(NHOF)

btest: $(NHTO)

nativestat: $(STATO)

%.fst.o: %.cpp
	$(CXX) $(FASTFLAGS) -c -o $@ $<

%.dbg.o: %.cpp
	$(CXX) $(DBGFLAGS) -c -o $@ $<

%.o: %.cpp
	$(CXX) $(FLAGS) -c -o $@ $<

clean:
	rm -f $(NHO) $(NHOD) $(NHOF) $(OUT) $(NHTO) $(TOUT) $(STATO)


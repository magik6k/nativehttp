#Makefile for NativeHTTP
#By Magik6000
#For more please visit nativehttp.org

OUT = ./bin/nativehttp
TOUT = ./bin/httptest
FLAGS = -std=c++0x -O3 -w -Iinclude -Isrc

LIBS += -rdynamic
LIBS += -ldl
LIBS += -lSDL
LIBS += -lSDL_net
LIBS += -lsqlite3

NHD = ./src
NHTD = ./testsrc

NHS = \
	$(NHD)/api.cpp \
	$(NHD)/cfg.cpp \
	$(NHD)/cookie.cpp \
	$(NHD)/main.cpp \
	$(NHD)/mime.cpp \
	$(NHD)/nbase.cpp \
	$(NHD)/nhp_to_cpp.cpp \
	$(NHD)/postget.cpp \
	$(NHD)/superstring.cpp \
	$(NHD)/system.cpp \
	$(NHD)/data/sqlite.cpp \
	$(NHD)/http/data.cpp \
	$(NHD)/http/error.cpp \
	$(NHD)/http/executor.cpp \
	$(NHD)/http/filesender.cpp \
	$(NHD)/http/in.cpp \
	$(NHD)/http/init.cpp \
	$(NHD)/http/manager.cpp \
	$(NHD)/http/newclient.cpp \
	$(NHD)/http/sender.cpp \
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
	$(NHTD)/main.cpp \

NHO = $(NHS:%.cpp=%.o)

NHTO = $(NHTS:%.cpp=%.o)

all: nativehttp btest
	$(CXX) $(FLAGS) $(NHO) $(LIBS) -o $(OUT)
	$(CXX) $(FLAGS) $(NHTO) $(LIBS) -o $(TOUT)

directories:
	mkdir /etc/nativehttp
	mkdir /var/www
	mkdir /var/www/error
	mkdir /usr/include/nativehttp

install_files:
	cp ./bin/nativehttp /usr/bin
	cp ./etc/* /etc/nativehttp
	cp ./var/error/* /var/www/error
	cp ./include/* /usr/include/nativehttp


install: directories install_files	

remove:
	rm -r /etc/nativehttp
	rm -r /usr/include/nativehttp
	rm /usr/bin/nativehttp

nativehttp: $(NHO)

btest: $(NHTO)

%.o: %.cpp
	$(CXX) $(FLAGS) -c -o $@ $<

clean:
	rm -f $(NHO) $(OUT) $(NHTO) $(TOUT)


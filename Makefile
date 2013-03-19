#Makefile for NativeHTTP
#By Magik6000
#For more please visit nativehttp.org

OUT = ./bin/nativehttp
TOUT = ./bin/httptest
FLAGS = -std=c++0x -O3 -w

LIBS += -rdynamic
LIBS += -ldl
LIBS += -lSDL
LIBS += -lSDL_net

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
	$(NHD)/pagemap.cpp \
	$(NHD)/postget.cpp \
	$(NHD)/superstring.cpp \
	$(NHD)/system.cpp \
	$(NHD)/http/data.cpp \
	$(NHD)/http/error.cpp \
	$(NHD)/http/executor.cpp \
	$(NHD)/http/filesender.cpp \
	$(NHD)/http/in.cpp \
	$(NHD)/http/init.cpp \
	$(NHD)/http/manager.cpp \
	$(NHD)/http/newclient.cpp \
	$(NHD)/http/sender.cpp 

NHTS = \
	$(NHTD)/main.cpp \

NHO = $(NHS:%.cpp=%.o)

NHTO = $(NHTS:%.cpp=%.o)

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

all: nativehttp btest
	$(CXX) $(FLAGS) $(NHO) $(LIBS) -o $(OUT)
	$(CXX) $(FLAGS) $(NHTO) $(LIBS) -o $(TOUT)


nativehttp: $(NHO)

btest: $(NHTO)

%.o: %.cpp
	$(CXX) $(FLAGS) -c -o $@ $<

clean:
	rm -f $(NHO) $(OUT) $(NHTO) $(TOUT)


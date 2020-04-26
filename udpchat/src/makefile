CFLAGS=-std=c++11 -g
LDFLAGS=-lpthread -ljsoncpp 

SVRBIN=../bin/svr/ChatSvr
CLIBIN=../bin/cli/ChatCli

.PHONY:
all:$(SVRBIN) $(CLIBIN)
$(SVRBIN):ChatServer.cpp
	g++ $^ -o $@ $(CFLAGS) $(LDFLAGS) 
$(CLIBIN):ChatClient.cpp 
	g++ $^ -o $@ $(CFLAGS) $(LDFLAGS) -lncurses

.PHONY:clean
clean:
	rm -f $(SVRBIN) $(CLIBIN)

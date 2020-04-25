CFLAGS=-std=c++11 -g
LDFLAGS=-lpthread -ljsoncpp 

SVRBIN=./ChatSvr
CLIBIN=./ChatCli

.PHONY:
all:$(SVRBIN) $(CLIBIN)
$(SVRBIN):ChatServer.cpp
	g++ $^ -o $@ $(CFLAGS) $(LDFLAGS) 
$(CLIBIN):ChatClient.cpp 
	g++ $^ -o $@ -g

.PHONY:clean
clean:
	rm -f $(SVRBIN) $(CLIBIN)

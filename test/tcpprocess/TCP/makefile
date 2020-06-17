all:svr cli main epoll
epoll:epoll.cpp 
	g++ $^ -o $@ -g
svr:svr.cpp
	g++ $^ -o $@ -g
cli:cli.cpp
	g++ $^ -o $@ -g
main:main.cpp
	g++ $^ -o $@ -g
clean:
	rm svr cli main

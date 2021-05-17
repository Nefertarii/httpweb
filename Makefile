TARGET = Server
OBJECTS = httphead.o servhead.o process.o servprocess.o main.o database.o
$(TARGET): $(OBJECTS)
	g++ -std=c++11 -lmysqlcppconn8 -g -o $(TARGET) $(OBJECTS)
httphead.o: httphead.h
	g++ -std=c++11 -g -c httphead.cpp
servhead.o: servhead.h serverror.h
	g++ -std=c++11 -g -c servhead.cpp
database.o: ../database/database.h
	g++ -std=c++11 -g -c ../database/database.cpp
process.o: process.h httphead.h servhead.h
	g++ -std=c++11 -g -c process.cpp
servprocess.o: servprocess.h ../record/record.h process.h
	g++ -std=c++11 -g -c servprocess.cpp
main.o: servprocess.h
	g++ -std=c++11 -g -c main.cpp
.PHONY:clean
clean:
	-rm *.o $(TARGET)

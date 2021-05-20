TARGET = Server
OBJECTS = database.o jsonprocess.o httphead.o servhead.o process.o servprocess.o main.o 
$(TARGET): $(OBJECTS)
	g++ -std=c++17 -lmysqlcppconn8 -g -o $(TARGET) $(OBJECTS)
database.o: ../database/database.h
	g++ -std=c++17 -g -c ../database/database.cpp
jsonprocess.o: ../database/jsonprocess.h
	g++ -std=c++17 -g -c ../database/jsonprocess.cpp
httphead.o: httphead.h
	g++ -std=c++17 -g -c httphead.cpp
servhead.o: servhead.h serverror.h
	g++ -std=c++17 -g -c servhead.cpp
process.o: process.h httphead.h servhead.h
	g++ -std=c++17 -g -c process.cpp
servprocess.o: servprocess.h ../record/record.h process.h
	g++ -std=c++17 -g -c servprocess.cpp
main.o: servprocess.h
	g++ -std=c++17 -g -c main.cpp
.PHONY:clean
clean:
	-rm *.o $(TARGET)

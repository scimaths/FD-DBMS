all: exec.o

exec.o:server.cpp query_select.cpp arithmetic_expr.cpp file.cpp query.cpp metadata.cpp type.cpp  token.cpp data.cpp
	g++ server.cpp query_select.cpp arithmetic_expr.cpp file.cpp query.cpp metadata.cpp type.cpp  token.cpp data.cpp  -o exec.o
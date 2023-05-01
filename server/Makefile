all: exec.o

exec.o: query_processing.cpp query_select.cpp arithmetic_expr.cpp file.cpp query.cpp metadata.cpp type.cpp token.cpp data.cpp
	g++ query_processing.cpp query_select.cpp arithmetic_expr.cpp file.cpp query.cpp metadata.cpp type.cpp token.cpp data.cpp  -o exec.o
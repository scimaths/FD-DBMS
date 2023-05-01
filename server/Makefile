all: exec.o

exec.o: server.cpp query_processing.cpp query_select.cpp arithmetic_expr.cpp file.cpp query.cpp metadata.cpp type.cpp token.cpp data.cpp local_constraint.cpp func_dep.cpp global_constraint.cpp
	g++ server.cpp query_processing.cpp query_select.cpp arithmetic_expr.cpp file.cpp query.cpp metadata.cpp type.cpp token.cpp data.cpp local_constraint.cpp func_dep.cpp global_constraint.cpp -o exec.o
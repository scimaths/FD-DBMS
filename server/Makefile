all: exec.o

exec.o: query_processing.cpp query_select.cpp arithmetic_expr.cpp file.cpp query.cpp metadata.cpp type.cpp token.cpp data.cpp local_constraint.cpp func_dep.cpp global_constraint.cppsss
	g++ query_processing.cpp query_select.cpp arithmetic_expr.cpp file.cpp query.cpp metadata.cpp type.cpp token.cpp data.cpp local_constraint.cpp func_dep.cpp global_constraint.cpp -o exec.o
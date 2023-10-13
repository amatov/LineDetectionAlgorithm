CMP = gcc
OPT = -O3 -g
LIB = -ltk -ltcl -lm
INC = /usr/local/inlcude

OBJS = leon.o utils.o command_line.o data_output.o read_data.o\
	create_equations.o math_functions.o update_equations.o\
	memory_alloc.o bmp_8b.o leon_tcltk.o line_detect_tk.o\
	conic_detect_tk.o\
	search_biggest_subsystem_AR_perceptron.o\
	search_biggest_subsystem_AR_winnow.o\
	search_biggest_subsystem_OPT.o\
	search_biggest_subsystem_LDT.o\
	search_biggest_subsystem_CDT.o\
	search_biggest_subsystem_CDT2.o\
	search_biggest_subsystem_CDT3.o

ALL =  leon.h struct.h

plime: $(OBJS)
	$(CMP) $(OBJS) -o plime $(OPT) $(LIB)

leon.o: command_line.h data_output.h read_data.h create_equations.h\
	utils.h leon_tcltk.h line_detect_tk.h conic_detect_tk.h\
	search_biggest_subsystem_AR_perceptron.h\
	search_biggest_subsystem_AR_winnow.h\
	search_biggest_subsystem_OPT.h\
	search_biggest_subsystem_LDT.h\
	search_biggest_subsystem_CDT.h\
	search_biggest_subsystem_CDT2.h\
	search_biggest_subsystem_CDT3.h\
	leon.cc $(ALL)
	$(CMP) -c leon.cc $(OPT)

utils.o: utils.h utils.cc $(ALL)
	$(CMP) -c utils.cc $(OPT)

command_line.o: command_line.h command_line.cc utils.h $(ALL)
	$(CMP) -c command_line.cc $(OPT)

data_output.o: data_output.h data_output.cc math_functions.h utils.h\
	inline_functions_AR.cc $(ALL)
	$(CMP) -c data_output.cc $(OPT)

read_data.o: read_data.h read_data.cc memory_alloc.h utils.h $(ALL)
	$(CMP) -c read_data.cc $(OPT)

create_equations.o: create_equations.h\
	create_equations.cc memory_alloc.h $(ALL)
	$(CMP) -c create_equations.cc $(OPT)

update_equations.o: conic_detect_tk.h\
	update_equations.cc math_functions.h inline_functions_AR.cc $(ALL)
	$(CMP) -c update_equations.cc $(OPT)

search_biggest_subsystem_AR_winnow.o: search_biggest_subsystem_AR_winnow.h\
	search_biggest_subsystem_AR_winnow.cc\
	math_functions.h memory_alloc.h data_output.h inline_functions.cc $(ALL)
	$(CMP) -c search_biggest_subsystem_AR_winnow.cc $(OPT)

search_biggest_subsystem_AR_perceptron.o:\
	search_biggest_subsystem_AR_perceptron.h\
	search_biggest_subsystem_AR_perceptron.cc\
	math_functions.h memory_alloc.h data_output.h inline_functions.cc $(ALL)
	$(CMP) -c search_biggest_subsystem_AR_perceptron.cc $(OPT)

search_biggest_subsystem_OPT.o: search_biggest_subsystem_OPT.h\
	 search_biggest_subsystem_OPT.cc\
	math_functions.h memory_alloc.h data_output.h inline_functions.cc $(ALL)
	$(CMP) -c search_biggest_subsystem_OPT.cc $(OPT)

search_biggest_subsystem_LDT.o: search_biggest_subsystem_LDT.h\
	search_biggest_subsystem_LDT.cc\
	math_functions.h memory_alloc.h data_output.h inline_functions.cc \
	line_detect_tk.h $(ALL)
	$(CMP) -c search_biggest_subsystem_LDT.cc $(OPT)

search_biggest_subsystem_CDT.o: search_biggest_subsystem_CDT.h\
	search_biggest_subsystem_CDT.cc update_equations.h \
	math_functions.h memory_alloc.h data_output.h inline_functions.cc \
	conic_detect_tk.h $(ALL)
	$(CMP) -c search_biggest_subsystem_CDT.cc $(OPT)

search_biggest_subsystem_CDT2.o: search_biggest_subsystem_CDT2.h\
	search_biggest_subsystem_CDT2.cc update_equations.h \
	math_functions.h memory_alloc.h data_output.h inline_functions.cc \
	conic_detect_tk.h $(ALL)
	$(CMP) -c search_biggest_subsystem_CDT2.cc $(OPT)

search_biggest_subsystem_CDT3.o: search_biggest_subsystem_CDT3.h\
	search_biggest_subsystem_CDT3.cc update_equations.h \
	math_functions.h memory_alloc.h data_output.h inline_functions.cc \
	conic_detect_tk.h $(ALL)
	$(CMP) -c search_biggest_subsystem_CDT3.cc $(OPT)

math_functions.o: math_functions.h math_functions.cc leon.h
	$(CMP) -c math_functions.cc $(OPT)

memory_alloc.o: memory_alloc.h memory_alloc.cc utils.h leon.h
	$(CMP) -c memory_alloc.cc $(OPT)

bmp_8b.o: bitmap.h memory_alloc.h utils.h bmp_8b.cc leon.h $(ALL)
	$(CMP) -c bmp_8b.cc $(OPT)

leon_tcltk.o: leon_tcltk.cc $(ALL)
	$(CMP) -c leon_tcltk.cc $(OPT)

line_detect_tk.o: line_detect_tk.cc line_detect_tk.h leon.h $(ALL)
	$(CMP) -c line_detect_tk.cc $(OPT)

conic_detect_tk.o: conic_detect_tk.cc conic_detect_tk.h leon.h $(ALL)
	$(CMP) -c conic_detect_tk.cc $(OPT)

clean:
	rm -f plime *.o core *.out

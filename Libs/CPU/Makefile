
DEBUG_FLAGS   = -Wshadow -Winit-self -Wredundant-decls -Wcast-align -Wundef -Wfloat-equal -Winline -Wunreachable-code -Wmissing-declarations -Wmissing-include-dirs -Wswitch-enum -Wswitch-default -Weffc++ -Wmain -Wextra -Wall -g -pipe -fexceptions -Wcast-qual -Wconversion -Wctor-dtor-privacy -Wempty-body -Wformat-security -Wformat=2 -Wignored-qualifiers -Wlogical-op -Wno-missing-field-initializers -Wnon-virtual-dtor -Woverloaded-virtual -Wpointer-arith -Wsign-promo -Wstack-usage=8192 -Wstrict-aliasing -Wstrict-null-sentinel -Wtype-limits -Wwrite-strings -Werror=vla -D_DEBUG -D_EJUDGE_CLIENT_SIDE
RELEASE_FLAGS = -O2
FLAGS 		  = $(RELEASE_FLAGS)
all: create_folders logging assembler_exe processor_exe file_work
	echo Successfully make

assembler_exe: assembler_o create_folders logging file_work comand_system
	g++ Assembler/main.cpp Obj/Assembler.o Obj/ComandSystem.o Obj/Logging.o Obj/FileWork.o $(FLAGS) -o Exe/Comp.exe

assembler_o: create_folders logging file_work comand_system
	g++ -c Assembler/Assembler.cpp -o Obj/Assembler.o 

processor_exe: processor_o create_folders logging comand_system
	g++ Processor/main.cpp Obj/Processor.o Obj/Logging.o Obj/ComandSystem.o -o Exe/Start.exe

processor_o:
	g++ -c  Processor/Processor.cpp -o Obj/Processor.o

interpret_exe: create_folders file_work
	g++ Interpreter.cpp.cpp Obj/FileWork.o -o Exe/Start.exe
	Exe/./Start.exe

disassembler_exe: processor_o create_folders comand_system logging
	g++ Disassembler/Disassembler.cpp Obj/Processor.o Obj/ComandSystem.o Obj/Logging.o -o Exe/Disass.exe
	Exe/./Disass.exe a.sy

file_work: create_folders
	g++ -c Libs/FileWork/FileWork.cpp -o Obj/FileWork.o

logging: create_folders
	g++ -c Libs/Logging/Logging.cpp -o Obj/Logging.o

comand_system: create_folders
	g++ -c Libs/ComandSystem/ComandSystem.cpp -o Obj/ComandSystem.o
	
create_folders:
	-mkdir Obj
	-mkdir Exe
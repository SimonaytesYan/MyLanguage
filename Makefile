#Linux:
C_FLAGS   = -D _DEBUG -ggdb3 -std=c++2a -O0 -Wall -Wextra -Weffc++ -Waggressive-loop-optimizations -Wc++14-compat -Wmissing-declarations -Wcast-align -Wcast-qual -Wchar-subscripts -Wconditionally-supported -Wconversion -Wctor-dtor-privacy -Wempty-body -Wfloat-equal -Wformat-nonliteral -Wformat-security -Wformat-signedness -Wformat=2 -Winline -Wlogical-op -Wnon-virtual-dtor -Wopenmp-simd -Woverloaded-virtual -Wpacked -Wpointer-arith -Winit-self -Wredundant-decls -Wshadow -Wsign-conversion -Wsign-promo -Wstrict-null-sentinel -Wstrict-overflow=2 -Wsuggest-attribute=noreturn -Wsuggest-final-methods -Wsuggest-final-types -Wsuggest-override -Wswitch-default -Wswitch-enum -Wsync-nand -Wundef -Wunreachable-code -Wunused -Wuseless-cast -Wvariadic-macros -Wno-literal-suffix -Wno-missing-field-initializers -Wno-narrowing -Wno-old-style-cast -Wno-varargs -Wstack-protector -fcheck-new -fsized-deallocation -fstack-protector -fstrict-overflow -flto-odr-type-merging -fno-omit-frame-pointer -Wstack-usage=8192 -pie -fPIE -fsanitize=address,alignment,bool,bounds,enum,float-cast-overflow,float-divide-by-zero,integer-divide-by-zero,nonnull-attribute,leak,null,object-size,return,returns-nonnull-attribute,shift,signed-integer-overflow,undefined,unreachable,vla-bound,vptr
#windows:
#C_FLAGS = -Wshadow -Winit-self -Wredundant-decls -Wcast-align -Wundef -Wfloat-equal -Winline -Wunreachable-code -Wmissing-declarations -Wmissing-include-dirs -Wswitch-enum -Wswitch-default -Weffc++ -Wmain -Wextra -Wall -g -pipe -fexceptions -Wcast-qual -Wconversion -Wctor-dtor-privacy -Wempty-body -Wformat-security -Wformat=2 -Wignored-qualifiers -Wlogical-op -Wno-missing-field-initializers -Wnon-virtual-dtor -Woverloaded-virtual -Wpointer-arith -Wsign-promo -Wstack-usage=8192 -Wstrict-aliasing -Wstrict-null-sentinel -Wtype-limits -Wwrite-strings -Werror=vla -D_DEBUG -D_EJUDGE_CLIENT_SIDE

all: logging recursive_descent in_and_out lexical_analis back_end save_get_tree rebuild_code
	g++ main.cpp Obj/LexicalAnalysis.o Obj/Logging.o Obj/RebuildCodeFromTree.o Obj/RecursiveDescent.o Obj/InAndOut.o Obj/Backend.o Obj/SaveAndGetTree.o $(C_FLAGS) -o Exe/Start.exe

create_dir:
	mkdir Exe
	mkDir Obj
	
run:
	cd Libs/CPU && .\Exe\Comp.exe ../../Main.sy && .\Exe\Start.exe a.sy

rebuild_code:
	g++ -c Libs/RebuildCodeFromTree/RebuildCodeFromTree.cpp $(C_FLAGS) -o Obj/RebuildCodeFromTree.o

save_get_tree:
	g++ -c Libs/SaveAndGetTree/SaveAndGetTree.cpp $(C_FLAGS) -o Obj/SaveAndGetTree.o

back_end: logging in_and_out
	g++ -c Libs/CreateAssembler/CreateAssembler.cpp Obj/Logging.o Obj/InAndOut.o $(C_FLAGS) -o Obj/Backend.o

recursive_descent: in_and_out
	g++ -c Libs/ResursiveDescent/RecursiveDescent.cpp Obj/InAndOut.o Obj/Logging.o $(C_FLAGS) -o Obj/RecursiveDescent.o

in_and_out:
	g++ -c Libs/InAndOut/InAndOut.cpp -o Obj/InAndOut.o

lexical_analis:
	g++ -c Libs/LexicalAnalysis/LexicalAnalysis.cpp $(C_FLAGS) -o Obj/LexicalAnalysis.o

logging:
	g++ -c Libs/Logging/Logging.cpp $(C_FLAGS) -o Obj/Logging.o


assembler_exe: assembler_o create_folders logging file_work comand_system
	g++ Assembler/main.cpp Obj/Assembler.o Obj/ComandSystem.o Obj/Logging.o Obj/FileWork.o $(C_FLAGS) -o Exe/Comp.exe

assembler_o: create_folders logging file_work comand_system
	g++ -c Assembler/Assembler.cpp -o Obj/Assembler.o 

processor_exe: processor_o logging comand_system
	g++ Processor/main.cpp Obj/Processor.o Obj/Logging.o Obj/ComandSystem.o -o Exe/Start.exe

processor_o:
	g++ -c  Libs/Processor/Processor.cpp -o Obj/Processor.o
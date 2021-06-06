#include "option.h"

static char tree = 0;
static char symtabs = 0;
static char help = 0;

char OPTION_TREE(){return tree;}
char OPTION_SYMTABS(){return symtabs;}

static void print_help(void){
    puts("Utilisation:\n\t./tpcc [OPTIONS]\n\nAnalyseur syntaxique et sémantique de langage tpc.\n"
    "Traducteur en nasm (incomplet).\n\nOptions :");
    puts("\t-t, --tree          affiche l'arbre abstrait sur la sortie standard");
    puts("\t-s, --symtabs       affiche toutes les tables de symboles sur la sortie standard");
    puts("\t-h, --help          affiche une description de l'interface utilisatuer et termine l'exécution\n");
}

int parse_options(int argc, char** argv){
    int c, option_index = 0;

    static struct option long_options[] = {
           {"tree", no_argument, 0, 't'},
           {"symtabs", no_argument, 0, 's'},
           {"help", no_argument, 0, 'h'},
           {0, 0, 0, 0}
    };

    while((c = getopt_long_only(argc, argv, "tsh", long_options, &option_index)) != -1){
		switch(c){
			case 't':
				tree=1; break;
			case 's': 
                symtabs=1; break;
			case 'h': 
                help=1; print_help(); return 0;
			default:
                print_help();
                exit(3);
		}
    }
    return 1;
}
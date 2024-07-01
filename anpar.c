/*
 *	Analizador L�xico	
 *	Curso: Compiladores y Lenguajes de Bajo de Nivel
 *	Pr�ctica de Programaci�n Nro. 1
 *	
 *	Descripcion:
 *	Implementa un analizador l�xico que reconoce n�meros, identificadores, 
 * 	palabras reservadas, operadores y signos de puntuaci�n para un lenguaje
 * 	con sintaxis tipo Pascal.
 *	
 */

/*********** Inclusi�n de cabecera **************/
#include "anlex.h"
#include "anpar.h"


/************* Variables globales **************/

int consumir;			/* 1 indica al analizador lexico que debe devolver
						el sgte componente lexico, 0 debe devolver el actual */

char cad[5*TAMLEX];		// string utilizado para cargar mensajes de error
token t;				// token global para recibir componentes del Analizador Lexico

// variables para el analizador lexico

FILE *archivo;			// Fuente json
FILE *output;			// Output a generar
char buff[2*TAMBUFF];	// Buffer para lectura de archivo fuente
char lexema[TAMLEX];	// Utilizado por el analizador lexico
int delantero=-1;		// Utilizado por el analizador lexico
int fin=0;				// Utilizado por el analizador lexico
int numLinea=1;			// Numero de Linea
bool imprimir=1;   // Bandera booleana para la impresion de tokens


/**************** Funciones **********************/


// Rutinas del analizador lexico

void error(const char* mensaje)
{	
	imprimir=0;
	fprintf(output, "Lin %d: Error Lexico. %s.",numLinea,mensaje);
}

void getToken()
{
	int i=0;
	char c=0;
	int acepto=0;
	int estado=0;
	char msg[41];
	entrada e;

	while((c=fgetc(archivo))!=EOF)
	{

		if (c=='\t')
			fprintf(output,"\t");	//formatear con identacion
		else if (c==' ')
			fprintf(output," ");
		else if(c=='\n')
		{
			//incrementar el numero de linea
			fprintf(output,"\n");
			numLinea++;
			imprimir=1;
			continue;
		}
		else if (isalpha(c)){
			// es un boolean o un null
			//es un identificador (o palabra reservada)
			i=0;
			do{
				lexema[i]=c;
				i++;
				c=fgetc(archivo);
				if (i>=TAMLEX)
					error("Longitud de Identificador excede tamaño de buffer");
			}while(isalpha(c) || isdigit(c));
			lexema[i]='\0';
			if (c!=EOF)
				ungetc(c,archivo);
			else
				c=0;
			t.pe=buscar(lexema);
			t.compLex=t.pe->compLex;
			break;
		}
		else if (isdigit(c))
		{
				//es un numero
				i=0;
				estado=0;
				acepto=0;
				lexema[i]=c;
				
				while(!acepto)
				{
					switch(estado){
					case 0: //una secuencia netamente de digitos, puede ocurrir . o e
						c=fgetc(archivo);
						if (isdigit(c))
						{
							lexema[++i]=c;
							estado=0;
						}
						else if(c=='.'){
							lexema[++i]=c;
							estado=1;
						}
						else if(tolower(c)=='e'){
							lexema[++i]=c;
							estado=3;
						}
						else{
							estado=6;
						}
						break;
					
					case 1://un punto, debe seguir un digito (caso especial de array, puede venir otro punto)
						c=fgetc(archivo);						
						if (isdigit(c))
						{
							lexema[++i]=c;
							estado=2;
						}
						else if(c=='.')
						{
							i--;
							fseek(archivo,-1,SEEK_CUR);
							estado=6;
						}
						else{
							sprintf(msg,"No se esperaba '%c'",c);
							estado=-1;
						}
						break;
					case 2://la fraccion decimal, pueden seguir los digitos o e
						c=fgetc(archivo);
						if (isdigit(c))
						{
							lexema[++i]=c;
							estado=2;
						}
						else if(tolower(c)=='e')
						{
							lexema[++i]=c;
							estado=3;
						}
						else
							estado=6;
						break;
					case 3://una e, puede seguir +, - o una secuencia de digitos
						c=fgetc(archivo);
						if (c=='+' || c=='-')
						{
							lexema[++i]=c;
							estado=4;
						}
						else if(isdigit(c))
						{
							lexema[++i]=c;
							estado=5;
						}
						else{
							sprintf(msg,"No se esperaba '%c'",c);
							estado=-1;
						}
						break;
					case 4://necesariamente debe venir por lo menos un digito
						c=fgetc(archivo);
						if (isdigit(c))
						{
							lexema[++i]=c;
							estado=5;
						}
						else{
							sprintf(msg,"No se esperaba '%c'",c);
							estado=-1;
						}
						break;
					case 5://una secuencia de digitos correspondiente al exponente
						c=fgetc(archivo);
						if (isdigit(c))
						{
							lexema[++i]=c;
							estado=5;
						}
						else{
							estado=6;
						}break;
					case 6://estado de aceptacion, devolver el caracter correspondiente a otro componente lexico
						if (c!=EOF)
							ungetc(c,archivo);
						else
							c=0;
						lexema[++i]='\0';
						acepto=1;
						t.pe=buscar(lexema);
						if (t.pe->compLex==-1)
						{
							strcpy(e.lexema,lexema);
							e.compLex=LITERAL_NUM;
							insertar(e);
							t.pe=buscar(lexema);
						}
						t.compLex=LITERAL_NUM;
						break;
					case -1:
						if (c==EOF)
							error("No se esperaba el fin de archivo");
						else
							error(msg);
						exit(1);
					}
				}
			break;
		}
		else if (c==',')
		{
			t.compLex=',';
			t.pe=buscar(",");
			break;
		}
		else if (c==':')
		{
			t.compLex=':';
			t.pe=buscar(":");
			break;
		}
		else if (c=='{')
		{
			t.compLex='{';
			t.pe=buscar("{");
			break;
		}
		else if (c=='}')
		{
			t.compLex='}';
			t.pe=buscar("}");
			break;
		}
		else if (c=='[')
		{
			t.compLex='[';
			t.pe=buscar("[");
			break;
		}
		else if (c==']')
		{
			t.compLex=']';
			t.pe=buscar("]");
			break;
		}
		else if (c=='\"')
		{//un caracter o una cadena de caracteres
			i=0;
			lexema[i]=c;
			i++;
			do{	
				c=fgetc(archivo);
				if (c=='\"')
				{	
					c=fgetc(archivo);
					if (c=='\"')
					{
						lexema[i]=c;
						i++;
						lexema[i]=c;
						i++;
					}

					else
					{
						lexema[i]='\"';
						i++;
						break;
					}
				}
				/*else if (!isascii(c) && imprimir == 1){
					error("Caracter no encontrado");
				}*/
				else if(c==EOF)
				{
					error("Se llego al fin de archivo sin finalizar un literal");
				}
				else{
					lexema[i]=c;
					i++;
				}
			}while(c != EOF);
			lexema[i]='\0';
			if (c!=EOF)
				ungetc(c,archivo);
			else
				c=0;
			t.pe=buscar(lexema);
			t.compLex=t.pe->compLex;
			if (t.pe->compLex==-1)
			{
				strcpy(e.lexema,lexema);				
				e.compLex=LITERAL_CADENA;
				insertar(e);
				t.pe=buscar(lexema);
				t.compLex=e.compLex;
			}
			break;
		}
	}
	if (c==EOF)
	{
		t.compLex=EOF;
		// strcpy(e.lexema,"EOF");
		sprintf(e.lexema,"EOF");
		t.pe=&e;
	}
	
}

void json(int synchset[], int synchset_size) {
	printf("json\n");
    int synchset_element[] = {EOF, ',', '}', ']'};
    int synchset_element_size = sizeof(synchset_element)/sizeof(synchset_element[0]);
	printf("%d\n", synchset_element_size);

    int first_json[] = {'{','['};
    int first_size = sizeof(first_json)/ sizeof(first_json[0]);
	printf("%d\n", first_size); 
	check_input(first_json, first_size, synchset, synchset_size);
	printf("antes if json\n");
	if (!token_in_set(t.compLex, synchset, synchset_size)){
		printf("if json");
        element(synchset_element, synchset_element_size);
        match(EOF);
		check_input(synchset, synchset_size, first_json, first_size);
	}
}

void element(int synchset[], int synchset_size) {
	printf("element\n");
    int synchset_object_array[] = {EOF, ',', ']', '}'};
    int synchset_object_array_size = sizeof(synchset_object_array) / sizeof(synchset_object_array[0]);

    int first_element[] = {'{', '['};
    int first_element_size = sizeof(first_element) / sizeof(first_element_size);

	check_input(first_element, first_element_size, synchset, synchset_size);
	if (!token_in_set(t.compLex, synchset, synchset_size)){
		printf("entra element\n");
		switch (t.compLex)
		{
		case '{':
			object(synchset_object_array, synchset_object_array_size);
			break;
        case '[':
            array(synchset_object_array, synchset_object_array_size);
            break;
		
		default:
			error_sintactico("xx");
		}
		printf("Check input element");
	    check_input(synchset, synchset_size, first_element, first_element_size);
	}
}

void array(int synchset[], int synchset_size) {
	printf("array\n");
    int synchset_e_prime[] = {']'};
    int synchset_e_prime_size = sizeof(synchset_e_prime) / sizeof(synchset_e_prime[0]);

    int first_array[] = {'['};
    int first_array_size = sizeof(first_array) / sizeof(first_array[0]);
    
	check_input(first_array, first_array_size, synchset, synchset_size);
	if (!token_in_set(t.compLex, synchset, synchset_size)){
		match('[');
		e_prime(synchset_e_prime, synchset_e_prime_size);
		match(']');
		check_input(synchset, synchset_size, first_array, first_array_size);
	}
}

void e_prime(int synchset[], int synchset_size) {
    printf("eprime\n");
	int synchset_element_list[] = {']'};
    int synchset_element_list_size = sizeof(synchset_element_list)/ sizeof(synchset_element_list[0]);

    int first_e_prime[] = {'{', '['};
    int first_e_prime_size = sizeof(first_e_prime) / sizeof(first_e_prime[0]);

	//check_input(first_e_prime, first_e_prime_size, synchset, synchset_size);
	if (!token_in_set(t.compLex, synchset, synchset_size)){
		printf("Entra e prime");
		switch (t.compLex)
		{
		case '{':
		case '[':
			element_list(synchset_element_list, synchset_element_list_size);
			break;
		}
		check_input(synchset, synchset_size, first_e_prime, first_e_prime_size);
	}
}

void element_list(int synchset[], int synchset_size) {
    printf("element list\n");
	// Sets de sincronizacion de element() y element_list()
    int synchset_element[] = {EOF, ',', ']', '}'};
    int synchset_element_size = sizeof(synchset_element) / sizeof(synchset_element[0]);
    int synchset_element_list_prime[] = {']'};
    int synchset_element_list_prime_size = sizeof(synchset_element_list_prime) / sizeof(synchset_element_list_prime[0]);

    // Conjunto primero de element_list()
    int first_element_list[] = {'{', '['};
    int first_element_list_size = sizeof(first_element_list) / sizeof(first_element_list[0]);

    check_input(first_element_list, first_element_list_size, synchset, synchset_size);
	if (!token_in_set(t.compLex, synchset, synchset_size)){
        element(synchset_element, synchset_element_size);
        element_list_prime(synchset_element_list_prime, synchset_element_list_prime_size);
        check_input(synchset, synchset_size, first_element_list, first_element_list_size);
	}
}

void element_list_prime(int synchset[], int synchset_size) {
    printf("element list prime\n");
	// Sets de sincronizacion de element() y element_list()
    int synchset_element[] = {EOF, ',', '}', ']'};
    int synchset_element_size = sizeof(synchset_element) / sizeof(synchset_element[0]);
    int synchset_element_list_prime[] = {']'};
    int synchset_element_list_prime_size = sizeof(synchset_element_list_prime) / sizeof(synchset_element_list_prime[0]);

    // Conjunto primero de element_list_prime()
    int first_element_list_prime[] = {','};
    int first_element_list_prime_size = sizeof(first_element_list_prime) / sizeof(first_element_list_prime[0]); 

	if (t.compLex != ']' ){
		check_input(first_element_list_prime, first_element_list_prime_size, synchset, synchset_size);
		if (!token_in_set(t.compLex, synchset, synchset_size)){
			printf("Entra element list prime");
			switch (t.compLex)
			{
			case ',':
				match(',');
				printf("xd element list prime\n");
				element(synchset_element, synchset_element_size);
				printf("xd element list prime\n");
				element_list_prime(synchset_element_list_prime, synchset_element_list_prime_size);
				break;
			}
			printf("check input element list prime\n");
			check_input(synchset, synchset_size, first_element_list_prime, first_element_list_prime_size);
		}
	}
}

void object(int synchset[], int synchset_size) {
	printf("object\n");
    // Conjunto de sincronizacion de a_prime()
    int synchset_a_prime[] = {'}'};
    int synchset_a_prime_size = sizeof(synchset_a_prime) / sizeof(synchset_a_prime[0]);
;
    // Conjunto primero de object()
    int first_object[] = {'{'};
    int first_object_size = sizeof(first_object) / sizeof(first_object_size);

	check_input(first_object, first_object_size, synchset, synchset_size);	
	if (!token_in_set(t.compLex, synchset, synchset_size)){
        match('{');
        a_prime(synchset_a_prime, synchset_a_prime_size);
        match('}');
		check_input(synchset, synchset_size, first_object, first_object_size);
	}
}

void a_prime(int synchset[], int synchset_size) {
	printf("a prime\n");
    // Conjunto de sincronizacion de attribute_list()
    int synchset_attribute_list[] = {'}'};
    int synchset_attribute_list_size = sizeof(synchset_attribute_list) / sizeof(synchset_attribute_list[0]);

    // Conjunto primero de a_prime()
    int firstset_a_prime[] = {LITERAL_CADENA};
    int firstset_a_prime_size = sizeof(firstset_a_prime) / sizeof(synchset_attribute_list[0]);

    check_input(firstset_a_prime, firstset_a_prime_size, synchset, synchset_size);	
	if (!token_in_set(t.compLex, synchset, synchset_size)){	
		printf("Entro a prima\n");
		switch (t.compLex)
    	{
    	case LITERAL_CADENA:
    	    attribute_list(synchset_attribute_list, synchset_attribute_list_size);
    	    break;
    	}
		check_input(synchset, synchset_size, firstset_a_prime, firstset_a_prime_size);
	}
}

void attribute_list(int synchset[], int synchset_size) {
	printf("attribute list\n");
    // Conjuntos de sincronizacion de attribute() y attribute_list_prime()
    int synchset_attribute[] = {',', '}'};
    int synchset_attribute_size = sizeof(synchset_attribute) / sizeof(synchset_attribute[0]);
    int synchset_attribute_list_prime[] = {'}'};
    int synchset_attribute_list_prime_size = sizeof(synchset_attribute_list_prime) / sizeof(synchset_attribute_list_prime[0]);


    // Conjunto primero de attribute_list()
    int first_attribute_list[] = {LITERAL_CADENA};
    int first_attribute_list_size = sizeof(first_attribute_list) / sizeof(first_attribute_list[0]);

    check_input(first_attribute_list, first_attribute_list_size, synchset, synchset_size);
	if (!token_in_set(t.compLex, synchset, synchset_size)){
        attribute(synchset_attribute, synchset_attribute_size);
        attribute_list_prime(synchset_attribute_list_prime, synchset_attribute_list_prime_size);
		check_input(synchset, synchset_size, first_attribute_list, first_attribute_list_size);
	}
}

void attribute_list_prime(int synchset[], int synchset_size) {
	printf("attribute list prime\n");
    // Conjuntos de sincronizacion de attribute() y attribute_list_prime()
    int synchset_attribute[] = {',', '}'};
    int synchset_attribute_size = sizeof(synchset_attribute) / sizeof(synchset_attribute[0]);
    int synchset_attribute_list_prime[] = {'}'};
    int synchset_attribute_list_prime_size = sizeof(synchset_attribute_list_prime) / sizeof(synchset_attribute_list_prime[0]);

    // Conjunto primero de attribute_list_prime()
    int first_attribute_list_prime[] = {','};
    int first_attribute_list_prime_size = sizeof(first_attribute_list_prime) / sizeof(first_attribute_list_prime[0]);

	if (t.compLex  != '}'){
		check_input(first_attribute_list_prime, first_attribute_list_prime_size, synchset, synchset_size);
		if (!token_in_set(t.compLex, synchset, synchset_size)){
			switch (t.compLex)
			{
			case ',':
				match(',');
				attribute(synchset_attribute, synchset_attribute_size);
				attribute_list_prime(synchset_attribute_list_prime, synchset_attribute_list_prime_size);
				break;
			}
			printf("vuelve atribute prime list");
			check_input(synchset, synchset_size, first_attribute_list_prime, first_attribute_list_prime_size);
		}
	}
}

void attribute(int synchset[], int synchset_size) {
	printf("attribute\n");
    // Conjuntos de sincronizacion de attribute_name() y attribute_value()
    int synchset_attribute_name[] = {':'};
    int synchset_attribute_name_size = sizeof(synchset_attribute_name) / sizeof(synchset_attribute_name[0]);
    int synchset_attribute_value[] = {',', '}'};
    int synchset_attribute_value_size = sizeof(synchset_attribute_value) / sizeof(synchset_attribute_value[0]);

    //Conjunto primero de attribute()
    int first_attribute[] = {LITERAL_CADENA};
    int first_attribute_size = sizeof(first_attribute) / sizeof(first_attribute[0]);

    check_input(first_attribute, first_attribute_size, synchset, synchset_size);	
	if (!token_in_set(t.compLex, synchset, synchset_size)){	
        attribute_name(synchset_attribute_name, synchset_attribute_name_size);
        match(':');
        attribute_value(synchset_attribute_value, synchset_attribute_value_size);
		check_input(synchset, synchset_size, first_attribute, first_attribute_size);
	}
}

void attribute_name(int synchset[], int synchset_size) { 
	printf("attribute name\n");
    // Conjunto primero de attribute_name()
    int first_attribute_name [] = {LITERAL_CADENA};
    int first_attribute_name_size = sizeof(first_attribute_name) / sizeof(first_attribute_name[0]);

    check_input(first_attribute_name, first_attribute_name_size, synchset, synchset_size);
	if (!token_in_set(t.compLex, synchset, synchset_size)){
    	match(LITERAL_CADENA);
		check_input(synchset, synchset_size, first_attribute_name, first_attribute_name_size);
	}
}

void attribute_value(int synchset[], int synchset_size) {
	printf("attribute value\n");
    // Conjunto de sincronizacion de element()
    int synchset_element[] = {EOF, ',', ']', '}'};
    int synchset_element_size = sizeof(synchset_element) / sizeof(synchset_element[0]);

    // Conjunto primero de attribute_value()
    int first_attribute_value[] = {'{', '[', LITERAL_CADENA, LITERAL_NUM, PR_TRUE, PR_FALSE, PR_NULL};
    int first_attribute_value_size = sizeof(first_attribute_value) / sizeof(first_attribute_value[0]);

	check_input(first_attribute_value, first_attribute_value_size, synchset, synchset_size);
	if (!token_in_set(t.compLex, synchset, synchset_size)){
		printf("Entra attribute value\n");
		switch (t.compLex)
		{
		case LITERAL_CADENA:
		case LITERAL_NUM:
		case PR_TRUE:
		case PR_FALSE:
		case PR_NULL:
			match(t.compLex);
			break;
		case '{':
		case '[':
			element(synchset_element, synchset_element_size);
			break;
		default:
			error_sintactico("x");
		}
		printf("Check input de attribute value\n");
		check_input(synchset, synchset_size, first_attribute_value, first_attribute_value_size);
		printf("After check input\n");
	}
}

int token_in_set(int token, int set[], int set_size){
	for (int i = 0; i < set_size; i++){
		if (token == set[i]){
			return 1;
		}
	}
	return 0;
};

void match(int expectedToken) {
	printf("Expected token %d\n", expectedToken);
    if (t.compLex == expectedToken) {
        getToken();
    } else {
        error_sintactico("Token no esperado");
    }
}

void check_input(int firstset[], int firstset_size, int followset[], int followset_size){
    int firstfollow_size = firstset_size + followset_size;
    int firstfollow[firstfollow_size];

    for (int i = 0; i < firstset_size; i++){
        firstfollow[i] = firstset[i];
		printf("first %d\n", firstset[i]);
    }

    for (int i = 0; i < followset_size; i++){
        firstfollow[firstset_size + i] = followset[i];
		printf("follow %d\n", followset[i]);
    }
	printf("t.complex %d\n", t.compLex);
    if (!token_in_set(t.compLex, firstset, firstset_size)){
        error_sintactico("No reconocido");
        scan_to(firstfollow, firstfollow_size);
    }
}

void scan_to(int synchset[], int synchset_size){
	for (int i = 0; i < synchset_size; i++){
		synchset[i] = synchset[i];
	}

	while (!token_in_set(t.compLex, synchset, synchset_size) || t.compLex != EOF){
		getToken();
	}
}

void error_sintactico(const char* mensaje) {
    printf("Error sintáctico en línea %d: %s.\n", numLinea, mensaje);
}

int main(int argc, char* args[]) {
    // Inicializar analizador léxico
    initTabla();
    initTablaSimbolos();
    int synchset_json[] = {EOF};
    int synchset_size = sizeof(synchset_json) / sizeof(synchset_json[0]); 

    if (argc > 1) {
		printf("Envia argumentos\n");
        if (!(archivo = fopen(args[1], "rt"))) {
            printf("Archivo no encontrado.\n");
            exit(1);
        }
		getToken();
        json(synchset_json, synchset_size);
        fclose(archivo);

        if (t.compLex == EOF) printf("Parseado con éxito.");
        else printf("Símbolo inesperado");
    } else {
        printf("Debe pasar como parámetro el path al archivo fuente.\n");
        exit(1);
    }

    return 0;
}
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

// Declaraciones de funciones de parsing
void json();
void element();
void array();
void e_prime();
void element_list();
void element_list_prime();
void object();
void a_prime();
void attribute_list();
void attribute_list_prime();
void attribute();
void attribute_name();
void attribute_value();

void match(int expectedToken);
void error_sintactico(const char* mensaje);

void json() {
	switch (t.compLex)
    {
    case L_LLAVE:
        object();
        break;
    case L_CORCHETE:
        array();
        break;
    
	default:
        break;
    }
}

void element() {

	switch (t.compLex)
    {
    case L_CORCHETE:
        match(L_CORCHETE);
        e_prime();
        match(R_CORCHETE);
        break;
    
    default:
        break;
    }
}

void array() {
    switch (t.compLex)
    {
    case L_CORCHETE:
        match(L_CORCHETE);
        e_prime();
        match(R_CORCHETE);
        break;
    
    default:
        break;
    }
}

void e_prime() {
    switch (t.compLex)
    {
    case L_LLAVE:
    case L_CORCHETE:
        element_list();
        break;
    }
}

void element_list() {
    switch (t.compLex)
    {
    case L_LLAVE:
    case L_CORCHETE:
        element();
        element_list();
        break;
    default:
        break;
    }
}

void element_list_prime() {
    switch (t.compLex)
    {
    case COMA:
        match(COMA);
        element();
        element_list();
        break;
    }
}

void object() {
   switch (t.compLex)
    {
    case L_LLAVE:
        match(L_LLAVE);
        a_prime();
        match(R_LLAVE);
        break;
    }
}

void a_prime() {
    switch (t.compLex)
    {
    case LITERAL_CADENA:
        attribute_list();
        break;
    }
}

void attribute_list() {
    switch (t.compLex)
    {
    case LITERAL_CADENA:
        attribute();
        attribute_list_prime();
        break;
    
    default:
        break;
    }
}

void attribute_list_prime() {
    switch (t.compLex)
    {
    case COMA:
        match(COMA);
        attribute();
        attribute_list_prime();
        break;
    }
}

void attribute() {
    switch (t.compLex)
    {
    case LITERAL_CADENA:
        attribute_name();
        match(DOS_PUNTOS);
        attribute_value();
        break;
    
    default:    
        break;
    }
}

void attribute_name() {
    switch (t.compLex)
    {
    case LITERAL_CADENA:
        match(LITERAL_CADENA);
        break;
    
    default:
        break;
    }
}

void attribute_value() {
    if (t.compLex == LITERAL_CADENA || t.compLex == LITERAL_NUM || t.compLex == PR_TRUE || t.compLex == PR_FALSE || t.compLex == PR_NULL) {
        match(t.compLex);
    } else if (t.compLex == L_LLAVE || t.compLex == L_CORCHETE) {
        element();
    } else {
		error("x");
	}
}

void match(int expectedToken) {
    if (t.compLex == expectedToken) {
        getToken();
    } else {
        error_sintactico("Token no esperado");
    }
}

void error_sintactico(const char* mensaje) {
    printf("Error sintáctico en línea %d: %s.\n", numLinea, mensaje);
}

int main(int argc, char* args[]) {
    // Inicializar analizador léxico
    initTabla();
    initTablaSimbolos();

    if (argc > 1) {
        if (!(archivo = fopen(args[1], "rt"))) {
            printf("Archivo no encontrado.\n");
            exit(1);
        }
        output = fopen("output.txt", "w");
        json();
		while (t.compLex!=EOF){
			getToken();
			if(imprimir==1)
				fprintf(output, "%s ",buscarNombreCompLex(t.pe->compLex));
		}		
        if (t.compLex == EOF) {
            printf("Parsing completo sin errores.\n");
        } else {
            printf("Símbolos inesperados después del fin de la entrada");
        }
        fclose(output);
        fclose(archivo);
    } else {
        printf("Debe pasar como parámetro el path al archivo fuente.\n");
        exit(1);
    }

    return 0;
}

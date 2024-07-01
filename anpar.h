/************* Prototipos ********************/
void json(int synchset[], int synchset_size);
void element(int synchset[], int synchset_size);
void array(int synchset[], int synchset_size);
void e_prime(int synchset[], int synchset_size);
void element_list(int synchset[], int synchset_size);
void element_list_prime(int synchset[], int synchset_size);
void object(int synchset[], int synchset_size);
void a_prime(int synchset[], int synchset_size);
void attribute_list(int synchset[], int synchset_size);
void attribute_list_prime(int synchset[], int synchset_size);
void attribute(int synchset[], int synchset_size);
void attribute_name(int synchset[], int synchset_size);
void attribute_value(int synchset[], int synchset_size);
int token_in_set(int token, int set[], int set_size);
void match(int expectedToken);
void check_input(int firstset[], int firstset_size, int followset[], int followset_size);
void scan_to(int synchset[], int synchset_size);
void error_sintactico(const char* mensaje);

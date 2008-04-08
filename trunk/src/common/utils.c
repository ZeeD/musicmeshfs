/*
    This file is part of MusicMeshFS[cd].

    MusicMeshFS[cd] is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    MusicMeshFS[cd] is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with MusicMeshFS[cd].  If not, see <http://www.gnu.org/licenses/>.
*/

#include "utils.h"

/**
    analizza due vettori dinamici di interi, di dimensione nota a priori, e
    concatena il secondo al primo, aggiornandone la lunghezza.

    Nel caso len_array2 == 0, non vi è alcuna modifica a len_array1 e a array1.
    Nel caso len_array2 < 0, array1 è privato di len_array2 elementi (in questo
    caso si assume array2 = NULL)
    \param array1 primo vettore (valore modificato in-place)
    \param array2 secondo vettore
    \sa extend_str(), free_int(), init_int()
*/
void extend_int(dynamic_int_t* array1, dynamic_int_t array2) {
    int new_size = array1->size + array2.size;
    int* new_buf = realloc(array1->buf, new_size * sizeof(int));
    for (int i=0; i<array2.size; i++)
        new_buf[array1->size + i] = array2.buf[i];
    array1->buf = new_buf;
    array1->size = new_size;
}

/**
    analizza due vettori dinamici di char*, di dimensione nota a priori, e
    concatena il secondo al primo, aggiornandone la lunghezza.

    Nel caso array2.len == 0, non vi è alcuna modifica a array1
    \param array1 primo vettore (valore modificato in-place)
    \param array2 secondo vettore
    \sa extend_int(), free_str()
*/
void extend_str(dynamic_str_t* array1, dynamic_str_t array2) {
    int new_size = array1->size + array2.size;
    for (int i=1; i<=-new_size; i++)
        free(array1->buf[array1->size - i]);
    char** new_buf = realloc(array1->buf, new_size * sizeof(char*));
    for (int i=0; i<array2.size; i++)
        new_buf[array1->size + i] = strdup(array2.buf[i]);
    array1->buf = new_buf;
    array1->size = new_size;
}

/**
    svuota un vettore dinamico di interi

    \param array vettore da cancellare (modificato in-place)
    \sa free_str(), extend_int(), init_int()
*/
void free_int(dynamic_int_t* array) {
    if (array->buf) {
        free(array->buf);
        array->buf = NULL;
        array->size = 0;
    }
}

/**
    svuota un vettore dinamico di char*

    \param array vettore da cancellare (modificato in-place)
    \sa free_int(), extend_str(), init_str()
*/
void free_str(dynamic_str_t* array) {
    if (array->buf) {
        for (int i=0; i<array->size; i++)
            free(array->buf[i]);
        free(array->buf);
        array->buf = NULL;
        array->size = 0;
    }
}

/**
    aggiunge in coda ad array il valore i

    \param array vettore dinamico di interi (modificato in-place)
    \param i intero da aggiungere in coda ad array
    \sa extend_int(), free_int()
*/
void append_int(dynamic_int_t* array, const int i) {
    array->buf = realloc(array->buf, sizeof(int) * (array->size + 1));
    array->buf[array->size] = i;
    array->size += 1;
}

/**
    aggiunge in coda ad array il valore str

    \param array vettore dinamico di interi (modificato in-place)
    \param str stringa da aggiungere in coda ad array
    \sa extend_str(), free_str()
 */
void append_str(dynamic_str_t* array, const char* str) {
    array->buf = realloc(array->buf, sizeof(char*) * (array->size + 1));
    array->buf[array->size] = strdup(str);
    array->size += 1;
}

/**
    inizializza un vettore dinamico di int

    \param array vettore da inizializzare (modificato in-place)
    \sa init_str(), extend_int(), free_int()
*/
void init_int(dynamic_int_t* array) {
    array->size = 0;
    array->buf = NULL;
}

/**
    inizializza un vettore dinamico di char*

    \param array vettore da inizializzare (modificato in-place)
    \sa init_int(), extend_str(), free_str()
*/
void init_str(dynamic_str_t* array) {
    array->size = 0;
    array->buf = NULL;
}

/**
    inizializza un vettore dinamico di int, partendo da un vettore di interi

    \param s dimensione del vettore da usare per inizializzare array
    \param from vettore di interi da usare per inizializzare array
    \return vettore di interi inizializzato

    (NON è ROBUSTO!)
    \sa init_str(), extend_int(), free_int()
 */
dynamic_int_t from_int(const int* from, const int s) {
    dynamic_int_t array;
    init_int(&array);
    for (int i=0; i<s; i++)
        append_int(&array, from[i]);
    return array;
}

/**
    inizializza un vettore dinamico di char*, partendo da un vettore di stringhe

    \param s dimensione del vettore da usare per inizializzare array
    \param from vettore di interi da usare per inizializzare array

    (NON è ROBUSTO!)
    \return vettore di interi inizializzato
    \sa init_int(), extend_str(), free_str()
 */
dynamic_str_t from_str(const char** from, const int s) {
    dynamic_str_t array;
    init_str(&array);
    for (int i=0; i<s; i++)
        append_str(&array, from[i]);
    return array;
}

/**
    Crea un nuovo dynamic_int_t (malloc`ed!!!) tale che gli elementi ritornati
    sono quelli compresi tra begin (incluso) ed end (escluso)
    es: slice_int([1,2,3,4], 1, 3) == [2,3]
    \param array vettore da "affettare"
    \param begin valore iniziale
    \param end valore finale
    \return dynamic_int_t con elementi di array per ogni begin <= i < end
*/
dynamic_int_t slice_int(dynamic_int_t array, int begin, int end) {
    if (begin < -array.size)    // [1,2,3][-30:X] == [1,2,3][0:X] <= [1,2,3]
        begin = 0;
    else if (begin > array.size)// [1,2,3][30:X] == [1,2,3][len:X] == []
        begin = array.size;
    else if (begin < 0)         // [1,2,3][-1:X] == [1,2,3][len-1:X] <= [1,2,3]
        begin += array.size;

    if (end < -array.size)      // [1,2,3][X:-30] == [1,2,3][X:0] == []
        end = 0;
    else if (end > array.size)  // [1,2,3][X:30] == [1,2,3][X:len] <= [1,2,3]
        end = array.size;
    else if (end < 0)           // [1,2,3][X:-1] == [1,2,3][X:len-1] <= [1,2,3]
        end += array.size;

    dynamic_int_t r;
    init_int(&r);
    for (int i=begin; i<end; i++)
        append_int(&r, array.buf[i]);
    return r;
}

/**
    Crea un nuovo dynamic_str_t (malloc`ed!!!) tale che gli elementi ritornati
    sono quelli compresi tra begin (incluso) ed end (escluso)
    es: slice_str(["ciao","mondo","come","va?"], 1, 3) == ["mondo", "come"]
    \param array vettore da "affettare"
    \param begin valore iniziale
    \param end valore finale
    \return dynamic_str_t con elementi di array per ogni begin <= i < end
 */
dynamic_str_t slice_str(dynamic_str_t array, int begin, int end) {
    if (begin < -array.size)    // ["a","b","c"][-30:X] == ["a","b","c"][0:X] <= ["a","b","c"]
        begin = 0;
    else if (begin > array.size)// ["a","b","c"][30:X] == ["a","b","c"][len:X] == []
        begin = array.size;
    else if (begin < 0)         // ["a","b","c"][-1:X] == ["a","b","c"][len-1:X] <= ["a","b","c"]
        begin += array.size;

    if (end < -array.size)      // ["a","b","c"][X:-30] == ["a","b","c"][X:0] == []
        end = 0;
    else if (end > array.size)  // ["a","b","c"][X:30] == ["a","b","c"][X:len] <= ["a","b","c"]
        end = array.size;
    else if (end < 0)           // ["a","b","c"][X:-1] == ["a","b","c"][X:len-1] <= ["a","b","c"]
        end += array.size;

    dynamic_str_t r;
    init_str(&r);
    for (int i=begin; i<end && i<array.size; i++)
        append_str(&r, array.buf[i]);
    return r;
}

/**
    funzione di utility per effettuare il preprocessing dei dati da inserire nel
    database.

    Al momento valgono le seguenti regole:
    # ritornare NULL (e quindi usare il valore di default in sqlite3) quando si
            trova un tag vuoto tramite taglib
    # sostituire ogni carattere '/' col carattere '_'

    \param value stringa da verificare
    \return stringa con applicate le regole a value se value è non vuota,
            NULL altrimenti
    \sa add_local_file_in_db()
*/
char* preprocessing(const char* value) {
    if (value && value[0]) {
        char* ret = strdup(value);
        for (int i=0; ret[i]; i++)
            if (ret[i] == '/')
                ret[i] = '_';
        return ret;
    }
    return NULL;
}

/**
    Data una stringa (char* NULL-terminated, vabbè) rappresentante un path,
    ritorna l'estensione del path stesso
    (ex: extract_extension("file.mp3") == "mp3")

    da notare come definisco estensione come la parte di stringa dopo l'ultimo
    punto! ex: (extract_extension(file.tar.gz) == gz!)
    \param path percorso (di un file) da analizzare
    \return the pointer at the position (in the *path space memory) where is the
            extension
*/
const char* extract_extension(const char* path) {
    for (size_t i = strlen(path); i; i--)
        // ignoro trailing dots: extract_extension("file.") = NULL
        if (path[i-1] == '.')
            return path + i;
    return NULL;
}
/**
    Data una stringa (char* NULL-terminated, vabbè) rappresentante un path,
    ritorna il basename del path stesso
    (ex: extract_extension("path/file.mp3") == "file.mp3")

    \param path percorso (di un file) da analizzare
    \return the pointer at the position (in the *path space memory) where is the
            extension
 */
const char* extract_basename(const char* path) {
    if (!path)
        return NULL;
    int i = 0, last_slash_i = -1;
    while (path[i]) {
        if (path[i] == '/')
            last_slash_i = i+1;
        i++;
    }
    if (last_slash_i == -1)
        return NULL;
    return path+last_slash_i;
}

/**
    Data una stringa (char* NULL-terminated, vabbè) rappresentante un path,
    ritorna i permessi "others", riguardanti la lettura / scrittura sul path

    \param path percorso (di un file) da analizzare
    \return
            - 0 se non ci sono permessi di accesso né in lettura né in scrittura
            - 2 se il file è in sola lettura (??? ha poco senso)
            - 4 se il file è in sola scrittura
            - 6 se il file è accessibile in lettura / scrittura
            - -1 in caso d'errore
*/
int extract_group_perm(const char* path) {
    struct stat data;
    if (stat(path, &data) == -1) {
        perror("stat");
        return -1;
    }
    return (data.st_mode & S_IROTH) + (data.st_mode & S_IWOTH);
}

/**
    Data una stringa (char* NULL-terminated, vabbè) rappresentante un path,
    ritorna la dimensione, in byte, del file stesso

    \param path percorso (di un file) da analizzare
    \return dimensione di path
 */
int extract_size(const char* path) {
    struct stat buf;
    stat(path, &buf);
    return buf.st_size;
}

/**
    analizza e divide una stringa in un array dinamico di stringhe in base alla
    presenza in string del carattere separator
    es: split("path/ciao/mondo", '/') == ["path", "ciao", "mondo"]
    \param string stringa da splittare
    \param separator carattere da usare per splittare la stringa
    \return un array dinamico (malloc`ed!!) contenente gli elementi separati
    \todo rivederla in modo da evitare di fare la stessa operazione fuori dal
            ciclo!
*/
dynamic_str_t split(const char* string, const char separator) {
    dynamic_str_t r;
    init_str(&r);
    size_t i=0, j=0;
    for (; string[i]; i++)
        if (string[i] == separator) {
            char tmp[i-j+1];
            for (size_t k=0; k<(i-j); k++)
                tmp[k] = string[j+k];
            tmp[i-j] = '\0';
            append_str(&r, tmp);
            j = i + 1;
        }
    char tmp[i-j+1];
    for (size_t k=0; k<(i-j); k++)
        tmp[k] = string[j+k];
    tmp[i-j] = '\0';
    append_str(&r, tmp);
    j = i + 1;
    return r;
}

/**
    riunisce un vettore di stringhe in una sola (malloc`ed!); tra un elemento e
    l'altro inserisce il carattere separator
    es: join(["ciao", "mondo"], ' ') == "ciao mondo"
    \param array elenco di stringhe da unire
    \param separator carattere di unione
    \return stringa contenente tutti gli elementi di array uniti con separator
*/
char* join(dynamic_str_t array, char separator) {
    char* r = "";
    if (array.size) {
        r = strdup(array.buf[0]);
        char s[] = { separator, '\0' };
        for (int i=1; i<array.size; i++) {
            r = realloc(r, strlen(r) + 1 + strlen(array.buf[i]));
            r = strcat(r, s);
            r = strcat(r, array.buf[i]);
        }
    }
    return r;
}

/**
    \todo non so se usarla o meno, in produzione
*/
void dbgprint_str(dynamic_str_t a, char* s) {
    errprintf("[DBG][dbgprint_str][%s.size = %d]\n", s, a.size);
    for (int i=0; i<a.size; i++)
        errprintf( "[DBG][dbgprint_str][%s[%d] = `%s']\n", s, i, a.buf[i]);
}

/**
    \todo non so se usarla o meno, in produzione
*/
void dbgprint_int(dynamic_int_t a, char* s) {
    errprintf("[DBG][dbgprint_int][%s.size = %d]\n", s, a.size);
    for (int i=0; i<a.size; i++)
        errprintf("[DBG][dbgprint_int][%s[%d] = %d]\n", s, i, a.buf[i]);
}

/**
    Verifica se almeno (non è un set!) un elemento di array sia == a element
    \return 1 se element \f$ \in \f$ array, 0 altrimenti
*/
int contains_int(const dynamic_int_t array, const int element) {
    for (int i=0; i<array.size; i++)
        if (element == array.buf[i])
            return 1;
    return 0;
}

/**
    Verifica se almeno (non è un set!) un elemento di array sia == a element
    \return 1 se element \f$ \in \f$ array, 0 altrimenti
 */
int contains_str(const dynamic_str_t array, const char* element) {
    for (int i=0; i<array.size; i++) {
        if (!strcmp(element, array.buf[i]))
            return 1;
    }
    return 0;
}

/**
    Ritorna l'indice del primo (non è un set!) elemento di array che sia == a
    element
    \return indice dell'element se element \f$ \in \f$ array, -1 altrimenti
*/
int index_of_int(const dynamic_int_t array, const int element) {
    for (int i=0; i<array.size; i++)
        if (element == array.buf[i])
            return i;
    return -1;
}

/**
    Ritorna l'indice del primo (non è un set!) elemento di array che sia == a
    element
    \return indice dell'element se element \f$ \in \f$ array, -1 altrimenti
 */
int index_of_str(const dynamic_str_t array, const char* element) {
    for (int i=0; i<array.size; i++) {
        if (!strcmp(element, array.buf[i]))
            return i;
    }
    return -1;
}

/**
    allarga la prima stringa per farci entrare la seconda, e le concatena
    \param str1 stringa sorgente (modificata in-place) che verrà allargata
    \param str2 stringa che verrà concatenata a str1
    \return str1, (dopo che ha subìto un realloc() e un strcat())
    \sa strcat(), realloc(), malloc()
*/
char* strmalloccat(char* str1, const char* str2) {
    int l = strlen(str1) + strlen(str2);
    str1 = strcat(realloc(str1, l+1), str2);
    str1[l] = '\0';
    return str1;
}

/**
    `alias' per fprintf(stderr, fmt, ...);
    \param fmt stringa di formattazione da passare a fprintf
    \return il valore di ritorno di vfprintf
    \sa fprintf(), vfprintf()
 */
int errprintf(char* fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    int ret = vfprintf(stderr, fmt, ap);
    va_end(ap);
    return ret;
}

/**
    Dato un dizionario(str, str), nella forma coppia di dynamic_str_t (in
    speranza di dimensioni uguali), e una chiave, restituisce il valore
    corrispondente
    \param chiavi vettore delle chiavi presenti nel dizionario
    \param valori vettore dei valori corrispondenti alle chiavi
    \param chiave chiave usata per recuperare il valore
    \return valore corrispondente a chiave, se chiave \f$ \in \f$ chiavi,
            NULL altrimenti
*/
char* value_from_key(dynamic_str_t chiavi, dynamic_str_t valori, char* chiave) {
    int minsize = chiavi.size < valori.size ? chiavi.size : valori.size;
    for (int i=0; i<minsize; i++)
        if (!strcmp(chiavi.buf[i], chiave))
            return valori.buf[i];
    return NULL;
}

/**
    inizializza un vettore dinamico di "void*"

    \param array vettore da inizializzare (modificato in-place)
    \sa init_str(), extend_int(), free_int()
*/
void init_obj(dynamic_obj_t* array) {
    array->size = 0;
    array->buf = NULL;
}

/**
    svuota un vettore dinamico di void*

    \param array vettore da cancellare (modificato in-place)
    \sa free_str(), extend_int(), init_int()
*/
void free_obj(dynamic_obj_t* array) {
    free(array->buf);
    init_obj(array);
}

/**
    aggiunge in coda ad array il valore obj

    \param array vettore dinamico di void* (modificato in-place)
    \param obj intero da aggiungere in coda ad array
    \sa extend_int(), free_int()
 */
void append_obj(dynamic_obj_t* array, const void* obj) {
    array->buf = realloc(array->buf, (array->size+1) * sizeof(void**));
    array->buf[array->size++] = obj;
}

/**
    Rimuove (se presente) la prima occorrenza di element dall'array
    \param array vettore dinamico di stringhe (modificato in-place)
    \param elemnt elemento di array da rimuovere
    \sa dynamic_str_t
*/
void remove_str(dynamic_str_t* array, const char* element) {
    for (int i=0; i<array->size; i++)
        if (!strcmp(array->buf[i], element)) {
            for (; i<array->size; i++)
                array->buf[i] = array->buf[i+1];
            free(array->buf[i-1]);
            array->size--;
            break;
        }
}

/**
    Rimuove l'ultimo elemento dell'array, se questi non è vuoto
    \param array vettore dinamico di stringhe (modificato in-place)
    \return 0 se non ci sono stati problemi, -1 altrimenti (fondamentalmeente,
            array è vuoto)
*/
int pop_str(dynamic_str_t* array) {
    if (!array->size)
        return -1;
    free(array->buf[array->size-1]);
    array->size--;
    return 0;
}

/**
    Calcola il massimo tra due interi

    (ma possibile che dovevo scrivermela io?)
    \param a intero
    \param b intero
    \return il massimo tra a e b
 */
int max(int a, int b) {
    return a > b ? a : b;
}

/**
    Verifica se una stringa inizia con un'altra (più corta :))

    (equivalente al metodo str.startswith(str) di python)
        "ciao mondo".startswith("ciao") -> True
        "ciao mondo".startswith("mondo") -> False

    (ma possibile che dovevo scrivermela io? strncmp lavora in O(2*n) )

    \param string stringa grande
    \param substr stringa piccola
    \return l'indice subito dopo substr in string, se string.startswith(substr)
            -1 altrimenti
    \attention si assume implicitamente che le due stringhe abbiano la stessa
            codifica! Io faccio solo un controllo char per char!
*/
int startswith(const char* string, const char* substr) {
    if (!string || !substr)
        return -1;
    int i = 0;
    for (; substr[i]; i++)
        if (!string[i] || string[i] != substr[i])
            return -1;
    return i;
}

#include <iostream>
#include <vector>
#include <string>
#include <cstdlib>  // para rand() y srand()
#include <ctime>    // para time()
#include <queue>    // para BFS

using namespace std;

/* --- MUNDO --- */
const int tam = 21;  // Tamaño del laberinto (matriz cuadrada 21x21)
string pared = "🟦";      // Pared
string camino = "⬜";     // Camino libre
string explorador = "🔴"; // Al recorrer el laberinto marca el camino a la salida
string entrada = "🟩";    // Entrada del laberinto
string salida = "🏁";     // Salida del laberinto

/* --- DE CELDA --- */
struct Celda {
    int fila;
    int col;
};

/* --- CREAR MUNDO --- */
// Inicializa una matriz llena de paredes
vector<vector<string>> crear_mundo() {
    vector<vector<string>> matriz(tam, vector<string>(tam, pared));
    return matriz;
}

/* --- IMPRIMIR MUNDO --- */
// Recorre la matriz y la imprime por pantalla
void imprimir_mundo(const vector<vector<string>>& matriz) {
    for (int i = 0; i < tam; i++) {
        for (int j = 0; j < tam; j++) {
            cout << matriz[i][j];
        }
        cout << endl;
    }
    cout << endl;
}

/* --- DIRECCIONES --- */
// Direcciones que el explorador puede moverse al generar el laberinto (pasos de 2 para romper paredes y dejar el intemedio como camino)
vector<Celda> direcciones = {
    {-2, 0},  // arriba
    { 2, 0},  // abajo
    { 0,-2},  // izquierda
    { 0, 2}   // derecha
};

/* --- VALIDAR MOVIMIENTO --- */
// Verifica si el movimiento desde fila o columna en la dirección 'dir' es válido dentro de los límites y sobre pared
bool es_movimiento_valido(const vector<vector<string>>& matriz,
                          int fila, int col,
                          Celda dir) {

    int nueva_fila = fila + dir.fila;
    int nueva_col  = col + dir.col;

    // Revisar que no salga del laberinto
    if (nueva_fila <= 0 || nueva_fila >= tam - 1 ||
        nueva_col  <= 0 || nueva_col  >= tam - 1) {
        return false;
    }

    // Verificar que la celda de destino sea pared
    if (matriz[nueva_fila][nueva_col] != pared) {
        return false;
    }

    return true;
}

/* --- MOVER EXPLORADOR o ROMPER PARED --- */
// Dado un movimiento válido, rompe la pared intermedia y marca la celda destino como camino
Celda mover_explorador(vector<vector<string>>& matriz,
                       int fila, int col,
                       Celda dir) {

    // Calcular la celda intermedia (pared que se rompe)
    int inter_fila = fila + dir.fila / 2;
    int inter_col  = col  + dir.col  / 2;

    int nueva_fila = fila + dir.fila;
    int nueva_col  = col  + dir.col;

    // Romper la pared intermedia y marcar la nueva celda como camino
    matriz[inter_fila][inter_col] = camino;
    matriz[nueva_fila][nueva_col] = camino;

    return {nueva_fila, nueva_col}; // Retornar nueva posición del explorador
}

/* --- GENERAR LABERINTO - BACKTRACKING --- */
// DFS iterativo para generar laberinto
void generar_laberinto(vector<vector<string>>& matriz) {

    vector<Celda> memoria;  // Pila para backtracking

    Celda inicio = {1, 1};          // Empezamos desde la celda (1,1)
    matriz[inicio.fila][inicio.col] = camino; // Marcamos inicio como camino
    memoria.push_back(inicio);      // Añadimos inicio a la pila
    
    while (!memoria.empty()) {
        
        Celda actual = memoria.back(); // Tomamos la celda actual de la pila
        vector<Celda> direcciones_validas;

        // Revisar todas las direcciones posibles desde la celda actual
        for (Celda dir : direcciones) {
            if (es_movimiento_valido(matriz, actual.fila, actual.col, dir)) {
                direcciones_validas.push_back(dir); // Guardamos solo movimientos válidos
            }
        }

        if (!direcciones_validas.empty()) {
            // Escoger una dirección válida aleatoriamente
            Celda dir = direcciones_validas[rand() % direcciones_validas.size()];

            // Mover al explorador y romper la pared intermedia
            Celda nueva = mover_explorador(matriz, actual.fila, actual.col, dir);

            memoria.push_back(nueva); // Añadir nueva celda a la pila (continuar DFS)
        } else {
            memoria.pop_back(); // No hay movimientos, retroceder
        }
    }
}

/* --- RESOLVER LABERINTO - BFS --- */
// BFS para encontrar el camino más corto desde entrada hasta salida
void resolver_laberinto(vector<vector<string>> & matriz) {    
    
    Celda entrada = {0, 0};             // Inicializamos coordenadas de entrada
    Celda salida_celda = {tam - 1, tam - 1}; // Inicializamos coordenadas de salida
    
    queue<Celda> memoria;                // Cola para BFS

    vector<vector<bool>> visitada(tam, vector<bool>(tam, false)); // Matriz de visitados
    vector<vector<Celda>> padre(tam, vector<Celda>(tam));         // Para reconstruir el camino

    // Buscar posición real de entrada y salida en la primera y última fila
    for (int col = 0; col < tam; col++) {
        if (matriz[0][col] == "🟩") {
            entrada = {0, col};
            break;
        }
    }
    for (int col = 0; col < tam; col++) {
        if (matriz[tam-1][col] == "🏁") {
            salida_celda = {tam - 1, col};
            break;
        }
    }
    
    // La posición inicial de BFS es la celda debajo de la entrada (conectada al laberinto)
    Celda pos_actual = {entrada.fila + 1, entrada.col};
    visitada[pos_actual.fila][pos_actual.col] = true;       // Marcamos como visitado
    padre[pos_actual.fila][pos_actual.col] = {-1, -1};      // Padre inicial nulo para reconstruccion de camino

    memoria.push(pos_actual);  // Añadir a la cola

    // Vecinos posibles (arriba, abajo, izquierda, derecha)
    // un vecino es una celda que se puede alcanzar desde el actual ej. si estoy en (2,2) un vecino es (1,2) o (3,2) o (2,1) o (2,3)
    vector<pair<int,int>> vecino = {
        {-1, 0},
        { 1, 0},
        { 0,-1},
        { 0, 1}};
        
    bool encontrada = false;  // Bandera para indicar si se encontró la salida

    // Ciclo principal BFS
    while (!memoria.empty()) {
        
        pos_actual = memoria.front(); // Tomamos el primer elemento de la cola
        memoria.pop();                // Lo removemos

        // Si llegamos a la salida, terminamos
        if (pos_actual.fila == salida_celda.fila && pos_actual.col == salida_celda.col) {
            encontrada = true; 
            break;
        }

        // Revisamos vecinos de la celda actual
        for (auto dir : vecino) {

            int fila_vecino = pos_actual.fila + dir.first;
            int col_vecino  = pos_actual.col  + dir.second;

            // Solo agregamos vecinos dentro de límites, no visitados y que sean camino o salida
            if (fila_vecino >= 0 && fila_vecino < tam &&
                col_vecino  >= 0 && col_vecino  < tam &&
                visitada[fila_vecino][col_vecino] == false &&
                (matriz[fila_vecino][col_vecino] == camino ||
                 matriz[fila_vecino][col_vecino] == salida)) {

                visitada[fila_vecino][col_vecino] = true;    // Marcamos como visitado
                padre[fila_vecino][col_vecino] = pos_actual; // Guardamos de dónde venimos
                memoria.push({fila_vecino, col_vecino});     // Añadimos a la cola BFS
            }
        }
    }

    // Reconstruir camino desde la salida hasta la entrada usando matriz padre
    if (encontrada) {
        Celda actual = salida_celda;

        while (!(actual.fila == entrada.fila && actual.col == entrada.col)) {
            if (actual.fila < 0 || actual.col < 0 || actual.fila >= tam || actual.col >= tam) {
                break;  // Evitar acceso fuera de límites
            }
            matriz[actual.fila][actual.col] = "🔴"; // Marcamos el camino encontrado
            actual = padre[actual.fila][actual.col]; // Retrocedemos al padre
        }

        matriz[entrada.fila][entrada.col] = "🟩"; // Dejamos la entrada intacta
    }
}

/* --- MAIN --- */
int main() {

    srand(time(0)); // Semilla aleatoria basada en el tiempo

    cout << "Creando mundo..." << endl << endl;

    vector<vector<string>> mundo = crear_mundo(); // Crear matriz inicial llena de paredes

    generar_laberinto(mundo); // Generar laberinto aleatorio
    
    // Conectar entrada al laberinto (fila 0 con fila 1)
    for (int col = 0; col < tam; col++) {
        if (mundo[1][col] == camino) {
            mundo[0][col] = entrada;   // Icono de entrada
            mundo[1][col] = camino;    // Asegurar conexión real
            break;
        }
    }

    // Conectar salida al laberinto (fila tam-1 con fila tam-2)
    for (int col = tam-1; col >= 0; col--) {
        if (mundo[tam-2][col] == camino) {
            mundo[tam-1][col] = salida;   // Icono de salida
            mundo[tam-2][col] = camino;   // Asegurar conexión real
            break;
        }
    }

    imprimir_mundo(mundo); // Mostrar laberinto generado

    cout << "Resolviendo laberinto..." << endl << endl;

    resolver_laberinto(mundo); // Ejecutar BFS para encontrar camino

    imprimir_mundo(mundo); // Mostrar laberinto con camino encontrado

    return 0;
}

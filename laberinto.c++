#include <iostream>
#include <vector>
#include <string>
#include <cstdlib>  // para rand() y srand()
#include <ctime>    // para time()
#include <queue>    // para BFS

using namespace std;

/* --- MUNDO --- */
pair<int,int> tam;    // tam.first = filas, tam.second = columnas
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
    vector<vector<string>> matriz(tam.first, vector<string>(tam.second, pared));
    return matriz;
}

/* --- IMPRIMIR MUNDO --- */
void imprimir_mundo(const vector<vector<string>>& matriz) {
    for (int i = 0; i < tam.first; i++) {
        for (int j = 0; j < tam.second; j++) {
            cout << matriz[i][j];
        }
        cout << endl;
    }
    cout << endl;
}

/* --- DIRECCIONES --- */
vector<Celda> direcciones = {
    {-2, 0},  // arriba
    { 2, 0},  // abajo
    { 0,-2},  // izquierda
    { 0, 2}   // derecha
};

/* --- VALIDAR MOVIMIENTO --- */
bool es_movimiento_valido(const vector<vector<string>>& matriz,
                          int fila, int col,
                          Celda dir) {

    int nueva_fila = fila + dir.fila;
    int nueva_col  = col + dir.col;

    if (nueva_fila <= 0 || nueva_fila >= tam.first - 1 ||
        nueva_col  <= 0 || nueva_col  >= tam.second - 1) {
        return false;
    }

    if (matriz[nueva_fila][nueva_col] != pared) {
        return false;
    }

    return true;
}

/* --- MOVER EXPLORADOR --- */
Celda mover_explorador(vector<vector<string>>& matriz,
                       int fila, int col,
                       Celda dir) {

    int inter_fila = fila + dir.fila / 2;
    int inter_col  = col  + dir.col  / 2;

    int nueva_fila = fila + dir.fila;
    int nueva_col  = col  + dir.col;

    matriz[inter_fila][inter_col] = camino;
    matriz[nueva_fila][nueva_col] = camino;

    return {nueva_fila, nueva_col};
}

/* --- GENERAR LABERINTO --- */
void generar_laberinto(vector<vector<string>>& matriz) {

    vector<Celda> memoria;

    Celda inicio = {1, 1};
    matriz[inicio.fila][inicio.col] = camino;
    memoria.push_back(inicio);
    
    while (!memoria.empty()) {
        
        Celda actual = memoria.back();
        vector<Celda> direcciones_validas;
        
        for (Celda dir : direcciones) {
            if (es_movimiento_valido(matriz,
                                     actual.fila,
                                     actual.col,
                                     dir)) {
                direcciones_validas.push_back(dir);
            }
        }
        
        if (!direcciones_validas.empty()) {
            
            Celda dir = direcciones_validas[rand() % direcciones_validas.size()];

            Celda nueva = mover_explorador(matriz,
                actual.fila,
                actual.col,
                dir);
                                           
            memoria.push_back(nueva);
            
        } else {
            memoria.pop_back();
        }
    }
}

/* --- RESOLVER LABERINTO --- */
void resolver_laberinto(vector<vector<string>> & matriz) {    
    
    Celda entrada = {0, 0};
    Celda salida_celda = {tam.first - 1, tam.second - 1};
    
    queue<Celda> memoria;

    vector<vector<bool>> visitada(tam.first, vector<bool>(tam.second, false));
    vector<vector<Celda>> padre(tam.first, vector<Celda>(tam.second));

    for (int col = 0; col < tam.second; col++) {
        if (matriz[0][col] == "🟩") {
            entrada = {0, col};
            break;
        }
    }
    for (int col = 0; col < tam.second; col++) {
        if (matriz[tam.first-1][col] == "🏁") {
            salida_celda = {tam.first-1, col};
            break;
        }
    }
    
    Celda pos_actual = {entrada.fila + 1, entrada.col};
    visitada[pos_actual.fila][pos_actual.col] = true;
    padre[pos_actual.fila][pos_actual.col] = {-1, -1};

    memoria.push(pos_actual);
        
    vector<pair<int,int>> vecino = {
        {-1, 0},
        { 1, 0},
        { 0,-1},
        { 0, 1}
    };
        
    bool encontrada = false;

    while (!memoria.empty()) {
        
        pos_actual = memoria.front();
        memoria.pop();

        if (pos_actual.fila == salida_celda.fila && pos_actual.col == salida_celda.col) {
            encontrada = true; 
            break;
        }

        for (auto dir : vecino) {

            int fila_vecino = pos_actual.fila + dir.first;
            int col_vecino  = pos_actual.col  + dir.second;

            if (fila_vecino >= 0 && fila_vecino < tam.first &&
                col_vecino  >= 0 && col_vecino  < tam.second &&
                visitada[fila_vecino][col_vecino] == false &&
                (matriz[fila_vecino][col_vecino] == camino ||
                 matriz[fila_vecino][col_vecino] == salida)) {

                visitada[fila_vecino][col_vecino] = true;
                padre[fila_vecino][col_vecino] = pos_actual;
                memoria.push({fila_vecino, col_vecino});
            }
        }
    }

    if (encontrada) {
        Celda actual = salida_celda;

        while (!(actual.fila == entrada.fila && actual.col == entrada.col)) {
            if (actual.fila < 0 || actual.col < 0 || actual.fila >= tam.first || actual.col >= tam.second) {
                break;
            }
            matriz[actual.fila][actual.col] = "🔴";
            actual = padre[actual.fila][actual.col];
        }

        matriz[entrada.fila][entrada.col] = "🟩";
    } else {
        cout << "No se encontro camino a la salida" << endl;
    }
}

/* --- MAIN --- */
int main() {
    cout << "Ingrese cantidad de filas (impar y >=5): ";
    cin >> tam.first;
    cout << "Ingrese cantidad de columnas (impar y >=5): ";
    cin >> tam.second;

    srand(time(0));

    cout << "Creando mundo..." << endl << endl;

    vector<vector<string>> mundo = crear_mundo();

    generar_laberinto(mundo);
    
    for (int col = 0; col < tam.second; col++) {
        if (mundo[1][col] == camino) {
            mundo[0][col] = entrada;
            mundo[1][col] = camino;
            break;
        }
    }

    for (int col = tam.second-1; col >= 0; col--) {
        if (mundo[tam.first-2][col] == camino) {
            mundo[tam.first-1][col] = salida;
            mundo[tam.first-2][col] = camino;
            break;
        }
    }

    imprimir_mundo(mundo);

    cout << "Resolviendo laberinto..." << endl << endl;

    resolver_laberinto(mundo);

    imprimir_mundo(mundo);

    return 0;
}

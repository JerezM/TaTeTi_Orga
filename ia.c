#include <stdlib.h>
#include <stdio.h>

#include "lista.h"
#include "arbol.h"
#include "ia.h"

#include <time.h>

// Prototipos de funciones auxiliares.
static void ejecutar_min_max(tBusquedaAdversaria b);
static void crear_sucesores_min_max(tArbol a, tNodo n, int es_max, int alpha, int beta, int jugador_max, int jugador_min);
static int valor_utilidad(tEstado e, int jugador_max);
static tLista estados_sucesores(tEstado e, int ficha_jugador);
static void diferencia_estados(tEstado anterior, tEstado nuevo, int * x, int * y);
static tEstado clonar_estado(tEstado e);

void crear_busqueda_adversaria(tBusquedaAdversaria * b, tPartida p){
    int i, j;
    tEstado estado;

    (*b) = (tBusquedaAdversaria) malloc(sizeof(struct busqueda_adversaria));
    if ((*b) == NULL) exit(IA_ERROR_MEMORIA);

    estado = (tEstado) malloc(sizeof(struct estado));
    if (estado == NULL) exit(IA_ERROR_MEMORIA);

    // Se clona el estado del tablero de la partida, al estado inicial de la búsqueda adversaria.
    for(i=0; i<3; i++){
        for(j=0; j<3; j++){
            estado->grilla[i][j] = p->tablero->grilla[i][j];
        }
    }

    // Se asume que el estado de la partida es PART_EN_JUEGO por lo que, la utilidad del estado
    // inicialmente es IA_NO_TERMINO
    estado->utilidad = IA_NO_TERMINO;

    // Inicializa los valores que representarán a los jugadores MAX y MIN respectivamente.
    (*b)->jugador_max = p->turno_de;
    (*b)->jugador_min = (p->turno_de == PART_JUGADOR_1) ? PART_JUGADOR_2 : PART_JUGADOR_1;

    // Inicializa un árbol para la búsqueda adversaria inicialmente vacío.
    crear_arbol(&((*b)->arbol_busqueda));

    // Inicializa la raíz del árbol de búsqueda con el estado del tablero T.
    crear_raiz((*b)->arbol_busqueda, estado);

    // Ejecuta algoritmo Min-Max con podas Alpha-Beta.
    ejecutar_min_max((*b));

}

/**
>>>>>  A IMPLEMENTAR   <<<<<
*/
void proximo_movimiento(tBusquedaAdversaria b, int * x, int * y){
    tArbol arbol = b->arbol_busqueda;
    tNodo raiz= arbol->raiz;
    tEstado mejorEstado, Inicial;
    tPosicion PosActual, PosFinal;
    int ValorAComparar, actualUtilidad;
    PosActual = l_primera(arbol->raiz->hijos);
    PosFinal = l_fin(arbol->raiz->hijos);
    Inicial = a_recuperar(arbol, arbol->raiz);
    mejorEstado = NULL;
    ValorAComparar = 0;

    while (PosActual != PosFinal && ValorAComparar != IA_GANA_MAX) {
        tEstado estadoSucesor=a_recuperar(arbol, l_recuperar(raiz->hijos, PosActual));
        actualUtilidad = estadoSucesor->utilidad;
        if (actualUtilidad > ValorAComparar && actualUtilidad != IA_NO_TERMINO) {
            mejorEstado = ((tEstado)a_recuperar(arbol, l_recuperar(raiz->hijos, PosActual)));
            ValorAComparar = actualUtilidad;
        }
        PosActual = l_siguiente(raiz->hijos, PosActual);
    }
    if (mejorEstado == NULL)
        mejorEstado = ((tEstado)a_recuperar(arbol, l_recuperar(raiz->hijos, l_ultima(raiz->hijos))));

    diferencia_estados(Inicial, mejorEstado, x, y);
}

void EliminarEstado(tEstado *e){
    //free((*e)->grilla); Por que esta linea hace que se crashee el codigo?
}

/**
>>>>>  A IMPLEMENTAR   <<<<<
**/
void destruir_busqueda_adversaria(tBusquedaAdversaria * b){
    a_destruir(&(*b)->arbol_busqueda, (void (*)(tElemento)) &EliminarEstado);
    free(*b);
}

// ===============================================================================================================
// FUNCIONES Y PROCEDEMIENTOS AUXILIARES
// ===============================================================================================================

/**
Ordena la ejecución del algoritmo Min-Max para la generación del árbol de búsqueda adversaria, considerando como
estado inicial el estado de la partida almacenado en el árbol almacenado en B.
**/
static void ejecutar_min_max(tBusquedaAdversaria b){
    tArbol arbol= b->arbol_busqueda;
    tNodo root= a_raiz(arbol);
    int jugador_max= b->jugador_max;
    int jugador_min= b->jugador_min;

    crear_sucesores_min_max(arbol, root, 1, IA_INFINITO_NEG, IA_INFINITO_POS, jugador_max, jugador_min);
}

/**
 * Consulta el mayor valor entre dos enteros parametrizados.
 * @param v1 Valor1 a evaluar.
 * @param v2 Valor2 a evaluar.
 * @return El mayor valor.
 */
int MAX(int v1,int v2){
    int ret;

    if(v1>=v2){
        ret= v1;
    }
    else{
        ret= v2;
    }
    return ret;
}

/**
 * Consulta el menor valor entre dos enteros parametrizados.
 * @param v1 Valor1 a evaluar.
 * @param v2 Valor2 a evaluar.
 * @return El menor valor.
 */
int MIN(int v1,int v2){
    int ret;

    if(v1>=v2){
        ret= v2;
    }
    else{
        ret= v1;
    }
    return ret;
}

/**
>>>>>  A IMPLEMENTAR   <<<<<
Implementa la estrategia del algoritmo Min-Max con podas Alpha-Beta, a partir del estado almacenado en N.
- A referencia al árbol de búsqueda adversaria.
- N referencia al nodo a partir del cual se construye el subárbol de búsqueda adversaria.
- ES_MAX indica si N representa un nodo MAX en el árbol de búsqueda adversaria.
- ALPHA y BETA indican sendos valores correspondientes a los nodos ancestros a N en el árbol de búsqueda A.
- JUGADOR_MAX y JUGADOR_MIN indican las fichas con las que juegan los respectivos jugadores.
**/
static void crear_sucesores_min_max(tArbol a, tNodo n, int es_max, int alpha, int beta, int jugador_max, int jugador_min){
    tEstado estado = a_recuperar(a,n);
    tEstado hijoEstado;
    tPosicion posActual;
    tLista sucesores;
    int utilidad=valor_utilidad(estado,jugador_max);
    int mayor_valor_sucesor;

    if(utilidad!=IA_NO_TERMINO){
        estado->utilidad=utilidad;
    }
    else {
        if (es_max) {

            mayor_valor_sucesor = IA_INFINITO_NEG;//se hace para calcular alpha
            sucesores = estados_sucesores(estado, jugador_max);
            posActual = l_primera(sucesores);

            while (posActual != NULL) {
                //Incializo el estado hijo
                hijoEstado = l_recuperar(sucesores, posActual);
                hijoEstado->utilidad = valor_utilidad(hijoEstado, jugador_max);
                //Inserto el estado hijo
                a_insertar(a, n, NULL, hijoEstado);
                //llamo recursivamente con el nuevo hijo, y cambio al estado a min con 0
                crear_sucesores_min_max(a, l_recuperar(a_hijos(a, n), l_ultima(a_hijos(a, n))), 0, alpha, beta,
                                        jugador_max, jugador_min);
                //se calcula el valor de alpha
                mayor_valor_sucesor=MAX(mayor_valor_sucesor,hijoEstado->utilidad);
                alpha = MAX(alpha, mayor_valor_sucesor);
                estado->utilidad = alpha;
                if (beta <= alpha) {
                    break;
                }
                posActual = posActual == l_ultima(sucesores) ? NULL : l_siguiente(sucesores, posActual);
            }

        } else {

            mayor_valor_sucesor = IA_INFINITO_POS;//se hace para calcular beta
            sucesores = estados_sucesores(estado, jugador_min);
            posActual = l_primera(sucesores);

            while (posActual != NULL) {
                //inicializo el estado hijo
                hijoEstado = l_recuperar(sucesores, posActual);
                hijoEstado->utilidad = valor_utilidad(hijoEstado, jugador_min);
                //inserto el estado hijo
                a_insertar(a, n, NULL, hijoEstado);
                //llamo recursivameente con el nuevo hijo y cambio al estado max con 1
                crear_sucesores_min_max(a, l_recuperar(a_hijos(a, n), l_ultima(a_hijos(a, n))), 1, alpha, beta,
                                        jugador_max, jugador_min);
                //se calcula el valor para beta
                mayor_valor_sucesor=MIN(mayor_valor_sucesor,hijoEstado->utilidad);
                beta = MIN(beta, mayor_valor_sucesor);
                estado->utilidad = beta;
                if (beta <= alpha) {
                    break;
                }
                posActual = posActual == l_ultima(sucesores) ? NULL : l_siguiente(sucesores, posActual);
            }


        }
    }
}

/**
Computa el valor de utilidad correspondiente al estado E, y la ficha correspondiente al JUGADOR_MAX, retornado:
- IA_GANA_MAX si el estado E refleja una jugada en el que el JUGADOR_MAX ganó la partida.
- IA_EMPATA_MAX si el estado E refleja una jugada en el que el JUGADOR_MAX empató la partida.
- IA_PIERDE_MAX si el estado E refleja una jugada en el que el JUGADOR_MAX perdió la partida.
- IA_NO_TERMINO en caso contrario.
**/
static int valor_utilidad(tEstado e, int jugador_max){
    int utilidad= IA_NO_TERMINO;//Flag que indica si la utilidad del estado actual
    int objetivoMAX;//Este resultado se dara en el caso de que gane MAX
    int objetivoMIN;//Este resultado se dara en el caso de que gane MIN
    int espaciosDisponibles= 0;//Indica si todavia hay espacion disponibles para jugar en el tablero
    int hayDisponibles= 0;//Flag para indicar que hay espacios disponibles

    if(jugador_max==PART_JUGADOR_1){
        objetivoMAX= 1; objetivoMIN= 8;
    }
    else{
        objetivoMAX= 8; objetivoMIN= 1;
    }

    int aux_d1= 1; int aux_d2= 1;
    for(int i=0; i<3 && utilidad==IA_NO_TERMINO; i++){
        int aux_i= 1; int aux_j= 1;
        aux_d1 *= e->grilla[i][i];  //Calcula la diagonal (0,0) (1,1) (2,2)
        aux_d2 *= e->grilla[i][2-i];//Calcula la diagonal (0,2) (1,1) (2,0)

        for(int j=0; j<3; j++){
            aux_i *= e->grilla[i][j];
            aux_j *= e->grilla[j][i];
        }

        if(aux_i== objetivoMAX || aux_j== objetivoMAX){
            utilidad= IA_GANA_MAX;//Victoria de MAX
        }
        else if(aux_i== objetivoMIN || aux_j== objetivoMIN){
            utilidad= IA_PIERDE_MAX;//Victoria de MIN
        }
        else if(aux_i==espaciosDisponibles || aux_j==espaciosDisponibles){//Si todavia hay casillas disponibles
            utilidad= IA_NO_TERMINO;
            hayDisponibles= 1;
        }
        else{//Si las filas y columnas estan llenas y ningun jugador gano todavia
           if(!hayDisponibles && i==2) {
               utilidad = IA_EMPATA_MAX;//Empate
           }
        }

    }
    if(aux_d1==objetivoMAX || aux_d2==objetivoMAX){
        utilidad= IA_GANA_MAX;//Victoria de MAX
    }
    else if(aux_d1==objetivoMIN || aux_d2==objetivoMIN){
        utilidad= IA_PIERDE_MAX;//Victoria de MIN
    }

    return utilidad;
}

/**
>>>>>  A IMPLEMENTAR   <<<<<
Computa y retorna una lista con aquellos estados que representan estados sucesores al estado E.
Un estado sucesor corresponde a la clonación del estado E, junto con la incorporación de un nuevo movimiento
realizado por el jugador cuya ficha es FICHA_JUGADOR por sobre una posición que se encuentra libre en el estado E.
La lista de estados sucesores se debe ordenar de forma aleatoria, de forma tal que una doble invocación de la función
estados_sucesores(estado, ficha) retornaría dos listas L1 y L2 tal que:
- L1 y L2 tienen exactamente los mismos estados sucesores de ESTADO a partir de jugar FICHA.
- El orden de los estado en L1 posiblemente sea diferente al orden de los estados en L2.
**/
static tLista estados_sucesores(tEstado e, int ficha_jugador){
    tEstado toCreate;
    tLista list;
    tPosicion actual, fin;
    crear_lista(&list);
    int pos, size,i,j;
    srand(time(NULL));

    size = 0;
    for (i = 0; i < 3; i++) {
        for ( j = 0; j < 3; j++) {
            if (e->grilla[i][j] == 0) {
                size++;
                toCreate = clonar_estado(e);

                toCreate->grilla[i][j] = ficha_jugador;

                pos = rand() % size;
                actual = l_primera(list);
                fin = l_fin(list);

                while(pos != 0 && actual != fin) {
                    actual = l_siguiente(list, actual);
                    pos--;
                }

                l_insertar(list, actual, toCreate);
            }
        }
    }

    return list;
}

/**
>>>>>  A IMPLEMENTAR   <<<<<
Inicializa y retorna un nuevo estado que resulta de la clonación del estado E.
Para esto copia en el estado a retornar los valores actuales de la grilla del estado E, como su valor
de utilidad.
**/
static tEstado clonar_estado(tEstado e){
    int i,j;
    tEstado estadoN=(tEstado)malloc(sizeof(struct estado));
    if(estadoN==NULL){
        exit(PART_ERROR_MEMORIA);
    }
    for(i=0;i<3;i++){
        for(j=0;j<3;j++){
            estadoN->grilla[i][j]=e->grilla[i][j];
        }
    }
    estadoN->utilidad=e->utilidad;

    return estadoN;
}

/**
Computa la diferencia existente entre dos estados.
Se asume que entre ambos existe sólo una posición en el que la ficha del estado anterior y nuevo difiere.
La posición en la que los estados difiere, es retornada en los parámetros *X e *Y.
**/
static void diferencia_estados(tEstado anterior, tEstado nuevo, int * x, int * y){
    int i,j, hallado = 0;

    for(i=0; i<3 && !hallado; i++){
        for(j=0; j<3 && !hallado; j++){
            if(anterior->grilla[i][j] != nuevo->grilla[i][j]){
                *x= i;
                *y= j;
                hallado= 1;
            }
        }
    }
}


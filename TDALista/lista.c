#include <stdio.h>
#include <stdlib.h>

#include "lista.h"


/**
 Inicializa una lista vacia.
 Una referencia a la lista creada es referenciada en *L.
**/
void crear_lista(tLista * l) {
    *l= (tLista)malloc(sizeof(struct celda));
    if((*l)==NULL){
        exit(LST_ERROR_MEMORIA);
    }

    (*l)->siguiente= NULL;
    (*l)->elemento= NULL;
}

/**
 Inserta el elemento E, en la posicion P, en L.
 Con L = A,B,C,D y la posicion P direccionando C, luego:
 L' = A,B,E,C,D
**/
void l_insertar(tLista l, tPosicion p, tElemento e) {
    tPosicion nuevo;

    nuevo=(tPosicion)malloc(sizeof(struct celda));
    if(nuevo==NULL){
        exit(LST_ERROR_MEMORIA);
    }

    nuevo->siguiente= p->siguiente;
    nuevo->elemento= e;
    p->siguiente= nuevo;
}

/**
 Elimina el nodo que se encuentra en la posicion P de L.
 El elemento almacenado en la posicion P es eliminado mediante la funcion fEliminar parametrizada.
 Si P es fin(L), finaliza indicando LST_POSICION_INVALIDA.
**/
extern void l_eliminar(tLista l, tPosicion p, void (*fEliminar)(tElemento)){
    if(p==NULL){
        exit(LST_POSICION_INVALIDA);
    }

    tPosicion aEliminar= p->siguiente;

    p->siguiente= aEliminar->siguiente;
    aEliminar->siguiente= NULL;
    fEliminar(aEliminar->elemento);
    free(aEliminar);
 }

 void l_destruirAux(tPosicion aux, void (*fEliminar)(tElemento)){
    if(aux->siguiente==NULL){
        fEliminar(aux->elemento);
        free(aux);
    }
    else{
        l_destruirAux(aux->siguiente,fEliminar);
        aux->siguiente=NULL;
        fEliminar(aux->elemento);
        free(aux);
    }

 }

/**
 Destruye la lista L, elimininando cada una de sus posiciones. Los elementos almacenados en las posiciones son eliminados mediante la funcion fEliminar parametrizada.
**/
 void l_destruir(tLista * l, void (*fEliminar)(tElemento)){
    tPosicion aux= *l;
    if(aux->elemento!=NULL){//Si la lista no esta vacia
        l_destruirAux(aux,fEliminar);
    }
    else{
        free(aux);
    }
    (*l)=NULL;
 }


/**
 Recupera y retorna el elemento en la posicion P.
 Si P es fin(L), finaliza indicando LST_POSICION_INVALIDA.
**/
tElemento l_recuperar(tLista l, tPosicion p){
    if(p==NULL||p->siguiente==NULL)
        exit(LST_POSICION_INVALIDA);

    return p->siguiente->elemento;
}

/**
 Recupera y retorna la primera posicion de L.
 Si L es vacia, primera(L) = ultima(L) = fin(L).
**/
tPosicion l_primera(tLista l){

    if(l==NULL)
        exit(LST_ELEMENTO_NULO);

     return l;
}

/**
 Recupera y retorna la posicion siguiente a P en L.
 Si P es fin(L), finaliza indicando LST_NO_EXISTE_SIGUIENTE.
**/
tPosicion l_siguiente(tLista l, tPosicion p){
    if(p==NULL)
        exit(LST_POSICION_INVALIDA);
    if(p->siguiente==NULL)
        exit(LST_NO_EXISTE_ANTERIOR);
    return (p->siguiente);
}

/**
 Recupera y retorna la posicion anterior a P en L.
 Si P es primera(L), finaliza indicando LST_NO_EXISTE_ANTERIOR.
**/
tPosicion l_anterior(tLista l, tPosicion p){
    tPosicion aux= l;

     if(p == aux||aux->siguiente==NULL)
        exit(LST_NO_EXISTE_ANTERIOR);

    while((aux->siguiente)!=p && (aux->siguiente)!=NULL)
        aux= aux->siguiente;

    return aux;
}

/**
 Recupera y retorna la ultima posicion de L.
 Si L es vacia, primera(L) = ultima(L) = fin(L).
**/
tPosicion l_ultima(tLista l){
     tPosicion aux=l;
     while(aux->siguiente->siguiente!=NULL)
        aux= aux->siguiente;

     return aux;
}

/**
 Recupera y retorna la posicion fin de L.
 Si L es vacia, primera(L) = ultima(L) = fin(L).
**/
tPosicion l_fin(tLista l){
  tPosicion aux=l;
     while(aux->siguiente!=NULL)
        aux= aux->siguiente;

     return aux;
}

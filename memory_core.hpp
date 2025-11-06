#pragma once
#include <iostream>
#include <vector>
#include <cstdint>   


struct FreeBlock {
    size_t start;       // índice inicial del hueco
    size_t size;        // tamaño en bytes
    FreeBlock* next;    // siguiente hueco libre
    FreeBlock(size_t s, size_t sz) : start(s), size(sz), next(nullptr) {}
};

//Lista enlazada de huecos libres
// gestion de memoria dinamica para aceder de manera mas facil y eficiente
class FreeList {
private:
    FreeBlock* head = nullptr; //Apunta al primer huevo libre

public:
    ~FreeList();
    void clear();
    void pushHoleSortedAndCoalesce(size_t start, size_t size);
    void resetToSingleHole(size_t totalSize);
    void printFreeList() const;
    FreeBlock* getHead() const { return head; }

    /**
     * Busca el hueco en donde insertar un nuevo proceso según algoritmo primer ajuste.
     * Retorna el primer hueco en el que cabe el proceso a insertar.
     * @param size Tamaño en bytes del proceso a insertar
     * @param prevOut Puntero que almacenará el nodo previo al hueco encontrado, para poder manejar la lista
     * @return El hueco en que insertar el proceso según primer ajuste. Si no encontró ninguno, devuelve null
    */
    FreeBlock* findFirstFit(size_t size, FreeBlock*& prevOut);

    /**
     * Busca el hueco en donde insertar un nuevo proceso según algoritmo mejor ajuste.
     * Retorna el hueco con el menor espacio en donde cabe el proceso a insertar.
     * @param size Tamaño en bytes del proceso a insertar
     * @param prevOut Puntero que almacenará el nodo previo al hueco encontrado, para poder manejar la lista
     * @return El hueco en que insertar el proceso según mejor ajuste. Si no encontró ninguno, devuelve null
    */
    FreeBlock* findBestFit(size_t size, FreeBlock*& prevOut);
};


class MemoryCore {
private:
    std::vector<uint8_t> ram; // arreglo principal (memoria física)
    //cada byte es representado por un uint8_t
    FreeList freeList;          // huecos libres
    size_t totalSize = 0;     // tamaño total de la RAM en bytes

public:
    void init(size_t bytes);    // un único hueco [0, bytes)
    void reset();               
    void printMap(size_t columns = 64) const;

    const FreeList& holes() const { return freeList; }
    FreeList& holes() { return freeList; }
};

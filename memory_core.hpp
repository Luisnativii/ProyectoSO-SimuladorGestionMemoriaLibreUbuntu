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
    /** Limpia la lista de espacios libres*/
    void clear();
    /**
     * Inserta un nuevo hueco en la lista, y si encuentra huecos adyacentes,
     * los fusiona para formar un solo hueco.
     * @param start Lugar de memoria en el que insertar el hueco
     * @param size Tamaño del hueco a insertar
     */
    void pushHoleSortedAndCoalesce(size_t start, size_t size);
    /** Resetea la lista para crear un solo hueco del tamaño indicado */
    void resetToSingleHole(size_t totalSize);
    /** Imprime la lista de espacios vacíos */
    void printFreeList() const;
    /** Retorna la punta de la lista de huecos vacíos */
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

    // NUEVO: reservar desde la lista de huecos (primer/mejor ajuste)
    bool allocate(size_t size, size_t& outStart, bool bestFit = false);

    // NUEVO: liberar región y fusionar con adyacentes
    void release(size_t start, size_t size);
};

// Representa un proceso cargado en memoria
struct Process {
    std::string name;    // Identificador del proceso (ej: "P1", "Chrome")
    size_t start;        // Dirección de inicio en memoria
    size_t size;         // Tamaño en bytes
    
    Process(const std::string& n, size_t s, size_t sz) 
        : name(n), start(s), size(sz) {}
};


class MemoryCore {
private:
    std::vector<uint8_t> ram; // arreglo principal (memoria física)
    //cada byte es representado por un uint8_t
    FreeList freeList;          // huecos libres
    size_t totalSize = 0;     // tamaño total de la RAM en bytes

    std::vector<Process> processes; //Llevará la lista de procesos cargados en memoria

public:
    void init(size_t bytes);    // un único hueco [0, bytes)
    void reset();               
    void printMap(size_t columns = 64) const;

    const FreeList& holes() const { return freeList; }
    FreeList& holes() { return freeList; }

    bool allocate(size_t size, size_t& outAddr, bool bestFit = false);
    bool release(size_t start, size_t size);

    public:
    //Reserva memoria y aloja el proceso
    bool allocateProcess(const std::string& name, size_t size, bool bestFit = false); 
    // Busca el proceso por nombre y lo libera de memoria
    bool releaseProcess(const std::string& name);
    //Método auxiliar para buscar un proceso por nombre
    Process* findProcess(const std::string& name);
    //Muestra una tabla con todos los procesos en memoria
    void printProcesses() const;

};

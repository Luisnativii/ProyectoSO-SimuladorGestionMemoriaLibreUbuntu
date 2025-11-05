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

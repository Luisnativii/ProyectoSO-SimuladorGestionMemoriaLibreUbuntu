#include "memory_core.hpp"
#include <iostream>

int main() {
    MemoryCore mem;

    
    const size_t MEM_SIZE = 128; // ajustable
    mem.init(MEM_SIZE);

    std::cout << "=== Sistema Inicializado ===\n";
    mem.holes().printFreeList();
    mem.printMap();

    //Datos de prueba quemados 
    std::cout << "\n=== Prueba de Fusion ===\n";
    mem.holes().clear();
    mem.holes().pushHoleSortedAndCoalesce(0, 20);
    mem.holes().pushHoleSortedAndCoalesce(20, 30); // fusiona → [0,50)
    mem.holes().pushHoleSortedAndCoalesce(70, 10);
    mem.holes().pushHoleSortedAndCoalesce(80, 10); // fusiona → [70,20)
    mem.holes().printFreeList();

    std::cout << "\n=== Reset al estado inicial ===\n";
    mem.reset();
    mem.holes().printFreeList();

    return 0;
}

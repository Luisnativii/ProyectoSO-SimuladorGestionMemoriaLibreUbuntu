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

    // Ejemplo de uso algoritmos 
    std::cout << "\n=== Prueba de primer ajuste ===\n";
    mem.holes().clear();
    mem.holes().pushHoleSortedAndCoalesce(0, 30);
    mem.holes().pushHoleSortedAndCoalesce(70, 20);
    mem.holes().printFreeList();

    // Busca el primer ajuste para un espacio de 10 bytes
    FreeBlock* prevFirstFind = nullptr;
    FreeBlock* firstFind = mem.holes().findFirstFit(10, prevFirstFind);
    std::cout << "\n Hueco encontrado en "<< firstFind->start << ". Con tamano " << firstFind->size <<"\n";
    
    std::cout << "\n=== Prueba de mejor ajuste ===\n";
    // Busca el mejor ajuste para un espacio de 10 bytes
    FreeBlock* prevBestFind = nullptr;
    FreeBlock* bestFind = mem.holes().findBestFit(10, prevBestFind);
    std::cout << "\n Hueco encontrado en "<< bestFind->start << ". Con tamano " << bestFind->size <<"\n";

    std::cout << "\n=== Ejemplo de allocate/release ===\n";
    mem.reset(); // un único hueco libre [0, MEM_SIZE)

    // Reservas con primer ajuste
    size_t p1, p2, p3;
    mem.allocate(30, p1);  // [0,30) ocupado
    mem.allocate(20, p2);  // [30,50) ocupado
    mem.allocate(10, p3);  // [50,60) ocupado
    std::cout << "Asignados: p1=" << p1 << " (30), p2=" << p2 << " (20), p3=" << p3 << " (10)\n";
    mem.holes().printFreeList();
    mem.printMap();

    // Liberar el bloque del medio para crear dos huecos
    mem.release(p2, 20);   // crea huecos [30,50) y [60,128)
    std::cout << "\nTras liberar p2 (20 bytes desde " << p2 << ")\n";
    mem.holes().printFreeList();
    mem.printMap();

    // Mejor ajuste: debe elegir el hueco más pequeño donde quepa (15 -> [30,50))
    size_t p4;
    if (mem.allocate(15, p4, true)) {
        std::cout << "\nBest fit: reservado p4=" << p4 << " (15)\n";
    } else {
        std::cout << "\nBest fit: no hay espacio\n";
    }
    mem.holes().printFreeList();
    mem.printMap();

    // Liberar todo y observar la fusión (coalescing)
    mem.release(p1, 30);
    mem.release(p3, 10);
    mem.release(p4, 15);
    std::cout << "\nTras liberar p1, p3 y p4 (coalescing)\n";
    mem.holes().printFreeList();
    mem.printMap();

    return 0;
}

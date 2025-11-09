#include "memory_core.hpp"
#include "command_parser.hpp"
#include "demos.hpp"
#include <iostream>
#include <string>

int main() {
    // Inicializar memoria
    MemoryCore mem;
    const size_t MEM_SIZE = 128;
    mem.init(MEM_SIZE);

    // Menú principal
    std::cout << "\n";
    std::cout << "╔══════════════════════════════════════════════════════════════╗\n";
    std::cout << "║     SIMULADOR DE GESTIÓN DE MEMORIA FÍSICA                   ║\n";
    std::cout << "╚══════════════════════════════════════════════════════════════╝\n";
    std::cout << "\n¿Qué deseas hacer?\n";
    std::cout << "  1. Ejecutar DEMOSTRACIONES automáticas\n";
    std::cout << "  2. Modo INTERACTIVO\n";
    std::cout << "\nSelecciona (1 o 2): ";
    
    int choice;
    std::cin >> choice;
    std::cin.ignore();  // Limpiar buffer
    
    if (choice == 1) {
        // Ejecutar todas las demos
        DemoRunner demo(mem);
        demo.runAllDemos();
    } else {
        // Modo interactivo
        CommandParser parser(mem);
        parser.showWelcome();
        
        std::cout << "Estado inicial de la memoria:\n";
        mem.printMap(32);
        
        std::string line;
        while (true) {
            std::cout << "memoria> ";
            
            if (!std::getline(std::cin, line)) {
                break;
            }
            
            if (!parser.processCommand(line)) {
                break;
            }
        }
    }
    
    return 0;
}
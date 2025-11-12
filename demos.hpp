#pragma once
#include "memory_core.hpp"
#include <iostream>
#include <thread>
#include <chrono>

class DemoRunner {
private:
    // Referencia al núcleo de memoria que se usará en las demostraciones
    MemoryCore& memory;

    // Pausa la ejecución por un tiempo determinado (por defecto 1.5 segundos)
    void pause(int milliseconds = 1500) {
        std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds));
    }

    // Espera a que el usuario presione ENTER antes de continuar
    void waitForUser() {
        std::cout << "\n[Presiona ENTER para continuar...]";
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << "\n";
    }

    // Muestra un encabezado visual con el texto descriptivo del paso actual
    void showStep(const std::string& description) {
        std::cout << "\n" << std::string(70, '=') << "\n";
        std::cout << "  " << description << "\n";
        std::cout << std::string(70, '=') << "\n";
    }

public:
    // Constructor: recibe la referencia al objeto principal de memoria
    DemoRunner(MemoryCore& mem) : memory(mem) {}

    // Ejecuta una demostración básica de asignación y liberación
    void demo1_SecuenciaBasica();

    // Ejecuta una demostración sobre la fusión de huecos libres
    void demo2_Fusion();

    // Compara los algoritmos de asignación First Fit y Best Fit
    void demo3_ComparacionAlgoritmos();

    // Ejecuta todas las demostraciones en secuencia
    void runAllDemos();
};

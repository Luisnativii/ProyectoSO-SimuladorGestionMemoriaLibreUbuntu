#pragma once
#include "memory_core.hpp"
#include <iostream>
#include <thread>
#include <chrono>

class DemoRunner {
private:
    MemoryCore& memory;
    
    // Pausa para que el usuario pueda ver cada paso
    void pause(int milliseconds = 1500) {
        std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds));
    }
    
    void waitForUser() {
        std::cout << "\n[Presiona ENTER para continuar...]";
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << "\n";
    }
    
    void showStep(const std::string& description) {
        std::cout << "\n" << std::string(70, '=') << "\n";
        std::cout << "  " << description << "\n";
        std::cout << std::string(70, '=') << "\n";
    }
    
public:
    DemoRunner(MemoryCore& mem) : memory(mem) {}
    
    void demo1_SecuenciaBasica();
    void demo2_Fusion();
    void demo3_ComparacionAlgoritmos();
    void runAllDemos();
};
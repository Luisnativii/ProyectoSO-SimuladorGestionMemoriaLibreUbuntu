#pragma once
#include "memory_core.hpp"
#include <string>
#include <sstream>

class CommandParser {
private:
    MemoryCore& memory;
    bool useBestFit;  // true = mejor ajuste, false = primer ajuste
    
    // Métodos auxiliares para procesar comandos
    void cmdSolicitar(std::istringstream& iss);
    void cmdLiberar(std::istringstream& iss);
    void cmdMapa();
    void cmdHuecos();
    void cmdProcesos();
    void cmdAlgoritmo(std::istringstream& iss);
    void cmdReset();
    void cmdAyuda();
    
public:
    CommandParser(MemoryCore& mem) : memory(mem), useBestFit(false) {}
    
    // Procesa un comando y retorna true si debe continuar, false si debe salir
    bool processCommand(const std::string& line);
    
    // Muestra el menú de ayuda
    void showWelcome();
};
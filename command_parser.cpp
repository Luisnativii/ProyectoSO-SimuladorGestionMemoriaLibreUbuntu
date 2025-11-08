#include "command_parser.hpp"
#include <iostream>
#include <algorithm>

void CommandParser::showWelcome() {
    std::cout << "\n";
    std::cout << "╔════════════════════════════════════════════════════════════╗\n";
    std::cout << "║     SIMULADOR DE GESTIÓN DE MEMORIA FÍSICA                ║\n";
    std::cout << "╚════════════════════════════════════════════════════════════╝\n";
    std::cout << "\nAlgoritmo actual: " << (useBestFit ? "MEJOR AJUSTE" : "PRIMER AJUSTE") << "\n";
    std::cout << "\nComandos disponibles:\n";
    std::cout << "  solicitar <tamaño> <nombre>  - Asignar memoria a un proceso\n";
    std::cout << "  liberar <nombre>             - Liberar memoria de un proceso\n";
    std::cout << "  mapa                         - Mostrar mapa visual de memoria\n";
    std::cout << "  huecos                       - Mostrar lista de huecos libres\n";
    std::cout << "  procesos                     - Mostrar procesos activos\n";
    std::cout << "  algoritmo <first|best>       - Cambiar algoritmo de asignación\n";
    std::cout << "  reset                        - Reiniciar memoria\n";
    std::cout << "  ayuda                        - Mostrar esta ayuda\n";
    std::cout << "  salir                        - Salir del programa\n";
    std::cout << "\n";
}

bool CommandParser::processCommand(const std::string& line) {
    if (line.empty()) return true;
    
    std::istringstream iss(line);
    std::string cmd;
    iss >> cmd;
    
    // Convertir a minúsculas para facilitar comparación
    std::transform(cmd.begin(), cmd.end(), cmd.begin(), ::tolower);
    
    if (cmd == "solicitar") {
        cmdSolicitar(iss);
    } else if (cmd == "liberar") {
        cmdLiberar(iss);
    } else if (cmd == "mapa") {
        cmdMapa();
    } else if (cmd == "huecos") {
        cmdHuecos();
    } else if (cmd == "procesos") {
        cmdProcesos();
    } else if (cmd == "algoritmo") {
        cmdAlgoritmo(iss);
    } else if (cmd == "reset") {
        cmdReset();
    } else if (cmd == "ayuda" || cmd == "help") {
        cmdAyuda();
    } else if (cmd == "salir" || cmd == "exit" || cmd == "quit") {
        std::cout << "\n¡Hasta luego!\n\n";
        return false;  // Señal para terminar el programa
    } else {
        std::cout << " Comando desconocido: '" << cmd << "'. Escribe 'ayuda' para ver comandos.\n\n";
    }
    
    return true;
}

void CommandParser::cmdSolicitar(std::istringstream& iss) {
    size_t size;
    std::string name;
    
    if (!(iss >> size >> name)) {
        std::cout << " Uso: solicitar <tamaño> <nombre>\n";
        std::cout << "   Ejemplo: solicitar 30 Chrome\n\n";
        return;
    }
    
    std::cout << "\n→ Solicitando " << size << " bytes para proceso '" << name << "' ";
    std::cout << "(" << (useBestFit ? "Mejor Ajuste" : "Primer Ajuste") << ")...\n";
    
    if (memory.allocateProcess(name, size, useBestFit)) {
        std::cout << "\n";
    } else {
        std::cout << "\n";
    }
}

void CommandParser::cmdLiberar(std::istringstream& iss) {
    std::string name;
    
    if (!(iss >> name)) {
        std::cout << " Uso: liberar <nombre>\n";
        std::cout << "   Ejemplo: liberar Chrome\n\n";
        return;
    }
    
    std::cout << "\n→ Liberando proceso '" << name << "'...\n";
    memory.releaseProcess(name);
    std::cout << "\n";
}

void CommandParser::cmdMapa() {
    memory.printMap(32);  // 32 bytes por fila
}

void CommandParser::cmdHuecos() {
    std::cout << "\n";
    memory.holes().printFreeList();
}

void CommandParser::cmdProcesos() {
    memory.printProcesses();
}

void CommandParser::cmdAlgoritmo(std::istringstream& iss) {
    std::string algo;
    
    if (!(iss >> algo)) {
        std::cout << " Uso: algoritmo <first|best>\n";
        std::cout << "   first = Primer Ajuste\n";
        std::cout << "   best  = Mejor Ajuste\n\n";
        return;
    }
    
    std::transform(algo.begin(), algo.end(), algo.begin(), ::tolower);
    
    if (algo == "first" || algo == "primer" || algo == "primero") {
        useBestFit = false;
        std::cout << "\n✓ Algoritmo cambiado a: PRIMER AJUSTE\n\n";
    } else if (algo == "best" || algo == "mejor") {
        useBestFit = true;
        std::cout << "\n✓ Algoritmo cambiado a: MEJOR AJUSTE\n\n";
    } else {
        std::cout << " Algoritmo desconocido. Use 'first' o 'best'.\n\n";
    }
}

void CommandParser::cmdReset() {
    std::cout << "\n→ Reiniciando memoria...\n";
    memory.reset();
    std::cout << "✓ Memoria reiniciada. Toda la memoria está libre.\n\n";
}

void CommandParser::cmdAyuda() {
    showWelcome();
}
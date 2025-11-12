#pragma once
#include "memory_core.hpp"
#include <string>
#include <sstream>

class CommandParser {
private:
    // Referencia al núcleo de memoria donde se ejecutarán las operaciones
    MemoryCore& memory;

    // Indica qué algoritmo de asignación se está usando:
    // true = Best Fit (mejor ajuste), false = First Fit (primer ajuste)
    bool useBestFit;

    // Métodos internos para manejar los distintos comandos del usuario
    void cmdSolicitar(std::istringstream& iss);  // Solicitar memoria para un proceso
    void cmdLiberar(std::istringstream& iss);    // Liberar memoria de un proceso
    void cmdMapa();                              // Mostrar el mapa de memoria
    void cmdHuecos();                            // Mostrar los huecos libres
    void cmdProcesos();                          // Mostrar los procesos activos
    void cmdAlgoritmo(std::istringstream& iss);  // Cambiar el algoritmo de ajuste
    void cmdReset();                             // Reiniciar el estado de la memoria
    void cmdAyuda();                             // Mostrar los comandos disponibles

public:
    // Constructor: recibe la referencia al objeto de memoria y
    // establece por defecto el algoritmo First Fit
    CommandParser(MemoryCore& mem) : memory(mem), useBestFit(false) {}

    // Procesa un comando de texto introducido por el usuario.
    // Devuelve true para continuar o false si el comando indica salir.
    bool processCommand(const std::string& line);

    // Muestra el mensaje de bienvenida y la lista de comandos disponibles.
    void showWelcome();
};

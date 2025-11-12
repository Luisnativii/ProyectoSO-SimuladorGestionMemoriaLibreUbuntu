#include "memory_core.hpp"
#include <iomanip>
#include <algorithm> // std::min

FreeList::~FreeList() { clear(); }

//limpiar la lista de huecos libres o liberar memoria
void FreeList::clear() {
    // Recorrer la lista
    while (head) {
        // Almacenar el nodo actual
        FreeBlock* tmp = head;
        head = head->next;
        // Elimina el nodo actual
        delete tmp;
    }
}

// Inserta un nuevo hueco y fusiona espacios adyacentes
void FreeList::pushHoleSortedAndCoalesce(size_t start, size_t size) {
    // Finalizar si se intenta insertar un espacio de tamaño 0
    if (size == 0) return;
    FreeBlock* node = new FreeBlock(start, size); // Nuevo hueco a insertar

    // Inserción ordenada por dirección
    // Si no hay punta o el nuevo nodo se encuentra antes que la punta
    if (!head || start < head->start) {
        node->next = head; // El nuevo nodo apuntará a la punta
        head = node; // La nueva punta de la lista es el nodo a insertar
    } else {
        // Inicializar puntero para recorrer lista
        FreeBlock* cur = head;
        // Recorrer la lista hasta encontrar la posición de inicio del nuevo nodo
        while (cur->next && cur->next->start < start) cur = cur->next;
        // Inserta nuevo hueco en la posición encontrada
        node->next = cur->next;
        cur->next = node;
    }

    // Fusión hacia adelante
    // Si el nuevo hueco tiene un hueco siguiente y este inicia donde termina el nuevo hueco
    if (node->next && node->start + node->size == node->next->start) {
        // Obtener el hueco a fusionar con el nuevo
        FreeBlock* nxt = node->next;
        // Actualizar el tamaño del nuevo nodo 
        node->size += nxt->size;
        // Actualizar enlace del nuevo nodo
        node->next = nxt->next;
        // Eliminar hueco fusionado
        delete nxt;
    }

    // Fusión hacia atrás
    // Si hay elementos antes del nuevo hueco
    if (head != node) {
        // Puntero para recorrer la lista
        FreeBlock* prev = head;
        // Encontrar el nodo anterior al nuevo hueco insertado
        while (prev && prev->next != node) prev = prev->next;
        // Si el hueco anterior termina donde empieza el nuevo hueco
        if (prev && prev->start + prev->size == node->start) {
            // Actualizar el tamaño del hueco anterior
            prev->size += node->size;
            // Actualizar enlace de nodo anterior
            prev->next = node->next;
            // Eliminar el hueco fusionado
            delete node;
        }
    }
}

//resetar la memoria al inicilizar 
void FreeList::resetToSingleHole(size_t totalSize) {
    // limpia la lista actual
    clear();
    // Crea una nueva lista con un solo hueco del tamaño indicado
    if (totalSize > 0) head = new FreeBlock(0, totalSize);
}

void FreeList::printFreeList() const {
    std::cout << "[Lista de Huecos Libres]\n";
    // Enviar mensaje si la lista de huecos está vacía
    if (!head) { std::cout << "  (vacía)\n\n"; return; }

    // Recorrer la lista de huecos, imprimiendo el inicio y tamaño de cada hueco
    for (FreeBlock* cur = head; cur; cur = cur->next) {
        std::cout << "  Inicio: " << std::setw(4) << cur->start
                  << "  Tamano: " << std::setw(4) << cur->size << " bytes\n";
    }
    std::cout << std::endl;
}

// Implementación del algoritmo de primer ajuste
FreeBlock* FreeList::findFirstFit(size_t size, FreeBlock*& prevOut) {
    // Punteros para recorrer la lista de espacios vacíos
    FreeBlock* prev = nullptr;
    FreeBlock* cur = head;

    // Recorre la lista de espacios vacíos
    while (cur) {
        // Si encuentra un hueco que cumpla con el tamaño
        if (cur->size >= size) {
            prevOut = prev;
            return cur;   // Retornar el primer hueco en el que cabe
        }
        // Actualiza punteros para recorrer lista
        prev = cur;
        cur = cur->next;
    }

    prevOut = nullptr;
    return nullptr; // No encontró hueco, retornar null
}

// Implementación de algoritmo de mejor ajuste
FreeBlock* FreeList::findBestFit(size_t size, FreeBlock*& prevOut) {
    // Punteros para recorrer la lista
    FreeBlock* prev = nullptr;
    FreeBlock* cur = head;

    FreeBlock* best = nullptr; // Puntero del mejor candidato encontrado
    FreeBlock* bestPrev = nullptr; // El nodo anterior al mejor candidato

    // Recorrer la lista de huecos
    while (cur) {
        // Si el proceso cabe en el hueco actual
        if (cur->size >= size) {
            // Si no está definido un mejor hueco o si el hueco actual es más pequeño que el mejor hueco hasta ahora
            if (!best || cur->size < best->size) {
                // Actualiza los punteros del mejor hueco
                best = cur;
                bestPrev = prev;
            }
        }
        // Actualiza punteros para recorrer lista
        prev = cur;
        cur = cur->next;
    }

    // Asigna la referencia del hueco anterior al mejor candidato
    prevOut = bestPrev;
    return best;  // Si no se encontró ningún hueco donde cabe, retorna null
}

// NUEVO: reservar desde la lista de huecos (divide o elimina el hueco)
bool FreeList::allocate(size_t size, size_t& outStart, bool bestFit) {
    outStart = static_cast<size_t>(-1);
    if (size == 0) return false;

    FreeBlock* prev = nullptr;
    FreeBlock* hole = bestFit ? findBestFit(size, prev) : findFirstFit(size, prev);
    if (!hole) return false;

    outStart = hole->start;

    if (hole->size == size) {
        // Caso ajuste exacto: quitar el nodo
        if (prev) prev->next = hole->next;
        else head = hole->next;
        delete hole;
    } else {
        // Dividir el hueco: reservar al inicio
        hole->start += size;
        hole->size  -= size;
    }
    return true;
}

// NUEVO: liberar región e insertar con coalescing
void FreeList::release(size_t start, size_t size) {
    if (size == 0) return;
    pushHoleSortedAndCoalesce(start, size);
}

//-------------------------- MemoryCore ------------------------------------------------
void MemoryCore::init(size_t bytes) {
    totalSize = bytes;
    ram.assign(totalSize, 0);              // 0 = libre (solo representativo)
    freeList.resetToSingleHole(totalSize); // un único hueco: toda la RAM
}

//Resetea la lista de huecos y limpia el registro de procesos
void MemoryCore::reset() {
    freeList.resetToSingleHole(ram.size());
    processes.clear();  //limpiar registro de procesos
    std::fill(ram.begin(), ram.end(), 0);  //limpiar ram
}

void MemoryCore::printMap(size_t columns) const {
    // Códigos ANSI para colores
    const std::string RESET = "\033[0m";
    const std::string GREEN = "\033[32m";  // Libre
    const std::string BLUE = "\033[34m";
    const std::string YELLOW = "\033[33m";
    const std::string RED = "\033[31m";
    const std::string CYAN = "\033[36m";
    const std::string MAGENTA = "\033[35m";
    
    std::vector<std::string> colors = {BLUE, YELLOW, RED, CYAN, MAGENTA};
    
    std::cout << "\n[Mapa de Memoria] (" << ram.size() << " bytes, " 
              << columns << " bytes por fila)\n";
    std::cout << std::string(columns + 10, '=') << "\n";
    
    std::vector<char> display(ram.size(), '.');
    std::vector<int> colorIndex(ram.size(), -1);  // -1 = libre (verde)
    
    // Asignar color a cada proceso
    int procIdx = 0;
    for (const auto& proc : processes) {
        char symbol = proc.name.empty() ? 'X' : proc.name[0];
        int color = procIdx % colors.size();
        
        const size_t end = std::min(ram.size(), proc.start + proc.size);
        for (size_t i = proc.start; i < end; ++i) {
            display[i] = symbol;
            colorIndex[i] = color;
        }
        procIdx++;
    }
    
    // Imprimir con colores
    for (size_t i = 0; i < ram.size(); ++i) {
        if (i % columns == 0) {
            std::cout << std::setw(4) << i << " | ";
        }
        
        // Aplicar color
        if (colorIndex[i] == -1) {
            std::cout << GREEN << display[i] << RESET;
        } else {
            std::cout << colors[colorIndex[i]] << display[i] << RESET;
        }
        
        if ((i + 1) % columns == 0) {
            std::cout << " | " << std::setw(4) << (i + 1) << "\n";
        }
    }
    
    if (ram.size() % columns != 0) {
        size_t remaining = columns - (ram.size() % columns);
        std::cout << std::string(remaining, ' ') 
                  << " | " << std::setw(4) << ram.size() << "\n";
    }
    
    std::cout << std::string(columns + 10, '=') << "\n";
    std::cout << GREEN << "'.' = libre" << RESET;
    
    procIdx = 0;
    for (const auto& proc : processes) {
        std::cout << ", " << colors[procIdx % colors.size()] 
                  << "'" << proc.name[0] << "' = " << proc.name << RESET;
        procIdx++;
    }
    std::cout << "\n\n";
}


bool MemoryCore::allocate(size_t size, size_t& outAddr, bool bestFit) {
    outAddr = static_cast<size_t>(-1);
    if (size == 0 || size > ram.size()) return false;

    if (!freeList.allocate(size, outAddr, bestFit)) return false;

    // Marcar región como ocupada (opcional, el mapa usa la free list)
    const size_t end = std::min(ram.size(), outAddr + size);
    for (size_t i = outAddr; i < end; ++i) ram[i] = 1;
    return true;
}

bool MemoryCore::release(size_t start, size_t size) {
    if (size == 0) return false;
    if (start >= ram.size()) return false;
    if (start + size > ram.size()) return false;

    for (size_t i = start; i < start + size; ++i) ram[i] = 0;
    freeList.release(start, size);
    return true;
}

//Implementaciones de los nuevos métodos para manejo de procesos

// Recorre el vector buscando coincidencia de nombres. Retorna puntero al proceso o nullptr.
Process* MemoryCore::findProcess(const std::string& name) {
    for (auto& p : processes) {
        if (p.name == name) return &p;
    }
    return nullptr;
}

// Primero valida que no exista duplicado, luego llama a la función allocate() existente, para reservar memoria 
//y finalmente registra el proceso en el vector.

bool MemoryCore::allocateProcess(const std::string& name, size_t size, bool bestFit) {
    if (findProcess(name)) {
        std::cout << "ERROR: El proceso '" << name << "' ya existe en memoria.\n";
        return false;
    }
    
    // Intentar asignar memoria usando la función existente
    size_t addr;
    if (!allocate(size, addr, bestFit)) {
        std::cout << "ERROR: No hay suficiente memoria para '" << name 
                  << "' (" << size << " bytes).\n";
        return false;
    }
    
    // Registrar el proceso
    processes.emplace_back(name, addr, size);
    std::cout << "✓ Proceso '" << name << "' asignado en dirección " 
              << addr << " (" << size << " bytes)\n";
    return true;
}

// Usa std::find_if para buscar por nombre, libera la memoria con release() existente, y borra del vector con erase().
bool MemoryCore::releaseProcess(const std::string& name) {
    // Buscar el proceso
    auto it = std::find_if(processes.begin(), processes.end(),
                           [&name](const Process& p) { return p.name == name; });
    
    if (it == processes.end()) {
        std::cout << "ERROR: El proceso '" << name << "' no existe en memoria.\n";
        return false;
    }
    
    // Liberar su memoria
    release(it->start, it->size);
    
    std::cout << "✓ Proceso '" << name << "' liberado (dirección " 
              << it->start << ", " << it->size << " bytes)\n";
    
    // Remover del registro
    processes.erase(it);
    return true;
}

//Imprime tabla formateada con todos los procesos.
void MemoryCore::printProcesses() const {
    std::cout << "\n[Procesos Activos]\n";
    if (processes.empty()) {
        std::cout << "  (ninguno)\n";
        return;
    }
    
    std::cout << std::setw(15) << "Nombre" 
              << std::setw(10) << "Inicio" 
              << std::setw(10) << "Tamaño" << "\n";
    std::cout << std::string(35, '-') << "\n";
    
    for (const auto& p : processes) {
        std::cout << std::setw(15) << p.name
                  << std::setw(10) << p.start
                  << std::setw(10) << p.size << " bytes\n";
    }
    std::cout << std::endl;
}
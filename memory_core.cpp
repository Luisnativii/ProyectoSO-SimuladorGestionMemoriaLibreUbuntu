#include "memory_core.hpp"
#include <iomanip>
#include <algorithm> // std::min

FreeList::~FreeList() { clear(); }

//limpair la lista de huecos libres o liberar memoria
void FreeList::clear() {
    while (head) {
        FreeBlock* tmp = head;
        head = head->next;
        delete tmp;
    }
}

void FreeList::pushHoleSortedAndCoalesce(size_t start, size_t size) {
    if (size == 0) return;
    FreeBlock* node = new FreeBlock(start, size);

    // Inserción ordenada por dirección
    if (!head || start < head->start) {
        node->next = head;
        head = node;
    } else {
        FreeBlock* cur = head;
        while (cur->next && cur->next->start < start) cur = cur->next;
        node->next = cur->next;
        cur->next = node;
    }

    // Fusión hacia adelante
    if (node->next && node->start + node->size == node->next->start) {
        FreeBlock* nxt = node->next;
        node->size += nxt->size;
        node->next = nxt->next;
        delete nxt;
    }

    // Fusión hacia atrás
    if (head != node) {
        FreeBlock* prev = head;
        while (prev && prev->next != node) prev = prev->next;
        if (prev && prev->start + prev->size == node->start) {
            prev->size += node->size;
            prev->next = node->next;
            delete node;
        }
    }
}

//resetar la memoria al inicilizar 
void FreeList::resetToSingleHole(size_t totalSize) {
    clear();
    if (totalSize > 0) head = new FreeBlock(0, totalSize);
}

void FreeList::printFreeList() const {
    std::cout << "[Lista de Huecos Libres]\n";
    if (!head) { std::cout << "  (vacía)\n\n"; return; }
    for (FreeBlock* cur = head; cur; cur = cur->next) {
        std::cout << "  Inicio: " << std::setw(4) << cur->start
                  << "  Tamano: " << std::setw(4) << cur->size << " bytes\n";
    }
    std::cout << std::endl;
}

FreeBlock* FreeList::findFirstFit(size_t size, FreeBlock*& prevOut) {
    FreeBlock* prev = nullptr;
    FreeBlock* cur = head;

    while (cur) {
        if (cur->size >= size) {
            prevOut = prev;
            return cur;   // Retornar el primer hueco en el que cabe
        }
        prev = cur;
        cur = cur->next;
    }

    prevOut = nullptr;
    return nullptr; // No encontró hueco, retornar null
}

FreeBlock* FreeList::findBestFit(size_t size, FreeBlock*& prevOut) {
    FreeBlock* prev = nullptr;
    FreeBlock* cur = head;

    FreeBlock* best = nullptr;
    FreeBlock* bestPrev = nullptr;

    while (cur) {
        // Buscar el hueco más pequeño donde cabe el proceso
        if (cur->size >= size) {
            if (!best || cur->size < best->size) {
                best = cur;
                bestPrev = prev;
            }
        }
        prev = cur;
        cur = cur->next;
    }

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
    std::cout << "[Mapa de Memoria] (" << ram.size() << " bytes)\n";
    std::vector<bool> isFree(ram.size(), false);
    for (FreeBlock* cur = freeList.getHead(); cur; cur = cur->next) {
        const size_t end = std::min(ram.size(), cur->start + cur->size);
        for (size_t i = cur->start; i < end; ++i) isFree[i] = true;
    }
    for (size_t i = 0; i < ram.size(); ++i) {
        std::cout << (isFree[i] ? '.' : '#');
        if ((i + 1) % columns == 0) std::cout << '\n';
    }
    if (ram.size() % columns != 0) std::cout << '\n';
    std::cout << std::endl;
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
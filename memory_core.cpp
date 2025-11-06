#include "memory_core.hpp"
#include <iomanip>

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

//-------------------------- MemoryCore ------------------------------------------------
void MemoryCore::init(size_t bytes) {
    totalSize = bytes;
    ram.assign(totalSize, 0);              // 0 = libre (solo representativo)
    freeList.resetToSingleHole(totalSize); // un único hueco: toda la RAM
}

void MemoryCore::reset() {
    freeList.resetToSingleHole(ram.size());
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

#include "demos.hpp"

// Ejecuta todas las demos
void DemoRunner::runAllDemos() {
    std::cout << "\n";
    std::cout << "╔══════════════════════════════════════════════════════════════╗\n";
    std::cout << "║            DEMOSTRACIONES DEL SIMULADOR                      ║\n";
    std::cout << "╚══════════════════════════════════════════════════════════════╝\n";
    std::cout << "\nEste programa ejecutará 3 demostraciones:\n";
    std::cout << "  1. Secuencia de solicitudes y liberaciones\n";
    std::cout << "  2. Demostración de fusión de huecos\n";
    std::cout << "  3. Comparación: Primer Ajuste vs Mejor Ajuste\n";
    
    waitForUser();
    
    demo1_SecuenciaBasica();
    demo2_Fusion();
    demo3_ComparacionAlgoritmos();
    
    std::cout << "\n";
    std::cout << "╔══════════════════════════════════════════════════════════════╗\n";
    std::cout << "║            DEMOSTRACIONES COMPLETADAS                        ║\n";
    std::cout << "╚══════════════════════════════════════════════════════════════╝\n\n";
}

// ============================================================================
// DEMO 1: Secuencia básica de solicitudes y liberaciones
// ============================================================================
void DemoRunner::demo1_SecuenciaBasica() {
    std::cout << "\n\n";
    std::cout << "████████████████████████████████████████████████████████████████\n";
    std::cout << "█                                                              █\n";
    std::cout << "█          DEMO 1: SECUENCIA DE SOLICITUDES Y LIBERACIONES     █\n";
    std::cout << "█                                                              █\n";
    std::cout << "████████████████████████████████████████████████████████████████\n";
    
    memory.reset();
    
    // ========== PASO 1 ==========
    showStep("PASO 1: Estado inicial - Toda la memoria libre");
    memory.printMap(32);
    memory.holes().printFreeList();
    waitForUser();
    
    // ========== PASO 2 ==========
    showStep("PASO 2: Solicitar 30 bytes para Chrome");
    memory.allocateProcess("Chrome", 30);
    memory.printProcesses();
    memory.printMap(32);
    memory.holes().printFreeList();
    waitForUser();
    
    // ========== PASO 3 ==========
    showStep("PASO 3: Solicitar 20 bytes para VSCode");
    memory.allocateProcess("VSCode", 20);
    memory.printProcesses();
    memory.printMap(32);
    memory.holes().printFreeList();
    waitForUser();
    
    // ========== PASO 4 ==========
    showStep("PASO 4: Solicitar 15 bytes para Spotify");
    memory.allocateProcess("Spotify", 15);
    memory.printProcesses();
    memory.printMap(32);
    memory.holes().printFreeList();
    waitForUser();
    
    // ========== PASO 5 ==========
    showStep("PASO 5: Solicitar 25 bytes para Discord");
    memory.allocateProcess("Discord", 25);
    memory.printProcesses();
    memory.printMap(32);
    memory.holes().printFreeList();
    waitForUser();
    
    // ========== PASO 6 ==========
    showStep("PASO 6: Liberar VSCode (20 bytes) - Crea hueco en medio");
    memory.releaseProcess("VSCode");
    memory.printProcesses();
    memory.printMap(32);
    memory.holes().printFreeList();
    std::cout << " Observa: Ahora hay un HUECO LIBRE en medio de la memoria\n";
    waitForUser();
    
    // ========== PASO 7 ==========
    showStep("PASO 7: Liberar Discord (25 bytes) - Crea otro hueco");
    memory.releaseProcess("Discord");
    memory.printProcesses();
    memory.printMap(32);
    memory.holes().printFreeList();
    std::cout << " Observa: Dos huecos libres separados + el hueco al final\n";
    waitForUser();
}

// ============================================================================
// DEMO 2: Demostración de fusión de huecos (Coalescing)
// ============================================================================
void DemoRunner::demo2_Fusion() {
    std::cout << "\n\n";
    std::cout << "████████████████████████████████████████████████████████████████\n";
    std::cout << "█                                                              █\n";
    std::cout << "█          DEMO 2: FUSIÓN DE HUECOS ADYACENTES (COALESCING)    █\n";
    std::cout << "█                                                              █\n";
    std::cout << "████████████████████████████████████████████████████████████████\n";
    
    memory.reset();
    
    // ========== CONFIGURACIÓN ==========
    showStep("CONFIGURACIÓN: Crear 5 procesos consecutivos pequeños");
    memory.allocateProcess("P1", 10);
    memory.allocateProcess("P2", 10);
    memory.allocateProcess("P3", 10);
    memory.allocateProcess("P4", 10);
    memory.allocateProcess("P5", 10);
    memory.printProcesses();
    memory.printMap(32);
    memory.holes().printFreeList();
    waitForUser();
    
    // ========== PASO 1 ==========
    showStep("PASO 1: Liberar P2 (en medio) - Crea un hueco");
    memory.releaseProcess("P2");
    memory.printProcesses();
    memory.printMap(32);
    memory.holes().printFreeList();
    std::cout << " Un hueco de 10 bytes entre P1 y P3\n";
    waitForUser();
    
    // ========== PASO 2 ==========
    showStep("PASO 2: Liberar P4 (más adelante) - Crea otro hueco");
    memory.releaseProcess("P4");
    memory.printProcesses();
    memory.printMap(32);
    memory.holes().printFreeList();
    std::cout << " DOS huecos separados: uno de 10 bytes y otro de 10 bytes\n";
    waitForUser();
    
    // ========== PASO 3 - LA MAGIA ==========
    showStep("PASO 3: Liberar P3 (entre los dos huecos) - ¡FUSIÓN!");
    std::cout << "  CRÍTICO: P3 está entre dos huecos libres\n";
    std::cout << "    Al liberarlo, debe FUSIONAR los tres bloques en UNO SOLO\n\n";
    memory.releaseProcess("P3");
    memory.printProcesses();
    memory.printMap(32);
    memory.holes().printFreeList();
    std::cout << "\n";
    std::cout << "  ¡FUSIÓN EXITOSA! Los 3 bloques se convirtieron en 1 hueco de 30 bytes\n";
    std::cout << "   Antes: [hueco 10] + [P3 10] + [hueco 10] = 3 bloques\n";
    std::cout << "   Después: [hueco 30] = 1 bloque grande\n";
    waitForUser();
    
    // ========== PASO 4 ==========
    showStep("PASO 4: Liberar P5 (adyacente al hueco grande)");
    std::cout << "P5 está justo después del hueco de 30 bytes\n";
    std::cout << "Al liberarlo, debe fusionarse con el hueco anterior\n\n";
    memory.releaseProcess("P5");
    memory.printProcesses();
    memory.printMap(32);
    memory.holes().printFreeList();
    std::cout << "\n Fusión: 30 + 10 = 40 bytes de hueco continuo\n";
    waitForUser();
    
    // ========== PASO 5 ==========
    showStep("PASO 5: Liberar P1 (al inicio, adyacente al hueco)");
    memory.releaseProcess("P1");
    memory.printProcesses();
    memory.printMap(32);
    memory.holes().printFreeList();
    std::cout << "\n FUSIÓN COMPLETA: Toda la memoria ocupada se fusionó en huecos\n";
    std::cout << "   ¡El algoritmo combate exitosamente la fragmentación!\n";
    waitForUser();
}

// ============================================================================
// DEMO 3: Comparación First Fit vs Best Fit
// ============================================================================
void DemoRunner::demo3_ComparacionAlgoritmos() {
    std::cout << "\n\n";
    std::cout << "████████████████████████████████████████████████████████████████\n";
    std::cout << "█                                                              █\n";
    std::cout << "█     DEMO 3: PRIMER AJUSTE vs MEJOR AJUSTE (COMPARACIÓN)      █\n";
    std::cout << "█                                                              █\n";
    std::cout << "████████████████████████████████████████████████████████████████\n";
    
    std::cout << "\nVamos a demostrar la diferencia clave entre ambos algoritmos:\n";
    std::cout << "  • PRIMER AJUSTE: Usa el PRIMER hueco donde cabe\n";
    std::cout << "  • MEJOR AJUSTE: Usa el hueco MÁS PEQUEÑO donde cabe\n";
    waitForUser();
    
    // ========================================================================
    // CONFIGURACIÓN INICIAL (común para ambos algoritmos)
    // ========================================================================
    memory.reset();
    
    showStep("CONFIGURACIÓN: Asignar 5 procesos iniciales");
    std::cout << "Asignando:\n";
    std::cout << "  - Spotify: 30 bytes\n";
    std::cout << "  - Chrome: 20 bytes\n";
    std::cout << "  - Discord: 15 bytes\n";
    std::cout << "  - VSCode: 10 bytes\n";
    std::cout << "  - Windsurf: 15 bytes\n\n";
    
    memory.allocateProcess("Spotify", 30, false);
    memory.allocateProcess("Chrome", 20, false);
    memory.allocateProcess("Discord", 15, false);
    memory.allocateProcess("VSCode", 10, false);
    memory.allocateProcess("Windsurf", 15, false);
    
    memory.printProcesses();
    memory.printMap(32);
    memory.holes().printFreeList();
    waitForUser();
    
    // ========================================================================
    // CREAR DOS HUECOS DE DIFERENTES TAMAÑOS
    // ========================================================================
    showStep("CREAR HUECOS: Liberar Chrome (20 bytes) y VSCode (10 bytes)");
    std::cout << "Liberando Chrome y VSCode para crear DOS huecos:\n";
    std::cout << "  - Hueco 1: 20 bytes (donde estaba Chrome)\n";
    std::cout << "  - Hueco 2: 10 bytes (donde estaba VSCode)\n\n";
    
    memory.releaseProcess("Chrome");
    memory.releaseProcess("VSCode");
    
    memory.printProcesses();
    memory.printMap(32);
    memory.holes().printFreeList();
    
    std::cout << "\n SITUACIÓN ACTUAL:\n";
    std::cout << "   Hay DOS huecos libres:\n";
    std::cout << "   1. Hueco de 20 bytes (dirección 30)\n";
    std::cout << "   2. Hueco de 10 bytes (dirección 65)\n";
    std::cout << "\n   Vamos a solicitar 10 bytes para 'Notion'\n";
    std::cout << "   ¿En cuál hueco se colocará?\n";
    waitForUser();
    
    // ========================================================================
    // PARTE A: PRIMER AJUSTE
    // ========================================================================
    std::cout << "\n";
    std::cout << "┏━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┓\n";
    std::cout << "┃                    PRUEBA A: PRIMER AJUSTE                 ┃\n";
    std::cout << "┗━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┛\n";
    
    showStep("A1: Solicitar 10 bytes para Notion (PRIMER AJUSTE)");
    std::cout << " Algoritmo: PRIMER AJUSTE\n";
    std::cout << "   Busca desde el inicio de la lista de huecos...\n";
    std::cout << "   - Encuentra hueco de 20 bytes → ✓ Cabe (20 >= 10)\n";
    std::cout << "   - ¡Lo usa inmediatamente sin buscar más!\n\n";
    
    memory.allocateProcess("Notion", 10, false);  // FALSE = First Fit
    
    memory.printProcesses();
    memory.printMap(32);
    memory.holes().printFreeList();
    
    std::cout << "\n RESULTADO:\n";
    std::cout << "   ✓ Notion se colocó en el PRIMER hueco (20 bytes)\n";
    std::cout << "   ✓ Quedan 10 bytes libres en ese hueco\n";
    std::cout << "   ✓ El hueco de 10 bytes sigue intacto\n";
    std::cout << "   → Ahora hay DOS huecos de 10 bytes cada uno\n";
    waitForUser();
    
    // ========================================================================
    // LIMPIAR PARA LA SEGUNDA PRUEBA
    // ========================================================================
    showStep("Preparar para la segunda prueba");
    std::cout << "Liberando Notion para volver al estado anterior...\n\n";
    
    memory.releaseProcess("Notion");
    
    memory.printProcesses();
    memory.printMap(32);
    memory.holes().printFreeList();
    
    std::cout << "\n✓ Memoria restaurada: dos huecos (20 bytes y 10 bytes)\n";
    waitForUser();
    
    // ========================================================================
    // PARTE B: MEJOR AJUSTE
    // ========================================================================
    std::cout << "\n";
    std::cout << "┏━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┓\n";
    std::cout << "┃                    PRUEBA B: MEJOR AJUSTE                  ┃\n";
    std::cout << "┗━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┛\n";
    
    showStep("B1: Solicitar 10 bytes para Notion (MEJOR AJUSTE)");
    std::cout << " Algoritmo: MEJOR AJUSTE\n";
    std::cout << "   Recorre TODA la lista buscando el hueco más pequeño...\n";
    std::cout << "   - Hueco 1: 20 bytes → ✓ Cabe (20 >= 10), desperdicio = 10\n";
    std::cout << "   - Hueco 2: 10 bytes → ✓ Cabe (10 >= 10), desperdicio = 0\n";
    std::cout << "   - ¡Elige el hueco de 10 bytes (ajuste PERFECTO)!\n\n";
    
    memory.allocateProcess("Notion", 10, true);  // TRUE = Best Fit
    
    memory.printProcesses();
    memory.printMap(32);
    memory.holes().printFreeList();
    
    std::cout << "\n RESULTADO:\n";
    std::cout << "   ✓ Notion se colocó en el SEGUNDO hueco (10 bytes)\n";
    std::cout << "   ✓ Ese hueco se llenó COMPLETAMENTE (ajuste perfecto)\n";
    std::cout << "   ✓ El hueco de 20 bytes sigue intacto\n";
    std::cout << "   → Ahora solo queda UN hueco de 20 bytes\n";
    waitForUser();

}
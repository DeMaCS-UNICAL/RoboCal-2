# Scenari di Ricerca e Obiettivi

## 1. Percezione e Modello del Mondo

### 1.1 Aree di Ricerca: Percezione
*   I moduli di percezione vengono eseguiti nei thread *Lower* e *Upper*.
*   Non c'è visione stereo né sincronizzazione temporale.
*   **Pre-elaborazione (Preprocessing):**
    1.  L'immagine YCbCr viene divisa in scala di grigi, saturazione e tonalità (hue).
    2.  In base a delle soglie, ogni pixel viene classificato come campo, bianco, nero e altri.
    3.  Rilevamento di regioni degli stessi colori sulle linee di scansione (scan lines) e dei confini tra il campo e l'ambiente circostante.
*   **Moduli specializzati:**
    1.  Percezione delle linee del campo, dei segni di rigore (penalty marks), ecc.
    2.  Percezione della palla: identificazione dei punti (spots) e loro classificazione tramite una Rete Neurale Convoluzionale (CNN).
*   **Sviluppi futuri:** Sostituire la segmentazione dei colori con soluzioni specifiche per il problema che non richiedono calibrazione e possono adattarsi alle condizioni di luce.

### 1.2 Integrazione del Modello del Mondo
**Obiettivo:** Integrare i risultati "rumorosi" della percezione in un modello coerente degli oggetti nell'ambiente nel corso del tempo.

*   Tale modello include informazioni non misurabili direttamente, come:
    1.  Posizione e orientamento del robot stesso nel campo.
    2.  Posizione e velocità degli altri robot.
    3.  Posizione e velocità della palla.
*   Possono esserci misurazioni completamente errate (falsi positivi) o nessuna misurazione del tutto (falsi negativi).
*   Utilizzo di ipotesi che si sono formate nel tempo, potendo escludere i falsi positivi nel processo (Filtro di Kalman, Filtro a Particelle, ecc.).
*   Ogni robot ha un modello incompleto dell'ambiente: si scambiano il modello locale via WiFi (aiuta anche a rilevare misurazioni false positive).

## 2. Processo Decisionale e Comportamento
**Obiettivo:** In base al modello del mondo stimato e alle informazioni ricevute dai compagni di squadra, un robot deve decidere quale azione eseguire successivamente.

*   **Assegnazione dei ruoli:** Attaccante (striker), supporto (supporter) e portiere (goal keeper).
*   In base al modello del mondo e al ruolo, un robot seleziona un'azione:
    *   Pre e post condizioni determinano l'inizio e la fine dell'azione (es. le azioni di supporto sono fortemente dipendenti dalla sua posizione e dalla posizione relativa all'attaccante).
    *   Queste azioni di alto livello dipendono da abilità (skills) di livello inferiore, come camminare verso una posizione o eseguire un calcio.
    *   Le azioni richiedono un comportamento specifico dal motore di movimento (motion engine) e gestiscono i dettagli, come la scelta di un percorso per raggiungere una posizione.
*   Alcune azioni e skill sono implementate come macchine a stati (C-based Agent Behavior Specification Language - CABSL).

## 3. Sensori e Cinematica (Dati non Visivi)
**Obiettivo:** Elaborare i dati da tutti i sensori, ad eccezione delle telecamere, per calcolare:
    1.  Pose degli arti del robot.
    2.  Centro di massa del robot.
    3.  Orientamento 3D.
    4.  Possibile caduta (falling over).

*   **Alcuni sensori:**
    1.  **Feet Force Sensitive Resistors (FSRs):** Indicano su quale piede c'è più peso (camminata).
    2.  **IMU (Giroscopio e Accelerometro):** I dati filtrati vengono utilizzati per stimare la rotazione del busto del robot. Questa stima, combinata con la cinematica diretta, consente di calcolare una trasformazione tra un punto di riferimento sul terreno e un punto fisso tra i fianchi del robot. È possibile quindi localizzare un oggetto rispetto al sistema di riferimento locale.

## 4. Movimento e Camminata
**Obiettivo:** Consentire al robot di eseguire alcuni movimenti predefiniti:
    1.  Stare in piedi (Standing).
    2.  Camminare (Walking).
    3.  Calciare da fermo (calci potenti).
    4.  Calciare mentre cammina (calci corti e medi).
    5.  Rialzarsi (Getting up).

*   **Dettagli sulla camminata:**
    1.  Sollevando alternativamente ogni gamba, il robot oscilla naturalmente lateralmente senza alcun vero e proprio movimento dell'anca, il che consente alla gamba sollevata di muoversi sul terreno.
    2.  La sincronizzazione avviene grazie ai sensori di pressione sui piedi.
    3.  Per mantenere il robot in posizione verticale, la misurazione del giroscopio attorno all'asse di beccheggio (pitch axis) viene filtrata passa-basso e aggiunta direttamente all'angolo dell'articolazione della caviglia (ankle pitch joint angle) richiesto del piede che è attualmente a terra.

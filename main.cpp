#include <iostream>
#include <filesystem>
#include <cstdlib>
#include <iomanip>
#include <fstream>

namespace fs = std::filesystem;
using namespace std;

// ==========================================
// 1. ANALIZZA LA CARTELLA TEMP DI WINDOWS
// ==========================================
void analizzaTemp(const string& percorso) {
    int fileTrovati = 0;
    uintmax_t spazioTotaleBytes = 0;

    cout << "\nAnalisi in corso..." << endl;

    try {
        for (const auto& entry : fs::directory_iterator(percorso, fs::directory_options::skip_permission_denied)) {
            if (fs::is_regular_file(entry.path())) {
                fileTrovati++;
                std::error_code ec;
                uintmax_t dim = fs::file_size(entry.path(), ec);
                if (!ec) {
                    spazioTotaleBytes += dim;
                }
            }
        }

        double spazioMB = static_cast<double>(spazioTotaleBytes) / (1024.0 * 1024.0);

        cout << "\n=================================" << endl;
        cout << "       RISULTATO ANALISI         " << endl;
        cout << "=================================" << endl;
        cout << "File temporanei trovati: " << fileTrovati << endl;
        cout << "Spazio occupato stimato: " << fixed << setprecision(2) << spazioMB << " MB" << endl;
        cout << "---------------------------------" << endl;

    } catch (const fs::filesystem_error& e) {
        cout << "[ERRORE DI SISTEMA] " << e.what() << endl;
    }
}

// ==========================================
// 2. SPOSTA I FILE TEMP NEL CESTINO SICURO
// ==========================================
void spostaNelCestino(const string& percorsoTemp, const string& percorsoCestino) {
    int fileSpostati = 0;
    uintmax_t spazioSpostatoBytes = 0;
    std::error_code ec;

    if (!fs::exists(percorsoCestino)) {
        fs::create_directories(percorsoCestino, ec);
    }

    cout << "\nSpostamento dei file nel Cestino di sicurezza..." << endl;

    try {
        for (const auto& entry : fs::directory_iterator(percorsoTemp, fs::directory_options::skip_permission_denied)) {
            if (fs::is_regular_file(entry.path())) {
                std::error_code ecSize;
                uintmax_t dim = fs::file_size(entry.path(), ecSize);
                
                fs::path destinazione = fs::path(percorsoCestino) / entry.path().filename();
                
                if (fs::exists(destinazione)) {
                    fs::remove(destinazione, ec);
                }

                fs::rename(entry.path(), destinazione, ec);
                if (!ec) {
                    fileSpostati++;
                    if (!ecSize) spazioSpostatoBytes += dim;
                }
            }
        }

        double spazioMB = static_cast<double>(spazioSpostatoBytes) / (1024.0 * 1024.0);

        cout << "\n=================================" << endl;
        cout << "       SPOSTAMENTO AL CESTINO    " << endl;
        cout << "=================================" << endl;
        cout << "File spostati nel Cestino: " << fileSpostati << endl;
        cout << "Spazio liberato dalla Temp: " << fixed << setprecision(2) << spazioMB << " MB" << endl;
        cout << "---------------------------------" << endl;

    } catch (const fs::filesystem_error& e) {
        cout << "[ERRORE DI SISTEMA] " << e.what() << endl;
    }
}

// ==========================================
// 3. GESTIONE DEL CESTINO
// ==========================================
void gestisciCestino(const string& percorsoCestino) {
    std::error_code ec;
    if (!fs::exists(percorsoCestino)) {
        cout << "\nIl Cestino e' attualmente vuoto." << endl;
        return;
    }

    int sceltaCestino;
    do {
        cout << "\n=================================" << endl;
        cout << "      GESTIONE CESTINO SICURO    " << endl;
        cout << "=================================" << endl;
        cout << "1. Elenca file nel Cestino" << endl;
        cout << "2. Svuota Cestino (Elimina definitivamente)" << endl;
        cout << "3. Torna al menu principale" << endl;
        cout << "Scegli un'opzione: ";

        if (!(cin >> sceltaCestino)) {
            cout << "[ERRORE] Inserisci un numero valido!" << endl;
            cin.clear();
            cin.ignore(10000, '\n');
            continue;
        }

        switch (sceltaCestino) {
            case 1: {
                cout << "\n--- FILE PRESENTI NEL CESTINO ---" << endl;
                int count = 0;
                for (const auto& entry : fs::directory_iterator(percorsoCestino)) {
                    if (fs::is_regular_file(entry.path())) {
                        cout << "- " << entry.path().filename().string() << endl;
                        count++;
                    }
                }
                if (count == 0) cout << "(Il Cestino e' vuoto)" << endl;
                break;
            }
            case 2: {
                char conferma;
                cout << "Sei sicuro di voler eliminare DEFINITIVAMENTE i file dal Cestino? (s/n): ";
                cin >> conferma;
                if (conferma == 's' || conferma == 'S') {
                    uintmax_t eliminati = 0;
                    for (const auto& entry : fs::directory_iterator(percorsoCestino)) {
                        if (fs::is_regular_file(entry.path())) {
                            fs::remove(entry.path(), ec);
                            if (!ec) eliminati++;
                        }
                    }
                    cout << "-> Cestino svuotato! Eliminati definitivamente " << eliminati << " file." << endl;
                } else {
                    cout << "Operazione annullata." << endl;
                }
                break;
            }
            case 3:
                cout << "Ritorno al menu principale..." << endl;
                break;
            default:
                cout << "Scelta non valida." << endl;
        }
    } while (sceltaCestino != 3);
}

// ==========================================
// 4. GESTORE CARTELLA PERSONALIZZATA
// ==========================================
void gestisciCartellaPersonalizzata() {
    string percorso;
    cout << "\n--- GESTORE CARTELLA PERSONALIZZATA ---" << endl;
    cout << "Inserisci il percorso della cartella (es. C:\\Users\\TuoNome\\Desktop\\MiaCartella): ";
    cin.ignore();
    getline(cin, percorso);

    std::error_code ec;
    if (!fs::exists(percorso)) {
        cout << "La cartella non esiste. Vuoi crearla? (s/n): ";
        char sceltaCrea;
        cin >> sceltaCrea;
        if (sceltaCrea == 's' || sceltaCrea == 'S') {
            if (fs::create_directories(percorso, ec)) {
                cout << "-> Cartella creata con successo!" << endl;
            } else {
                cout << "[ERRORE] Impossibile creare la cartella." << endl;
                return;
            }
        } else {
            return;
        }
    }

    int subScelta;
    do {
        cout << "\n=================================" << endl;
        cout << " CARTELLA: " << percorso << endl;
        cout << "=================================" << endl;
        cout << "1. Elenca tutti i file" << endl;
        cout << "2. Aggiungi un nuovo file" << endl;
        cout << "3. Elimina un file" << endl;
        cout << "4. Rinomina un file" << endl;
        cout << "5. Torna al menu principale" << endl;
        cout << "Scegli un'opzione: ";

        if (!(cin >> subScelta)) {
            cout << "[ERRORE] Inserisci un numero valido!" << endl;
            cin.clear();
            cin.ignore(10000, '\n');
            continue;
        }

        switch (subScelta) {
            case 1: {
                cout << "\n--- ELENCO FILE ---" << endl;
                int count = 0;
                for (const auto& entry : fs::directory_iterator(percorso)) {
                    if (fs::is_regular_file(entry.path())) {
                        cout << "- " << entry.path().filename().string() << endl;
                        count++;
                    }
                }
                if (count == 0) cout << "(La cartella e' vuota)" << endl;
                break;
            }
            case 2: {
                string nomeNuovoFile, contenuto;
                cout << "Inserisci il nome del nuovo file (es. nota.txt): ";
                cin >> nomeNuovoFile;
                
                fs::path percorsoCompleto = fs::path(percorso) / nomeNuovoFile;
                ofstream nuovoFile(percorsoCompleto.c_str());
                
                if (nuovoFile.is_open()) {
                    cout << "Scrivi il contenuto del file: ";
                    cin.ignore();
                    getline(cin, contenuto);
                    nuovoFile << contenuto;
                    nuovoFile.close();
                    cout << "-> File creato con successo!" << endl;
                } else {
                    cout << "[ERRORE] Impossibile creare il file." << endl;
                }
                break;
            }
            case 3: {
                string nomeFileDel;
                cout << "Inserisci il nome esatto del file da eliminare: ";
                cin >> nomeFileDel;
                
                fs::path percorsoCompleto = fs::path(percorso) / nomeFileDel;
                
                if (fs::exists(percorsoCompleto)) {
                    fs::remove(percorsoCompleto);
                    cout << "-> File eliminato con successo!" << endl;
                } else {
                    cout << "[ERRORE] Il file non esiste." << endl;
                }
                break;
            }
            case 4: {
                string vecchioNome, nuovoNome;
                cout << "Inserisci il nome attuale del file: ";
                cin >> vecchioNome;
                cout << "Inserisci il nuovo nome per il file: ";
                cin >> nuovoNome;

                fs::path vecchioPercorso = fs::path(percorso) / vecchioNome;
                fs::path nuovoPercorso = fs::path(percorso) / nuovoNome;

                if (fs::exists(vecchioPercorso)) {
                    fs::rename(vecchioPercorso, nuovoPercorso);
                    cout << "-> File rinominato con successo!" << endl;
                } else {
                    cout << "[ERRORE] Il file originale non esiste." << endl;
                }
                break;
            }
            case 5:
                cout << "Ritorno al menu principale..." << endl;
                break;
            default:
                cout << "Scelta non valida." << endl;
        }

    } while (subScelta != 5);
}

// ==========================================
// 5. CONTROLLO ERRORI SISTEMA (SFC /SCANNOW)
// ==========================================
void eseguiSfcScannow() {
    cout << "\n=================================" << endl;
    cout << "   CONTROLLO INTEGRITA' SISTEMA  " << endl;
    cout << "=================================" << endl;
    cout << "NOTA: Questo comando (sfc /scannow) esamina" << endl;
    cout << "i file di sistema di Windows e richiede" << endl;
    cout << "i privilegi di AMMINISTRATORE per ripararli." << endl;
    cout << "Vuoi avviare la scansione ora? (s/n): ";
    
    char conferma;
    cin >> conferma;
    if (conferma == 's' || conferma == 'S') {
        cout << "\nAvvio della scansione SFC in corso..." << endl;
        cout << "Si aprira' la finestra di Windows dedicata. Attendere prego...\n" << endl;
        
        // Esegue il comando di sistema nativo di Windows
        int risultato = system("sfc /scannow");
        
        cout << "\n---------------------------------" << endl;
        if (risultato == 0) {
            cout << "-> Scansione e controllo completati!" << endl;
        } else {
            cout << "[AVVISO] Se il comando non e' andato a buon fine, prova ad" << endl;
            cout << "avviare Dev-C++ o il programma eseguibile cliccando con" << endl;
            cout << "il tasto destro e scegliendo 'Esegui come amministratore'." << endl;
        }
        cout << "---------------------------------" << endl;
    } else {
        cout << "Scansione annullata." << endl;
    }
}

// ==========================================
// FUNZIONE PRINCIPALE (MENU GLOBALE)
// ==========================================
int main() {
    const char* tempEnv = getenv("TEMP");
    if (tempEnv == nullptr) {
        cout << "[ERRORE] Impossibile trovare la cartella Temp del sistema." << endl;
        return 1;
    }
    
    string percorsoTemp = tempEnv;
    string percorsoCestino = percorsoTemp + "\\MiniCleaner_Trash";
    int scelta;

    do {
        cout << "\n=================================" << endl;
        cout << "   MINI-CLEANER & FILE MANAGER   " << endl;
        cout << "=================================" << endl;
        cout << "1. Analizza cartella Temp Windows" << endl;
        cout << "2. Sposta file Temp nel Cestino di sicurezza" << endl;
        cout << "3. Gestisci Cestino (Visualizza / Svuota)" << endl;
        cout << "4. Apri e gestisci una tua cartella personalizzata" << endl;
        cout << "5. Controllo errori sistema (sfc /scannow)" << endl;
        cout << "6. Esci" << endl;
        cout << "Scegli un'opzione: ";

        if (!(cin >> scelta)) {
            cout << "\n[ERRORE] Inserisci un numero valido da 1 a 6!" << endl;
            cin.clear();
            cin.ignore(10000, '\n');
            scelta = 0;
            continue;
        }

        switch (scelta) {
            case 1:
                analizzaTemp(percorsoTemp);
                break;
            case 2: {
                char conferma;
                cout << "Vuoi spostare i file temporanei nel Cestino? (s/n): ";
                cin >> conferma;
                if (conferma == 's' || conferma == 'S') {
                    spostaNelCestino(percorsoTemp, percorsoCestino);
                } else {
                    cout << "Operazione annullata." << endl;
                }
                break;
            }
            case 3:
                gestisciCestino(percorsoCestino);
                break;
            case 4:
                gestisciCartellaPersonalizzata();
                break;
            case 5:
                eseguiSfcScannow();
                break;
            case 6:
                cout << "Uscita dal programma. A presto!" << endl;
                break;
            default:
                cout << "Scelta non valida! Inserisci un numero tra 1 e 6." << endl;
        }

    } while (scelta != 6);

    return 0;
}

#include <iostream>
#include <filesystem>
#include <cstdlib>
#include <iomanip>
#include <fstream>
#include <ctime> // Per la gestione di data e ora

namespace fs = std::filesystem;
using namespace std;

// ==========================================
// 1. ANALIZZA LA TEMP E LO SPAZIO DISCO
// ==========================================
void analizzaTemp(const string& percorso) {
    int fileTrovati = 0;
    uintmax_t spazioTotaleBytes = 0;

    cout << "\nAnalisi in corso..." << endl;

    try {
        // Informazioni sullo spazio del disco fisso
        fs::space_info si = fs::space(percorso);
        double spazioTotaleGB = static_cast<double>(si.capacity) / (1024.0 * 1024.0 * 1024.0);
        double spazioLiberoGB = static_cast<double>(si.free) / (1024.0 * 1024.0 * 1024.0);

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
        cout << "       STATISTICHE DISCO & TEMP  " << endl;
        cout << "=================================" << endl;
        cout << "Spazio Totale Disco: " << fixed << setprecision(2) << spazioTotaleGB << " GB" << endl;
        cout << "Spazio Libero Disco: " << fixed << setprecision(2) << spazioLiberoGB << " GB" << endl;
        cout << "---------------------------------" << endl;
        cout << "File temporanei trovati: " << fileTrovati << endl;
        cout << "Spazio occupato da Temp: " << fixed << setprecision(2) << spazioMB << " MB" << endl;
        cout << "---------------------------------" << endl;

    } catch (const fs::filesystem_error& e) {
        cout << "[ERRORE DI SISTEMA] " << e.what() << endl;
    }
}

// ==========================================
// 2. SPOSTA TEMP NEL CESTINO + LOG DATA/ORA
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

        // Richiesta salvataggio log data e ora su fileTempData.txt
        char rispostaLog;
        cout << "Vuoi salvare sul file fileTempData.txt la data e l'ora dell'ultima pulizia? (s/n): ";
        cin >> rispostaLog;
        if (rispostaLog == 's' || rispostaLog == 'S') {
            time_t oraCorrente = time(nullptr);
            char buffer[80];
            strftime(buffer, sizeof(buffer), "%d-%m-%Y %H:%M:%S", localtime(&oraCorrente));

            ofstream logFile("fileTempData.txt", ios::app); // Apre in modalità append
            if (logFile.is_open()) {
                logFile << "Pulizia eseguita il: " << buffer << " - Spazio liberato: " << fixed << setprecision(2) << spazioMB << " MB" << endl;
                logFile.close();
                cout << "-> Dati salvati con successo su fileTempData.txt!" << endl;
            } else {
                cout << "[ERRORE] Impossibile aprire il file di log." << endl;
            }
        }

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
// 4. GESTORE CARTELLA + RICERCA RAPIDA FILE
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
        cout << "5. Cerca un file nella cartella" << endl;
        cout << "6. Torna al menu principale" << endl;
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
            case 5: {
                string chiave;
                cout << "Inserisci il nome o l'estensione da cercare (es. .txt o report): ";
                cin >> chiave;
                cout << "\n--- RISULTati RICERCA ---" << endl;
                int trovati = 0;
                for (const auto& entry : fs::directory_iterator(percorso)) {
                    if (fs::is_regular_file(entry.path())) {
                        string nomeFile = entry.path().filename().string();
                        // Controlla se la parola chiave è contenuta nel nome del file
                        if (nomeFile.find(chiave) != string::npos) {
                            cout << "Trovato: " << nomeFile << endl;
                            trovati++;
                        }
                    }
                }
                cout << "Totale file corrispondenti trovati: " << trovati << endl;
                break;
            }
            case 6:
                cout << "Ritorno al menu principale..." << endl;
                break;
            default:
                cout << "Scelta non valida." << endl;
        }

    } while (subScelta != 6);
}

// ==========================================
// 5. CONTROLLO ERRORI SISTEMA (SFC)
// ==========================================
void eseguiSfcScannow() {
    cout << "\n=================================" << endl;
    cout << "   CONTROLLO INTEGRITA' SISTEMA  " << endl;
    cout << "=================================" << endl;
    cout << "NOTA: Richiede privilegi di AMMINISTRATORE." << endl;
    cout << "Vuoi avviare la scansione sfc /scannow ora? (s/n): ";
    
    char conferma;
    cin >> conferma;
    if (conferma == 's' || conferma == 'S') {
        cout << "\nAvvio della scansione SFC in corso... Attendere...\n" << endl;
        system("sfc /scannow");
        cout << "\n-> Scansione completata!" << endl;
    } else {
        cout << "Scansione annullata." << endl;
    }
}

// ==========================================
// 6. RIPARAZIONE IMMAGINE DISM
// ==========================================
void eseguiDism() {
    cout << "\n=================================" << endl;
    cout << "   RIPARAZIONE IMMAGINE DISM     " << endl;
    cout << "=================================" << endl;
    cout << "NOTA: Richiede privilegi di AMMINISTRATORE." << endl;
    cout << "Vuoi avviare la riparazione DISM ora? (s/n): ";
    
    char conferma;
    cin >> conferma;
    if (conferma == 's' || conferma == 'S') {
        cout << "\nAvvio della riparazione DISM in corso... Attendere...\n" << endl;
        system("dism /Online /Cleanup-Image /RestoreHealth");
        cout << "\n-> Riparazione immagine completata!" << endl;
    } else {
        cout << "Riparazione annullata." << endl;
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
        cout << "1. Analizza Temp Windows e Spazio Disco" << endl;
        cout << "2. Sposta file Temp nel Cestino (+ Log data/ora)" << endl;
        cout << "3. Gestisci Cestino (Visualizza / Svuota)" << endl;
        cout << "4. Apri e gestisci cartella personalizzata" << endl;
        cout << "5. Controllo errori sistema (sfc /scannow)" << endl;
        cout << "6. Riparazione immagine Windows (DISM)" << endl;
        cout << "7. Esci" << endl;
        cout << "Scegli un'opzione: ";

        if (!(cin >> scelta)) {
            cout << "\n[ERRORE] Inserisci un numero valido da 1 a 7!" << endl;
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
                eseguiDism();
                break;
            case 7:
                cout << "Uscita dal programma. A presto!" << endl;
                break;
            default:
                cout << "Scelta non valida! Inserisci un numero tra 1 e 7." << endl;
        }

    } while (scelta != 7);

    return 0;
}

#include <iostream>
#include <filesystem>
#include <cstdlib>
#include <iomanip>
#include <cstdint>
#include <fstream> // Indispensabile per creare e scrivere i file

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
// 2. PULISCI LA CARTELLA TEMP DI WINDOWS
// ==========================================
void pulisciTemp(const string& percorso) {
    int fileTrovati = 0;
    int fileEliminati = 0;
    uintmax_t spazioLiberatoBytes = 0;

    cout << "\nPulizia in corso..." << endl;

    try {
        for (const auto& entry : fs::directory_iterator(percorso, fs::directory_options::skip_permission_denied)) {
            if (fs::is_regular_file(entry.path())) {
                fileTrovati++;
                std::error_code ec;
                uintmax_t dim = fs::file_size(entry.path(), ec);
                
                fs::remove(entry.path(), ec);
                if (!ec) {
                    fileEliminati++;
                    spazioLiberatoBytes += dim;
                }
            }
        }

        double spazioLiberatoMB = static_cast<double>(spazioLiberatoBytes) / (1024.0 * 1024.0);

        cout << "\n=================================" << endl;
        cout << "       REPORT PULIZIA            " << endl;
        cout << "=================================" << endl;
        cout << "File eliminati con successo: " << fileEliminati << endl;
        cout << "Spazio liberato sul disco: " << fixed << setprecision(2) << spazioLiberatoMB << " MB" << endl;
        cout << "---------------------------------" << endl;

    } catch (const fs::filesystem_error& e) {
        cout << "[ERRORE DI SISTEMA] " << e.what() << endl;
    }
}

// ==========================================
// 3. GESTORE CARTELLA PERSONALIZZATA
// ==========================================
void gestisciCartellaPersonalizzata() {
    string percorso;
    cout << "\n--- GESTORE CARTELLA PERSONALIZZATA ---" << endl;
    cout << "Inserisci il percorso della cartella (es. C:\\Users\\TuoNome\\Desktop\\MiaCartella): ";
    cin.ignore(); // Pulisce il buffer della tastiera
    getline(cin, percorso);

    // Controlla se la cartella esiste, altrimenti offre di crearla
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
                
                // Unione sicura del percorso con filesystem e conversione c_str()
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
// FUNZIONE PRINCIPALE (MENU GLOBALE)
// ==========================================
int main() {
    const char* tempEnv = getenv("TEMP");
    string percorsoTemp = (tempEnv != nullptr) ? tempEnv : "";
    int scelta;

    do {
        cout << "\n=================================" << endl;
        cout << "   MINI-CLEANER & FILE MANAGER   " << endl;
        cout << "=================================" << endl;
        cout << "1. Analizza cartella Temp Windows" << endl;
        cout << "2. Pulisci cartella Temp Windows" << endl;
        cout << "3. Apri e gestisci una tua cartella (Crea, Elenca, Rinomina, Elimina)" << endl;
        cout << "4. Esci" << endl;
        cout << "Scegli un'opzione: ";

        if (!(cin >> scelta)) {
            cout << "\n[ERRORE] Inserisci un numero valido da 1 a 4!" << endl;
            cin.clear();
            cin.ignore(10000, '\n');
            scelta = 0;
            continue;
        }

        switch (scelta) {
            case 1:
                if (!percorsoTemp.empty()) analizzaTemp(percorsoTemp);
                else cout << "[ERRORE] Percorso Temp non trovato." << endl;
                break;
            case 2: {
                char conferma;
                cout << "Sei sicuro di voler pulire la Temp? (s/n): ";
                cin >> conferma;
                if (conferma == 's' || conferma == 'S') {
                    if (!percorsoTemp.empty()) pulisciTemp(percorsoTemp);
                }
                break;
            }
            case 3:
                gestisciCartellaPersonalizzata();
                break;
            case 4:
                cout << "Uscita dal programma. A presto!" << endl;
                break;
            default:
                cout << "Scelta non valida! Inserisci un numero tra 1 e 4." << endl;
        }

    } while (scelta != 4);

    return 0;
}
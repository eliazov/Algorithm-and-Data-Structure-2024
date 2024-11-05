#include <fstream>
#include <iostream>
#include <map>
#include <algorithm>
#include <vector>
#include <queue>
#include <cmath>
#include <chrono> //Da togliere

using namespace std;

struct Nodo {
    vector<int> adj;
    vector<int> peso;
    vector<int> dipendenti;
    vector<int> dipendentiNodi;
    bool occupato;
};

map<int, Nodo> grafo;
int nNodi, nArchi;
vector<int> d;
vector<int> dNodi;
vector<int> dNodiPesata;

int pesoDistanceNodi = 0;
int B=0;
int A;
int kGlobal=0;
bool kFinale=false;

void distancePeso(int n, bool controlloOccupato);
void distanceNodi(int n, bool controlloOccupato);
bool contains(vector<int> vettore, int valore);
void cambiaDipendentiRic(int n);
void stampaDistance(vector<int> &v, string s);
bool containsOccupati(vector<int>& vettore);
void stampaDipendenti(vector<int> &v, int nodo);
void pulisciDipendenti();
void stampaBA();
void riavviaDistance();
void funzionaRicorsiva(vector<int> sx, vector<int> dx, int distanzaSx, int distanzaDx);

int main() {
    //Calcola tempo
    auto start = std::chrono::high_resolution_clock::now();
    vector<int> outputDipendenzeNodi;
    vector<int> outputDipendenzeBreveK0;
    ifstream in("input.txt");
    in >> nNodi >> nArchi;
    A = nNodi - 1;
    for (int i=0; i < nNodi; i++) {
        Nodo t;
        grafo[i] = t;
        t.occupato = false;
    }
    for (int i = 0; i < nArchi; i++) {
        int from, to, peso;
        in >> from >> to >> peso;
        grafo[from].adj.push_back(to);
        grafo[to].adj.push_back(from);
        grafo[from].peso.push_back(peso);
        grafo[to].peso.push_back(peso);
    }
    int nodiOccupati;
    in >> nodiOccupati;
    for (int j = 0; j < nodiOccupati; j++)
    {
        int t;
        in >> t;
        grafo[t].occupato = true;
    }
    riavviaDistance();

    distancePeso(B,false);
    distanceNodi(B,false);
    outputDipendenzeBreveK0 = grafo[A].dipendenti; //strada tempo minore
    outputDipendenzeNodi = grafo[A].dipendentiNodi; //strada tempo minore

    //stampaDistance(d,"");
    //stampaDistance(dNodi,"nodi");
    stampaBA();
    //Calcoli per il K
    //stampa su file
    ofstream out("output.txt");
    //Riga 1
    if (!containsOccupati(grafo[A].dipendentiNodi))
        out << -1 << endl;
    else if(dNodiPesata[A]==d[A])
           out << -2 << endl;
         else{
           funzionaRicorsiva(grafo[A].dipendenti,grafo[A].dipendentiNodi,d[A],dNodiPesata[A]);
           if(!kFinale)
            out<<-2<<endl;
           else
            out<<kGlobal<<endl;
         }
    //Riga 2 e 3
    out << outputDipendenzeBreveK0.size() + 1 << endl;
    for (int i = outputDipendenzeBreveK0.size() - 1; i >= 0; i--)
    {
        out << outputDipendenzeBreveK0[i] << " ";
    }
    out << A << endl;
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration = end - start;

    // Converti la durata in millisecondi o secondi
    double milliseconds = duration.count() * 1000;
    cout << "Tempo impiegato: " << milliseconds << endl;
}

void funzionaRicorsiva(vector<int> sx, vector<int> dx, int distanzaSx, int distanzaDx){
  int distanzaSxAggiornata=distanzaSx+(sx.size()*kGlobal);
  int distanzaDxAggiornata=distanzaDx+(dx.size()*kGlobal);
  float k=((float) (distanzaDxAggiornata-distanzaSxAggiornata))/(sx.size()-dx.size()); //punto d'intersezione
  int kApprox=ceil(k)-1;
  kGlobal+=kApprox;
  int distanceControllo=distanzaSx+(sx.size()*kGlobal);

  pulisciDipendenti();
  riavviaDistance();
  distancePeso(B,false);
  distanceNodi(B,false);

  //il grafo in A è la retta centrale
  cout<<d[A]<<endl;
  cout<<distanceControllo<<endl;
  //cout<<"\n\n"<<endl;
  if(d[A]<distanceControllo && !containsOccupati(grafo[A].dipendenti) && !kFinale){
    funzionaRicorsiva(grafo[A].dipendenti,dx,d[A]-(grafo[A].dipendenti.size()*kGlobal),distanzaDx);
  }else if(d[A]<distanceControllo && containsOccupati(grafo[A].dipendenti) && !kFinale){
    funzionaRicorsiva(grafo[A].dipendenti,dx,d[A]-(grafo[A].dipendenti.size()*kGlobal),distanzaDx);
    if(!kFinale)
    funzionaRicorsiva(sx,grafo[A].dipendenti,distanzaSx,d[A]-(grafo[A].dipendenti.size()*kGlobal));
  }else if(!containsOccupati(grafo[A].dipendenti)){
    kFinale=true;
  }

}

void pulisciDipendenti(){
  for (int i = 0; i < nNodi; i++) {
    grafo[i].dipendenti.clear();
    grafo[i].dipendentiNodi.clear();
  }
}

void riavviaDistance(){
  d.clear();
  dNodi.clear();
  dNodiPesata.clear();
  for (int j = 0; j < nNodi; j++) {
      d.push_back(-1);
      dNodi.push_back(-1);
      dNodiPesata.push_back(-1);
  }
}
bool contains(vector<int> vettore, int valore) {
    return find(vettore.begin(), vettore.end(), valore) != vettore.end();
}

bool containsOccupati(vector<int>& vettore) {
    bool ritorno = false;
    for (int i = 0; i < vettore.size() && !ritorno; i++)
    {
      //cout<<"Nodo: "<<vettore[i]<<" is occupato: "<<grafo[vettore[i]].occupato<<endl;
        if (grafo[vettore[i]].occupato)
            ritorno = true;
    }
    return ritorno;
}

void stampaDistance(vector<int> &v, string s) {
    cout << "Distance: "<<s << endl;
    for (int i = 0; i < nNodi; i++) {
        cout << v[i] << " ";
    }
    cout << endl;
}

void stampaDipendenti(vector<int> &v, int nodo){
  cout<<"Stampa di: "<<nodo<<endl;
  for (int i = v.size()-1; i>=0; i--) {
    cout<<v[i]<<" ";
  }
  cout<<nodo<<endl;
}

void stampaBA(){
    cout << "\nLa strada per pesata arrivarci e':" << endl;
    stampaDipendenti(grafo[A].dipendenti,A);
    cout << "Il peso totale del percorso piu' breve e': " << d[A] << endl;
    cout <<"il suo numero di nodi e': "<<grafo[A].dipendenti.size()<<endl;

    cout << "\nLa strada per (per numero di nodi) arrivarci e':" << endl;
    stampaDipendenti(grafo[A].dipendentiNodi,A);
    cout << "Il peso totale del percorso con meno nodi e': " << dNodiPesata[A] << endl;
    cout <<"il suo numero di nodi e': "<<grafo[A].dipendentiNodi.size()<<endl;

}

void distanceNodi(int n, bool controlloOccupato) {
    queue<int> Q;
    Q.push(n);
    dNodi[n] = 0;
    dNodiPesata[n] = 0;
    while (!Q.empty()) {
        int t = Q.front();
        //cout << "Entro come nodo: " << t << endl;
        Q.pop();
        for (int i = 0; i < grafo[t].adj.size() && t != A; i++) {
            int tv = grafo[t].adj[i];
            //cout<<"Entro nel nodo "<<tv<<endl;
            if(!(controlloOccupato && grafo[tv].occupato)){
              //cout << "Visito: " << tv << endl;
              if(dNodi[t]+1 == dNodi[tv]) {
                int distanzaPrima=dNodiPesata[tv];
                int distanzaNuova=dNodiPesata[t]+grafo[t].peso[i]+kGlobal;
                if(distanzaPrima == distanzaNuova){
                  bool vecchiaStradaOccupata=containsOccupati(grafo[tv].dipendentiNodi) || grafo[tv].occupato;
                  bool nuovaStradaOccupata=containsOccupati(grafo[t].dipendentiNodi) || grafo[tv].occupato || grafo[t].occupato;
                  if(!vecchiaStradaOccupata && nuovaStradaOccupata){
                    //se la strada vecchia non contiene un occupato allora la sostituisco con quella nuova (che lo contiene)
                    grafo[tv].dipendentiNodi.clear();
                    //if(t!=P)
                    grafo[tv].dipendentiNodi.push_back(t);
                    grafo[tv].dipendentiNodi.insert(grafo[tv].dipendentiNodi.end(),
                        grafo[t].dipendentiNodi.begin(),
                        grafo[t].dipendentiNodi.end());
                  }
                }

                if(distanzaNuova < distanzaPrima){//Sistema il percorso con il più corto
                  dNodiPesata[tv]=distanzaNuova;
                    grafo[tv].dipendentiNodi.clear();
                    //if(t!=P)
                    grafo[tv].dipendentiNodi.push_back(t);
                    grafo[tv].dipendentiNodi.insert(grafo[tv].dipendentiNodi.end(),
                        grafo[t].dipendentiNodi.begin(),
                        grafo[t].dipendentiNodi.end());
                  }
              }

              if (dNodi[tv] == -1) { //Non pesato
                if(!(tv != A && dNodi[t]+1 >= dNodi[A] && dNodi[A] != -1)){
                  Q.push(tv);
                  dNodi[tv] = dNodi[t] + 1;
                  dNodiPesata[tv] = dNodiPesata[t] + grafo[t].peso[i]+kGlobal;
                  //cout << "La distance di tv (" << tv << ") e': " << dNodi[tv] << endl;
                  grafo[tv].dipendentiNodi.push_back(t);
                  grafo[tv].dipendentiNodi.insert(grafo[tv].dipendentiNodi.end(),
                      grafo[t].dipendentiNodi.begin(),
                      grafo[t].dipendentiNodi.end());
                }
              }
            }
        }
    }
}

void distancePeso(int n, bool controlloOccupato) {
    queue<int> Q;
    Q.push(n);
    d[n] = 0;
    vector<int> dNodiLocale;
    while (!Q.empty()) {
        int t = Q.front();
        //cout << "Entro come nodo: " << t << endl;
        Q.pop();
        for (int i = 0; i < grafo[t].adj.size() && t != A; i++) {
            int tv = grafo[t].adj[i];
            if(!(controlloOccupato && grafo[tv].occupato)){

              //cout << "Visito: " << tv << endl;
              if (d[tv] == -1) {
                if(!(tv != A && d[t]+grafo[t].peso[i]+kGlobal >= d[A] && d[A] != -1)){//Pesato
                    Q.push(tv);
                    d[tv] = d[t] + grafo[t].peso[i]+kGlobal;
                    grafo[tv].dipendenti.clear();
                    //if(t!=P)
                    grafo[tv].dipendenti.push_back(t);
                    grafo[tv].dipendenti.insert(grafo[tv].dipendenti.end(),
                        grafo[t].dipendenti.begin(),
                        grafo[t].dipendenti.end());
                      }
                }
                else if (d[t] + (grafo[t].peso[i]+kGlobal) < d[tv]) {
                  if(!(tv != A && d[t]+(grafo[t].peso[i]+kGlobal) >= d[A] && d[A] != -1)){//Pesato
                      Q.push(tv);
                      d[tv] = d[t] + grafo[t].peso[i] + kGlobal;
                      grafo[tv].dipendenti.clear();
                      //if(t!=P)
                      grafo[tv].dipendenti.push_back(t);
                      grafo[tv].dipendenti.insert(grafo[tv].dipendenti.end(),
                          grafo[t].dipendenti.begin(),
                          grafo[t].dipendenti.end());
                      //Cambiare i dipendenti nel numero aggiornato
                      cambiaDipendentiRic(tv);
                  }
              }else if(d[t] + (grafo[t].peso[i]+kGlobal) == d[tv]){
                //Serve per prendere il percorso di peso minore che contenga il minor numero di nodi e che sia occupato
                if(grafo[t].dipendenti.size()+1 < grafo[tv].dipendenti.size() ||
                  (!grafo[tv].occupato &&
                    grafo[t].dipendenti.size()+1 == grafo[tv].dipendenti.size() &&
                    (containsOccupati(grafo[t].dipendenti)||grafo[t].occupato) &&
                     !containsOccupati(grafo[tv].dipendenti))){
                       grafo[tv].dipendenti.clear();
                       //if(t!=P)
                       grafo[tv].dipendenti.push_back(t);
                       grafo[tv].dipendenti.insert(grafo[tv].dipendenti.end(),
                           grafo[t].dipendenti.begin(),
                           grafo[t].dipendenti.end());
                       //Cambiare i dipendenti nel numero aggiornato
                       cambiaDipendentiRic(tv);
                }
              }
            }
        }
    }
}

void cambiaDipendentiRic(int n) {
    for (int i = 0; i < grafo[n].adj.size(); i++) {
        int t = grafo[n].adj[i];
        if (contains(grafo[t].dipendenti, n)) {
            grafo[t].dipendenti.clear();
            grafo[t].dipendenti.push_back(n);
            grafo[t].dipendenti.insert(grafo[t].dipendenti.end(),
                grafo[n].dipendenti.begin(),
                grafo[n].dipendenti.end());
            cambiaDipendentiRic(t);
        }
    }
}

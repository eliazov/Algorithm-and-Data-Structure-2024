#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <cstdlib>
#include <cmath>
#include <set>
#include <numeric>
#include <random>
#include "nonna.h"

using namespace std;
int c = 0;
int g = 0;

struct Centrino
{
    int id;
    int nUscenti = 0;
    vector<int> gomitoliConnessi;
    int k_centrino = 0;

    Centrino(int id)
    {
        this->id = id;
        this->gomitoliConnessi = vector<int>();
    }

    void addGomitolo(int idGomitolo)
    {
        gomitoliConnessi.push_back(idGomitolo);
        nUscenti++;
    }

    void ordinaGomitoli()
    {
        sort(gomitoliConnessi.begin(), gomitoliConnessi.end());
    }

    void resetNUscenti()
    {
        nUscenti = gomitoliConnessi.size();
    }
};

struct Gomitolo
{
    int nEntranti = 0;
    Gomitolo()
    {
    }

    void addEntrante()
    {
        nEntranti++;
    }
};

void stampaCentriniEGomitoli(vector<Centrino> &centrini, vector<Gomitolo> &gomitoli)
{
    for (int i = 0; i < c; i++)
    {
        cout << "Centrino " << centrini[i].id << "\nk =" << centrini[i].k_centrino << "\nconnesso a " << centrini[i].nUscenti << " gomitoli: ";
        for (int j = 0; j < centrini[i].gomitoliConnessi.size(); j++)
        {
            cout << centrini[i].gomitoliConnessi[j] << " ";
        }
        cout << endl;
        cout << endl;
    }

    for (int i = 0; i < g; i++)
    {
        cout << "Gomitolo " << i << " con " << gomitoli[i].nEntranti << " entranti" << endl;
    }
}

// Calcola le somme cumulative degli elementi in un vettore
void calcolaSommeCumulative(const vector<Gomitolo> &gomitoli, vector<int> &sommaCumulativa)
{
    sommaCumulativa[0] = gomitoli[0].nEntranti;
    for (size_t i = 1; i < gomitoli.size(); ++i)
    {
        sommaCumulativa[i] = sommaCumulativa[i - 1] + gomitoli[i].nEntranti;
    }
}

int calcolaSommaGomitoli(const vector<int> &sommaCumulativa, int indice)
{
    return sommaCumulativa[indice - 1];
}

int calcolaTuttiK(vector<Centrino> &centrini, vector<Gomitolo> &gomitoli)
{ 
    int sommaGomitoli = 0;
    vector<int> nEntrantiOriginali(gomitoli.size());
    for (size_t i = 0; i < gomitoli.size(); ++i)
    {
        nEntrantiOriginali[i] = gomitoli[i].nEntranti;
    }

    for (int i = 0; i < centrini.size(); i++)
    {
        vector<int> sommaCumulativa(gomitoli.size());
        calcolaSommeCumulative(gomitoli, sommaCumulativa);
        int cUscentiLocal = centrini[i].nUscenti - 1;
        int gC = centrini[i].gomitoliConnessi.size() - 1;
        for (int j = gC; j >= 0; j--)
        {
            centrini[i].k_centrino += calcolaSommaGomitoli(sommaCumulativa, centrini[i].gomitoliConnessi[j]) - cUscentiLocal;
            cUscentiLocal--;
            gomitoli[centrini[i].gomitoliConnessi[j]].nEntranti--;
        }
        sommaGomitoli += centrini[i].k_centrino;
        centrini[i].k_centrino = 0;
    }

    for (size_t i = 0; i < gomitoli.size(); ++i)
    {
        gomitoli[i].nEntranti = nEntrantiOriginali[i];
    }

    return sommaGomitoli;
}

vector<Centrino> calcolaCoeffEOrdinaMediana(vector<Centrino> &centrini, vector<Gomitolo> &gomitoli, vector<pair<int, double>> &coppie, int &minK, ofstream &outputFile)
{
    for (int i = 0; i < c; i++)
    {
        float coeff = 0.0f;
        for (const auto &gomitolo : centrini[i].gomitoliConnessi)
        {
            coeff += gomitolo;
        }

        if (coeff != 0 && centrini[i].nUscenti > 0)
        {
            if (centrini[i].gomitoliConnessi.size() % 2 != 0)
            {
                coeff = centrini[i].gomitoliConnessi[centrini[i].gomitoliConnessi.size() / 2];
            }
            else
            {
                coeff = (centrini[i].gomitoliConnessi[centrini[i].gomitoliConnessi.size() / 2] +
                         centrini[i].gomitoliConnessi[(centrini[i].gomitoliConnessi.size() / 2) - 1]) / 2.0;
            }
        }

        coppie.emplace_back(i, coeff);
    }
    sort(coppie.begin(), coppie.end(), [](const pair<int, double> &a, const pair<int, double> &b)
         { return a.second < b.second; });

    vector<Centrino> centriniOrdinati;
    for (const auto &coppia : coppie)
    {
        centriniOrdinati.push_back(centrini[coppia.first]);
    }

    int k = calcolaTuttiK(centriniOrdinati, gomitoli);
    if (k > minK)
        return centrini;
    minK = k;
    outputFile << minK << endl;
    for (const auto &centrino : centriniOrdinati)
    {
        outputFile << centrino.id << " ";
    }
    outputFile << endl
               << "***" << endl;
    return centriniOrdinati;
}

vector<Centrino> calcolaCoeffEOrdinaMedia(vector<Centrino> &centrini, vector<Gomitolo> &gomitoli, vector<pair<int, double>> &coppie, int &minK, ofstream &outputFile, int fattoreDivisivo)
{
    for (int i = 0; i < c; i++)
    {
        float coeff = accumulate(centrini[i].gomitoliConnessi.begin(), centrini[i].gomitoliConnessi.end(), 0.0f);

        if (coeff != 0 && centrini[i].nUscenti > 0)
        {
            coeff /= centrini[i].nUscenti;
            int range = centrini[i].gomitoliConnessi.back() - centrini[i].gomitoliConnessi.front();
            double offset = (coeff - centrini[i].gomitoliConnessi.front()) / range;
            coeff += (offset - 0.5) * coeff / fattoreDivisivo;
        }

        coppie.emplace_back(i, coeff);
    }
    sort(coppie.begin(), coppie.end(), [](const pair<int, double> &a, const pair<int, double> &b)
         { return a.second < b.second; });

    vector<Centrino> centriniOrdinati;
    for (const auto &coppia : coppie)
    {
        centriniOrdinati.push_back(centrini[coppia.first]);
    }

    int k = calcolaTuttiK(centriniOrdinati, gomitoli);
    if (k > minK)
        return centrini;
    minK = k;
    outputFile << minK << endl;
    for (const auto &centrino : centriniOrdinati)
    {
        outputFile << centrino.id << " ";
    }
    outputFile << endl
               << "***" << endl;
    return centriniOrdinati;
}

vector<Centrino> calcolaCoeffEOrdinaMix(vector<Centrino> &centrini, vector<Gomitolo> &gomitoli, vector<pair<int, double>> &coppie, int &minK, ofstream &outputFile)
{
    for (int i = 0; i < c; i++)
    {
        float coeff = accumulate(centrini[i].gomitoliConnessi.begin(), centrini[i].gomitoliConnessi.end(), 0.0f);

        if (coeff != 0 && centrini[i].nUscenti > 0)
        {
            coeff /= centrini[i].nUscenti;
            if (centrini[i].gomitoliConnessi.size() % 2 != 0)
            {
                coeff = centrini[i].gomitoliConnessi[centrini[i].gomitoliConnessi.size() / 2];
            }
            else
            {
                coeff = (centrini[i].gomitoliConnessi[centrini[i].gomitoliConnessi.size() / 2] +
                         centrini[i].gomitoliConnessi[(centrini[i].gomitoliConnessi.size() / 2) - 1]) / 2.0;
            }
        }

        coppie.emplace_back(i, coeff);
    }
    sort(coppie.begin(), coppie.end(), [](const pair<int, double> &a, const pair<int, double> &b)
         { return a.second < b.second; });

    vector<Centrino> centriniOrdinati;
    for (const auto &coppia : coppie)
    {
        centriniOrdinati.push_back(centrini[coppia.first]);
    }

    int k = calcolaTuttiK(centriniOrdinati, gomitoli);
    if (k > minK)
        return centrini;
    minK = k;
    outputFile << minK << endl;
    for (const auto &centrino : centriniOrdinati)
    {
        outputFile << centrino.id << " ";
    }
    outputFile << endl
               << "***" << endl;
    return centriniOrdinati;
}

void swap(vector<Centrino> &centrini, int i, int j)
{
    if (i < j)
    {
        Centrino temp = centrini[i];
        for (int k = i; k < j; k++)
        {
            centrini[k] = centrini[k + 1];
        }
        centrini[j] = temp;
    }
    else
    {
        Centrino temp = centrini[i];
        for (int k = i; k > j; k--)
        {
            centrini[k] = centrini[k - 1];
        }
        centrini[j] = temp;
    }
}

void swapInfinity(vector<Centrino> &centrini, vector<Gomitolo> &gomitoli, ofstream &outputFile, int &minK, int range)
{
    // partendo dal primo avanza randomizzando con un range in base ai tre valori c,g,A

    for (int i = 0; i < c - 1; i+=rand()%2+1)
    {
        int random;
        //cout<<"range "<<range<<" i: "<<i<<endl;
        random = (rand() % range-1)+1;
        int end = i + random;
        if (end > c - 1)
        {
            end = c - 1;
        }
        

        vector<Centrino> centriniCopy(centrini.begin() + i, centrini.begin() + end + 1);
        //cout<<"range "<<range<<" i: "<<i<<" end: "<<end<<endl;
        int kPrimo = calcolaTuttiK(centriniCopy, gomitoli);
        swap(centriniCopy, 0, centriniCopy.size() - 1);
        int kSecondo = calcolaTuttiK(centriniCopy, gomitoli);
        //cout<<"kPrimo: "<<kPrimo<<" kSecondo: "<<kSecondo<<endl;
        int k = minK+1; //per non farlo entrare nella stampa sempre
        if(kPrimo > kSecondo)
        {
            swap(centrini, i, end);
            k = minK - kPrimo + kSecondo;
        }
        
        if (k < minK)
        {
            minK = k;
            outputFile << k << endl;
            for (int i = 0; i < c; i++)
            {
                outputFile << centrini[i].id << " ";
            }
            outputFile << endl
                       << "***" << endl;
            // i==end;
        }
    }
}

int calcoloKA(int a)
{
    if (a < 1000)
        return 4;
    else if (a < 2000)
        return 9;
    else if (a < 3500)
        return 13;
    else if (a < 5000)
        return 18;
    else if (a < 7500)
        return 29;
    else if (a < 10000)
        return 35;
    else if (a < 15000)
        return 55;
    else if (a < 23000)
        return 84;
    else if (a < 30000)
        return 120;
    else if (a < 40000)
        return 140;
    else
        return 165;
}

int calcoloKC(int c)
{
    if (c < 400)
        return 4;
    else if (c < 500)
        return 11;
    else if (c < 600)
        return 18;
    else if (c < 750)
        return 31;
    else if (c < 900)
        return 46;
    else if (c < 1100)
        return 62;
    else if (c < 1300)
        return 84;
    else if (c < 1500)
        return 101;
    else if (c < 1700)
        return 120;
    else
        return 165;
}

void controllaDoppioni(vector<Centrino> centrini, vector<pair<int, double>> &coppie, int k, ofstream &outputFile, vector<Gomitolo> gomitoli)
{
    int gruppo = coppie[0].second;
    int start = 0;
    int end = 0;
    for (int i = 1; i < centrini.size(); i++)
    {
        if (int(coppie[i].second) == gruppo)
        {
            end++;
        }
        else
        {
            cout << "Gruppo: " << gruppo << " n valori: " << end - start + 1 << endl;
            start = i;
            end = i;
            gruppo = coppie[i].second;
        }
    }
}

int main()
{
    ifstream inputFile("input.txt");   // Replace "input.txt" with the actual input file name
    ofstream outputFile("output.txt"); // Replace "output.txt" with the actual output file name

    int A;

    if (inputFile.is_open() && outputFile.is_open())
    {
        // Inizializza dati
        vector<Centrino> centrini;
        vector<Gomitolo> gomitoli;

        inputFile >> c >> g >> A;
        for (int i = 0; i < c; i++)
        {
            centrini.push_back(Centrino(i));
        }
        for (int i = 0; i < g; i++)
        {
            gomitoli.push_back(Gomitolo());
        }

        for (int i = 0; i < A; i++)
        {
            int idCentrino, idGomitolo;
            inputFile >> idCentrino >> idGomitolo;
            idGomitolo -= c;
            centrini[idCentrino].addGomitolo(idGomitolo);
            gomitoli[idGomitolo].addEntrante();
        }

        for (int i = 0; i < c; i++) // devo ordinare i gomitoli connessi
        {
            centrini[i].ordinaGomitoli();
        }
        int k = calcolaTuttiK(centrini, gomitoli);
        outputFile << k << endl;

        for (int i = 0; i < c; i++)
        {
            outputFile << centrini[i].id << " ";
        }
        outputFile << endl
                   << "***" << endl;

        int minK = k;

        vector<pair<int, double>> coppie;
        centrini = calcolaCoeffEOrdinaMediana(centrini, gomitoli, coppie, minK, outputFile);
        
        
        for (int i = 512; i < 10000; i *= 2)
        {
            vector<pair<int, double>> coppie;
            centrini = calcolaCoeffEOrdinaMedia(centrini, gomitoli, coppie, minK, outputFile, i);
        }
        for (int i = 512; i < 10000; i *= 2)
        {
            vector<pair<int, double>> coppie;
            centrini = calcolaCoeffEOrdinaMix(centrini, gomitoli, coppie, minK, outputFile);
        }
        
        double div = 0.0;
        // controllaDoppioni(centrini, coppie, minK, outputFile, gomitoli);
        
        //range = 2;
        // cout << "Range: " << range << endl;
        //outputFile << "ciao" << endl;
        int ka=calcoloKA(A);
        int kc=calcoloKC(c);
        int kgc=calcoloKC(g);
        for (int i = 0; i < 1000000; i++) // brute force
        {
            //cout << "i: " << i << endl;
            div = div + 0.1;
            int range = ( ka* div + kc * div +  kgc* div) / 3;
            if (range < 2)
                range = 2;
            //cout<<"range: "<<range<<endl;
            swapInfinity(centrini, gomitoli, outputFile, minK, range);
        }

        // Close the files
        inputFile.close();
        outputFile.close();
    }
    else
    {
        std::cout << "Failed to open the input or output file." << std::endl;
    }

    return 0;
}
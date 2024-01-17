///Author: Michał Bernacki-Janson

#include <iostream>
#include <chrono>
#include <fstream>
#include <vector>
#include "GraphMatrix.h"

using namespace std;

int main()
{
    fstream in;
    fstream out;
    //
    ///otwieranie plik konfiguracyjnego, wczytawnie kolejnych lini, obliczanie ich liczby i wykrywanie nazwy pliku wyjściowego
    in.open("start.ini", ios::in);
    if(!in.is_open()){
        cout<<"Nie można otworzyć pliku konfiguracyjnego!"<<endl;
        return 1;
    }
    int tests = 0;
    string line;
    string csvName;
    vector<string> namesOfTests;
    while(getline(in, line)){
        if(in.eof()){
            csvName = line;
        } else{
            namesOfTests.push_back(line);
            tests++;
        }
    }
    in.close();
    //
    out.open(csvName, ios::out);
    if(!out.is_open()){
        cout<<"Nie można utworzyć pliku wynikowego"<<endl;
        return 0;
    }
    for(int t =0; t<tests;t++)
    {
        auto temp = namesOfTests.at(t).find(' ');
        //
        ///ekstrakcja nazwy pliku testu z lini
        GraphMatrix matrix = GraphMatrix(namesOfTests.at(t).substr(0, temp));
        //
        double timeAvg = 0.0;
        //
        ///ekstrakcja liczby powtórzeń danego testu
        int b1 = stoi(namesOfTests.at(t).substr(temp, namesOfTests.at(t).substr(temp).find(' ')-temp));
        //
        //int minSum;
        //int *minSeq;
        for (int k = 0; k < b1; k++) {

            //
            /*///inicjalizacja zmiennych
            int *seq = new int[matrix.v-1];//matrix.v -> liczba wierzchołków w grafie
            for (int i = 0; i < matrix.v-1; ++i) {
                seq[i] = i+1;
            }
            minSum = INT32_MAX;
            minSeq = new int[matrix.v];
            int sum;*/
            auto t1 = std::chrono::high_resolution_clock::now();//pomiar czasu

            //----------------//
            //główny algorytm
            //--------------//

            ///tablica feromonów
            auto feromons = new double*[matrix.v];
            for (int i = 0; i < matrix.v; ++i) {
                feromons[i] = new double [matrix.v];
                for (int j = 0; j < matrix.v; ++j) feromons[i][j] = 0.2;
            }

            int iterations = 1000;
            for (int i = 0; i < iterations; ++i) {
                for (int j = 0; j < matrix.v; ++j) { ///dla j-tej mrówki

                }


            }



            auto t2 = std::chrono::high_resolution_clock::now();
            auto tDiv = std::chrono::duration_cast<std::chrono::duration<double>>(t2 - t1);
            //matrix.display();
            //cout << endl << minSum << endl;
            /*for (int i = 0; i < matrix.v-1; ++i) {
                cout << minSeq[i] << " ";
            }*/
            cout << tDiv.count();
            out << tDiv.count() << endl;
            timeAvg += tDiv.count();
            //delete[] seq;
            //if(k!=b1-1)delete[] minSeq;
            //----------------//
        }
        timeAvg /= (double) b1;
       /* out << namesOfTests.at(t).substr(0, temp) <<";" << timeAvg<<";" << minSum<< "; [0 ";
        for (int i = 0; i < matrix.v-1; ++i) {
            out << minSeq[i] << " ";
        }
        out<<"0]"<<endl<<endl;
        delete[] minSeq;*/
    }
    out.close();
    //system("pause");
    return 0;
}

///Author: Michał Bernacki-Janson

#include <iostream>
#include <chrono>
#include <random>
#include <fstream>
#include <vector>
#include <list>
#include <queue>
#include <math.h>
#include <numeric>
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
    for(int t = 0; t<tests;t++)
    {
        auto tempString = namesOfTests.at(t).find(' ');
        //
        ///ekstrakcja nazwy pliku testu z lini
        GraphMatrix matrix = GraphMatrix(namesOfTests.at(t).substr(0, tempString));
        //

        //
        ///ekstrakcja liczby powtórzeń danego testu
        int b1 = stoi(namesOfTests.at(t).substr(tempString, namesOfTests.at(t).substr(tempString).find(' ') - tempString));
        vector<pair<pair<int, int>, bool>> vars;
        vars.emplace_back(pair(1,1), true);
        vars.emplace_back(pair(1,2), true);
        vars.emplace_back(pair(1,3), true);
        vars.emplace_back(pair(1,5), true);
        vars.emplace_back(pair(2,1), true);
        vars.emplace_back(pair(3,1), true);
        vars.emplace_back(pair(5,1), true);

        for(auto var : vars){
            double timeAvg = 0.0;
            int avgSum = 0;
            for (int k = 0; k < b1; k++) {

                auto t1 = std::chrono::high_resolution_clock::now();//pomiar czasu
                auto t2 = std::chrono::high_resolution_clock::now();

                //----------------//
                //główny algorytm
                //--------------//

                unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
                std::mt19937 gen(seed);
                std::uniform_real_distribution<double> rdDouble(0.0, 1.0);

                int a = var.first.first, b = var.first.second;

                ///tablica feromonów
                auto feromons = new double*[matrix.v];
                for (int i = 0; i < matrix.v; ++i) {
                    feromons[i] = new double [matrix.v];
                    for (int j = 0; j < matrix.v; ++j) feromons[i][j] = 0.2;
                }

                auto seq = new int*[matrix.v];
                for (int i = 0; i < matrix.v; ++i) {
                    seq[i] = new int [matrix.v + 1];
                    for (int j = 0; j < matrix.v + 1; ++j) seq[i][j] = -1;
                }

                int best = INT32_MAX;
                int* bestSeq = nullptr;
                int iterations = 1000;
                auto iterWithOutChange = 0;
                for (int i = 0; iterWithOutChange<20 &&
                (std::chrono::duration_cast<std::chrono::duration<double>>(std::chrono::high_resolution_clock::now() - t1)
                < (std::chrono::duration<double>)600.0); ++i) {

                    bool change = false;
                    //cout<<"iteration: "<<i<<endl;
                    for (int j = 0; j < matrix.v; ++j) {
                        for (int l = 0; l < matrix.v + 1; ++l) seq[j][l] = 0;
                    }

                    for (int j = 0; j < matrix.v; ++j) { ///dla j-tej mrówki

                        list<int> remainingNodes;
                        for (int l = 0; l < matrix.v; ++l) {
                            if(l != j)remainingNodes.push_back(l);
                        }
                        int lastNode = j;
                        seq[j][1] = j;
                        for (int l = 0; l < matrix.v-1; ++l) {
                            /*cout<<endl<<endl;
                            for(auto g:remainingNodes) cout<<g<<" ";*/
                            double sum = 0;
                            queue<pair<double, int>> probabilities;
                            for(auto node:remainingNodes){
                                auto temp = pow(feromons[lastNode][node], a) * pow(200.0 / matrix.matrix[lastNode][node], b);
                                sum += temp;
                                probabilities.emplace(temp, node);
                            }

                            auto rdN = rdDouble(gen);
                            pair<double, int> temp;
                            do {
                                temp = probabilities.front();
                                rdN -= temp.first / sum;
                                probabilities.pop();
                            }while (rdN > 0);
                            seq[j][0] += matrix.matrix[lastNode][temp.second];
                            remainingNodes.remove(temp.second);
                            seq[j][l + 2] = temp.second;
                            lastNode = temp.second;
                        }
                        seq[j][0] += matrix.matrix[lastNode][j];
                    }
                    for (int l = 0; l < matrix.v; ++l) {
                        for (int j = 0; j < matrix.v; ++j) feromons[l][j] *= 0.5;
                    }
                    for (int j = 0; j < matrix.v ; ++j) {
                        for (int l = 1; l < matrix.v; ++l) {
                            feromons[seq[j][l]][seq[j][l + 1]] += seq[j][0];
                        }
                        feromons[seq[j][matrix.v]][seq[j][1]] += seq[j][0];
                    }
                    for (int j = 0; j < matrix.v; ++j) {
                        if(seq[j][0] < best) {
                            best = seq[j][0];
                            bestSeq = seq[j];
                            cout<<best<<endl;
                            change = true;
                            t2 = std::chrono::high_resolution_clock::now();
                        }
                    }
                    if (change){
                        iterWithOutChange = 0;
                    }else iterWithOutChange++;


                    /*
                    cout<<endl<<endl;
                    for (int z = 0; z < matrix.v; ++z) {
                        for (int q = 0; q < matrix.v; ++q) cout<<seq[z][q]<<" ";
                        cout<<endl;
                    }*/
                }
                auto tDiv = std::chrono::duration_cast<std::chrono::duration<double>>(t2 - t1);
                //matrix.display();
                //cout << endl << minSum << endl;
                /*for (int i = 0; i < matrix.v-1; ++i) {
                    cout << minSeq[i] << " ";
                }*/
                cout << tDiv.count();
                out << tDiv.count() << ";" << best << "; [ ";
                for (int i = 1; i < matrix.v + 1; ++i) {
                   out << bestSeq[i] << " ";
                }
                out << "]" << endl;
                timeAvg += tDiv.count();
                avgSum += best;
            }
            timeAvg /= (double) b1;
            avgSum /= b1;
            out << namesOfTests.at(t).substr(0, tempString) <<";" << timeAvg<<";" << avgSum <<";" << var.first.first <<";" << var.first.second << endl <<endl;
        }



    }
    out.close();
    //system("pause");
    return 0;
}

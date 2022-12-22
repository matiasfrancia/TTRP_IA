#include <iostream>
#include "stdlib.h"
#include <cstdlib>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include "math.h"
#include <tuple>
#include <algorithm>
#include <numeric>
#include <ctime>
using namespace std;


struct client {
    int id;
    float x;
    float y;
    float demand;
    bool isTruck;
};

struct transport {
    int qTrucks;
    int qTrailers;
    float truckCapacity;
    float trailerCapacity;
    int nClients;
};

void printClient(client c) {
    cout << "\nClient " << c.id << ":" << endl;
    cout << "X: " << c.x << endl;
    cout << "Y: " << c.y << endl;
    cout << "Demand: " << c.demand << endl;
    cout << "Is Truck: " << c.isTruck << endl;
}

void printArray(std::vector<int> v) {
    cout << "[";
    for(int i = 0; i < (int) v.size(); i++)
        cout << v[i] << ",";
    cout << "]" << endl;
}

void printArrayClients(std::vector<client> v) {
    cout << "================= Clients array =================" << endl;
    for(int i = 0; i < (int) v.size(); i++)
        printClient(v[i]);
}

void printTransport(transport t) {
    cout << "\nTransport:" << endl;
    cout << "Quantity of trucks: " << t.qTrucks << endl;
    cout << "Quantity of trailers: " << t.qTrailers << endl;
    cout << "Truck capacity: " << t.truckCapacity << endl;
    cout << "Trailer capacity: " << t.trailerCapacity << endl;
    cout << "N clients: " << t.nClients << endl;
}

void splitClient(string line, std::vector<client>& v) {
    string arr[5]; 
    client clientObj;

    std::istringstream iss(line);
    for(int i = 0; i < 5; i++)
        iss >> arr[i];
        
    clientObj.id = stoi(arr[0]);
    clientObj.x = stof(arr[1]);
    clientObj.y = stof(arr[2]);
    clientObj.demand = stof(arr[3]);
    istringstream(arr[4]) >> clientObj.isTruck;

    v.push_back(clientObj);
}

void splitTransport(string line, transport& transportObj) {
    string arr[5];

    std::istringstream iss(line); 
    for(int i = 0; i < 5; i++)
        iss >> arr[i];
        
    transportObj.qTrucks = stoi(arr[0]);
    transportObj.truckCapacity = stof(arr[1]);
    transportObj.qTrailers = stoi(arr[2]);
    transportObj.trailerCapacity = stof(arr[3]);
    transportObj.nClients = stoi(arr[4]);
}

template <typename T>
void printMatrix(std::vector<std::vector<T>> matrix) {
    for(int i = 0; i < (int) matrix.size(); i++) {
        for(int j = 0; j < (int) matrix[i].size(); j++) {
            cout << round(matrix[i][j]) << " ";
        }
        cout << endl;
    }
}

float calcEuclideanDistance(float x1, float y1, float x2, float y2) {
    float x = x1 - x2;
    float y = y1 - y2;
    return sqrt(pow(x, 2) + pow(y, 2));
}

void generateDistanceMatrix(std::vector<vector<float>> &matrix, std::vector<client> clients) {
    for(int i = 0; i < (int) matrix.size(); i++) {
        for(int j = 0; j < (int) matrix[i].size(); j++) {
            matrix[i][j] = calcEuclideanDistance(clients[i].x, clients[i].y, clients[j].x, clients[j].y);
        }
    }
}

// función que obtiene cantidad de clientes de camión que hay
std::tuple<int, std::vector<int>, int, std::vector<int>> getTruckClients(
    transport transportData, std::vector<client> clients) {
    
    int qTruck = 0;
    int qVehicle = 0;
    std::vector<int> truckClients, vehicleClients;

    for(int i = 1; i < transportData.nClients+1; i++) {
        if(clients[i].isTruck == 1) {
            truckClients.push_back(i);
            qTruck++;
        }
        else {
            vehicleClients.push_back(i);
            qVehicle++;
        }
    }

    return {qTruck, truckClients, qVehicle, vehicleClients};
}

void fillRow(std::vector<int> &row, int qCols, int colRef, int startPoint, 
    std::vector<client> clients, std::vector<int> refList) {

    for(int i = startPoint*colRef; i < startPoint*colRef+qCols; i++) {
        row.push_back(clients[refList[i]].id);
    }

    row.insert(row.begin() + 1, 0);
    row.insert(row.end(), 0);
}

void printArrayString(std::vector<string> vec) {
    cout << "[";
    for(int i = 0; i < (int) vec.size(); i++) {
        cout << vec[i] << ", ";
    }
    cout << "]" << endl;
}

void printArrayTuples(std::vector<std::tuple<int, int, int>> vector) {
    for(int i = 0; i < (int) vector.size(); i++) {
        cout << "{" << get<0>(vector[i]) << ", " << get<1>(vector[i]) << ", " << get<2>(vector[i]) << "}" << ", ";
    }
    cout << endl;
}

std::vector<string> splitByComma(string s) {

    string del = ",";
    int start, end = -1*del.size();
    std::vector<string> subrouteVec;
    
    do {
        start = end + del.size();
        end = s.find(del, start);
        subrouteVec.push_back(s.substr(start, end - start));
    } while (end != -1);

    return subrouteVec;
}

std::tuple<int, int, int> getSubrouteTuple(string subroute) {

    if(subroute.length() == 0) return {-1, -1, -1};

    string del = ",";
    int start, end = -1*del.size();
    std::vector<string> subrouteVec;
    
    do {
        start = end + del.size();
        end = subroute.find(del, start);
        string sb = subroute.substr(start, end - start);
        if(sb != "") subrouteVec.push_back(sb);

    } while (end != -1);

    return make_tuple(stoi(subrouteVec[0]), stoi(subrouteVec[1]), stoi(subrouteVec[2]));
}

// esta función retorna un vector de tupla con pares ordenados con cada una de las subrutas de una ruta
std::vector<std::tuple<int, int, int>> getSubroutesVector(string subroutesRow) {

    std::vector<std::tuple<int, int, int>> subroutesVec;
    subroutesVec.clear();

    // subroutes = "P1,2,3P1,2,3P1,2,3";
    string subroutes = subroutesRow.erase(0, 1);
    string del = "P";

    int start, end = -1*del.size();

    do {
        start = end + del.size();
        end = subroutes.find(del, start);
        string subroute = subroutes.substr(start, end - start);

        std::tuple<int, int, int> tupleSR = getSubrouteTuple(subroute);
        if(get<0>(tupleSR) != -1) subroutesVec.push_back(tupleSR);
    } while (end != -1);

    return subroutesVec;
}

// agregamos una subruta al string de subrutas ["P root1 pos11 pos12 P root2 pos21 pos22 ...", ...]
void addSubroute(std::vector<string> &subroutes, int i, int root, int pos1, int pos2) {
    string prevString = 'P' + to_string(root) + ',' + to_string(pos1) + ',' + to_string(pos2);
    subroutes[i].append(prevString);
}

// modificamos una subruta
void updateSubroute(std::vector<string> &subroutes, int route, int root, int newRoot, 
    int pos1, int newPos1, int pos2, int newPos2) {

    string prevString = 'P' + to_string(root) + ',' + to_string(pos1) + ',' + to_string(pos2);
    string newString = 'P' + to_string(newRoot) + ',' + to_string(newPos1) + ',' + to_string(newPos2);
    int position = subroutes[route].find(prevString);
    subroutes[route].replace(position, prevString.length(), newString);
}

int getSubrouteDemand(std::vector<string> subroute, std::vector<client> clients, std::vector<int> route) {

    // calculamos la capacidad total desde 'inicio' hasta 'termino'
    int demand = 0;
    for(int i = stoi(subroute[1])+1; i < stoi(subroute[2]); i++) {
        demand += clients[route[i]].demand;
    }
    return demand;
}

std::vector<float> getTotalDemandSubroutes(string subroutes, std::vector<client> clients, std::vector<int> route) {

    // subroutes = "P1,2,3P1,2,3P1,2,3";
    subroutes = subroutes.erase(0, 1);
    string del = "P";
    std::vector<float> demandSubroutes;

    int start, end = -1*del.size();

    do {
        start = end + del.size();
        end = subroutes.find(del, start);
        string subroute = subroutes.substr(start, end - start);
        demandSubroutes.push_back(getSubrouteDemand(splitByComma(subroute), clients, route));
    } while (end != -1);

    return demandSubroutes;
}

float f_eval(std::vector<std::vector<int>> matrixSol, std::vector<std::vector<float>> distM,
    std::vector<client> clients, transport transportData, std::vector<string> subroutes) {

    float eval = 0;
    float routeDemand = 0;

    // definimos las penalizaciones
    float penaltyRoute = 100;
    float penaltySubroute = 50;

    // sumamos las distancias de entre cada uno de los nodos de cada ruta y entre todas las rutas
    // y le agregamos una penalización a la función de evaluación por restricciones insatisfechas

    for(int i = 0; i < (int) matrixSol.size(); i++) {
        routeDemand = 0;
        for(int j = 1; j < (int) matrixSol[i].size()-1; j++) {

            // sumamos las distancias entre cada nodo de una ruta
            int posA = matrixSol[i][j];
            int posB = matrixSol[i][j+1];
            eval += distM[posA][posB];

            // sumamos la demanda de la ruta
            routeDemand += clients[posA].demand;
        }

        std::vector<std::tuple<int, int, int>> subroutesVec;
        if(subroutes[i].size() > 0) {
            subroutesVec = getSubroutesVector(subroutes[i]);

            // restamos a la demanda una vez todas las raíces de subruta
            
            for(int k = 0; k < (int) subroutesVec.size(); k++) {
                routeDemand -= clients[get<0>(subroutesVec[k])].demand;
            }
        }

        // cout << "Route demand of " << i << ": " << routeDemand << endl;

        float capacity = 0;
        
        // demanda ruta camión supera capacidad camión
        if(matrixSol[i][0] == 1) {
            capacity = transportData.truckCapacity;
        }
        // demanda ruta vehículo supera capacidad camión más trailer
        else {
            capacity = transportData.truckCapacity + transportData.trailerCapacity;
        }

        if(routeDemand > capacity) {
            eval += (routeDemand - capacity) * penaltyRoute;
        }

        // obtenemos la suma de las demandas de las subrutas
        std::vector<float> subroutesDemand;
        if(subroutes[i].length() > 0) {
            subroutesDemand = getTotalDemandSubroutes(subroutes[i], clients, matrixSol[i]);
        }

        for(int j = 0; j < (int) subroutesDemand.size(); j++) {
            if(subroutesDemand[i] > transportData.truckCapacity) {
                eval += (subroutesDemand[i] - transportData.truckCapacity) * penaltySubroute;
            }
        }
    }

    return eval;
}

// Construimos una solución inicial aleatoria
std::vector<std::vector<int>> first_phase(transport transportData, std::vector<client> clients) {

    // [[1, 2, 3],[4, 5, 6, 7, 8, 6, 9], [....]]
    int qTruckC, qVehicleC;
    std::vector<int> truckClients, vehicleClients;
    std::tie(qTruckC, truckClients, qVehicleC, vehicleClients) = getTruckClients(transportData, clients);

    // Crear una fila de la matriz por cada camión del que se dispone
    std::vector<std::vector<int>> matrixSol(
            transportData.qTrucks,
            vector<int> (1, 0)
        );

    // si es cliente de camión, le cambiamos el 0 de la primera columna por 1, en caso contrario se deja igual
    for(int i = transportData.qTrailers; i < transportData.qTrucks; i++) {
        matrixSol[i][0] = 1;
    }

    // dividir en módulo la cantidad de clientes de vehículos por la cantidad de trailers k y obtener n + r
    int qVehicleRow = (int) qVehicleC / transportData.qTrailers;
    int qVehicleRemaind = qVehicleC % transportData.qTrailers;

    // llenar las primeras k-1 filas con n clientes de vehículo, y con n + r en la fila k
    int qCols = 0;
    // printArrayClients(clients);

    for(int i  = 0; i < transportData.qTrailers; i++) {

        if(i == transportData.qTrailers - 1) {
            qCols = qVehicleRow + qVehicleRemaind;
        }
        else {
            qCols = qVehicleRow;
        }
        fillRow(matrixSol[i], qCols, qVehicleRow, i, clients, vehicleClients);
    }

    // dividir en módulo la cantidad de clientes de camión con la resta de camiones con los trailer q - r en módulo
    int qTruckRow = (int) qTruckC / (transportData.qTrucks - transportData.qTrailers);
    int qTruckRemaind = qTruckC % (transportData.qTrucks - transportData.qTrailers);

    // llenar desde k+1 hasta q-r-1 con clientes de camión, el q-r se llena con lo que quedó
    for(int i = 0; i < (transportData.qTrucks - transportData.qTrailers); i++) {

        if(i == transportData.qTrucks - transportData.qTrailers - 1) {
            qCols = qTruckRow + qTruckRemaind;
        }
        else {
            qCols = qTruckRow;
        }
        fillRow(matrixSol[i+transportData.qTrailers], qCols, qTruckRow, i, clients, truckClients);
    }

    return matrixSol;
}

// obtenemos un número perteneciente a diversos intervalos, se le entrega [<root1, start1, end1>, <...>]
int getRandomInVehicleRoute(std::vector<std::tuple<int, int, int>> intervals, int length) {

    int totalRange = 0;
    std::vector<int> sortedSubroutes;

    for(int i = 0; i < (int) intervals.size(); i++) {
        sortedSubroutes.push_back(get<1>(intervals[i]));
        sortedSubroutes.push_back(get<2>(intervals[i]));
    }

    sortedSubroutes.push_back(length-1);

    totalRange = length - 2;
    sort(sortedSubroutes.begin(), sortedSubroutes.end());

    for(int i = 0; i < (int) sortedSubroutes.size()/2; i++) {
        totalRange -= sortedSubroutes[2*i+1] - sortedSubroutes[2*i] + 1;
    }

    int randomNumber, lowerBound, upperBound;
    randomNumber = (rand() % totalRange) + 1;
    lowerBound = 1;
    upperBound = sortedSubroutes[0];

    for(int i = 0; i < (int) sortedSubroutes.size()/2+1; i++) {
        if(randomNumber <= upperBound - lowerBound) {
            if(i == 0) return randomNumber;
            return lowerBound + randomNumber - 1;
        }
        else {
            randomNumber -= upperBound - lowerBound;
            lowerBound = sortedSubroutes[2*i+1] + 1;
            upperBound = sortedSubroutes[2*i+2];
        }
    }

    return {};
}

void second_phase(std::vector<std::vector<int>> &matrixSol, float *initialEval, transport transportData,
    std::vector<string> &subroutes, std::vector<std::vector<float>> distancesM, std::vector<client> clients) {
    
    // probabilidad de que haga swap
    int prob = 75;

    // buscamos un número aleatorio que no pertenezca a una subruta para cambiarlo a otra ruta
    int prevRoute, nextRoute;
    prevRoute = rand() % (transportData.qTrucks - transportData.qTrailers + 1);

    if((rand() % 100) < prob) nextRoute = rand() % (transportData.qTrucks - transportData.qTrailers + 1);
    else nextRoute = prevRoute;

    // buscamos un número aleatorio dentro de diversos rangos (que no pertenezca a una subruta)
    int prevPos = getRandomInVehicleRoute(getSubroutesVector(subroutes[prevRoute]), matrixSol[prevRoute].size()-1) + 1;
    int nextPos = getRandomInVehicleRoute(getSubroutesVector(subroutes[nextRoute]), matrixSol[nextRoute].size()-1) + 1;

    // hacemos el swap entre los nodos
    int prevNode = matrixSol[prevRoute][prevPos];
    int nextNode = matrixSol[nextRoute][nextPos];

    // hay que hacer la distinción si son de la misma ruta o no
    if(prevRoute != nextRoute) {
        matrixSol[prevRoute].erase(matrixSol[prevRoute].begin() + prevPos);
        matrixSol[nextRoute].erase(matrixSol[nextRoute].begin() + nextPos);
        matrixSol[prevRoute].insert(matrixSol[prevRoute].begin() + prevPos, nextNode);
        matrixSol[nextRoute].insert(matrixSol[nextRoute].begin() + nextPos, prevNode);
    }
    else {
        swap(matrixSol[prevRoute][prevPos], matrixSol[prevRoute][nextPos]);
    }

    float eval = f_eval(matrixSol, distancesM, clients, transportData, subroutes);

    // evaluamos si mejora la solución
    if(eval < *initialEval) {
        *initialEval = eval;
    }
    // si la solución no mejora, lo deshacemos
    else {
        // deshacemos los cambios
        if(prevRoute != nextRoute) {
            matrixSol[prevRoute].erase(matrixSol[prevRoute].begin() + prevPos);
            matrixSol[nextRoute].erase(matrixSol[nextRoute].begin() + nextPos);
            matrixSol[prevRoute].insert(matrixSol[prevRoute].begin() + prevPos, prevNode);
            matrixSol[nextRoute].insert(matrixSol[nextRoute].begin() + nextPos, nextNode);
        }
        else {
            swap(matrixSol[prevRoute][prevPos], matrixSol[prevRoute][nextPos]);
        }
    }
}

void third_phase_0(std::vector<std::vector<int>> &matrixSol, float *initialEval, transport transportData,
    std::vector<string> &subroutes, std::vector<std::vector<float>> distancesM, std::vector<client> clients) {

    // MOVIMIENTO 0: reordenar rutas de camión
    // escogemos dos rutas de camión

    // probabilidad de que haga swap en la misma ruta (generalmente es más, pero mínimo será prob)
    int prob = 75;

    // buscamos un número aleatorio que no pertenezca a una subruta para cambiarlo a otra ruta
    int prevRoute = rand() % (transportData.qTrailers-1) + transportData.qTrucks - transportData.qTrailers + 1;
    int nextRoute;

    if((rand() % 100) < prob) nextRoute = rand() % (transportData.qTrailers-1) + transportData.qTrucks - transportData.qTrailers + 1;
    else nextRoute = prevRoute;

    // buscamos un número aleatorio dentro de diversos rangos (que no pertenezca a una subruta)
    int prevPos = rand() % (matrixSol[prevRoute].size() - 3) + 2;
    int nextPos = rand() % (matrixSol[nextRoute].size() - 3) + 2;

    // hacemos el swap entre los nodos
    int prevNode = matrixSol[prevRoute][prevPos];
    int nextNode = matrixSol[nextRoute][nextPos];

    // hay que hacer la distinción si son de la misma ruta o no
    if(prevRoute != nextRoute) {
        matrixSol[prevRoute].erase(matrixSol[prevRoute].begin() + prevPos);
        matrixSol[nextRoute].erase(matrixSol[nextRoute].begin() + nextPos);
        matrixSol[prevRoute].insert(matrixSol[prevRoute].begin() + prevPos, nextNode);
        matrixSol[nextRoute].insert(matrixSol[nextRoute].begin() + nextPos, prevNode);
    }
    else {
        swap(matrixSol[prevRoute][prevPos], matrixSol[prevRoute][nextPos]);
    }

    float eval = f_eval(matrixSol, distancesM, clients, transportData, subroutes);

    // evaluamos si mejora la solución
    if(eval < *initialEval) {
        *initialEval = eval;
    }
    // si la solución no mejora, lo deshacemos
    else {
        // deshacemos los cambios
        if(prevRoute != nextRoute) {
            matrixSol[prevRoute].erase(matrixSol[prevRoute].begin() + prevPos);
            matrixSol[nextRoute].erase(matrixSol[nextRoute].begin() + nextPos);
            matrixSol[prevRoute].insert(matrixSol[prevRoute].begin() + prevPos, prevNode);
            matrixSol[nextRoute].insert(matrixSol[nextRoute].begin() + nextPos, nextNode);
        }
        else {
            swap(matrixSol[prevRoute][prevPos], matrixSol[prevRoute][nextPos]);
        }
    }
}

void third_phase_1(std::vector<std::vector<int>> &matrixSol, float *initialEval, transport transportData,
    std::vector<string> &subroutes, std::vector<std::vector<float>> distancesM, std::vector<client> clients) {
    
    // escogemos una ruta de camión y una ruta de vehículo
    int prevRoute = rand() % (transportData.qTrailers-1) + transportData.qTrucks - transportData.qTrailers + 1;
    int nextRoute = rand() % (transportData.qTrucks - transportData.qTrailers);

    // escogemos un nodo dentro de la ruta de camión y un nodo dentro de la ruta de vehículo
    int prevPos = rand() % (matrixSol[prevRoute].size() - 3) + 2;
    int nextPos = getRandomInVehicleRoute(getSubroutesVector(subroutes[nextRoute]), matrixSol[nextRoute].size()-1) + 1;

    std::vector<std::tuple<int, int, int>> subroutesVec = getSubroutesVector(subroutes[nextRoute]);
    bool goodNode = true;

    // {raiz, inicio, termino}, ..., {...}
    for(int i = 0; i < (int) subroutesVec.size(); i++) {
        if((nextPos >= get<1>(subroutesVec[i]) && nextPos <= get<2>(subroutesVec[i])) || 
        matrixSol[nextRoute][nextPos] == get<0>(subroutesVec[i])) goodNode = false;
    }
    if(goodNode && matrixSol[prevRoute].size() >= 5) {

        // creamos la subruta: eliminamos el nodo de la ruta de camión y lo agregamos en la ruta de vehículo
        // y le agregamos el nodo que estaba en esa posición nuevamente
        int prevNode = matrixSol[prevRoute][prevPos];
        int nextNode = matrixSol[nextRoute][nextPos];

        matrixSol[prevRoute].erase(matrixSol[prevRoute].begin() + prevPos);
        matrixSol[nextRoute].insert(matrixSol[nextRoute].begin() + nextPos, prevNode);
        matrixSol[nextRoute].insert(matrixSol[nextRoute].begin() + nextPos, nextNode);

        float eval = f_eval(matrixSol, distancesM, clients, transportData, subroutes);

        if(eval < *initialEval) {
            *initialEval = eval;
            // agregamos la subruta a nuestra lista de subrutas
            addSubroute(subroutes, nextRoute, nextNode, nextPos, nextPos+2);
        }
        else {
            matrixSol[nextRoute].erase(matrixSol[nextRoute].begin() + nextPos);
            matrixSol[nextRoute].erase(matrixSol[nextRoute].begin() + nextPos);
            matrixSol[prevRoute].insert(matrixSol[prevRoute].begin() + prevPos, prevNode);
        }
    }    
}

std::tuple<int, std::tuple<int, int, int>> getRandomInSubroutes(std::vector<std::tuple<int, int, int>> subroutesVec) {

    int range = 0;
    for(int i = 0; i < (int) subroutesVec.size(); i++) {
        range += get<2>(subroutesVec[i]) - get<1>(subroutesVec[i]) - 1;
    }

    int rawRandom = rand() % range;

    for(int i = 0; i < (int) subroutesVec.size(); i++) {
        int interval = get<2>(subroutesVec[i]) - get<1>(subroutesVec[i]) - 1;
        if(rawRandom < interval) return {get<1>(subroutesVec[i]) - 1 + rawRandom, subroutesVec[i]};
        rawRandom -= interval;
    }

    return {};
}

void third_phase_2(std::vector<std::vector<int>> &matrixSol, float *initialEval, transport transportData,
    std::vector<string> &subroutes, std::vector<std::vector<float>> distancesM, std::vector<client> clients) {

    // SEGUNDO MOVIMIENTO: insertar nuevos nodos de camión en subrutas

    // escogemos una ruta de camión y una ruta de vehículo
    int prevRoute, nextRoute;
    prevRoute = rand() % (transportData.qTrailers-1) + transportData.qTrucks - transportData.qTrailers + 1;
    nextRoute = rand() % (transportData.qTrucks - transportData.qTrailers);

    std::vector<std::tuple<int, int, int>> subroutesVec;
    if(subroutes[nextRoute].length() == 0) return;
    else subroutesVec = getSubroutesVector(subroutes[nextRoute]);

    // escogemos un nodo dentro de camión y un nodo de subruta
    int prevPos = rand() % (matrixSol[prevRoute].size() - 3) + 2;
    int nextPos, root, start, end;
    std::tuple<int, int, int> subrouteTuple;
    std::tie(nextPos, subrouteTuple) = getRandomInSubroutes(subroutesVec);
    std::tie(root, start, end) = subrouteTuple;

    // si mejora la solución, eliminar el nodo de la ruta de camión e insertarlo en la subruta
    int prevNode = matrixSol[prevRoute][prevPos];

    matrixSol[prevRoute].erase(matrixSol[prevRoute].begin() + prevPos);
    matrixSol[nextRoute].insert(matrixSol[nextRoute].begin() + nextPos, prevNode);

    float eval = f_eval(matrixSol, distancesM, clients, transportData, subroutes);

    if(eval < *initialEval) {
        *initialEval = eval;
        updateSubroute(subroutes, nextRoute, root, root, start, start, end, end+1);
    }
    else {
        matrixSol[prevRoute].insert(matrixSol[prevRoute].begin() + prevPos, prevNode);
        matrixSol[nextRoute].erase(matrixSol[nextRoute].begin() + nextPos);
    }
}

int getRandomInSubroute(std::tuple<int, int, int> subroute) {

    int range = get<2>(subroute) - get<1>(subroute) - 1;
    int rawRandom = rand() % range;

    return  get<1>(subroute) + 1 + rawRandom;
}

void third_phase_3(std::vector<std::vector<int>> &matrixSol, float *initialEval, transport transportData,
    std::vector<string> &subroutes, std::vector<std::vector<float>> distancesM, std::vector<client> clients) {
    
    // TERCER MOVIMIENTO: reordenar subrutas

    // escogemos una ruta al azar
    int route = rand() % (transportData.qTrucks - transportData.qTrailers);

    // escogemos una subruta de esa ruta
    std::vector<std::tuple<int, int, int>> subroutesVec;
    if(subroutes[route].length() <= 0) return;
    else subroutesVec = getSubroutesVector(subroutes[route]);

    int subroutePos = rand() % (subroutesVec.size());
    std::tuple<int, int, int> subroute = subroutesVec[subroutePos];

    // escogemos dos nodos de esa subruta
    int prevPos = getRandomInSubroute(subroute);
    int nextPos = getRandomInSubroute(subroute);

    // hacemos swap entre esos nodos
    swap(matrixSol[route][prevPos], matrixSol[route][nextPos]);

    // vemos si se mejora la función de evaluación
    float eval = f_eval(matrixSol, distancesM, clients, transportData, subroutes);

    // si mejora, se realiza el cambio
    if(eval < *initialEval) {
        *initialEval = eval;
    }
    else {
        swap(matrixSol[route][nextPos], matrixSol[route][prevPos]);
    }
}

void third_phase_4(std::vector<std::vector<int>> &matrixSol, float *initialEval, transport transportData,
    std::vector<string> &subroutes, std::vector<std::vector<float>> distancesM, std::vector<client> clients) {

    // CUARTO MOVIMIENTO: movimiento para cambiar nodo raíz

    // escogemos una ruta aleatoria de vehículo
    int route = rand() % (transportData.qTrucks - transportData.qTrailers);

    std::vector<std::tuple<int, int, int>> subroutesVec;
    if(subroutes[route].length() <= 0) return;
    else subroutesVec = getSubroutesVector(subroutes[route]);

    // escogemos un nodo raíz de esa ruta
    int randomSubroute = rand() % (subroutesVec.size());
    int root, begin, end;
    std::tie(root, begin, end) = subroutesVec[randomSubroute];
    
    // escogemos un nodo de esa ruta que no pertenezca a una subruta
    int newRootPos = getRandomInVehicleRoute(subroutesVec, matrixSol[route].size()-1) + 1;
    int newRoot = matrixSol[route][newRootPos];

    // realizamos el cambio de nodo según si está a la izquierda o a la derecha de la subruta
    if(newRootPos < begin) {
        replace(matrixSol[route].begin(), matrixSol[route].end(), newRoot, root);
        replace(matrixSol[route].begin() + newRootPos + 1, matrixSol[route].end(), root, newRoot);
    }
    else {
        replace(matrixSol[route].begin(), matrixSol[route].end(), newRoot, root);
        replace(matrixSol[route].begin(), matrixSol[route].end() - newRootPos - 1, root, newRoot);
    }

    // si no se mejora la solución se deshace el cambio
    float eval = f_eval(matrixSol, distancesM, clients, transportData, subroutes);

    if(eval < *initialEval) {
        *initialEval = eval;
        // si se mejora la solución se deja
        updateSubroute(subroutes, route, root, newRoot, begin, begin, end, end);

    }
    else {
        // deshacemos el cambio
        if(newRootPos < begin) {
            replace(matrixSol[route].begin(), matrixSol[route].end(), root, newRoot);
            replace(matrixSol[route].begin() + newRootPos + 1, matrixSol[route].end(), newRoot, root);
        }
        else {
            replace(matrixSol[route].begin(), matrixSol[route].end(), root, newRoot);
            replace(matrixSol[route].begin(), matrixSol[route].end() - newRootPos - 1, newRoot, root);
        }
    }
}

void fourth_phase(std::vector<std::vector<int>> &matrixSol, float *initialEval, transport transportData,
    std::vector<string> &subroutes, std::vector<std::vector<float>> distancesM, std::vector<client> clients) {

    // CUARTA FASE: movemos un nodo de vehículo a una subruta

    // escoger ruta de vehículo
    int route = rand() % (transportData.qTrucks - transportData.qTrailers);

    // escoger nodo que no pertenezca a subruta
    int prevPos = getRandomInVehicleRoute(getSubroutesVector(subroutes[route]), matrixSol[route].size()-1) + 1;

    std::vector<std::tuple<int, int, int>> subroutesVec;
    if(subroutes[route].length() == 0) return;
    else subroutesVec = getSubroutesVector(subroutes[route]);

    // escoger subruta que esté en el mismo camino
    int nextPos, root, start, end;
    std::tuple<int, int, int> subrouteTuple;
    std::tie(nextPos, subrouteTuple) = getRandomInSubroutes(subroutesVec);
    std::tie(root, start, end) = subrouteTuple;

    // si mejora la solución, eliminar el nodo de la ruta de camión e insertarlo en la subruta
    int prevNode = matrixSol[route][prevPos];

    if(prevPos < nextPos) {
        matrixSol[route].insert(matrixSol[route].begin() + nextPos, prevNode);
        matrixSol[route].erase(matrixSol[route].begin() + prevPos);
    }
    else {
        matrixSol[route].erase(matrixSol[route].begin() + prevPos);
        matrixSol[route].insert(matrixSol[route].begin() + nextPos, prevNode);
    }

    float eval = f_eval(matrixSol, distancesM, clients, transportData, subroutes);

    if(eval < *initialEval) {
        *initialEval = eval;
        updateSubroute(subroutes, route, root, root, start, start, end, end+1);
    }
    else {
        if(prevPos < nextPos) {
            matrixSol[route].insert(matrixSol[route].begin() + prevPos, prevNode);
            matrixSol[route].erase(matrixSol[route].begin() + nextPos);
        }
        else {
            matrixSol[route].erase(matrixSol[route].begin() + nextPos);
            matrixSol[route].insert(matrixSol[route].begin() + prevPos, prevNode);
        }
    }
}

void getSubroutes(std::vector<std::vector<int>> matrixSol, std::vector<string> &subroutes) {

    std::vector<string> added;

    for(int i = 0; i < (int) matrixSol.size(); i++) {
        int pos1 = 0, pos2 = 0;
        for(int j = 2; j < (int) matrixSol[i].size()-1; j++) {
            pos1 = j;
            for(int k = j+1; k < (int) matrixSol[i].size()-1; k++) {
                if(matrixSol[i][j] == matrixSol[i][k]) {
                    pos2 = k;
                    if(std::find(added.begin(), added.end(), to_string(matrixSol[i][j]) + to_string(pos1) + to_string(pos2)) == added.end()) {
                        addSubroute(subroutes, i, matrixSol[i][j], pos1, pos2);
                        added.push_back(to_string(matrixSol[i][j]) + to_string(pos1) + to_string(pos2));
                        pos1 = k;
                    }
                }
            }
        }
    }
}

void TTRP(transport transportData, std::vector<client> clients, std::vector<std::vector<float>> distancesM, int M) {
    
    int iters2 = M*200;
    int iters3 = M*200;
    int iters4 = M*100;

    // probabilities
    int p3_0 = 25, p3_1 = 50, p3_2 = 75;

    srand(time(NULL));

    // definimos una nueva estructura que nos servirá para guardar las subrutas
    std::vector<string> subroutes(transportData.qTrucks, "");

    // PRIMERA FASE: construimos la solución
    std::vector<std::vector<int>> initialSolution = first_phase(transportData, clients);

    initialSolution = {{0,0,40,4,17,37,4,13,18,25,33,42,19,41,42,0},
        {0,0,43,6,47,6,14,23,7,23,24,26,27,44,48,49,0},
        {0,0,35,2,16,2,11,20,21,29,30,32,46,32,34,38,0},
        {1,0,5,9,10,12,15,45,39,0},
        {1,0,36,1,3,8,22,28,31,50,0}};

    getSubroutes(initialSolution, subroutes);

    printMatrix(initialSolution);
    cout << endl;
    for(int i = 0; i < (int) subroutes.size(); i++) {
        cout << subroutes[i] << endl;
    }

    float initialEval = f_eval(initialSolution, distancesM, clients, transportData, subroutes);

    cout << "Solución inicial:" << initialEval << endl << endl;

    // SEGUNDA FASE: mejoramos los caminos de vehículo
    for(int iters = 0; iters < M; iters++) {
    
        for(int i = 0; i < iters2; i++) {
            second_phase(initialSolution, &initialEval, transportData, subroutes, distancesM, clients);
        }

        for(int i = 0; i < iters3; i++) {
            if(rand() % 100 <= p3_0) {
                // TERCERA FASE .0
                third_phase_0(initialSolution, &initialEval, transportData, subroutes, distancesM, clients);
            }
            else if(rand() % 100 <= p3_1) {
                // TERCERA FASE .1
                third_phase_1(initialSolution, &initialEval, transportData, subroutes, distancesM, clients);
            }
            else if(rand() % 100 <= p3_2) {
                // TERCERA FASE .2
                third_phase_2(initialSolution, &initialEval, transportData, subroutes, distancesM, clients);
            }
            else {
                // TERCERA FASE .3
                third_phase_3(initialSolution, &initialEval, transportData, subroutes, distancesM, clients);
            }
        }
        
        for(int i = 0; i < iters2; i++) {
            second_phase(initialSolution, &initialEval, transportData, subroutes, distancesM, clients);
        }

        for(int i = 0; i < iters4; i++) {
            // CUARTA FASE

        // cout << "before fourth" << endl;
            fourth_phase(initialSolution, &initialEval, transportData, subroutes, distancesM, clients);

        // cout << "after fourth" << endl;
        }
    }

    cout << "Solución final:" << f_eval(initialSolution, distancesM, clients, transportData, subroutes) << endl << endl;
    printMatrix(initialSolution);
}

int main(int argc, char *argv[]) {

    time_t instanteInicial, instanteFinal;
    instanteInicial = time(0);

    // Almacenamos ruta de archivo y cantidad de iteraciones máximas
    string route = argv[1];
    int maxIterations = stoi(argv[2]);

    // Abrimos archivo y parseamos los datos que nos entrega en estructura adecuada
    string line;
    ifstream myfile (route);
    std::vector<client> clients;
    transport transportData;
    transportData.nClients = 0;

    // Si está abierto el archivo parseamos los datos de los clientes y el transporte en estructuras convenientes
    if (myfile.is_open()) {

        getline(myfile, line);
        splitTransport(line, transportData);

        for(int i = 0; i <= transportData.nClients; i++) {
            getline(myfile, line);
            splitClient(line, clients);
        }

        myfile.close();
    }
    else cout << "Unable to open file";

    // Inicializamos el vector bidimensional que almacenará las distancias con 0's
    std::vector<std::vector<float>> distancesM(
        transportData.nClients+1, 
        std::vector<float> (transportData.nClients+1, 0)
    );

    generateDistanceMatrix(distancesM, clients);

    // Ejecutamos la función TTRP() entregándole los parámetros
    TTRP(transportData, clients, distancesM, maxIterations);

    instanteFinal = time(0);
    cout << "Tiempo total de ejecución: " << instanteFinal - instanteInicial << " [s]" << endl;

    return 0;
}
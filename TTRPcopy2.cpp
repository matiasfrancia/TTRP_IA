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
    for(int i = 0; i < vec.size(); i++) {
        cout << vec[i] << ", ";
    }
    cout << "]" << endl;
}

int getSubrouteDemand(std::vector<string> subroute, std::vector<client> clients, std::vector<int> route) {

    // calculamos la capacidad total desde 'inicio' hasta 'termino
    int demand = 0;
    for(int i = stoi(subroute[1]); i <= stoi(subroute[2]); i++) {
        demand += clients[i].demand;
    }
    return demand;
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

int getTotalDemandSubroutes(string subroutes, std::vector<client> clients, std::vector<int> route) {

    // subroutes = "P1,2,3P1,2,3P1,2,3";
    subroutes = subroutes.erase(0, 1);
    string del = "P";

    int start, end = -1*del.size();
    int demandSubroutes = 0;

    do {
        start = end + del.size();
        end = subroutes.find(del, start);
        string subroute = subroutes.substr(start, end - start);
        demandSubroutes += getSubrouteDemand(splitByComma(subroute), clients, route);
    } while (end != -1);

    return demandSubroutes;
}

// modificamos una subruta
void updateSubroute(std::vector<string> subroutes, int i, int pivot, int newPivot, int pos1, int pos2) {
    // buscamos en qué posición del string está la subruta

    // TODO: arreglar esta función por lo de las comas
    int j = subroutes[i].find('P' + to_string(pivot)) + 1;
    subroutes[i][j] = char(newPivot);
    subroutes[i][j+1] = char(pos1);
    subroutes[i][j+2] = char(pos2);
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
void addSubroute(std::vector<string> &subroutes, int i, int pivot, int pos1, int pos2) {
    string prevString = 'P' + to_string(pivot) + ',' + to_string(pos1) + ',' + to_string(pos2);
    subroutes[i].append(prevString);
}

float f_eval(std::vector<std::vector<int>> matrixSol, std::vector<std::vector<float>> distM,
    std::vector<client> clients, transport transportData, std::vector<string> subroutes) {

    float eval = 0;
    float routeDemand = 0;
    float subrouteDemand = 0;

    // definimos las penalizaciones
    float penaltyRoute = 500;
    float penaltySubroute = 200;

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
            for(int k = 0; k < subroutesVec.size(); k++) {
                routeDemand -= clients[get<0>(subroutesVec[i])].demand;
            }
        }

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
        // subrouteDemand = 0;
        // if(subroutes[i].size() > 0) {
        //     subrouteDemand = getTotalDemandSubroutes(subroutes[i], clients, matrixSol[i]);
        // }

        // if(subrouteDemand > transportData.truckCapacity) {
        //     eval += (subrouteDemand - transportData.truckCapacity) * penaltySubroute;
        // }
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

    for(int i = 0; i < intervals.size(); i++) {
        sortedSubroutes.push_back(get<1>(intervals[i]));
        sortedSubroutes.push_back(get<2>(intervals[i]));
    }

    sortedSubroutes.push_back(length-1);

    totalRange = length - 2;
    sort(sortedSubroutes.begin(), sortedSubroutes.end());

    for(int i = 0; i < sortedSubroutes.size()/2; i++) {
        totalRange -= sortedSubroutes[2*i+1] - sortedSubroutes[2*i] + 1;
    }

    int randomNumber, lowerBound, upperBound;
    randomNumber = (rand() % totalRange) + 1;
    lowerBound = 1;
    upperBound = sortedSubroutes[0];

    for(int i = 0; i < sortedSubroutes.size()/2+1; i++) {
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
}

float getCapacityPenalty(std::vector<std::vector<int>> &matrixSol, float capacity, int penalty, int penaltySubroute, 
    std::vector<string> subroutes, std::vector<client> clients, int prevRoute, int nextRoute, int prevPos, int nextPos, 
    int node1, int node2, transport transportData) {
    
    float newDemandPrev = 0, newDemandNext = 0, demandPrev = 0, demandNext = 0;
    for(int i = 1; i < matrixSol[prevRoute].size()-1; i++) {
        demandPrev += clients[matrixSol[prevRoute][i]].demand;
    }

    // le restamos los nodos repetidos
    std::vector<std::tuple<int, int, int>> subroutesVec;
    if(subroutes[prevRoute].size() > 0) {
        subroutesVec = getSubroutesVector(subroutes[prevRoute]);

        // restamos a la demanda una vez todas las raíces de subruta
        for(int k = 0; k < subroutesVec.size(); k++) {
            demandPrev -= clients[get<0>(subroutesVec[prevRoute])].demand;
        }
    }

    newDemandPrev = demandPrev + clients[node2].demand - clients[node1].demand;

    // recalculamos capacidad ruta previa con el cambio
    for(int i = 1; i < matrixSol[nextRoute].size()-1; i++) {
        demandNext += clients[matrixSol[nextRoute][i]].demand;
    }

    subroutesVec.clear();

    if(subroutes[nextRoute].size() > 0) {
        subroutesVec = getSubroutesVector(subroutes[nextRoute]);

        // restamos a la demanda una vez todas las raíces de subruta
        for(int k = 0; k < subroutesVec.size(); k++) {
            demandNext -= clients[get<0>(subroutesVec[nextRoute])].demand;
        }
    }

    newDemandNext = demandNext + clients[node1].demand - clients[node2].demand;
    float newPenalty = 0;

    // calculamos la nueva función de evaluación de acuerdo a la ruta prev
    if(demandPrev <= capacity && newDemandPrev > capacity) {
        // se le debe agregar penalización según cuánto se excedió de la capacidad del camión
        newPenalty += (newDemandPrev - capacity) * penalty;
    }
    else if(demandPrev > capacity && newDemandPrev > capacity) {
        // se calcula nuevamente la penalización según si aumento o disminuyó
        newPenalty += (newDemandPrev - demandPrev) * penalty;
    }
    else if(demandPrev > capacity && newDemandPrev <= capacity) {
        // se le quita la penalización
        newPenalty += -1*(demandPrev - capacity) * penalty;
    }

    // calculamos la nueva función de evaluación de acuerdo a la ruta next
    if(demandNext <= capacity && newDemandNext > capacity) {
        // se le debe agregar penalización según cuánto se excedió de la capacidad del camión
        newPenalty += (newDemandNext - capacity) * penalty;
    }
    else if(demandNext > capacity && newDemandNext > capacity) {
        // se calcula nuevamente la penalización según si aumento o disminuyó
        newPenalty += (newDemandNext - demandNext) * penalty;
    }
    else if(demandNext > capacity && newDemandNext <= capacity) {
        // se le quita la penalización
        newPenalty += -1*(demandNext - capacity) * penalty;
    }

    // si se genera una subruta, se debe verificar que cumpla con que la demanda de la subruta sea menor que la capacidad del camión
    
    // obtenemos la suma de las demandas de las subrutas
    // int subrouteDemand = 0;
    // if(subroutes[].size() > 0) {
    //     subrouteDemand = getTotalDemandSubroutes(subroutes[i], clients, matrixSol[i]);
    // }

    // if(subrouteDemand > transportData.truckCapacity) {
    //     newPenalty += (subrouteDemand - transportData.truckCapacity) * penaltySubroute;
    // }

    // return newPenalty;
}

void makeSwap(std::vector<std::vector<int>> &matrixSol, float *initialEval, std::vector<std::vector<float>> distancesM,
    std::vector<client> clients, int prevRoute, int nextRoute, int prevPos, int nextPos, float capacity, int penalty, 
    transport transportData, std::vector<string> subroutes) {

    // verificamos si mejora la solución (sin realizar el cambio aún)
    float newEval = *initialEval;

    // restamos a la f(x) eval las conexiones al nodo (prev), y la conexión del nodo anterior con el actual del destino
    int prevNode1;
    int node1;
    int nextNode1;
    int prevNode2;
    int node2;
    int nextNode2;

    // variables prev
    prevNode1 = matrixSol[prevRoute][prevPos-1];
    node1 = matrixSol[prevRoute][prevPos];
    nextNode1 = matrixSol[prevRoute][prevPos+1];

    // variables next
    prevNode2 = matrixSol[nextRoute][nextPos-1];
    node2 = matrixSol[nextRoute][nextPos];
    nextNode2 = matrixSol[nextRoute][nextPos+1];

    // prev route
    // si prevPos está antes de nextPos
    if(prevRoute == nextRoute && prevPos - nextPos == -1) {
        newEval += -distancesM[prevNode1][node1] + distancesM[node1][nextNode2] - distancesM[node2][nextNode2] + distancesM[prevNode1][node2];
    }
    // si está después
    else if(prevRoute == nextRoute && prevPos - nextPos == 1) {
        newEval += -distancesM[node1][nextNode1] + distancesM[prevNode2][node1] - distancesM[prevNode2][node2] + distancesM[node2][nextNode1];
    }
    else {
        newEval += -distancesM[prevNode1][node1] - distancesM[node1][nextNode1];
        newEval += distancesM[prevNode1][node2] + distancesM[node2][nextNode1];

        newEval += -distancesM[prevNode2][node2] - distancesM[node2][nextNode2];
        newEval += distancesM[prevNode2][node1] + distancesM[node1][nextNode2];
    }

    int penaltySubroute = 200;

    newEval += getCapacityPenalty(matrixSol, capacity, penalty, penaltySubroute, subroutes, clients, 
        prevRoute, nextRoute, prevPos, nextPos, node1, node2, transportData);
    
    if(newEval < *initialEval) {
        *initialEval = newEval;

        // cout << "Efectuamos el cambio" << endl;
        
        // si prevRoute y nextRoute son distintas
        // borramos los nodos de ambas rutas (vectores)
        if(prevRoute != nextRoute) {
            matrixSol[prevRoute].erase(matrixSol[prevRoute].begin() + prevPos);
            matrixSol[nextRoute].erase(matrixSol[nextRoute].begin() + nextPos);

            // insertamos los nodos en ambas rutas (vectores)
            matrixSol[prevRoute].insert(matrixSol[prevRoute].begin() + prevPos, node2);
            matrixSol[nextRoute].insert(matrixSol[nextRoute].begin() + nextPos, node1);
        }
        // si no son distintas
        else {
            swap(matrixSol[prevRoute][prevPos], matrixSol[prevRoute][nextPos]);
        }

        // printMatrix(matrixSol);
    }
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

    // TODO: tener cuidado con la selección de los randoms, puesto que en el mismo segundo, entregan mismo resultado

    // buscamos un número aleatorio dentro de diversos rangos (que no pertenezca a una subruta)
    int prevPos = getRandomInVehicleRoute(getSubroutesVector(subroutes[prevRoute]), matrixSol[prevRoute].size()-1) + 1;
    int nextPos = getRandomInVehicleRoute(getSubroutesVector(subroutes[prevRoute]), matrixSol[nextRoute].size()-1) + 1;

    float totalCapacity = transportData.truckCapacity + transportData.truckCapacity;
    int routePenalty = 500;

    makeSwap(matrixSol, initialEval, distancesM, clients, prevRoute, nextRoute, prevPos, nextPos, 
        totalCapacity, routePenalty, transportData, subroutes);
}

void generateSubroute(int iteration, std::vector<std::vector<int>> &matrixSol, float *initialEval, 
    std::vector<std::vector<float>> distancesM, std::vector<client> clients, int prevRoute, int nextRoute, 
    int prevPos, int nextPos, transport transportData, std::vector<string> &subroutes) {

    // ver si mejora la solución al usarlo como nodo raíz de subruta
    // verificamos si mejora la solución (sin realizar el cambio aún)
    float newEval = *initialEval;

    // restamos a la f(x) eval las conexiones al nodo (prev), y la conexión del nodo anterior con el actual del destino
    int prevNode1;
    int node1;
    int nextNode1;
    int prevNode2;
    int node2;
    int nextNode2;

    // variables prev
    prevNode1 = matrixSol[prevRoute][prevPos-1];
    node1 = matrixSol[prevRoute][prevPos];
    nextNode1 = matrixSol[prevRoute][prevPos+1];

    // variables next
    prevNode2 = matrixSol[nextRoute][nextPos-1];
    node2 = matrixSol[nextRoute][nextPos];
    nextNode2 = matrixSol[nextRoute][nextPos+1];

    // prev route
    newEval += -distancesM[prevNode1][node1] - distancesM[node1][nextNode1];
    newEval += distancesM[prevNode1][nextNode1];

    // next route
    newEval += 2 * distancesM[node1][node2];
    
    // tenemos que considerar el cumplimiento de las restricciones en la nueva evaluación
    // recalculamos capacidad ruta previa con el cambio

    // TODO: considerar la penalización por demanda de subruta mayor a capacidad camión
    float capacity = transportData.truckCapacity + transportData.trailerCapacity;
    int penaltyRoute = 500;
    int penaltySubroute = 200;

    // cout << "==== flag " << iteration << " ====" << endl;
    // cout << "prevRoute: " << prevRoute << " - nextRoute: " << nextRoute << endl;
    // cout << "prevPos: " << prevPos << " - nextPos: " << nextPos << endl;

    newEval += getCapacityPenalty(matrixSol, capacity, penaltyRoute, penaltySubroute, subroutes, clients, 
        prevRoute, nextRoute, prevPos, nextPos, node1, node2, transportData);

    // si la mejora, duplicar el nodo de vehículo escogido e insertar entre ambos el nodo de camión
    if(newEval < *initialEval) {
        *initialEval = newEval;

        // eliminamos el nodo de camión de la ruta de camión
        if(matrixSol[prevRoute].size() > 6) {
            matrixSol[prevRoute].erase(matrixSol[prevRoute].begin() + prevPos);
            // replicamos el nodo escogido como raíz e insertamos el nodo de camión en medio
            matrixSol[nextRoute].insert(matrixSol[nextRoute].begin() + nextPos, node1);
            matrixSol[nextRoute].insert(matrixSol[nextRoute].begin() + nextPos, node2);
        }

        // TODO: agregar la nueva subruta a la lista de subrutas
        addSubroute(subroutes, nextRoute, node2, nextPos, nextPos+2);
        // printArrayString(subroutes);
        // for(int i = 0; i < subroutes.size(); i++) {
        //     if(subroutes[i].length() > 0) cout << i << ": " << subroutes[i] << endl;
        // }
        // printMatrix(matrixSol);
    }

}

void third_phase(int iteration, std::vector<std::vector<int>> &matrixSol, float *initialEval, transport transportData,
    std::vector<string> &subroutes, std::vector<std::vector<float>> distancesM, std::vector<client> clients) {

    // probabilidad de generar nuevas subrutas no debe ser muy alta

    // PRIMER MOVIMIENTO: mover nodos de ruta de camión

    // escogemos una ruta de camión
    int prevRoute, nextRoute;
    prevRoute = rand() % (transportData.qTrailers-1) + transportData.qTrucks - transportData.qTrailers + 1;
    nextRoute = rand() % (transportData.qTrucks);

    // cout << "Prev route: " << prevRoute << endl;
    // cout << "Next route: " << nextRoute << endl;

    // escogemos un nodo dentro de esa ruta
    int prevPos = rand() % (matrixSol[prevRoute].size() - 3) + 2;
    int nextPos = rand() % (matrixSol[nextRoute].size() - 3) + 2;

    // cout << "Prev Pos: " << prevPos << endl;
    // cout << "Next Pos: " << nextPos << endl;

    float truckCapacity = transportData.truckCapacity;
    int routePenalty = 500;

    // si el nodo es de camión, se hace un swap
    if(clients[matrixSol[nextRoute][nextPos]].isTruck == 1) {
        makeSwap(matrixSol, initialEval, distancesM, clients, prevRoute, nextRoute, prevPos, nextPos, 
            truckCapacity, routePenalty, transportData, subroutes);
    }
    // si el nodo es de vehículo se prueba crear una subruta a partir de él
    else {
        std::vector<std::tuple<int, int, int>> subroutesVec = getSubroutesVector(subroutes[nextRoute]);
        bool isRoot = false;
        for(int i = 0; i < subroutesVec.size(); i++) {
            if(get<1>(subroutesVec[i]) == nextPos || get<2>(subroutesVec[i]) == nextPos) isRoot = true;
        }
        if(!isRoot && matrixSol[prevRoute].size() >= 5) {
            generateSubroute(iteration, matrixSol, initialEval, distancesM, clients, prevRoute, nextRoute, 
                prevPos, nextPos, transportData, subroutes);

            // for(int i = 0; i < subroutes.size(); i++) {
            //     if(subroutes[i].length() > 0) cout << i << ": " << subroutes[i] << endl;
            // }
        }
    }

    // si el movimiento mejora la función de evaluación, se realiza el cambio

    // SEGUNDO MOVIMIENTO: reordenar subrutas
    // escogemos una ruta al azar
    // escogemos una subruta de esa ruta
    // escogemos un nodo de esa subruta al azar
    // buscamos otro nodo de subruta
    // hacemos swap entre esos nodos
    // vemos si se mejora la función de evaluación
    // si mejora, se realiza el cambio

    // TERCER MOVIMIENTO: insertar nuevos nodos de camión en subrutas
    // escoger aleatoriamente una ruta de camión
    // escoger aleatoriamente una subruta
    // determinar una posición dentro de la subruta para insertarlo
    // si mejora la solución, eliminar el nodo de la ruta de camión e insertarlo en la subruta

    // CUARTO MOVIMIENTO: generar nuevas subrutas

}

void TTRP(transport transportData, std::vector<client> clients, std::vector<std::vector<float>> distancesM) {
    
    int iters2 = 1000;
    int iters3 = 1000;

    srand(time(NULL));

    // definimos una nueva estructura que nos servirá para guardar las subrutas
    std::vector<string> subroutes(transportData.qTrucks, "");

    // PRIMERA FASE: construimos la solución
    std::vector<std::vector<int>> initialSolution = first_phase(transportData, clients);
    printMatrix(initialSolution);

    float initialEval = f_eval(initialSolution, distancesM, clients, transportData, subroutes);

    cout << "Solution evaluation:" << initialEval << endl << endl;

    // SEGUNDA FASE: mejoramos los caminos de vehículo
    for(int i = 0; i < iters2; i++) {
        second_phase(initialSolution, &initialEval, transportData, subroutes, distancesM, clients);
    }
    
    cout << "flag" << endl;

    printMatrix(initialSolution);
    
    // TERCERA FASE: mejoramos los caminos de camión y creamos nuevas subrutas
    for(int i = 0; i < iters3; i++) {
    
        third_phase(i, initialSolution, &initialEval, transportData, subroutes, distancesM, clients);
    }
    cout << "Solution evaluation:" << initialEval << endl << endl;
    cout << "f_eval evaluation:" << f_eval(initialSolution, distancesM, clients, transportData, subroutes) << endl << endl;

    // addSubroute(subroutes, 0, 13, 2, 7);
    // addSubroute(subroutes, 0, 14, 8, 10);
    // addSubroute(subroutes, 0, 9, 25, 40);

    for(int i = 0; i < subroutes.size(); i++) {
        cout << i << ": " << subroutes[i] << endl;
    }

    // std::vector<std::tuple<int, int, int>> subroutesVec = getSubroutesVector(subroutes[0]);
    
    // cout << endl;
    // cout << "[";
    // for(int i = 0; i < subroutesVec.size(); i++) {
    //     cout << "{" << get<0>(subroutesVec[i]) << "," << get<1>(subroutesVec[i]) << "," << get<2>(subroutesVec[i]) << "}" << endl;
    // }
    // cout << "]" << endl;

    // std::vector<std::tuple<int, int, int>> subroutesTest;
    
    // subroutesTest.push_back({4, 4, 8});
    // subroutesTest.push_back({11, 14, 17});
    // subroutesTest.push_back({12, 9, 12});

    // printArray(std::vector<int>{0,1,2,3,4,5,6,7,4,12,10,9,12,8,11,15,17,11,20,21,0});
    
    // cout << "Returned: " << getRandomInVehicleRoute(subroutesTest, 21) << endl;
    // cout << "Returned: " << getRandomInVehicleRoute(subroutesTest, 21) << endl;
}

int main(int argc, char *argv[]) {

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
    TTRP(transportData, clients, distancesM);

    // Almacenamos los resultados de la ejecución del programa

    // Mostramos en pantalla los resultados obtenidos

    return 0;
}
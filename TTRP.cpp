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

std::vector<string> splitByComma(string s, string del = ",") {

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

float f_eval(std::vector<std::vector<int>> matrixSol, std::vector<std::vector<float>> distM,
    std::vector<client> clients, transport transportData, std::vector<string> subroutes) {

    float eval = 0;
    float routeDemand = 0;
    float subrouteDemand = 0;

    // definimos las penalizaciones
    float penalityRoute = 500;
    float penalitySubroute = 200;

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

        float capacity = 0;
        
        // demanda ruta camión supera capacidad camión
        if(matrixSol[i][0] == 1) {
            capacity = transportData.qTrucks;
        }
        // demanda ruta vehículo supera capacidad camión más trailer
        else {
            capacity = transportData.qTrucks + transportData.qTrailers;
        }

        if(routeDemand > capacity) {
            eval += (routeDemand - capacity) * penalityRoute;
        }

        // TODO: revisamos si hay subrutas, y comprobamos si se excede la capacidad del camión en cada una de ellas
        // obtenemos la suma de las demandas de las subrutas
        subrouteDemand = 0;
        if(subroutes[i].size() > 0) {
            subrouteDemand = getTotalDemandSubroutes(subroutes[i], clients, matrixSol[i]);
        }

        if(subrouteDemand > transportData.truckCapacity) {
            eval += (subrouteDemand - transportData.truckCapacity) * penalitySubroute;
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

// modificamos una subruta
void updateSubroute(std::vector<string> subroutes, int i, int pivot, int newPivot, int pos1, int pos2) {
    // buscamos en qué posición del string está la subruta
    int j = subroutes[i].find('P' + to_string(pivot)) + 1;
    subroutes[i][j] = char(newPivot);
    subroutes[i][j+1] = char(pos1);
    subroutes[i][j+2] = char(pos2);
}

// agregamos una subruta al string de subrutas ["P root1 pos11 pos12 P root2 pos21 pos22 ...", ...]
void addSubroute(std::vector<string> subroutes, int i, int pivot, int pos1, int pos2) {
    subroutes[i] += 'P' + to_string(pivot) + to_string(pos1) + to_string(pos2);
}

// esta función retorna un vector de tupla con pares ordenados con cada una de las subrutas de una ruta
std::vector<std::tuple<int, int, int>> getSubroutesVector(string subroutesRow) {

    std::vector<std::tuple<int, int, int>> subroutesVec;

    for(int i = 0; i < subroutesRow.size()/4; i++) {
        subroutesVec.push_back({
            stoi(subroutesRow.substr(i*4, i*4+1)),
            stoi(subroutesRow.substr(i*4+1, i*4+2)),
            stoi(subroutesRow.substr(i*4+2, i*4+3))
        });
    }

    return subroutesVec;
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

void makeSwap(std::vector<std::vector<int>> &matrixSol, float *initialEval, std::vector<std::vector<float>> distancesM,
    std::vector<client> clients, int prevRoute, int nextRoute, int prevPos, int nextPos, int capacity, int penalty) {

    int newEval = *initialEval;

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
    newEval += -distancesM[prevNode1][node1] - distancesM[node1][nextNode1] + distancesM[prevNode1][node2] + distancesM[node2][nextNode1];

    // next route
    newEval += -distancesM[prevNode2][node2] - distancesM[node2][nextNode2] + distancesM[prevNode2][node1] + distancesM[node1][nextNode2];

    // tenemos que considerar el cumplimiento de las restricciones en la nueva evaluación
    // recalculamos capacidad ruta previa con el cambio
    float newDemandPrev = 0, newDemandNext = 0, demandPrev = 0, demandNext = 0;
    for(int i = 0; i < matrixSol[prevRoute].size(); i++) {
        demandPrev += clients[matrixSol[prevRoute][i]].demand;
    }
    newDemandPrev = demandPrev + clients[node2].demand - clients[node1].demand;

    // recalculamos capacidad ruta previa con el cambio
    for(int i = 0; i < matrixSol[nextRoute].size(); i++) {
        demandNext += clients[matrixSol[nextRoute][i]].demand;
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

    newEval += newPenalty;
    
    if(newEval < *initialEval) {
        *initialEval = newEval;
        
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
        // cout << "f_eval value: " << f_eval(matrixSol, distancesM, clients, transportData, subroutes) << endl;
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
    newEval += -distancesM[prevNode1][node1] - distancesM[node1][nextNode1] + distancesM[prevNode1][node2] + distancesM[node2][nextNode1];

    // next route
    newEval += -distancesM[prevNode2][node2] - distancesM[node2][nextNode2] + distancesM[prevNode2][node1] + distancesM[node1][nextNode2];

    // tenemos que considerar el cumplimiento de las restricciones en la nueva evaluación
    // recalculamos capacidad ruta previa con el cambio
    float newDemandPrev = 0, newDemandNext = 0, demandPrev = 0, demandNext = 0;
    for(int i = 0; i < matrixSol[prevRoute].size(); i++) {
        demandPrev += clients[matrixSol[prevRoute][i]].demand;
    }
    newDemandPrev = demandPrev + clients[node2].demand - clients[node1].demand;

    // recalculamos capacidad ruta previa con el cambio
    for(int i = 0; i < matrixSol[nextRoute].size(); i++) {
        demandNext += clients[matrixSol[nextRoute][i]].demand;
    }
    newDemandNext = demandNext + clients[node1].demand - clients[node2].demand;

    int routePenalty = 500;
    float totalCapacity = transportData.truckCapacity + transportData.truckCapacity;
    float newPenalty = 0;

    // calculamos la nueva función de evaluación de acuerdo a la ruta prev
    if(demandPrev <= totalCapacity && newDemandPrev > totalCapacity) {
        // se le debe agregar penalización según cuánto se excedió de la capacidad del camión
        newPenalty += (newDemandPrev - totalCapacity) * routePenalty;
    }
    else if(demandPrev > totalCapacity && newDemandPrev > totalCapacity) {
        // se calcula nuevamente la penalización según si aumento o disminuyó
        newPenalty += (newDemandPrev - demandPrev) * routePenalty;
    }
    else if(demandPrev > totalCapacity && newDemandPrev <= totalCapacity) {
        // se le quita la penalización
        newPenalty += -1*(demandPrev - totalCapacity) * routePenalty;
    }

    // calculamos la nueva función de evaluación de acuerdo a la ruta next
    if(demandNext <= totalCapacity && newDemandNext > totalCapacity) {
        // se le debe agregar penalización según cuánto se excedió de la capacidad del camión
        newPenalty += (newDemandNext - totalCapacity) * routePenalty;
    }
    else if(demandNext > totalCapacity && newDemandNext > totalCapacity) {
        // se calcula nuevamente la penalización según si aumento o disminuyó
        newPenalty += (newDemandNext - demandNext) * routePenalty;
    }
    else if(demandNext > totalCapacity && newDemandNext <= totalCapacity) {
        // se le quita la penalización
        newPenalty += -1*(demandNext - totalCapacity) * routePenalty;
    }

    newEval += newPenalty;
    
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
        // cout << "f_eval value: " << f_eval(matrixSol, distancesM, clients, transportData, subroutes) << endl;
    }
}

void third_phase(std::vector<std::vector<int>> &matrixSol, float *initialEval, transport transportData,
    std::vector<string> &subroutes, std::vector<std::vector<float>> distancesM, std::vector<client> clients) {

    // PRIMER MOVIMIENTO: mover nodos de ruta de camión
    // escogemos una ruta de camión
    int prevRoute, nextRoute;
    prevRoute = rand() % (transportData.qTrailers) + transportData.qTrucks - transportData.qTrailers;
    nextRoute = rand() % (transportData.qTrucks);

    cout << "Prev route: " << prevRoute << endl;
    cout << "Next route: " << nextRoute << endl;

    // escogemos un nodo dentro de esa ruta
    int prevNode = rand() % (matrixSol[prevRoute].size() - 3) + 2;
    int nextNode = rand() % (matrixSol[nextRoute].size() - 3) + 2;

    cout << "Prev node: " << prevNode << endl;
    cout << "Next node: " << nextNode << endl;

    // si el nodo es de camión, se hace un swap
    if(clients[matrixSol[nextRoute][nextNode]].isTruck) {

    }
    // si el nodo es de vehículo se prueba crear una subruta a partir de él
    else {

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
    // escoger aleatoriamente nodo de ruta de camión
    // escoger aleatoriamente nodo de ruta de vehículo que no pertenezca a una subruta
    // ver si mejora la solución al usarlo como nodo raíz de subruta
    // si la mejora, duplicar el nodo de vehículo escogido e insertar entre ambos el nodo de camión
    // agregar la nueva subruta a la lista de subrutas

}

void TTRP(transport transportData, std::vector<client> clients, std::vector<std::vector<float>> distancesM) {
    
    int iters2 = 100000;
    int iters3 = 100000;

    srand(time(NULL));

    // definimos una nueva estructura que nos servirá para guardar las subrutas
    std::vector<string> subroutes(transportData.qTrucks, "");

    std::vector<std::vector<int>> initialSolution = first_phase(transportData, clients);
    printMatrix(initialSolution);

    float initialEval = f_eval(initialSolution, distancesM, clients, transportData, subroutes);
    cout << "Solution evaluation:" << initialEval << endl << endl;

    // realizamos nuestro primer movimiendo del HC + AM una cantidad determinada de veces
    for(int i = 0; i < iters2; i++) {
        // second_phase(initialSolution, &initialEval, transportData, subroutes, distancesM, clients);
    }

    third_phase(initialSolution, &initialEval, transportData, subroutes, distancesM, clients);
    
    printMatrix(initialSolution);
    cout << "Solution evaluation:" << initialEval << endl << endl;

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
#include <cmath>
#include <random>
#include <string>
#include <iostream>
#include <fstream>
#include "json.hpp"
#include "util.hpp"


static f64 Square(f64 A)
{
    f64 Result = (A*A);
    return Result;
}

static f64 RadiansFromDegrees(f64 Degrees)
{
    f64 Result = 0.01745329251994329577f * Degrees;
    return Result;
}

// NOTE(casey): EarthRadius is generally expected to be 6372.8
static f64 ReferenceHaversine(f64 X0, f64 Y0, f64 X1, f64 Y1, f64 EarthRadius)
{
    /* NOTE(casey): This is not meant to be a "good" way to calculate the Haversine distance.
       Instead, it attempts to follow, as closely as possible, the formula used in the real-world
       question on which these homework exercises are loosely based.
    */
    
    f64 lat1 = Y0;
    f64 lat2 = Y1;
    f64 lon1 = X0;
    f64 lon2 = X1;
    
    f64 dLat = RadiansFromDegrees(lat2 - lat1);
    f64 dLon = RadiansFromDegrees(lon2 - lon1);
    lat1 = RadiansFromDegrees(lat1);
    lat2 = RadiansFromDegrees(lat2);
    
    f64 a = Square(sin(dLat/2.0)) + cos(lat1)*cos(lat2)*Square(sin(dLon/2));
    f64 c = 2.0*asin(sqrt(a));
    
    f64 Result = EarthRadius * c;
    
    return Result;
}

void CreateHaversineFile(std::string filename, int numPts, unsigned int seed){
    f64 longitude_max = 180;
    f64 latitude_max = 90;

    std::mt19937 gen(seed);
    std::uniform_real_distribution<> longitude_func(-longitude_max, longitude_max);
    std::uniform_real_distribution<> latitude_func(-latitude_max, latitude_max);

    json j;
    nlohmann::Pairs pair;
    std::vector<nlohmann::Pairs> pairs;

    for (int i = 0; i < numPts; i++){
        pair.x0 = longitude_func(gen);
        pair.y0 = latitude_func(gen);

        pair.x1 = longitude_func(gen);
        pair.y1 = latitude_func(gen);

        pairs.push_back(pair);
    }
    j["pairs"] = pairs;
    std::ofstream o(filename);
    o << std::setw(4) << j << std::endl;
}

void CreateHaversineCLusterFile(std::string filename, int numPts, unsigned int numClusters, unsigned int seed){
    f64 longitude_max = 180;
    f64 latitude_max = 90;

    std::mt19937 gen(seed);

    std::vector<ClusterGen> clusterGenVec;
    std::uniform_real_distribution<> longitude_interval(-longitude_max, longitude_max);
    std::uniform_real_distribution<> latitude_interval(-latitude_max, latitude_max);
    
    for(int i = 0; i < numClusters + 1; i++){
        f64 first_long = longitude_interval(gen);
        f64 second_long = longitude_interval(gen);
        f64 first_lat = latitude_interval(gen);
        f64 second_lat = latitude_interval(gen);
        ClusterGen clusterGen;

        if (first_long < second_long){
            std::uniform_real_distribution<> gen(first_long, second_long);
            clusterGen.longitudeGen = gen;
        }
        else{
            std::uniform_real_distribution<> gen(second_long, first_long);
            clusterGen.longitudeGen = gen;
        }

        if (first_lat < second_lat){
            std::uniform_real_distribution<> gen(first_lat, second_lat);
            clusterGen.latitudeGen = gen;
        }
        else{
            std::uniform_real_distribution<> gen(second_lat, first_lat);
            clusterGen.latitudeGen = gen;
        }

        clusterGenVec.push_back(clusterGen);
    }
    
    json j;
    nlohmann::Pairs pair;
    std::vector<nlohmann::Pairs> pairs;

    for (int i = 0; i < numPts; i++){
        int cluster_index = i % numClusters;
        pair.x0 = clusterGenVec[cluster_index].longitudeGen(gen);
        pair.y0 = clusterGenVec[cluster_index].latitudeGen(gen);

        pair.x1 = clusterGenVec[cluster_index].longitudeGen(gen);
        pair.y1 = clusterGenVec[cluster_index].latitudeGen(gen);

        pairs.push_back(pair);
    }
    j["pairs"] = pairs;
    std::ofstream o(filename);
    o << std::setw(4) << j << std::endl;
}

std::vector<nlohmann::Pairs> ReadPairsFromFile(std::string filename){
    std::ifstream i(filename);
    json j;
    i >> j;
    PairContainer pairContainer;
    j.at("pairs").get_to(pairContainer.pairs);
    return pairContainer.pairs;
}

f64 CalculateHaversineMean(std::vector<nlohmann::Pairs> pairs, const int numPts){
    nlohmann::Pairs pair;
    f64 sum = 0;
    for (int i = 0; i < numPts; i++){
        pair = pairs[i];
        sum += ReferenceHaversine(pair.x0, pair.y0, pair.x1, pair.y1, 6372.8);
    }
    return sum/numPts;
}

int main(){
    const int numPts = 100;
    unsigned int seed = 45234523;
    unsigned int numClusters = 10;
    std::string filename = "haversine.json";

    //CreateHaversineFile(filename, numPts, seed);
    CreateHaversineCLusterFile(filename, numPts, numClusters, seed);

    std::vector<nlohmann::Pairs> pairs = ReadPairsFromFile(filename);

    f64 mean_distance = CalculateHaversineMean(pairs, numPts);
    printf("Mean distance is: %f", mean_distance);
}
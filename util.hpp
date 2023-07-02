#include "json.hpp"
#include <random>


using json = nlohmann::json;
typedef double f64;
namespace nlohmann{
    
    typedef struct Pairs{
        f64 x0;
        f64 y0;
        f64 x1;
        f64 y1;
    } Pairs;



    template <>
    struct adl_serializer<Pairs> {
        static void to_json(json& j, const Pairs& value) {
            // calls the "to_json" method in T's namespace
            j = {
                {"x0", value.x0},
                {"y0", value.y0},
                {"x1", value.x1},
                {"y1", value.y1}
            };
        }

        static void from_json(const json& j, Pairs& value) {
            // same thing, but with the "from_json" method
            j.at("x0").get_to(value.x0);
            j.at("x1").get_to(value.x1);
            j.at("y0").get_to(value.y0);
            j.at("y1").get_to(value.y1);

        }
    };
}

typedef struct PairContainer{
    std::vector<nlohmann::Pairs> pairs;
} PairContainer;

typedef struct ClusterGen{
    std::uniform_real_distribution<> longitudeGen;
    std::uniform_real_distribution<> latitudeGen;
} ClusterGen;

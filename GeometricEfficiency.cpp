#include <bits/stdc++.h>

#define PI 3.141592653589793238463

#define STEP_SIZE 0.1
#define STEP_LIMIT 100
#define DETECTOR_SIZE 2

#define inch 2.54

using namespace std;

static default_random_engine generator;
static uniform_real_distribution<double> dist(0, 1.0);

struct coordinate{
    double x;
    double y;
    double z;

    coordinate(double posX, double posY, double posZ) : x(posX), y(posY), z(posZ) {}

    coordinate operator+=(coordinate c){
        x+=c.x;
        y+=c.y;
        z+=c.z;
    }
};

struct ray{
    coordinate* source;
    coordinate* direction;

    ray(coordinate* start, coordinate* dir) : source(start), direction(dir) {}
    ~ray() {
        delete source;
        delete direction;
    }
};

struct cylinder{
    coordinate* position;
    double length;
    double rad;

    cylinder(double posX, double posY, double posZ, double len, double r) : position(new coordinate(posX, posY, posZ)), length(len), rad(r) {}

    double zMin(){
        return position->z - length;
    }

    double zMax(){
        return position->z + length;
    }

    double distFromCenter(double posX, double posY){
        return sqrt( pow((posX - position->x),2) + pow((posY - position->y),2) );
    }
};

struct cube{
    coordinate* position;
    double dimX;
    double dimY;
    double dimZ;

    cube(double posX, double posY, double posZ, double x, double y, double z) : position(new coordinate(posX, posY, posZ)), dimX(x), dimY(y), dimZ(z) {}

    double xMin(){
        return position->x - dimX;
    }

    double xMax(){
        return position->x + dimX;
    }

    double yMin(){
        return position->y - dimY;
    }

    double yMax(){
        return position->y + dimY;
    }

    double zMin(){
        return position->z - dimZ;
    }

    double zMax(){
        return position->z + dimZ;
    }
};

ray* generateRay(cube* source){

    double posX;
    double posY;
    double posZ;

    double angleTheta;
    double anglePhi;

    posX = (source->xMin()) + dist(generator) * 2 * source->dimX;
    posY = (source->yMin()) + dist(generator) * 2 * source->dimY;
    posZ = (source->zMin()) + dist(generator) * 2 * source->dimZ;

    angleTheta = dist(generator) * PI;
    anglePhi = dist(generator) * 2 * PI;

    double dirX, dirY, dirZ;
    dirX = STEP_SIZE * sin(angleTheta) * cos(anglePhi);
    dirY = STEP_SIZE * sin(angleTheta) * sin(anglePhi);
    dirZ = STEP_SIZE * cos(angleTheta);

    return new ray(new coordinate(posX,posY,posZ),new coordinate(dirX,dirY,dirZ));
}

//CHANGE THIS TO A FAST RAY-TRACING ALGORITHM!!
bool stepRay(ray* track, cylinder* target){
    coordinate* source = track->source;
    coordinate position(source->x, source->y, source->z);
    for(int i=0;i < STEP_LIMIT; ++i){
        position += *(track->direction);

        if(position.z > target->zMin() && position.z < target->zMax()){
            if(target->distFromCenter(position.x, position.y) <= target->rad){
                return true;
            }
        }
    }
    return false;
}

double doSim(cube* source, cylinder* detector, int N){
    int counts = 0;
    for(int i=0; i < N; ++i){
        ray* genRay = generateRay(source);
        if(stepRay(genRay, detector))++counts;
        delete genRay;
    }

    return (double)counts / N;
}

int main(){
    //Foil Dims: 2 x 2 x 0.1 cm
    //Detector Dims: 2" x 2" x ??"
    //Detector pos: 2cm from detector

    cube source(0., 0., 0., 1.0, 1.0, 0.1);
    cylinder detector(0., 0., 3.0, DETECTOR_SIZE*inch/2, DETECTOR_SIZE*inch/2);

    int N;
    cin >> N;

    vector<double> counts;

    for(int i=0;i < 50;++i){
        cout << "Run " << i << endl;
        counts.push_back(doSim(&source, &detector, N));
    }

    double mean = 0;
    for(auto iter = counts.begin(); iter!=counts.end(); ++iter){
        mean += *iter;
    }
    mean /= 50;

    double var = 0;
    for(auto iter = counts.begin(); iter!=counts.end(); ++iter){
        var += ( pow(*iter - mean, 2) );
    }
    var /= 49;
    var = sqrt(var);

    cout << "Mean Efficiency: " << mean << endl;
    cout << "1 sigma: " << var << endl;
}

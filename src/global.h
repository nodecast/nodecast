#ifndef GLOBAL_H
#define GLOBAL_H



enum class Sphere_scope { PRIVATE, PUBLIC, FIXED};


struct Sphere_data {
    QString title="";
    Sphere_scope scope;
    QString url="";
    QString directory="";
};




#endif // GLOBAL_H

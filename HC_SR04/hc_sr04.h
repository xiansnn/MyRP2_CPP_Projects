#if !defined(HC_SR04_H)
#define HC_SR04_H


class HC_SR04
{
private:
    void trig();
public:
    HC_SR04();
    float get_distance();
};





#endif // HC_SR04_H

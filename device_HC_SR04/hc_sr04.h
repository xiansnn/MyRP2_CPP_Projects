/**
 * @file hc_sr04.h
 * @author xiansnn (xiansnn@hotmail.com)
 * @brief 
 * @version 0.1
 * @date 2023-04-25
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#if !defined(HC_SR04_H)
#define HC_SR04_H


class HC_SR04
{
private:
/**
 * @brief 
 * 
 */
    void trig();
public:
/**
 * @brief Construct a new hc sr04 object
 * 
 */
    HC_SR04();
    /**
     * @brief Get the distance object
     * 
     * @return float 
     */
    float get_distance();
};


#endif // HC_SR04_H

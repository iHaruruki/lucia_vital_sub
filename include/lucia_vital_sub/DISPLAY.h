#ifdef DISPLAY_H
#define DISPLAY_H

class DISPLAY
{
private:
    /* data */
public:
    int Phase;
    float Pulse;
    float spo2;
    float bp_max;
    float bp_min;
    void display(DDatea);
    void display_vital(VData);
};
#endif

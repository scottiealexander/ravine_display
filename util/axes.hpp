#ifndef AXES_HPP_
#define AXES_HPP_

class Axes;

class AxesLimits
{
    friend class Axes;
public:
    AxesLimits(float xmin, float xmax, float ymin, float ymax) :
        _xmin(xmin), _xmax(xmax), _ymin(ymin), _ymax(ymax) {}
    ~AxesLimits() {}

    inline void getXLimits(float& xmin, float& xmax)
    {
        xmin = _xmin;
        xmax = _xmax;
    }
    inline void getYLimits(float& ymin, float& ymax)
    {
        ymin = _ymin;
        ymax = _ymax;
    }
    inline float getXExtent() const { return _xmax - _xmin; }
    inline float getYExtent() const { return _ymax - _ymin; }

private:
    float _xmin;
    float _xmax;
    float _ymin;
    float _ymax;
};

class Axes
{
public:

private:
    float _xscale;
    float _xoffset;
};

#endif

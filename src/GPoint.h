#ifndef __GPOINT__
#define __GPOINT__

template<class T>
struct vec2 {
    union {
        T x;
        T w;
        T width;
    };
    union {
        T y;
        T h;
        T height;
    };
    vec2() { x = 0; y = 0;}
    vec2(const T x, const T y) : x(x), y(y) {}
    const T& operator [] (const int i) const { return *((&x) + i); }
    T& operator [] (const int i) { return *((&x) + i); }
    bool operator == (const vec2& b) const { return (b.x==x && b.y==y); }
    bool operator != (const vec2& b) const { return !(b == *this); }
    const vec2 operator - () const { return vec2( -x, -y ); }
    const vec2& operator = (const vec2& b) { x = b.x; y = b.y; return *this; }
    const vec2& operator += (const T s) { x += s; y += s; return *this; }
    const vec2& operator += (const vec2& b) { x += b.x; y += b.y; return *this; }
    const vec2& operator -= (const T s) { x -= s; y -= s; return *this; }
    const vec2& operator -= (const vec2& b) { x -= b.x; y -= b.y;  return *this; }
    const vec2& operator *= (const T s) { x *= s; y *= s;  return *this; }
    const vec2& operator *= (const vec2& b) { x *= b.x; y *= b.y;  return *this; }
    const vec2& operator /= (const T s) { const T r = 1 / s; x *= r; y *= r;  return *this; }
    const vec2& operator /= (const vec2& b) {  x /= b.x;  y /= b.y; return *this; }
    const vec2 operator + (const T s) const { return vec2(x + s, y + s); }
    const vec2 operator + (const vec2& b) const { return vec2(x + b.x, y + b.y); }
    friend inline const vec2 operator + (const T s, const vec2& v) { return v + s; }
    const vec2 operator - (const T s) const { return vec2(x - s, y - s); }
    const vec2 operator - (const vec2& b) const { return vec2(x - b.x, y - b.y); }
    friend inline const vec2 operator - (const T s, const vec2& v) { return -v + s; }
    const vec2 operator * (const T s) const { return vec2(x*s, y*s); }
    const vec2 operator * (const vec2& b) const { return vec2(x*b.x, y*b.y); }
    friend inline const vec2 operator * (const T s, const vec2& v) { return v * s; }
    const vec2 operator / (const T s) const { const T r = 1 / s; return vec2(x*r, y*r); }
    const vec2 operator / (const vec2& b) const { return vec2(x/b.x, y/b.y); }
    friend inline const vec2 operator / (const T s, const vec2& v) { return vec2(s/v.x, s/v.y); }
    bool operator > (const vec2 &other) const { return ((x>other.x) && (y>other.y)); }
    bool operator < (const vec2 &other) const { return ((x<other.x) && (y<other.y)); }
    bool operator >= (const vec2 &other) const { return ((x>=other.x) && (y>=other.y)); }
    bool operator <= (const vec2 &other) const { return ((x<=other.x) && (y<=other.y)); }
};

typedef vec2<int> GPoint;
typedef vec2<int> GSize;

typedef vec2<int> GPt;
typedef vec2<int> GSz;

const GPoint G_DefaultPosition = GPoint(-1, -1);

#endif

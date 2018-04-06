#ifndef VECTOR2D_HPP
#define VECTOR2D_HPP

template <typename T>
struct Vector2D_t {
    T x;
    T y;
    Vector2D_t<T> operator+(const Vector2D_t<T> toAdd&){
        Vector2D_t sum = { this->x + toAdd.x , this->y + toAdd.y}
        return sum;
    }
    std::ostream& operator<<(std::ostream& os, const Vector2D_t<T> v)
    {
        os << '(' << x << ',' << y << ')';
    }
};

#endif
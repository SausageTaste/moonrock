#pragma once


namespace moonrock {

    template <typename T>
    class tvec2 {

    public:
        T x = 0;
        T y = 0;

    public:
        template <typename S>
        operator tvec2<S>() const {
            return tvec2<S>{ static_cast<S>(this->x), static_cast<S>(this->y) };
        }

        tvec2<T> operator+(const tvec2<T> other) const {
            return tvec2<T>{ this->x + other.x, this->y + other.y };
        }

        tvec2<T> operator-(const tvec2<T> other) const {
            return tvec2<T>{ this->x - other.x, this->y - other.y };
        }

        template <typename _FactorTyp>
        tvec2<T> operator*(const _FactorTyp factor) const {
            return tvec2<T>{ this->x * factor, this->y * factor };
        }

        template <typename _FactorTyp>
        tvec2<T> operator/(const _FactorTyp factor) const {
            return tvec2<T>{ this->x / factor, this->y / factor };
        }

        tvec2<T> rotate_cw_90() const {
            return tvec2<T>{ y, -x };
        }

        tvec2<T> rotate_ccw_90() const {
            return tvec2<T>{ -y, x };
        }

        auto dot(tvec2<T> other) const {
            return this->x * other.x + this->y * other.y;
        }

        float length_sqr() const {
            return this->dot(*this);
        }

        float length() const {
            return sqrt(this->length_sqr());
        }

        tvec2<T> normalize() const {
            return this->operator/(this->length());
        }

    private:
        friend std::ostream& operator<<(std::ostream& os, const moonrock::tvec2<T>& obj) {
            os << "{" << obj.x << ", " << obj.y << "}";
            return os;
        }

    };


    template <typename T>
    class tvec3 {

    public:
        T x = 0;
        T y = 0;
        T z = 0;

    public:
        tvec3<T> operator+(const tvec3<T> other) const {
            return tvec3<T>{ this->x + other.x, this->y + other.y, this->z + other.z };
        }

        tvec3<T> operator-(const tvec3<T> other) const {
            return tvec3<T>{ this->x - other.x, this->y - other.y, this->z - other.z };
        }

        template <typename _FactorTyp>
        tvec3<T> operator*(const _FactorTyp factor) const {
            return tvec3<T>{ this->x * factor, this->y * factor, this->z * factor };
        }

        template <typename _FactorTyp>
        tvec3<T> operator/(const _FactorTyp factor) const {
            return tvec3<T>{ this->x / factor, this->y / factor, this->z / factor };
        }

        auto dot(tvec3<T> other) const {
            return (this->x * other.x) + (this->y * other.y) + (this->z * other.z);
        }

        float length_sqr() const {
            return this->dot(*this);
        }

        float length() const {
            return sqrt(this->length_sqr());
        }

        tvec3<T> normalize() const {
            return this->operator/(this->length());
        }

        tvec2<T> xy() const {
            return tvec2<T>{ this->x, this->y };
        }

    private:
        friend std::ostream& operator<<(std::ostream& os, const moonrock::tvec3<T>& obj) {
            os << "{" << obj.x << ", " << obj.y << ", " << obj.z << "}";
            return os;
        }

    };


    template <typename T>
    class tvec4 {

    public:
        T x = 0;
        T y = 0;
        T z = 0;
        T w = 0;

    public:
        tvec2<T> xy() const {
            return tvec2<T>{ this->x, this->y };
        }

    private:
        friend std::ostream& operator<<(std::ostream& os, const moonrock::tvec4<T>& obj) {
            os << "{" << obj.x << ", " << obj.y << ", " << obj.z << ", " << obj.w << "}";
            return os;
        }

    };


    using vec2 = tvec2<float>;
    using uvec2 = tvec2<uint32_t>;

    using vec3 = tvec3<float>;
    using uvec3 = tvec3<uint32_t>;

    using vec4 = tvec4<float>;
    using uvec4 = tvec4<uint32_t>;

}

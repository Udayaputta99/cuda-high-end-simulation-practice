#pragma once

#include <algorithm>
#include <array>
#include <cmath>
#include <cstddef>
#include <functional>
#include <initializer_list>
#include <numeric>

/**
 * @brief Fixed-size numeric vector of dimension DIM.
 *
 * Provides basic arithmetic operations, element access, and
 * magnitude computations on a small, statically sized vector.
 *
 * @tparam T   Component type.
 * @tparam DIM Vector dimension.
 */
template <typename T, std::size_t DIM>
class Vec {
public:
  /**
   * @brief Default-construct a vector with all components value-initialized.
   */
  Vec() : v_{} {}

  /**
   * @brief Construct a vector with all components set to the same value.
   */
  explicit Vec(T value) {
    v_.fill(value);
  }

  /**
   * @brief Construct a vector from an initializer list.
   *
   * Copies up to DIM values from the list. If fewer than DIM values are
   * provided, the remaining components are filled with zero.
   */
  Vec(std::initializer_list<T> initList) : v_{} {
    std::size_t i = 0;

    for (const auto& value : initList) {
      if (i >= DIM) {
        break;
      }

      v_[i] = value;
      ++i;
    }

    for (; i < DIM; ++i) {
      v_[i] = T{};
    }
  }

  /**
   * @brief Converting copy constructor from a vector of another type.
   */
  template <typename S>
  Vec(const Vec<S, DIM>& other) {
    for (std::size_t i = 0; i < DIM; ++i) {
      v_[i] = static_cast<T>(other[i]);
    }
  }

  [[nodiscard]] std::size_t size() const {
    return v_.size();
  }

  [[nodiscard]] T* data() {
    return v_.data();
  }

  [[nodiscard]] const T* data() const {
    return v_.data();
  }

  [[nodiscard]] T* begin() {
    return v_.data();
  }

  [[nodiscard]] const T* begin() const {
    return v_.data();
  }

  [[nodiscard]] T* end() {
    return v_.data() + DIM;
  }

  [[nodiscard]] const T* end() const {
    return v_.data() + DIM;
  }

  T& operator[](std::size_t idx) {
    return v_.at(idx);
  }

  const T& operator[](std::size_t idx) const {
    return v_.at(idx);
  }

  /**
   * @brief Equality comparison.
   */
  bool operator==(const Vec<T, DIM>& other) const {
    for (std::size_t i = 0; i < DIM; ++i) {
      if (v_[i] != other.v_[i]) {
        return false;
      }
    }

    return true;
  }

  bool operator!=(const Vec<T, DIM>& other) const {
    return !(*this == other);
  }

  /**
   * @brief Unary minus.
   */
  Vec<T, DIM> operator-() const {
    Vec<T, DIM> negated;

    for (std::size_t i = 0; i < DIM; ++i) {
      negated[i] = -v_[i];
    }

    return negated;
  }

  /**
   * @brief In-place component-wise addition.
   */
  Vec<T, DIM>& operator+=(const Vec<T, DIM>& other) {
    for (std::size_t i = 0; i < DIM; ++i) {
      v_[i] += other.v_[i];
    }

    return *this;
  }

  /**
   * @brief Component-wise vector addition.
   */
  Vec<T, DIM> operator+(const Vec<T, DIM>& other) const {
    Vec<T, DIM> vec = *this;
    vec += other;
    return vec;
  }

  /**
   * @brief In-place component-wise subtraction.
   */
  Vec<T, DIM>& operator-=(const Vec<T, DIM>& other) {
    for (std::size_t i = 0; i < DIM; ++i) {
      v_[i] -= other.v_[i];
    }

    return *this;
  }

  /**
   * @brief Component-wise vector subtraction.
   */
  Vec<T, DIM> operator-(const Vec<T, DIM>& other) const {
    Vec<T, DIM> vec = *this;
    vec -= other;
    return vec;
  }

  /**
   * @brief In-place scaling by a scalar factor.
   */
  template <typename A>
  Vec<T, DIM>& operator*=(const A& alpha) {
    for (std::size_t i = 0; i < DIM; ++i) {
      v_[i] = static_cast<T>(v_[i] * alpha);
    }

    return *this;
  }

  /**
   * @brief Scaling by a scalar factor.
   */
  template <typename A>
  Vec<T, DIM> operator*(const A& alpha) const {
    Vec<T, DIM> vec = *this;
    vec *= alpha;
    return vec;
  }

  /**
   * @brief In-place division by a scalar factor.
   */
  template <typename A>
  Vec<T, DIM>& operator/=(const A& alpha) {
    for (std::size_t i = 0; i < DIM; ++i) {
      v_[i] = static_cast<T>(v_[i] / alpha);
    }

    return *this;
  }

  /**
   * @brief Division by a scalar factor.
   */
  template <typename A>
  Vec<T, DIM> operator/(const A& alpha) const {
    Vec<T, DIM> vec = *this;
    vec /= alpha;
    return vec;
  }

  /**
   * @brief Compute squared Euclidean norm.
   */
  [[nodiscard]] T sqrMagnitude() const {
    T total = T{};

    for (std::size_t i = 0; i < DIM; ++i) {
      total += v_[i] * v_[i];
    }

    return total;
  }

  /**
   * @brief Compute Euclidean norm.
   *
   * Works naturally for float/double vectors.
   */
  [[nodiscard]] T magnitude() const {
    return static_cast<T>(std::sqrt(static_cast<double>(sqrMagnitude())));
  }

private:
  std::array<T, DIM> v_;
};

/**
 * @brief Scalar-vector multiplication.
 *
 * Allows expressions like alpha * vec.
 */
template <typename A, typename T, std::size_t DIM>
Vec<T, DIM> operator*(const A& alpha, const Vec<T, DIM>& vec) {
  return vec * alpha;
}

/**
 * @brief 3-dimensional vector of floats.
 */
using Vec3F = Vec<float, 3>;

/**
 * @brief 3-dimensional vector of doubles.
 */
using Vec3D = Vec<double, 3>;
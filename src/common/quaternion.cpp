#include "quaternion.h"

    namespace modelViewer::common {

        const double PI = std::atan(1.0)*4;
        
        quaternion::quaternion(float _w, float _x, float _y, float _z) : w(_w), x(_x), y(_y), z(_z)
        {}

        quaternion::quaternion() : w(1.0f), x(0.0f), y(0.0f), z(0.0f)
        {}

        quaternion quaternion::operator*(const quaternion &q) const {
            return {
                    w * q.w - x * q.x - y * q.y - z * q.z,
                    w * q.x + x * q.w + y * q.z - z * q.y,
                    w * q.y - x * q.z + y * q.w + z * q.x,
                    w * q.z + x * q.y - y * q.x + z * q.w
            };
        }

        quaternion quaternion::operator*(float scalar) const {
            return {w * scalar, x * scalar, y * scalar, z * scalar};
        }

        quaternion quaternion::operator+(const quaternion &q) const {
            return {w + q.w, x + q.x, y + q.y, z + q.z};
        }

        quaternion quaternion::rotate(float angle, const glm::vec3 &axis) const {
            float halfAngle = angle * 0.5f;
            float sinHalf = std::sin(halfAngle);
            return quaternion(
                    std::cos(halfAngle),
                    axis.x * sinHalf,
                    axis.y * sinHalf,
                    axis.z * sinHalf
            ) * (*this);
        }
        
        quaternion quaternion::fromEuler(float pitch, float yaw, float roll) 
        {
            double cr = cos(roll * 0.5);
            double sr = sin(roll * 0.5);
            double cp = cos(pitch * 0.5);
            double sp = sin(pitch * 0.5);
            double cy = cos(yaw * 0.5);
            double sy = sin(yaw * 0.5);

            quaternion q;
            q.w = cr * cp * cy + sr * sp * sy;
            q.x = sr * cp * cy - cr * sp * sy;
            q.y = cr * sp * cy + sr * cp * sy;
            q.z = cr * cp * sy - sr * sp * cy;

            return q;
        }

        glm::mat4 quaternion::toMat4() const {
            glm::mat4 result;

            result[0][0] = 1.0f - 2.0f * (y * y + z * z);
            result[0][1] = 2.0f * (x * y - w * z);
            result[0][2] = 2.0f * (x * z + w * y);
            result[0][3] = 0.0f;

            result[1][0] = 2.0f * (x * y + w * z);
            result[1][1] = 1.0f - 2.0f * (x * x + z * z);
            result[1][2] = 2.0f * (y * z - w * x);
            result[1][3] = 0.0f;

            result[2][0] = 2.0f * (x * z - w * y);
            result[2][1] = 2.0f * (y * z + w * x);
            result[2][2] = 1.0f - 2.0f * (x * x + y * y);
            result[2][3] = 0.0f;

            result[3][0] = 0.0f;
            result[3][1] = 0.0f;
            result[3][2] = 0.0f;
            result[3][3] = 1.0f;

            return result;
        }

        quaternion quaternion::lerp(const quaternion &q1, const quaternion &q2, float t) {
            t = std::max(0.0f, std::min(1.0f, t)); // Clamp t between 0 and 1
            return (q1 * (1.0f - t)) + (q2 * t);
        }

        quaternion quaternion::normalize() const {
            float magnitude = std::sqrt(w * w + x * x + y * y + z * z);
            return quaternion(w / magnitude, x / magnitude, y / magnitude, z / magnitude);
        }
        
        glm::vec3 quaternion::toEuler() const {
            glm::vec3 euler;

            // roll (x-axis rotation)
            double sinr_cosp = 2 * (w * x + y * z);
            double cosr_cosp = 1 - 2 * (x * x + y * y);
            euler.z = std::atan2(sinr_cosp, cosr_cosp);

            // pitch (y-axis rotation)
            double sinp = std::sqrt(1 + 2 * (w * y - x * z));
            double cosp = std::sqrt(1 - 2 * (w * y - x * z));
            euler.x = 2 * std::atan2(sinp, cosp) - PI / 2;

            // yaw (z-axis rotation)
            double siny_cosp = 2 * (w * z + x * y);
            double cosy_cosp = 1 - 2 * (y * y + z * z);
            euler.y = std::atan2(siny_cosp, cosy_cosp);

            return euler;
        }
        
        quaternion quaternion::fromEuler(const glm::vec3& angles) {
            return quaternion::fromEuler(angles.x,angles.y,angles.z);
        }

        quaternion::quaternion(const quaternion &copy) {
            x = copy.x;
            y = copy.y;
            z = copy.z;
            w = copy.w;
        }

        quaternion &quaternion::operator=(const quaternion& copy) {
            x = copy.x;
            y = copy.y;
            z = copy.z;
            w = copy.w;
            return *this;
        }
    } 
    
// common
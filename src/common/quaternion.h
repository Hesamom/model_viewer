#ifndef MODEL_VIEWER_QUATERNION_H
#define MODEL_VIEWER_QUATERNION_H
	
    
    namespace modelViewer::common {

        class quaternion {
        public:
            
            float w = 1, x = 0, y = 0, z = 0;

            quaternion(const quaternion& copy);
            quaternion();
            quaternion(float _w, float _x, float _y, float _z);
            quaternion& operator=(const quaternion& copy);
            
            quaternion operator+(const quaternion& q) const;

            quaternion operator*(float scalar) const;

            quaternion operator*(const quaternion& q) const;

            quaternion normalize() const;
            quaternion rotate(float angle, const glm::vec3& axis) const;
            glm::mat4 toMat4() const;
            glm::vec3 toEuler() const;

            //angles are in radian
            static quaternion fromEuler(float pitch, float yaw, float roll);
            //angles are in radian
            static quaternion fromEuler(const glm::vec3& angles);
            static quaternion lerp(const quaternion& q1, const quaternion& q2, float t);
        };
    } 


#endif //MODEL_VIEWER_QUATERNION_H

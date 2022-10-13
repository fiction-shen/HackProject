
#include <memory>
#include <vector>
#include <imu.h>

class Snowman
{
private:
    /* data */
    std::vector<std::shared_ptr<ImuData>> imu_vec_;
    
    

    float base_yaw_;
    float yaw_thresh_;
    
    float wx_thresh_;

public:
    Snowman(/* args */);
    ~Snowman();
};

Snowman::Snowman(/* args */)
{
}

Snowman::~Snowman()
{
}

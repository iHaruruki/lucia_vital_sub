#include <rclcpp/rclcpp.hpp>
#include <std_msgs/msg/string.hpp>

class SensorSubscriber : public rclcpp::Node {
public:
    SensorSubscriber() : Node("sensor_subscriber") {
        // Pressure Dataの購読
        pressure_subscriber_ = this->create_subscription<std_msgs::msg::String>(
            "pressure_data", 10, std::bind(&SensorSubscriber::pressure_callback, this, std::placeholders::_1));

        // Vital Dataの購読
        vital_subscriber_ = this->create_subscription<std_msgs::msg::String>(
            "vital_data", 10, std::bind(&SensorSubscriber::vital_callback, this, std::placeholders::_1));
    }

private:
    // 圧力データのコールバック関数
    void pressure_callback(const std_msgs::msg::String::SharedPtr msg) {
        RCLCPP_INFO(this->get_logger(), "Received Pressure Data: '%s'", msg->data.c_str());
    }

    // バイタルデータのコールバック関数
    void vital_callback(const std_msgs::msg::String::SharedPtr msg) {
        RCLCPP_INFO(this->get_logger(), "Received Vital Data: '%s'", msg->data.c_str());
    }

    rclcpp::Subscription<std_msgs::msg::String>::SharedPtr pressure_subscriber_;
    rclcpp::Subscription<std_msgs::msg::String>::SharedPtr vital_subscriber_;
};

int main(int argc, char *argv[]) {
    rclcpp::init(argc, argv);
    rclcpp::spin(std::make_shared<SensorSubscriber>());
    rclcpp::shutdown();
    return 0;
}

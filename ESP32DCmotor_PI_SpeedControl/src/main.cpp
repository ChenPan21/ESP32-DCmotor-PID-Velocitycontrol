#include "Encoder.h"
#include <Arduino.h>
#include "DCMotor.h"

// --- 硬件引脚定义 ---
#define AN2 25
#define AN1 26
#define PWMA 18
#define ENCA_PIN 34
#define ENCB_PIN 35

// --- 电机和编码器对象 ---
DCMotor motorL;
Encoder enc;

// --- 物理参数 ---
const int PPR = 13 * 4; // 编码器每转的脉冲数
const float GEAR_RATIO = 30.0;
const int SAMPLE_TIME_MS = 20;  // 控制周期，建议 10~20ms

// --- 增量式 PID 参数 ---
double Kp = 0.1;  // 增量式PI的Kp需要重新整定
double Ki = 0.004; // 增量式PI的Ki需要重新整定
double Kd = 0.000; // 增量式PI速度环一般不使用Kd

// --- 控制变量 ---
const int MAX_PWM = 255;
const int DEAD_ZONE_PWM = 33; // 根据实际测试得到的最小启动PWM值
const double RAMP_RATE = 6000.0; // 每秒目标转速变化率，单位：RPM/s
double target_rpm = 600; // 串口设定的目标转速 编码器速度
double current_target_rpm = 0; // 实际用于PID计算的平滑目标转速


double error = 0;
double last_error = 0; // 上一次的偏差
double last_last_error = 0; // 上上次的偏差，用于Kd
double filtered_rpm = 0;

// 假设你已经定义了轮子直径（单位：厘米）
const double WHEEL_DIAMETER_CM = 6.5; 

// 计算车轮转速（单位：RPM）
double wheel_rpm = 0;

// 计算车轮周长（单位：米）
double wheel_circumference_m = 0;
// 计算小车速度（单位：米/秒）
double actual_speed_mps = 0;

// --- 滑动平均滤波变量 ---
const int num_readings = 3; // 滤波窗口缩小，减少延迟
double readings[num_readings];
int read_index = 0;
double total = 0;

unsigned long last_control_time = 0;
// 增量式PID需要一个持续累加的PWM变量
double current_pwm = 0;

void setup() {
    Serial.begin(115200);

    // 初始化电机和编码器
    motorL.attach(AN1, AN2, PWMA);
    motorL.invertDirection(); // 根据实际情况决定是否反向
    motorL.stop();
    
    enc.attach(ENCA_PIN, ENCB_PIN);
    enc.clear();

    // 初始化滤波数组
    for (int i = 0; i < num_readings; i++) {
        readings[i] = 0;
    }

    Serial.println("Bi-Directional Incremental PID Speed Control Initialized.");
    Serial.println("-------------------------------------------------");
    Serial.println("Target RPM, Current Target RPM, Filtered RPM, Raw RPM, PWM Output, Dir");
}

void loop() {
    // 串口输入目标转速，可以输入正/负数
    if (Serial.available() > 0) {
        String input = Serial.readStringUntil('\n');
        double new_target = input.toFloat();
        target_rpm = new_target;
        // 增量式PID不需要清空积分项
        Serial.print("New Target RPM set to: ");
        Serial.println(target_rpm);
    }

    // ------------------------------------
    // PID 控制循环
    // ------------------------------------
    unsigned long now = millis();
    if (now - last_control_time >= SAMPLE_TIME_MS) {
        // 1. 读取并计算实际转速
        long delta_counts = enc.getDeltaAndClear();
        double raw_rpm = ((double)delta_counts / (PPR)) * (60000.0 / SAMPLE_TIME_MS);

        // 2. 滑动平均滤波
        total -= readings[read_index];
        readings[read_index] = raw_rpm;
        total += readings[read_index];
        read_index = (read_index + 1) % num_readings;
        filtered_rpm = total / num_readings;

        // 3. 目标转速斜坡函数
        double delta_ramp = RAMP_RATE * (SAMPLE_TIME_MS / 1000.0);
        if (abs(target_rpm - current_target_rpm) > delta_ramp) {
            current_target_rpm += copysign(delta_ramp, target_rpm - current_target_rpm);
        } else {
            current_target_rpm = target_rpm;
        }

        // 4. 计算增量式 PI
        error = current_target_rpm - filtered_rpm;
        
        // 使用增量式PI公式计算PWM增量
        // Pwm增量 = Kp * (本次误差 - 上次误差) + Ki * 本次误差
        double pwm_increment = Kp * (error - last_error) + Ki * error;
        
        // 累加PWM增量
        current_pwm += pwm_increment;
        
        // 保存历史误差
        last_last_error = last_error;
        last_error = error;

        // 5. 设置电机方向 & PWM
        // PWM值需要限制在最大值和最小值之间
        bool target_dir = (current_pwm >= 0); // 根据PID输出的符号决定方向
        int final_pwm_output = (int)constrain(abs(current_pwm), 0, MAX_PWM);
        
        // 死区补偿：仅在目标转速不为0时进行
        if (target_rpm != 0 && final_pwm_output > 0 && final_pwm_output < DEAD_ZONE_PWM) {
            final_pwm_output = DEAD_ZONE_PWM;
        }
        // 计算车轮转速（单位：RPM）
        wheel_rpm = filtered_rpm / GEAR_RATIO;
        // 计算车轮周长（单位：米）
        wheel_circumference_m = PI * WHEEL_DIAMETER_CM / 100.0;
        // 计算小车速度（单位：米/秒）
        actual_speed_mps = (wheel_rpm / 60.0) * wheel_circumference_m;


        motorL.setSpeed(target_dir, final_pwm_output);
        
        // 6. 串口输出数据
        Serial.print(target_rpm);
        Serial.print(", ");
        Serial.print(current_target_rpm);
        Serial.print(", ");
        Serial.print(filtered_rpm);//编码器速度
        Serial.print(", ");
        Serial.print(wheel_rpm);//轮速
        Serial.print(", ");
        Serial.print(actual_speed_mps);//地速
        Serial.print(", ");
        Serial.println(final_pwm_output);
        // Serial.print(", ");
        // Serial.println((target_dir) ? "FWD" : "REV");
        
        last_control_time = now;
    }
}
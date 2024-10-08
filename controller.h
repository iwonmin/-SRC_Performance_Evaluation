#include "mbed.h"
#include <cstdint>
#include <string>
#include <stdlib.h>
#include "GP2A.h"
#include "rtos.h"
#pragma region Preprocessor
#define MAXSPEED 0.5
#define ESCAPESPEED -0.5
#define PSD_INTERVAL_us 0.1 // @@ dummy value, should be defined !!@@
#define PSD_THRESHOLD 5 // encounter distance(cm) diff, must be defined with experiment - threshold / inverval = speed
#define CIRCLE_DISTANCE 70 //cm
#define WALL_DISTANCE 70 //cm
#define TIME_90DEGTURN 50 //ms, pwm == 0.5
#define Time_10CMMOVE 20 //ms, pwm == 0.5
#define IMU_THRESHOLD 7.f
#define ESCAPE_TIME 500 //ms
#pragma endregion Preprocessor
#pragma region external
extern InterruptIn btn;
extern DigitalOut DirL;
extern DigitalOut DirR;
extern PwmOut PwmL;
extern PwmOut PwmR;
extern GP2A psdlf;
extern GP2A psdf;
extern GP2A psdrf;
extern GP2A psdlc;
extern GP2A psdrc;
extern GP2A psdlb;
extern GP2A psdb;
extern GP2A psdrb;
extern DigitalIn irfl;
extern DigitalIn irfr;
extern DigitalIn irfc;
extern DigitalIn irbc;
extern DigitalIn irbl;
extern DigitalIn irbr;
extern class Controller controller;
extern Thread Thread1;
extern Thread Thread2;
extern Thread Thread3;
extern Thread Thread4;
extern Serial pc;
extern RawSerial device;
extern Serial ebimu;
#pragma endregion external
class Controller
{
     public:   
        enum class RoboState
    {
        //초기
        START,
        //대기
        IDLE,
        //감지
        DETECT,
        //공격
        ATTACK,
        //탈출
        ESCAPE,
        //노랑
        YELLOW
    };

        enum class ColorOrient
    {
        FRONT, TAN_LEFT, TAN_RIGHT, BACK, FRONT_LEFT, FRONT_RIGHT, BACK_LEFT, BACK_RIGHT, SAFE    
    };
        enum class Position
    {
        ClosetoLeftWall, CriticalLeftWall, ClosetoRightWall, CriticalRightWall,
        WallFront, WallBehind, ClosetoCenter, FartoCenter
    };
        enum class TiltState
    {
        FRONT, FRONT_LEFT, FRONT_RIGHT, SIDE_LEFT, SIDE_RIGHT, SAFE
    };
        bool StartFlag = false;

        uint16_t psd_val[8]; //psdlf, psdf, psdrf, psdlc, psdrc, psdlb, psdb, psdrb
    //객체 생성시 실행되는 생성자 함수
    Controller();
//-------------------Get & Set methods----------------------//
    //현재 로봇의 상태 반환
    RoboState GetState();

    void CheckStartTime();
    
    uint64_t GetStartTime();
    //로봇의 상태를 변환
    void SetState(RoboState state);
    
    //적 감지 여부 반환
    bool GetEnemyState();
    
    //적 감지 여부 변환
    void SetEnemyState(bool enemyState);

    bool GetStartFlag();

    void SetStartFlag(bool sf);
    //적 감지 여부 변환
    void SetAttackState(bool attackState);
    //적 감지 여부 반환
    bool GetAttackState();

    //Ir 감지 여부 변환
    void SetIrSafetyState(bool IrSafetyState);
    //Ir 감지 여부 반환
    bool GetIrSafetyState();

    //Psd 감지 여부 변환
    void SetImuSafetyState(bool ImuSafetyState);
    //Psd 감지 여부 반환
    bool GetImuSafetyState();

    //벽 감지 여부 변환
    void SetWallSafetyState(bool WallSafetyState);
    //벽 감지 여부 반환
    bool GetWallSafetyState();

    //좌측 바퀴 속도 반환
    float GetSpeedL();
    
    //우측 바퀴 속도 반환
    float GetSpeedR();
    
    // 양쪽 바퀴 같은 속도 설정
    void SetSpeed(float speed);

    //양쪽 바퀴 다른 속도 설정
    void SetSpeed(float sL, float sR);

    //현재 yaw값 반환
    float GetCurrentYaw();
    
    //현재 yaw값 설정
    void SetCurrentYaw(float yaw);

    //노랑플래그 반환
    bool GetYellow();
    //노랑플래그 설정
    void SetYellow(bool yellow);
    //현재 노란영역 수평각도 반환
    int GetYA();
    //현재 노란 영역 수평 각도 설정
    void SetYA(int yellowAngle);
    //현재 노란 영역 중앙으로부터 거리 반환
    int GetYHD();
    //현재 노란 영역 중앙으로부터 거리 설정정
    void SetYHD(int yellow_horizontal_distance);
    
    //적과의 수평거리 반환 함수
    int GetHD();

    //적과의 수평거리 변환
    void SetHD(int HD);

    float NormalizeYaw(float angle);


//--------------------State Machine methods----------------------//
    //초기상태 시 실행 함수
    void Start();

    //대기상태 시 실행 함수
    void Idle();

     //감지상태 시 실행 함수
    void Detect();

     //공격상태 시 실행 함수
    void Attack();

     //탈출상태 시 실행 함수
    void Escape();
    //노랑상태 시 실행 함수
    void Yellow();

    //주행 함수
    void Move(float sL, float sR);

    void EnemyDetect();

    void Sex();
    //-----------------------psd--------------------//
    uint16_t PsdDistance(GP2A, uint8_t);

    void PsdRefresh();
    
    void PsdDetect();

    void PsdWallDetect();

    void PsdWallEscape();
    
    //-------------------------IR------------------------//
    Position GetPosition();

    void IrRefresh();
    
    void IrRefresh_new();

    void EnemyPushPull();

    void IrEscape();

    // void IrEscapeWhenImuUnsafe();

    void ColorOrient();
    
    void ColorOrient_new();

    enum ColorOrient GetOrient();
    //----------------------적 찾기 & 위치파악 & 적 괴롭히기 전략-------------------------//
    // void SetPosition();

    void EnemyFind(Position);
    
    void EnemyFind_Extended(Position);

    void LeftWallTrack();

    void RightWallTrack();

    void WallTwerk();
    /*
    void CenterSpin();
    
    void FrontWall();

    void BehindWall();
    */

    //-----------------------MPU9250, IMU-----------------------------//

    void ImuDetect();

    void ImuParse();

    void ImuChartoData();
    
    void ImuEscape();

    bool ImuPitchLift = false;

    bool ImuRollLift = false;

    Timer Escape_Timer;

    float roll, pitch, yaw, currentyaw, prevyaw, normalized_yaw;

    float ax, ay, az;
    //------------------------Tester's Choice-------------------//
    void OrientViewer();

    void WallViewer();

    void ImuViewer();
//--------------------Private variables--------------------------//
    private:
    //시간 세기
    uint64_t StartTime = 0;
    //로봇 상태
    RoboState robo_state;
    //색 영역 위치
    enum ColorOrient Orient;
    //예상되는 위치
    enum Position CurrentPos;
    //Imu 상태
    enum TiltState tilt_state = TiltState::SAFE;
    //적 감지 여부
    volatile bool enemy = false;

    volatile bool attack = false;
    
    //적과 벌어진 거리
    int enemy_horizontal_distance = 1;

    //노란 영역 진입 플래그
    bool yellow = false;
    //노란영역 평행 각도
    float yellowAngle;
    //노란 영역 중앙까지 거리리
    int yellow_horizontal_distance;

    //위험 지역 여부
    volatile bool irSafe = true;

    volatile bool imuSafe = true;

    volatile bool wallSafe = true;

    //좌측 바퀴 속력
    float speedL;

    //우측 바퀴 속력
    float speedR;

    float currentYaw;

    const float alpha_psd = 0.9f;

    uint16_t prev_distance[8]; //psdlf, psdf, psdrf, psdlc, psdrc, psdlb, psdb, psdrb

    uint16_t now_distance[8]; //psdlf, psdf, psdrf, psdlc, psdrc, psdlb, psdb, psdrb

    uint16_t filtered_distance[8]; //psdlf, psdf, psdrf, psdlc, psdrc, psdlb, psdb, psdrb

    bool detection[8]; //psdlf, psdf, psdrf, psdlc, psdrc, psdlb, psdb, psdrb

    uint16_t MinValue = 0;

    uint8_t MinIndex = 0;

    uint8_t FollowIndex = 0;

    bool ir_val[6]; //irfl, irfr, irc, irbl, irbr //미리 선언되어야 함.

    uint8_t ir_total; 

    //벽 충돌 감지
    //벽이 두방향에서 보일때는 다 색영역인데 그냥 열거형 쓰기 ??
    bool FrontCollision;

    bool BackCollision;

    bool LeftCollision;

    bool RightCollision;

    int lastDirection;

    /*Good bye Mpu9250 ㅠㅠ
    const float alpha_imu = 0.93f;

    float gyro_angle_x, gyro_angle_y, gyro_angle_z;

    float accel_angle_x, accel_angle_y, mag_angle_z;
    */

    //-------------------------------EBIMU-------------------------------//
    char data[32] = "";

    Timer t; //for gyro integral;


    bool PitchLift = false;
    
    bool RollLift = false;
    
};

//-------------------------Thread----------------------------//
void ImuThread();

void PsdThread();

void DetectThread();

void DetectThread2();

void EnemyDetect3();

int calculateChecksum(char *data, int length);

void processPacket(char *data, int length);

void Starter();

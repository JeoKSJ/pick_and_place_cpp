#include <iostream>
using namespace std;

// ========================================
// 3차원 좌표를 저장하기 위한 구조체
// ========================================
struct Position {
    double x;
    double y;
    double z;
};

// ========================================
// Position 좌표 출력
// const &: 원본을 복사하지 않고 읽기만 함
// ========================================
void printPosition(const Position& p) {
    cout << "("
         << p.x << ", "
         << p.y << ", "
         << p.z << ")";
}

// ========================================
// 두 Position이 같은 위치인지 확인
//
// 현재는 가상 좌표이므로 == 비교 사용.
// 실제 로봇에서는 좌표 오차가 존재하기 때문에
// 허용 오차(tolerance)를 이용한 비교가 필요함.
// ========================================
bool isSamePosition(const Position& a, const Position& b) {
    return a.x == b.x &&
           a.y == b.y &&
           a.z == b.z;
}

// ========================================
// 로봇 위치 이동
//
// current : 현재 로봇 위치
// target  : 목표 위치
//
// current는 실제 값이 변경되어야 하므로 참조(&)
// target은 읽기만 하므로 const 참조(const &)
// ========================================
void moveTo(Position& current, const Position& target) {
    cout << "이동: ";
    printPosition(current);
    cout << " -> ";
    printPosition(target);
    cout << endl;

    // 현재 로봇 위치를 목표 위치로 변경
    current = target;
}

// ========================================
// Pick 동작
//
// holding_obj    : 현재 물체를 들고 있는지 여부
// robot_position : 현재 로봇 위치
// block_position : 현재 블록 위치
//
// 성공하면 true
// 실패하면 false 반환
// ========================================
bool pick(bool& holding_obj,
          const Position& robot_position,
          const Position& block_position) {

    // 이미 물체를 들고 있다면 추가 Pick 불가
    if (holding_obj) {
        cout << "이미 물체를 들고 있습니다." << endl;
        return false;
    }

    // 로봇이 블록 위치에 있지 않다면 Pick 불가
    if (!isSamePosition(robot_position, block_position)) {
        cout << "물체 위치가 아닙니다." << endl;
        return false;
    }

    // Pick 성공
    holding_obj = true;
    cout << "물체를 집었습니다." << endl;

    return true;
}

// ========================================
// Place 동작
//
// holding_obj     : 물체를 들고 있는지 여부
// block_position  : 현재 블록 위치
// robot_position  : 현재 로봇 위치
// place_position  : 물체를 내려놓을 목표 위치
//
// 성공하면 true
// 실패하면 false 반환
// ========================================
bool place(bool& holding_obj,
           Position& block_position,
           const Position& robot_position,
           const Position& place_position) {

    // 들고 있는 물체가 없으면 Place 불가
    if (!holding_obj) {
        cout << "놓을 물체가 없습니다." << endl;
        return false;
    }

    // 로봇이 목표 Place 위치에 도착하지 않았다면 중단
    if (!isSamePosition(robot_position, place_position)) {
        cout << "놓을 위치가 아닙니다." << endl;
        return false;
    }

    // Gripper에서 물체를 놓음
    holding_obj = false;

    // 물체의 새로운 위치를 현재 로봇 위치로 갱신
    block_position = robot_position;

    cout << "물체를 놓았습니다. 위치: ";
    printPosition(block_position);
    cout << endl;

    return true;
}

// ========================================
// Pick & Place 전체 작업 수행
//
// 1. 현재 블록 위치로 이동
// 2. Pick
// 3. 목표 위치로 이동
// 4. Place
//
// Pick, Place 실패 시 즉시 작업 중단
// 성공 시 true 반환
// ========================================
bool pickAndPlace(bool& holding_obj,
                  Position& block_position,
                  Position& robot_position,
                  const Position& target_position) {

    // ------------------------------------
    // 1. Pick
    // ------------------------------------

    // 현재 블록 위치로 로봇 이동
    moveTo(robot_position, block_position);

    // Pick 시도
    if (!pick(holding_obj, robot_position, block_position)) {
        cout << "Pick 실패 - 작업을 중단합니다." << endl;
        return false;
    }

    // ------------------------------------
    // 2. Place
    // ------------------------------------

    // 목표 위치로 로봇 이동
    moveTo(robot_position, target_position);

    // 물체 내려놓기
    if(!place(holding_obj, block_position, robot_position, target_position)){
        cout << "Place 실패 - 작업을 중단합니다." << endl;
        return false;
    }

    return true;
}

int main() {

    // ====================================
    // 작업에 사용할 고정 위치
    // ====================================

    // 로봇 Home 위치
    Position home{0.0, 0.0, 0.0};

    // 첫 번째 Place 위치
    Position place_position{-10.0, 10.0, 0.0};

    // 두 번째 Place 위치
    Position second_place{0.0, -10.0, 0.0};


    // ====================================
    // 현재 상태
    // ====================================

    // 초기 로봇 위치
    // Home이 아닌 위치로 설정하여 Home 복귀 동작 확인
    Position robot_position{30.0, 20.0, 5.0};

    // 초기 블록 위치
    Position block_position{10.0, 10.0, 0.0};

    // End Effector가 물체를 들고 있는지 나타내는 상태 변수
    // false : 물체 없음
    // true  : 물체 들고 있음
    bool holding_obj = false;


    // ====================================
    // Pick & Place Sequence
    // ====================================

    // 0. Home 이동
    moveTo(robot_position, home);

    // 1~2. 첫 번째 Pick & Place
    // Block:
    // (10, 10, 0)
    //      ↓
    // (-10, 10, 0)
    pickAndPlace(
        holding_obj,
        block_position,
        robot_position,
        place_position
    );

    // 3. Home 복귀
    moveTo(robot_position, home);

    // 4. 두 번째 Pick & Place
    //
    // 첫 번째 작업에서 block_position이
    // place_position으로 변경되었기 때문에
    // 현재 블록 위치에서 다시 Pick하게 됨.
    //
    // Block:
    // (-10, 10, 0)
    //       ↓
    // (0, -10, 0)
    pickAndPlace(
        holding_obj,
        block_position,
        robot_position,
        second_place
    );

    // 5. Home 복귀
    moveTo(robot_position, home);

    // 6. Pick & Place 성공 여부 확인
    //
    // 두 번째 작업 후 block_position은
    // second_place 위치로 갱신되어 있음.
    //
    // 해당 위치의 블록을 다시 place_position으로 이동
    if (pickAndPlace(
            holding_obj,
            block_position,
            robot_position,
            place_position))
    {
        cout << "Pick & Place 성공" << endl;
    }
    else {
        cout << "Pick & Place 실패" << endl;
    }

    return 0;
}
#include <TimerOne.h>

#define ACTIVE_TIME (500)       // 同じ方向の入力が有効である時間
#define MATCH_COUNT (3)         // 方向一致回数

// 入力方向判別
#define NONE  (0)
#define LEFT  (1)
#define RIGHT (2)

// チャタリング対策用カウンタ
#define COUNT_NONE  (cnt[0])
#define COUNT_RIGHT (cnt[1])
#define COUNT_LEFT  (cnt[2])

// センサ状態
#define S1_STATE_A (state[0])   // センサ1の前の状態
#define S2_STATE_A (state[1])   // センサ2の前の状態
#define S1_STATE_B (state[2])   // センサ1の現在の状態
#define S2_STATE_B (state[3])   // センサ2の現在の状態

const int SENSOR_1 = 3;         // PB3をセンサ1の入力とする
const int SENSOR_2 = 4;         // PB4をセンサ2の入力とする

const int OUTPUT_1 = 2;         // PB2を出力1とする
const int OUTPUT_2 = 1;         // PB1を出力2とする

int state[4] = {0, 0, 0, 0};    // センサの状態
int list[8][4] = {              // センサの動作パターンリスト
  {1, 1, 0, 1},
  {1, 0, 1, 1},
  {0, 0, 1, 0},
  {0, 1, 0, 0},
  {1, 1, 1, 0},
  {1, 0, 0, 0},
  {0, 1, 1, 1},
  {0, 0, 0, 1}
};

int input  = NONE;
int cnt[3] = {0, 0, 0};

// 1msごとに入力をチェックする割り込み関数
void input_check() {

  // センサ状態確認
  S1_STATE_B = digitalRead(SENSOR_1);
  S2_STATE_B = digitalRead(SENSOR_2);

  // 状態の比較
  for (int i = 0; i < 8; i++) {
    if (memcmp(list[i], state, sizeof(int) * 4) == 0) {
      COUNT_NONE = 0;
      if ((i >= 0) && (i < 4)) {            // 右回転
        COUNT_LEFT  = 0;
        if(COUNT_RIGHT++ >= MATCH_COUNT) {
          COUNT_RIGHT = 0;
          input       = RIGHT;
        }
      }
      else if ((i > 4) && (i < 8) ) {       // 左回転
        COUNT_RIGHT = 0;
        if(COUNT_LEFT++ >= MATCH_COUNT) {
          COUNT_LEFT = 0;
          input      = LEFT;
        }
      }
    }
    else {
      if ((COUNT_NONE++ >= ACTIVE_TIME) && (input != NONE)) {  // 入力なし
        COUNT_LEFT  = 0;
        COUNT_RIGHT = 0;
        input       = NONE;
      }
    }
  }

  // 今のセンサ状態を保存
  S1_STATE_A = S1_STATE_B;
  S2_STATE_A = S2_STATE_B;

}

// 初期設定
void setup() {

  pinMode(OUTPUT_1, OUTPUT);
  pinMode(OUTPUT_2, OUTPUT);

  pinMode(SENSOR_1, INPUT_PULLUP);
  pinMode(SENSOR_2, INPUT_PULLUP);
  
  S1_STATE_A = digitalRead(SENSOR_1);
  S2_STATE_A = digitalRead(SENSOR_2);

  // タイマ割り込み設定 1msごとに入力チェック
  Timer1.initialize(1000);
  Timer1.attachInterrupt(input_check);
  Timer1.start();

}

// メインループ
void loop() {

  switch (input) {
    case RIGHT:
      digitalWrite(OUTPUT_1, HIGH);
      digitalWrite(OUTPUT_2, LOW);
      break;
    case LEFT:
      digitalWrite(OUTPUT_1, LOW);
      digitalWrite(OUTPUT_2, HIGH);
      break;
    case NONE:
      digitalWrite(OUTPUT_1, HIGH);
      digitalWrite(OUTPUT_2, HIGH);
      break;
  }

}

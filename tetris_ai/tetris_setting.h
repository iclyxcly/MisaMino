#define PUBLIC_VERSION      1 // 发布模式
#define GAMEMODE_4W         0 // 4w模式
#define AI_TRAINING_SLOW    0
#define USE4W               1
#define AI_SHOW             0 // 不相互攻击，围观AI
#if AI_TRAINING_SLOW
#define AI_TRAINING_DEEP    16
#else
#define AI_TRAINING_DEEP    6 // 训练AI思考深度
#endif
#define TRAINING_ROUND      20
#define AI_TRAINING_0       9
#define AI_TRAINING_2       6

#define AI_DLL_VERSION      2 // dll版本

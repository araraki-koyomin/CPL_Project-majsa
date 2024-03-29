#include "AgariChecker.h"
#include "YakuChecker.h"
#include "majsa.h"

int isMachi[zhong + 1];

/// @brief 判断是否为胡牌型
/// @param status
/// @param handTile1
/// @param discardTile1
/// @param currentTile1
/// @param groupTile1
/// @return 满足胡牌型且不振听，返回true，否则返回false
bool AgariCheck(Status status, int *handTile1, int *discardTile1, int currentTile1, GroupInt *groupTile1)
{
    // 我们知道，麻将胡牌型为 4面子 + 1对子 / 7对子 / 国士无双，我们接下来将对这三种情况进行判断
    // 首先从 门前清（不含暗杠） 与 副露 两种状态进行判断
    int ismenzenchin = IsMenzenchin(status);
    if (ismenzenchin) {
        // 常见胡牌型判断
        if (IsAgari(handTile1, 4, discardTile1, currentTile1, status)) {
            // 判断振听。若振听，结果类型为FURUTEN，返回false
            if (IsFuritenInAgari(discardTile1, currentTile1, status))
            {
                result->type = FURITEN;
                return false;
            }
            // 未振听，返回true，表示胡牌
            return true;
        }
        // 七对子判断
        if (IsChiitoitsuHai(handTile1)) {
            // 判断振听。若振听，结果类型为FURUTEN，返回false
            if (IsFuritenInAgari(discardTile1, currentTile1, status)) {
                result->type = FURITEN;
                return false;
            }
            // 未振听，返回true，表示胡牌
            YakuCheck(status, handTile1, groupTile1, discardTile1, currentTile1);
            result = resultTemp;
            return true;
        }
        // 国士无双判断
        if (IsKoukushimusou(handTile1, currentTile1)) {
            if (IsFuritenInAgari(discardTile1, currentTile1, status)) {
                result->type = FURITEN;
                return false;
            }
            YakuCheck(status, handTile1, groupTile1, discardTile1, currentTile1);
            result = resultTemp;
            return true;
        }
        
    }
    else { // 副露状态（含暗杠）
        int fuluNumber = GroupTileLen;
        if (IsAgari(handTile1, 4 - fuluNumber, discardTile1, currentTile1, status)) {
                if (IsFuritenInAgari(discardTile1, currentTile1, status)) {
                    result->type = FURITEN;
                    return false;
                }
                return true;
        }
        else if (ismenzenchin) {
            // 检验是否七对听牌或国士无双听牌
            if (!Is7gTennpai(handTile1, discardTile1, currentTile1, status)) {
                // 计算向听数
                result->type = NOTEN;
            }
        }
    }
}

/// @brief 判断是否满足4+1胡牌型
/// @param handTile1
/// @param needMentsu
/// @return 满足返回true，否则返回false
bool IsAgari(int handTile1[], int needMentsu, int *discardTile1, int currentTile1, Status status) {
    bool flag = false;
    // 统计手牌中各牌个数，为虚听做好准备
    int bucket[zhong + 1] = {0};
    for (int i = 0; i < 14; i++)
        bucket[handTile1[i]]++;
    for (int i = 0; i <= 13; i++) {
        if (handTile1[i] == handTile1[i + 1]) {
            // 先找出雀头
            int tmp1 = handTile1[i], tmp2 = handTile1[i + 1];
            handTile1[i] = 0;
            handTile1[i + 1] = 0;
            mentsuType.jyan[0] = tmp1, mentsuType.jyan[1] = tmp2;
            // 再找出4个面子
            int mentsu = 0;
            if (FindShuntsu(handTile1, 0, mentsu, discardTile1, currentTile1, status, bucket, needMentsu) || FindKoutsu(handTile1, 0, mentsu, discardTile1, currentTile1, status, bucket, needMentsu)) {
                flag = 1;
            }
            handTile1[i] = tmp1, handTile1[i + 1] = tmp2;
        }
    }
    return flag;
}

/// @brief 判断在胡牌型的基础上是否振听
/// @param discardTile1
/// @param currentTile1
/// @param status
/// @return 振听返回true，否则返回false
bool IsFuritenInAgari(int *discardTile1, int currentTile1, Status status) {
    for (int i = 0; i < DisLen; i++) {
        if (discardTile1[i] == currentTile1 && status.currentPlayer != JICHA)
            return true;
    }
    return false;
}

/// @brief 判断是否门前清状态
/// @param status
/// @return 门前清返回true, 副露返回false
bool IsMenzenchin(const Status status) {
    if (status.groupTile[0].tile[0] == 0)
        return true;
    for (int i = 0; i < GroupTileLen; i++) {
        if (groupTile1[i].fulutype != Ankan)
            return false;
    }
    return true;
}

/// @brief 判断是否为七对子牌型
/// @param handTile1
/// @return 是七对子则返回true, 否则返回false
bool IsChiitoitsuHai(int *handTile1) {
    for (int i = 0; i < handTilelLen; i += 2)
        if (handTile1[i] != handTile1[i + 1] || handTile1[i + 1] == handTile1[i + 2])
            return false;
    // printf("*chiitoitsu*");
    return true;
}

/// @brief 判断是否为国士无双（十三面）
/// @param handTile1
/// @param currentTile1
/// @return 不是，返回0；是国士无双，返回1；是国士无双十三面，返回2
int IsKoukushimusou(const int *handTile1, const int currentTile1) {
    int counts[zhong + 1] = {0}, flag = 1;
    for (int i = 0; i < handTilelLen; i++) {
        if (handTile1[i] == im || handTile1[i] == km || handTile1[i] == ip || handTile1[i] == kp || handTile1[i] == is || handTile1[i] >= ks)
            counts[handTile1[i]]++;
        else
            return 0;
    }
    for (int i = im; i <= zhong; i++) {
        if (counts[i] > 2)
            return 0;
        else if (counts[i] != 1)
            flag = 0;
    }
    return flag == 1 ? 2 : 1;
}

/// @brief 从当前索引号index向右寻找顺子
/// @param handTile1
/// @param index
/// @return 能找到返回true，反之返回false
bool FindShuntsu(int handTile1[], int index, int mentsu, int *discardTile1, int currentTile1, Status status, int bucket[], int needMentsu) {
    // 返回成功条件：手牌清空
    int i1 = 0, i2 = 0, i3 = 0;
    int flag = 0;
    int value1 = handTile1[0], value2 = handTile1[0], value3 = handTile1[0];
    int checkIfZero = 0;
    for (int i = 0; i < 14; i++)
        if (handTile1[i] == 0)
            checkIfZero++;
    if (checkIfZero == 14) {
        YakuCheck(status, handTile2, groupTile1, discardTile1, currentTile1);
        if (resultTemp->han) {
            // result = (result->point[0] + result->point[1] + result->point[2]) > (resultTemp->point[0] + resultTemp->point[1] + resultTemp->point[2]) ? result : resultTemp;
            if ((result->point[0] + result->point[1] + result->point[2]) < (resultTemp->point[0] + resultTemp->point[1] + resultTemp->point[2])) {
                result->han = resultTemp->han;
                result->fu = resultTemp->fu;
                for (int i = KAMICHA; i <= SHIMOCHA; i++) {
                    result->point[i] = resultTemp->point[i];
                }
                result->type = resultTemp->type;
                for (int i = 0; resultTemp->yaku[i] != 0; i++) {
                    result->yaku[i] = resultTemp->yaku[i];
                }
            }
        }
        resultTemp->fu = 0;
        resultTemp->han = 0;
        memset(resultTemp->point, 0, 3);
        resultTemp->type = NOTEN;
        memset(resultTemp->yaku, 0, sizeof(resultTemp->yaku));
        return true;
    }
    if (index >= 12)
        return false;
    // 寻找顺子
    for (int i = index; i < 12; i++) {
        if (handTile1[i] == 0)
            continue;
        for (int j = 0; j < 13; j++) {
            if (handTile1[j] == handTile1[i] + 1) {
                for (int k = j + 1; k < 14; k++) {
                    if (handTile1[k] == handTile1[j] + 1) {
                        value1 = handTile1[i], value2 = handTile1[j], value3 = handTile1[k];
                        mentsuType.shuntsunum++;
                        mentsuType.shun[mentsuType.shuntsunum - 1][0] = value1,
                        mentsuType.shun[mentsuType.shuntsunum - 1][1] = value2,
                        mentsuType.shun[mentsuType.shuntsunum - 1][2] = value3;
                        i1 = i, i2 = j, i3 = k;
                        handTile1[i] = 0, handTile1[j] = 0, handTile1[k] = 0;
                        flag = 1;
                        goto NEXT1;
                        // 找到了就退出
                    }
                }
            }
        }
    }
    NEXT1:
    if (!flag) {
        return false;
    }
    bool CanFind = FindShuntsu(handTile1, index + 1, mentsu + 1, discardTile1, currentTile1, status, bucket, needMentsu);
    if (CanFind) {
        mentsuType.shuntsunum--;
        memset(mentsuType.shun[mentsuType.shuntsunum], 0, sizeof(int) * 3);
        handTile1[i1] = value1, handTile1[i2] = value2, handTile1[i3] = value3;
    }
    CanFind = FindKoutsu(handTile1, index + 1, mentsu + 1, discardTile1, currentTile1, status, bucket, needMentsu);
    if (CanFind) {
        mentsuType.shuntsunum--;
        memset(mentsuType.shun[mentsuType.shuntsunum], 0, sizeof(int) * 3);
        handTile1[i1] = value1, handTile1[i2] = value2, handTile1[i3] = value3;
    }
    if (result->type != TSUMO && result->type != RON) {
        // 检验是否听牌
        int IsTen = Is41Tennpai(mentsuType.koutsunum + mentsuType.shuntsunum + GroupTileLen, discardTile1, currentTile1, status, handTile1, bucket);
        if (result->type != TENPAI && result->type != FURITEN) {
            // 计算最大向听数
        } 
    }
    if (result->type == TSUMO || result->type == RON) {
        return true;
    } else {
        // 清算
        mentsuType.shuntsunum--;
        mentsuType.shun[mentsuType.shuntsunum][0] = 0,
        mentsuType.shun[mentsuType.shuntsunum][1] = 0,
        mentsuType.shun[mentsuType.shuntsunum][2] = 0;
        // 还原
        handTile1[i1] = value1, handTile1[i2] = value2, handTile1[i3] = value3;
        return false;
    }
}

/// @brief 从当前索引号index向右寻找刻子
/// @param handTile1
/// @param index
/// @return 能找到返回true，反之返回false
bool FindKoutsu(int handTile1[], int index, int mentsu, int *discardTile1, int currentTile1, Status status, int bucket[], int needMentsu) {
    // 返回成功条件：手牌清空
    int flag = 0;
    int value1, value2, value3;
    int checkIfZero = 0;
    for (int i = 0; i < 14; i++)
        if (handTile1[i] == 0)
            checkIfZero++;
    if (checkIfZero == 14) {
        YakuCheck(status, handTile2, groupTile1, discardTile1, currentTile1);
        if (resultTemp->han) {
            // result = (result->point[0] + result->point[1] + result->point[2]) > (resultTemp->point[0] + resultTemp->point[1] + resultTemp->point[2]) ? result : resultTemp;
            if ((result->point[0] + result->point[1] + result->point[2]) < (resultTemp->point[0] + resultTemp->point[1] + resultTemp->point[2])) {
                result->han = resultTemp->han;
                result->fu = resultTemp->fu;
                for (int i = KAMICHA; i <= SHIMOCHA; i++) {
                    result->point[i] = resultTemp->point[i];
                }
                result->type = resultTemp->type;
                for (int i = 0; resultTemp->yaku[i] != 0; i++) {
                    result->yaku[i] = resultTemp->yaku[i];
                }
            }
        }
        resultTemp->fu = 0;
        resultTemp->han = 0;
        memset(resultTemp->point, 0, 3);
        resultTemp->type = NOTEN;
        memset(resultTemp->yaku, 0, sizeof(resultTemp->yaku));
        return true;
    }
    if (index >= 12)
        return false;
    // 寻找刻子
    int i1 = 0;
    for (int i = index; i < 12; i++) {
        if (handTile1[i] == handTile1[i + 1] && handTile1[i + 1] == handTile1[i + 2] && handTile1[i] != 0) {
            mentsuType.koutsunum++;
            mentsuType.kou[mentsuType.koutsunum - 1][0] = handTile1[i];
            mentsuType.kou[mentsuType.koutsunum - 1][1] = handTile1[i];
            mentsuType.kou[mentsuType.koutsunum - 1][2] = handTile1[i];
            value1 = handTile1[i], value2 = handTile1[i + 1], value3 = handTile1[i + 2];
            handTile1[i] = 0, handTile1[i + 1] = 0, handTile1[i + 2] = 0;
            i1 = i;
            flag = 1;
            break;
        }
    }
    if (!flag) 
        return false;
    bool CanFind = FindShuntsu(handTile1, index + 1, mentsu + 1, discardTile1, currentTile1, status, bucket, needMentsu);
    if (CanFind) {
        mentsuType.koutsunum--;
        memset(mentsuType.kou[mentsuType.koutsunum], 0, sizeof(int) * 3);
        handTile1[i1] = value1, handTile1[i1 + 1] = value2, handTile1[i1 + 2] = value3;
    }
    CanFind = FindKoutsu(handTile1, index + 1, mentsu + 1, discardTile1, currentTile1, status, bucket, needMentsu);
    if (CanFind) {
        mentsuType.koutsunum--;
        memset(mentsuType.kou[mentsuType.koutsunum], 0, sizeof(int) * 3);
        handTile1[i1] = value1, handTile1[i1 + 1] = value2, handTile1[i1 + 2] = value3;
    }
    if (result->type != TSUMO && result->type != RON) {
        // 检验是否听牌
        int IsTen = Is41Tennpai(mentsuType.koutsunum + mentsuType.shuntsunum + GroupTileLen, discardTile1, currentTile1, status, handTile1, bucket);
        if (result->type != TENPAI && result->type != FURITEN) {
            // 计算最大向听数
        } 
    }
    if (result->type == TSUMO || result->type == RON) {
        return true;
    } else {
        // 清算
        mentsuType.koutsunum--;
        mentsuType.kou[mentsuType.koutsunum][0] = 0,
        mentsuType.kou[mentsuType.koutsunum][1] = 0,
        mentsuType.kou[mentsuType.koutsunum][2] = 0;
        // 还原
        handTile1[i1] = value1, handTile1[i1 + 1] = value2, handTile1[i1 + 2] = value3;
        return false;
    }
}

int Cmp(const void *a, const void *b) {
    return *(int *)a - *(int *)b;
}

/// @brief 检验是否4 + 1听牌
/// @param mentsu
/// @param discardTile1
/// @param currentTile1
/// @param status
/// @param handTile1
/// @return 听牌返回true， 未听返回false
bool Is41Tennpai(int mentsu, int *discardTile1, int currentTile1, Status status, int handTile1[], int bucket[]) {
    bool flag = false;
    if (mentsu != 3)
        return false;
    int nokoru[3], index = 0;
    for (int i = 0; i < 14; i++)
        if (handTile1[i])
            nokoru[index++] = handTile1[i];
    // 判断剩余3张牌是否形成搭子
    // 听对碰
    for (int i = 0; i < 2; i++) {
        if (nokoru[i] == nokoru[i + 1] && bucket[i] != 4) {
            flag = true;
            if (!isMachi[nokoru[i]]) {
                result->machi++;
                isMachi[nokoru[i]] = 1;
            }
            // 检验振听
            for (int j = 0; j < DisLen; j++)
                if (nokoru[i] == discardTile1[j]) {
                    result->type = FURITEN;
                    break;
                }
        }
    }
    // 听边张
    for (int i = 0; i < 2; i++) {
        if (nokoru[0] == im + 9 * i && nokoru[1] == nm + 9 * i && bucket[nokoru[0] + 2] != 4) {
            flag = true;
            if (!isMachi[nokoru[0] + 2]) {
                result->machi++;
                isMachi[nokoru[0] + 2] = 1;
            }
            for (int j = 0; j < DisLen; j++)
                if (nokoru[0] + 2 == discardTile1[j]) {
                    result->type = FURITEN;
                    break;
                }
        }
        if (nokoru[1] == hm + 9 * i && nokoru[2] == hm + 9 * i && bucket[nokoru[1] - 1] != 4) {
            flag = true;
            if (!isMachi[nokoru[1] - 1]) {
                result->machi++;
                isMachi[nokoru[1] - 1] = 1;
            }
            for (int j = 0; j < DisLen; j++)
                if (nokoru[1] - 1 == discardTile1[j]) {
                    result->type = FURITEN;
                    break;
                }
        }
    }
    // 听嵌张
    for (int i = 0; i < 2; i++) {
        if (nokoru[0] == nokoru[2] - 1 && nokoru[2] != im + 9 * i && nokoru[2] != nm + 9 * i && bucket[nokoru[0] + 1] != 4) {
            flag = true;
            if (!isMachi[nokoru[0] + 1]) {
                result->machi++;
                isMachi[nokoru[0] + 1] = 1;
            }
            for (int j = 0; j < DisLen; j++)
                if (nokoru[0] + 1 == discardTile1[j]) {
                    result->type = FURITEN;
                    break;
                }
        }
    }
    // 听两面
    for (int k = 0; k < 2; k++) {
        for (int i = 0; i < 2; i++) {
            if (nokoru[k] == nokoru[k + 1] - 1 && nokoru[k] > im + 9 * i && nokoru[k + 1] < km + 9 * i) {
                if (bucket[nokoru[k] - 1] != 4) {
                    flag = true;
                    if (!isMachi[nokoru[k] - 1]) {
                        result->machi++;
                        isMachi[nokoru[k] - 1] = 1;
                    }
                    for (int j = 0; j < DisLen; j++)
                        if (nokoru[k] - 1 == discardTile1[j]) {
                            result->type = FURITEN;
                            break;
                        }
                }
                if (bucket[nokoru[k + 1] + 1] != 4) {
                    flag = true;
                    if (!isMachi[nokoru[k + 1] + 1]) {
                        result->machi++;
                        isMachi[nokoru[k + 1] + 1] = 1;
                    }
                    for (int j = 0; j < DisLen; j++)
                        if (nokoru[k + 1] + 1 == discardTile1[j]) {
                            result->type = FURITEN;
                            break;
                        }
                }
            }
        }
    }
    // 总结
    if (result->type != FURITEN && result->type != RON && result->type != TSUMO && flag)
    {
        result->type = TENPAI;
        return true;
    }
    return false;
}

/// @brief 检验是否七对或国士无双听牌
/// @param handTile1
/// @param discardTile1
/// @param currentTile1
/// @param status
/// @return 听了返回true，没听返回false
bool Is7gTennpai(int handTile1[], int *discardTile1, int currentTile1, Status status)
{
    // 七对
    int bucket[zhong + 1] = {0}, count = 0, flag = 0;
    for (int i = 0; i < 14; i++) {
        bucket[handTile1[i]]++;
        if (bucket[handTile1[i]] == 2) {
            count++;
        }
    }
    if (count == 5) {
        for (int i = 0; i < 14; i++) {
            if (bucket[handTile1[i]] % 2 == 1) {
                for (int j = 0; j < DisLen; j++) {
                    if (discardTile1[j] == handTile1[i]) {
                        if (result->type != TENPAI && result->type != RON && result->type != TSUMO) {
                            result->type = FURITEN;
                            return true;
                        }
                    }
                }
            }
        }
        if (result->type != FURITEN || result->type != RON || result->type != TSUMO) {
            result->type = TENPAI;
            return true;
        }
    }
    // 国士无双
    memset(bucket, 0, zhong + 1);
    count = 0;
    for (int i = 0; i < 14; i++) {
        if (handTile1[i] == im || handTile1[i] == km || handTile1[i] == ip || handTile1[i] == kp || handTile1[i] == is || handTile1[i] == ks ||
            handTile1[i] == east || handTile1[i] == south || handTile1[i] == west || handTile1[i] == north ||
            handTile1[i] == bai || handTile1[i] == fa || handTile1[i] == zhong) {
            bucket[handTile1[i]]++;
            count++;
        }
    }
    if (count == 13) {
        for (int i = 0; i < DisLen; i++) {
            if (discardTile1[i] == im || discardTile1[i] == km || discardTile1[i] == ip || discardTile1[i] == kp || discardTile1[i] == is || discardTile1[i] == ks ||
                discardTile1[i] == east || discardTile1[i] == south || discardTile1[i] == west || discardTile1[i] == north ||
                discardTile1[i] == bai || discardTile1[i] == fa || discardTile1[i] == zhong) {
                if (result->type != TENPAI && result->type != RON && result->type != TSUMO) {
                    result->type = FURITEN;
                    return true;
                }
            }
        }
        if (result->type != FURITEN || result->type != RON || result->type != TSUMO) {
            result->type = TENPAI;
            return true;
        }
    }
    if (count == 12) {
        for (int i = 0; i < 2; i++) {
            if (!bucket[im + 9 * i]) {
                for (int j = 0; j < DisLen; j++) {
                    if (bucket[im + 9 * i] == discardTile1[j]) {
                        if (result->type != TENPAI && result->type != RON && result->type != TSUMO) {
                            result->type = FURITEN;
                            return true;
                        }
                    }
                }
            }
            else if (!bucket[km + 9 * i]) {
                for (int j = 0; j < DisLen; j++) {
                    if (bucket[im + 9 * i] == discardTile1[j]) {
                        if (result->type != TENPAI && result->type != RON && result->type != TSUMO) {
                            result->type = FURITEN;
                            return true;
                        }
                    }
                }
            }
        }
        for (int i = east; i <= zhong; i++) {
            if (!bucket[i]) {
                for (int j = 0; j < DisLen; j++) {
                    if (bucket[i] == discardTile1[j]) {
                        if (result->type != TENPAI && result->type != RON && result->type != TSUMO) {
                            result->type = FURITEN;
                            return true;
                        }
                    }
                }
            }
        }
        if (result->type != FURITEN || result->type != RON || result->type != TSUMO) {
            result->type = TENPAI;
            return true;
        }
    }
    return false;
}
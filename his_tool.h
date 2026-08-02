#pragma once

#include "his.h"

//清除输入缓冲区 丢弃cin中残留字符直到换行，防止残留数据影响下次读取
void ClearInputBuffer();

//字符串输入 读取一行，自动去除末尾\r（兼容Windows换行），失败返回false
bool inputLine(string& out);

//确认输入 读取一行，判断是否为Y/y
bool getConfirm();

//带提示的确认输入 打印msg + "(y/n):"，调用getConfirm()
bool confirmAction(const string& msg);

//等待回车 提示用户按回车，先清缓冲区再等待，避免残留字符跳过等待
void waitForEnter();

//选择输入校验 循环读取数字输入，范围[min,max]，非法则提示重输
int getVaildChoice(int min, int max);

//校验数字 字符串非空且全部为0-9
bool isValidNumber(const string& str);

//校验手机号 11位且以1开头，全数字
bool isValidPhone(const string& phone);

//校验身份证 18位，前17位数字，末位数字或X，GB 11643-1999加权校验
bool isValidIDCard(const string& id_card);

//校验无管道符 字符串不含'|'，用于CSV字段防注入
bool hasNoPipe(const string& str);

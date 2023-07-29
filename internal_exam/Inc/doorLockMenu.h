//
// Created by keqiu on 2023/7/28.
//

#ifndef INTERNAL_EXAM_DOORLOCKMENU_H
#define INTERNAL_EXAM_DOORLOCKMENU_H

#include "../Inc/doorLock.h"

void showMenu();
//菜单
uint8_t getType();
//获取请求类型
void requestMenu(uint8_t type,doorLock* door_lock,int& socket_fd);
//请求菜单
#endif //INTERNAL_EXAM_DOORLOCKMENU_H

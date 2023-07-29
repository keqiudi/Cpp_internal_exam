//
// Created by keqiu on 2023/7/21.
//

#ifndef INTERNAL_EXAM_USER_H
#define INTERNAL_EXAM_USER_H


#include <cstdint>
#include <cstring>
class User {

private:
    char userName[64]{};
    char passWord[64]={'1','2','3','4','5','6','\0'};
    char realName[64]{};
    uint8_t usertype{};
    uint64_t seatNumber{};//0表示没有座位
    uint8_t seatStatus;//0表示空闲，1表示固定座位，2表示流动座位
    uint64_t garbageTime{};
    uint8_t activityNumber{};
 public:
    User();

    User(char* userName,char* passWord,char* realName,uint8_t usertype,uint64_t seatNumber,uint8_t seatStatus,uint64_t garbageTime,uint8_t activityNumber);

    User(char *userName, char *passWord, uint8_t usertype,char* realName);

    inline void setUserName(char* userName){
        strncpy(this->userName,userName,64);
    };

    inline void setUserPassword(char* passWord){
        strncpy(this->passWord,passWord,64);
    };

    inline void setUserRealName(char* realName){
        strncpy(this->realName,realName,64);
    };

    inline void setUserType(uint8_t usertype){
        this->usertype = usertype;
    };

    inline void setUserSeatNumber(uint64_t seatNumber){
        this->seatNumber = seatNumber;
    };

    inline void setUserSeatStatus(uint8_t seatStatus){
        this->seatStatus = seatStatus;
    };

    inline void setUserGarbageTime(uint64_t garbageTime){
        this->garbageTime = garbageTime;
    };

    inline void setUserActivityNumber(uint8_t activityNumber){
        this->activityNumber = activityNumber;
    };

    inline char* getUserName(){
        return userName;
    };

    inline char* getUserPassword(){
        return passWord;
    };

    inline char* getUserRealName(){
        return realName;
    };

    inline uint8_t getUserType(){
        return usertype;
    };

    inline uint64_t getUserSeatNumber(){
        return seatNumber;
    };

    inline uint8_t getUserSeatStatus(){
        return seatStatus;
    };

    inline uint64_t getUserGarbageTime(){
        return garbageTime;
    };

    inline uint8_t getUserActivityNumber(){
        return activityNumber;
    };
};

class Manager :public User {
    private:


    public:

    Manager()=default;

    Manager(char* userName,char* passWord,char* realName,uint8_t usertype,uint64_t seatNumber,uint8_t seatStatus,uint64_t garbageTime,uint8_t activityNumber):User(userName,passWord,realName,usertype,seatNumber,seatStatus,garbageTime,activityNumber){};

    Manager(char *userName, char *passWord, uint8_t usertype,char* realName):User(userName,passWord,usertype,realName){};
    
    Manager& operator=(User& user){
        this->setUserName(user.getUserName());
        this->setUserPassword(user.getUserPassword());
        this->setUserRealName(user.getUserRealName());
        this->setUserType(user.getUserType());
        this->setUserSeatNumber(user.getUserSeatNumber());
        this->setUserSeatStatus(user.getUserSeatStatus());
        this->setUserGarbageTime(user.getUserGarbageTime());
        this->setUserActivityNumber(user.getUserActivityNumber());

        return *this;
    };

};


#endif //INTERNAL_EXAM_USER_H

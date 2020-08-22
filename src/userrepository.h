#ifndef USERREPOSITORY_H
#define USERREPOSITORY_H

#include "models.h"
#include <iostream>

class UserRepository
{
private:
    const int MaxUsers = 5;
    UserEntity* _users = nullptr;
    int _totalUsers = 0;
public:
    UserRepository();
    bool GetUser(const char* email, const char* password);
    void Add(UserEntity* user);
    void Add(const char* email, const char* password);
    void Print();
    ~UserRepository();
};

UserRepository::UserRepository()
{
    _users = new UserEntity[MaxUsers];
}

bool UserRepository::GetUser(const char* email, const char* password)
{
    for (int i = 0; i < _totalUsers; i++)
    {
        if(_users[i].Match(email, password))
        {
            return true;
        }
    }
    return false;
}

void UserRepository::Add(const char* email, const char* password)
{
    Add(new UserEntity(email, password));
}

void UserRepository::Add(UserEntity* user)
{
    if(_totalUsers < MaxUsers)
    {
        memcpy(_users + _totalUsers, user, sizeof(UserEntity));
        _totalUsers++;
    }
}

void UserRepository::Print()
{
    for (int i = 0; i < _totalUsers; i++)
    {
        std::cout << i+1 << ": " <<  _users[i].GetEmail() << "\t" << _users[i].GetPassword() << std::endl;
    }    
}

UserRepository::~UserRepository()
{
    if (_users != nullptr)
    {
        delete[] _users;
    }
}

#endif

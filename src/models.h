#ifndef MODELS_H
#define MODELS_H

#include <iostream>
#include <string.h>

class UserData
{
private:
    char userId[];
    char gameId[];
public:
    UserData();
    ~UserData();
};

class UserEntity
{
private:
    char _userId[33];
    char *_email = nullptr;
    char *_password = nullptr;
    void CopyTo(char **destination, const char *source);

public:
    UserEntity(){};
    UserEntity(const char *userId, const char *email, const char *password);
    const char *GetUserId() { return _userId; };
    const char *GetEmail() { return _email; };
    const char *GetPassword() { return _password; };
    bool Match(const char *email, const char *password);
    ~UserEntity();
};

UserEntity::UserEntity(const char *userId, const char *email, const char *password)
{
    strncpy(_userId, userId, sizeof(_userId));
    CopyTo(&_email, email);
    CopyTo(&_password, password);
}

bool UserEntity::Match(const char *email, const char *password)
{
    if (_email != nullptr && _password != nullptr && !strcasecmp(_email, email) && !strcmp(_password, password))
    {
        return true;
    }

    return false;
}

void UserEntity::CopyTo(char **destination, const char *source)
{
    size_t len = strlen(source);
    *destination = new char[len + 1];
    strcpy(*destination, source);
    (*destination)[len] = 0;
}

UserEntity::~UserEntity()
{
    if (_email != nullptr)
    {
        delete[] _email;
    }
    if (_password != nullptr)
    {
        delete[] _password;
    }
}

#endif

/**
 * Author: Nicholas Brown-Duncan
 * Course: COP4635 Project 3
 * Date: 03/25/23
 * Purpose: Definition file for class User.
*/

#include "user.hpp"

User::User(std::string userName, std::string password) {
    this->userName = userName;
    this->password = password;
}

void User::addLocation(std::string location) {
    if(this->subscribedLocations.size() == 0){
        this->subscribedLocations.push_back(location);
        return;
    }
    for(size_t i =0; i < this->subscribedLocations.size(); i++) {
        if(this->subscribedLocations.at(i) == location)
            return;
    }

    this->subscribedLocations.push_back(location);
}

void User::unsubscribeFromLocation(std::string location) {
    for(size_t i =0; i < this->subscribedLocations.size(); i++) {
        if(this->subscribedLocations.at(i) == location){
            this->subscribedLocations.erase(this->subscribedLocations.begin() + i);
            return;
        }
    }
}

void User::addReceiveMessage(std::string message) {

    if(this->receivedMessages.size() == 10)
        this->receivedMessages.erase(this->receivedMessages.begin());
    
    this->receivedMessages.push_back(message);
}

bool User::isSubscribedTo(std::string location) {
    for(size_t i =0; i < this->subscribedLocations.size(); i++)
        if(this->subscribedLocations.at(i) == location)
            return true;
    
    return false;
}
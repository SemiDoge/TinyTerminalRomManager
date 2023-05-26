#ifndef VEC_VIEW_H
#define VEC_VIEW_H

#include <vector>
#include "../inc/logging.h"

//Implements a simple viewer of fullVector
template <typename T>
class VecView { 
private:
    std::vector<T> fullVector; //original vector
public:
    std::size_t start; //start index of the view
    std::size_t length; //size of the view

    VecView(std::vector<T>& vector, std::size_t start, std::size_t length)
    : fullVector(vector), start(start), length(length) {

        if (start > vector.size()) {
            Logger::log(
                fmt::format("Starting position '{}' cannot be bigger than size of vector '{}'",
                            start + length, start, vector.size()),
                logSeverity::ERROR);
            exit(-1);
        }

        if (length > vector.size()) {
            Logger::log(
                fmt::format("Length: '{}' invalid due to being bigger than size of vector '{}'", length, vector.size()),
                logSeverity::ERROR);
            exit(-1);
        }

        if (start + length > vector.size()) {
            Logger::log(
                fmt::format("Length '{}' is too big for starting position '{}' due to being bigger than size of vector '{}'",
                            start + length, start, vector.size()),
                logSeverity::ERROR);
            exit(-1);
        }
    }

    std::size_t size() const {
        return length;
    }

    T& operator [](std::size_t index) {
        if (index >= fullVector.size()) {
            Logger::log(fmt::format("When sub-viewing a vector {} was >= '{}'", index, length), logSeverity::ERROR);
            exit(-1);
        }

        return fullVector[index];
    }

    typename std::vector<T>::iterator begin() {
        return fullVector.begin() + start;
    }

    typename std::vector<T>::const_iterator begin() const {
        return fullVector.begin() + start;
    }

    typename std::vector<T>::iterator end() {
        return fullVector.begin() + start + length;
    }

    typename std::vector<T>::const_iterator end() const {
        return fullVector.begin() + start + length;
    }
};

#endif
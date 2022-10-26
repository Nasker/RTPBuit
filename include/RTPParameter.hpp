#pragma once

class RTPParameter{ 
    int _minValue;
    int _maxValue;
    int _value;
public:
    RTPParameter(int minValue, int maxValue, int value){
        _minValue = minValue;
        _maxValue = maxValue;
        _value = value;
    }
    int incValue(){
        if(_value < _maxValue){
            _value++;
        }
        return _value;
    }
    int decValue(){
        if(_value > _minValue){
            _value--;
        }
        return _value;
    }
    int getValue(){
        return _value;
    }
    void setValue(int value){
        _value = value;
    }
};
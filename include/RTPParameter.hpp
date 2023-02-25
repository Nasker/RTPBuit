#pragma once

class RTPParameter{ 
    int _minValue;
    int _maxValue;
    int _value;
    String _name;
public:
    RTPParameter(int minValue, int maxValue, int value){
        _minValue = minValue;
        _maxValue = maxValue;
        _value = value;
        _name = "";
    }
    RTPParameter(int minValue, int maxValue, int value, String name){
        _minValue = minValue;
        _maxValue = maxValue;
        _value = value;
        _name = name;
    }
    int incValue(){
        _value++;
        if(_value >= _maxValue)
            _value = _maxValue;
        return _value;
    }
    int decValue(){
        _value--;
        if(_value <= _minValue)
            _value = _minValue;
        return _value;
    }
    int getValue(){
        return _value;
    }
    void setValue(int value){
        _value = value;
    }
    void setName(String name){
        _name = name;
    }
    String getName(){
        return _name;
    }
};
/*
 * File: adaptationFunctions.hpp
 * Author: Simon Lassourreuille <simon.lassourreuille@u-bordeaux.fr>
 * Topic: Adaptation functions definition for routing similation
 */
#ifndef STAGE_M1_ADAPTATIONFUNCTION_HPP
#define STAGE_M1_ADAPTATIONFUNCTION_HPP

#include "ProtocolStack.hpp"

/*
 * Enum: adaptType
 * ---------------
 * There is 3 types of adaptation functions:
 *   - CV: conversion. Converts the topIndex protocol to another protocol
 *
 *     ex: ('x', 'y', EC): `--x` → `--y`
 *
 *   - EC: encapsulation. Encapsulates the stack by adding a new topIndex
 *     protocol. Returns an error if the stack becomes full.
 *
 *     ex: ('x', 'y', EC): `--x` → `--xy`
 *
 *   - DC: decapsulation. Removes the topIndex protocol of the stack.
 *     Returns an error if the stack becomes empty.
 *
 *     ex: ('x', 'y', DC): `--xy` → `--x`
 *
 * In each example, the first argument is the topIndex-protocol of the stack
 * before application of the function, and the second argument is the
 * topIndex protocol after application.
 */
typedef enum {
    CV, EC, DC
} adaptType;

class AdaptationFunction {

  public:
    adaptType type;
    protocol in;
    protocol out;

  public:
    AdaptationFunction() = default;
    AdaptationFunction(protocol in, protocol out, adaptType type);
    ~AdaptationFunction() = default;
    void setAll(protocol in, protocol out, adaptType type);
    AdaptationFunction makeReverse();

    bool valid(ProtocolStack &);
    void apply(ProtocolStack &);

    ProtocolStack * In()const;
    ProtocolStack * Out()const;

    bool operator==(const AdaptationFunction &function);
    unsigned long hash();

    string toString();
};

#endif //STAGE_M1_ADAPTATIONFUNCTION_HPP
